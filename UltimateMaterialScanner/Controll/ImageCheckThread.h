#pragma once

#include <QObject>
#include <mutex>
#include <set>
#include <QThreadPool>

class CommandTaskThread;


/*!
 * \class ImageCheckThread
 *
 * \brief 检测图片是否损坏。
 */
class ImageCheckThread : public QObject
{
	Q_OBJECT

public:
	explicit ImageCheckThread(const QString& cameraName, QObject* parent = nullptr);
	~ImageCheckThread();

	void startCheck(const QString& imagePath, int totalCount);

	void stopCheck();

	bool isChecking() const { return m_isCheckImage; }
	bool isCheckResoult() const { return m_isCheckfinished; }

	std::map<int, bool> getCheckImageResout() const;
	std::set<int> getErrorImageResout() const;
	
	/*
	* 检测图片是否完整
	* 参数：
	*	filePath: 图片路径
	* 返回值：
	*	1：完整， -1：有问题， 0：无法打开
	*/
	static int testRawFileIsOk(const QString& filePath);
	
private:
	QString m_cameraName;
	volatile bool m_isCheckImage{ false };			// 检测图片状态 
	volatile bool m_isCheckfinished{ false };		// 检测图片结果
	mutable std::mutex m_mutex;
	std::map<int, bool> m_checkRet;					// 相机检测结果
	CommandTaskThread* m_imageCheckThread;			// 检测任务线程
	QThreadPool m_threadPool;
};
