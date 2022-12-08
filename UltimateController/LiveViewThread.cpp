#include "liveViewThread.h"
#include <iostream>
#include <utility>
#include <QTime>
#include <QtDebug>


LiveViewThread::LiveViewThread(QObject* parent) : QThread(parent)
{

}

LiveViewThread::~LiveViewThread()
{
    m_cameraHolder.reset();
    stop();
    quit();
    wait();

	deleteBuffer();
}

void LiveViewThread::run()
{
	int waitInterval = 5;

	while (!isInterruptionRequested())
    {
        m_camHolderMutex.lock();
        auto camHolder = m_cameraHolder.lock();
		m_camHolderMutex.unlock();

		if (!camHolder)
        {
            msleep(waitInterval);
            continue;
        }
        
        if (m_showLiveView && camHolder->isConnected())
        {
            SAFE_DELETE_ARRAY(m_buffer2);
			//QTime t;
			//t.start();

			m_camHolderMutex.lock();
			m_buffer2 = camHolder->getLiveViewBuffer((uint32_t*)&m_bufferSize);
			m_camHolderMutex.unlock();

			//qDebug() << "getLiveViewBuffer time : " << t.elapsed();

			if (m_bufferSize > 0 && m_buffer2)
            {
				std::lock_guard guard(m_camHolderMutex);
				// swap buffer
                std::swap(m_buffer, m_buffer2);
                m_bufferSize2 = m_bufferSize;
                m_bufPixmap.loadFromData((uchar*)m_buffer, m_bufferSize2, "jpg");
				//qDebug() << "to pixmap time : " << t.elapsed();

				emit refresh();
			}
        }
        
        msleep(waitInterval);
    }
}

void LiveViewThread::startUp()
{
    if (isStartUp)
    {
        return;
    }

    start();
    isStartUp = true;
}

void LiveViewThread::stop()
{
    requestInterruption();
}

void LiveViewThread::setCameraHolder(const ICameraHolderPtr& cam)
{
	std::lock_guard guard(m_camHolderMutex);
	m_cameraHolder = cam;
}

void LiveViewThread::clearCamera()
{
	std::lock_guard guard(m_camHolderMutex);
	m_cameraHolder.reset();
    deleteBuffer();
}

void LiveViewThread::setShowLiveView(bool isShow)
{
    m_showLiveView = isShow;
}

void LiveViewThread::getPixmap(QPixmap& pix) const
{
	std::lock_guard guard(m_camHolderMutex);
	if (m_buffer != nullptr)
	{
        //pix.loadFromData((uchar*)m_buffer, m_bufferSize2, "jpg");
        pix = m_bufPixmap;
    }
}

void LiveViewThread::deleteBuffer()
{
	SAFE_DELETE_ARRAY(m_buffer);
	SAFE_DELETE_ARRAY(m_buffer2);
}

