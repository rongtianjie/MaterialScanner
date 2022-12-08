#include "ScanCameraThread.h"
#include "CameraController.h"
#include <QTime>


ScanCameraThread::ScanCameraThread(QObject* parent)
{
}

ScanCameraThread::~ScanCameraThread()
{
	stop();
	quit();
	wait();
}

void ScanCameraThread::scanCamera()
{
	m_isScan = true;
}

void ScanCameraThread::setAutoScan(bool value)
{
	m_isAutoScan = value;

	if (value)
	{
		m_isScan = true;
	}
}

void ScanCameraThread::setAutoScanInterval(int32_t time)
{
	m_intervalTime = time;
}

void ScanCameraThread::setCameraController(CameraController* controller)
{
	m_camController = controller;
}

void ScanCameraThread::stop()
{
	requestInterruption();
}

void ScanCameraThread::run()
{
	QTime time;
	time.start();

	while (!isInterruptionRequested())
	{
		if (m_isScan || (m_isAutoScan && time.elapsed() > m_intervalTime))
		{
			m_isScan = false;
			if (m_camController)
			{
				m_camController->scan();
				emit refreshCameraView();
			}

			time.restart();
		}
		else
		{
			msleep(5);
		}
	}
}
