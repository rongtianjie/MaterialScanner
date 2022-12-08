#pragma once
#include <QThread>
#include <QQueue>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include "Singleton.h"
#include "UltimateCommon_global.h"


/*!
 * \class AsyncQueue
 *
 * \brief 异步队列
 */
template<typename T>
class AsyncQueue
{
public:
	AsyncQueue(int cacheCount = 20): m_cacheCount(cacheCount) {}
	virtual ~AsyncQueue() {}

	bool push(const T& t) {
		QMutexLocker locker(&m_mutex);
		if (m_queue.size() < m_cacheCount - 1)
		{
			m_queue.push_back(t);
			m_wait.wakeOne();
			return true;
		}

		return false;
	}

	bool waitPop(T& t)
	{
		QMutexLocker locker(&m_mutex);
		if (m_queue.isEmpty())
			m_wait.wait(&m_mutex);

		if (m_queue.isEmpty())
			return false;

        t = m_queue.front();
        m_queue.pop_front();

		return true;
	}

	bool pop(T& t)
	{
		QMutexLocker locker(&m_mutex);

		if (m_queue.isEmpty())
			return false;

		t = m_queue.pop_front();

		return true;
	}

	void wake()
	{
		QMutexLocker oMutexLocker(&m_mutex);
		m_wait.wakeAll();
	}

	void clear() {
		QMutexLocker oMutexLocker(&m_mutex);
		m_queue.clear();
	}

	void setCacheCount(int count) { m_cacheCount = count; }
	int getCacheCount() const { return m_cacheCount; }

private:
	volatile int m_cacheCount{ 20 };
	QMutex m_mutex;
	QWaitCondition m_wait;
	QQueue<T> m_queue;
};


using TaskFunc = std::function<void()>;

class TaskThread : public QThread
{
public:
	TaskThread() {}
	TaskThread(TaskFunc func) : m_func(func) {}
	~TaskThread() {
		stop();
	}

	void stop() {
		requestInterruption();
		quit();
		wait();
	}

protected:
	void run() override {
		if (m_func)
		{
			m_func();
		}
	}

protected:
	TaskFunc m_func{nullptr};
};



/*!
 * \class ICommandTaskThread
 *
 * \brief 命令任务线程模板
 */
template<typename T>
class ICommandTaskThread : public QThread
{
public:
	explicit ICommandTaskThread(int cacheCount = 20) : QThread() {
		m_taskQueue.setCacheCount(cacheCount);
		start();
	}

	virtual ~ICommandTaskThread() {
		m_taskQueue.clear();
        requestInterruption();
		m_taskQueue.wake();
		quit();
		wait();
	}

	bool pushTask(const T& t) {
		return m_taskQueue.push(t);
	}

public:
	virtual void runTask(const T& t) = 0;

protected:
	virtual void threadStart() {}
	virtual void threadEnd() {}

protected:
	void run() override {
		threadStart();

		T t;
		while (!isInterruptionRequested())
		{
			if (m_taskQueue.waitPop(t))
			{
				runTask(t);
			}
		}

		threadEnd();
	}

private:
	AsyncQueue<T> m_taskQueue;
};

#define FRIEND_COMMAND(t) friend class ICommandTaskThread<t>


class Task : public QObject
{
	Q_OBJECT
public:
	Task() {}
	virtual ~Task() {}

Q_SIGNALS:
	void finished();

public:
	virtual int getTaskId() const = 0;

	virtual void run() = 0;
};
DECLARE_STD_PTR(Task);



// 执行函数任务
class FunctionTask : public Task
{
	Q_OBJECT
public:
	FunctionTask(TaskFunc taskFunc) : m_taskFunc(taskFunc) {}

public:
    virtual int getTaskId() const override { return 1; };

	virtual void run() override {
		if (m_taskFunc)
		{
			m_taskFunc();
		}
		emit finished();
	}

private:
	TaskFunc m_taskFunc;
};
DECLARE_STD_PTR(FunctionTask);

/*!
 * \class ICommandTaskThread
 *
 * \brief 命令任务线程
 */
class CommandTaskThread : public ICommandTaskThread<TaskPtr>, public Singleton<CommandTaskThread>
{
	FRIEND_COMMAND(TaskPtr);
	FRIEND_SINGLETON(CommandTaskThread);
	Q_OBJECT
public:
    explicit CommandTaskThread() : ICommandTaskThread<TaskPtr>() {}

public:
	virtual void runTask(const TaskPtr& t) override
	{
		t->run();
	}

	void pushTaskFunc(TaskFunc taskFunc)
	{
		pushTask(std::make_shared<FunctionTask>(taskFunc));
	}

protected:
	virtual void threadStart() override {
		
	}

	virtual void threadEnd() override {
	
	}
};
