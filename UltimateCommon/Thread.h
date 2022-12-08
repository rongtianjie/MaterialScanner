#pragma once
#include <thread>
#include <memory>
#include <atomic>
#include <functional>


class Thread
{
public:
	explicit Thread();

	template<class _Fn, class... _Args>
	explicit Thread(_Fn _Fx, _Args&&... _ax)
	{
		auto func = std::bind(_Fx, _ax);
		execThread = new std::thread(std::bind(functionExec<_Fn, _Args>, std::placeholders::_1, this, std::placeholders::_2, func));
	}

	explicit Thread(const Thread&) = delete;
	virtual ~Thread();

	void start();
	void stop();

	bool wait(uint32_t time = 30000);

	bool isRuning() const {
		return m_isRuning;
	}

	uint32_t getId() const;

	void operator=(const Thread&) = delete;

	friend void functionExec(Thread* thread);

	template<class _Fn, class... _Args>
	friend void functionExec(Thread* thread);

protected:
	virtual void run();

protected:
	std::thread* execThread{ nullptr };
	std::atomic_bool m_isRuning{ false };
	std::atomic_bool isStop{ false };
	
};

template<class _Fn, class... _Args>
void functionExec(Thread* thread, std::function<_Args&&...> func)
{
	thread->m_isRuning = true;
	func();
	thread->m_isRuning = false;
}