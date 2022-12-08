#include "ImageCheckThread.h"
#include "libraw/libraw.h"
#include "libraw/libraw_types.h"
#include "CommandTaskThread.h"
#include "DeviceControl.h"
#include <QDir>
#include <QTime>
#include <QtDebug>
#include <QLockFile>



ImageCheckThread::ImageCheckThread(const QString& cameraName, QObject *parent)
	: QObject(parent)
	, m_cameraName(cameraName)
{
	// 图片检测任务线程
	m_imageCheckThread = new CommandTaskThread();
	auto threadCount = QThread::idealThreadCount() / 4;
	m_threadPool.setMaxThreadCount(threadCount);
}

ImageCheckThread::~ImageCheckThread()
{
	stopCheck();
	delete m_imageCheckThread;
	m_threadPool.clear();
	m_threadPool.waitForDone();
}

void ImageCheckThread::startCheck(const QString& imagePath, int totalCount)
{
	m_isCheckImage = true;
	m_isCheckfinished = false;
	
	auto taskFunc = [=]() {
		QDir imageDir(imagePath + "/" + m_cameraName);
		int checkRet;
		m_mutex.lock();
		m_checkRet.clear();
		m_mutex.unlock();
		int index = 1;

		while (m_isCheckImage)
		{
#if 0
			auto imageList = imageDir.entryList(QStringList() << "*.raf", QDir::Files);
			
			std::sort(imageList.begin(), imageList.end(), [](const QString& a, const QString& b) {
				if (a.size() == b.size())
					return a < b;
				
				return a.size() < b.size();
			});
			
			std::lock_guard guard(m_mutex);
			for (auto i = 0; i < imageList.size(); ++i)
			{
				if (m_checkRet.find(i) == m_checkRet.end())
				{
					auto filePath = imagePath + "/" + m_cameraName + "/" + imageList[i];
					checkRet = testRawFileIsOk(filePath);

					if (checkRet != 0)
					{
						m_checkRet[i] = checkRet == 1;
					}
				}
			}
#else
			bool ret = false;
			if (index <= totalCount)
			{
				auto filePath = QString("%1/%2/%2_%3.raf").arg(imagePath).arg(m_cameraName).arg(index);

				QLockFile lockFile(filePath + "_lock");

				if (QFile::exists(filePath) && lockFile.tryLock())
				{
					lockFile.unlock();

					m_threadPool.start([=]() {
						auto checkRet = testRawFileIsOk(filePath);
						if (checkRet != 0)
						{
							std::lock_guard guard(m_mutex);
							m_checkRet[index] = checkRet == 1;
						}
						});
					++index;
					ret = true;
				}
			}

			if (ret)
			{
				QThread::msleep(50);
			}
#endif

			if (AsyncDeviceControl::instance()->getLeftCamera() == nullptr || 
				AsyncDeviceControl::instance()->getRightCamera() == nullptr)
			{
				m_isCheckfinished = false;
				break;
			}
			
			m_mutex.lock();
			auto size = m_checkRet.size();
			m_mutex.unlock();
			if (size == totalCount)
			{
				m_isCheckfinished = true;
				break;
			}
		}

		m_isCheckImage = false;
	};
	m_imageCheckThread->pushTaskFunc(taskFunc);
}

void ImageCheckThread::stopCheck()
{
	m_isCheckImage = false;
}

std::map<int, bool> ImageCheckThread::getCheckImageResout() const
{
	std::lock_guard guard(m_mutex);
	return m_checkRet;
}

std::set<int> ImageCheckThread::getErrorImageResout() const
{
	std::lock_guard guard(m_mutex);
	std::set<int> errorImage;
	for (auto iter = m_checkRet.begin(); iter != m_checkRet.end(); ++iter)
	{
		if (iter->second == false)
		{
			errorImage.insert(iter->first);
		}
	}
	return errorImage;
}

int ImageCheckThread::testRawFileIsOk(const QString& filePath)
{
	QTime t;
	t.start();

	LibRaw RawProcessor;
	int ret;

	auto path = filePath.toStdWString();
	if ((ret = RawProcessor.open_file(path.c_str())) != LIBRAW_SUCCESS)
	{
		qDebug() << "Cannot open: " << libraw_strerror(ret);
		return 0;
	}

	if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS)
	{
		qDebug() << "Cannot unpack: " << libraw_strerror(ret);
	}

#if 0
	if (ret == LIBRAW_SUCCESS)
	{
		ret = RawProcessor.dcraw_process();

		libraw_processed_image_t* image = RawProcessor.dcraw_make_mem_image(&ret);

		if (image)
		{
			qDebug() << image->width << image->height << image->data_size;
		}

		LibRaw::dcraw_clear_mem(image);
	}
#endif

	RawProcessor.recycle(); // just for show this call
	auto t1 = t.elapsed();
	qDebug() << "raw read time: " << t1 << ret << ",path = " << filePath;

	return ret == LIBRAW_SUCCESS ? 1 : -1;
}