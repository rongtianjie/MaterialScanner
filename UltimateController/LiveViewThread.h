#pragma once
#include <QObject>
#include "ICameraHolder.h"
#include <QtWidgets/QLCDNumber>
#include <qthread.h>


/*!
 * \class classname
 *
 * \brief 相机图片获取线程
 */
class LiveViewThread : public QThread
{
    Q_OBJECT

public:
    explicit LiveViewThread(QObject* parent = nullptr);
    ~LiveViewThread();

signals:
	void refresh();

public:
    void startUp();
    void stop();

    void setCameraHolder(const ICameraHolderPtr& cam);
    void clearCamera();

    void setShowLiveView(bool isShow);
    bool isShowLiveView() const { return m_showLiveView; }

    void getPixmap(QPixmap& pix) const;

protected:
	void run() override;

private:
	void deleteBuffer();

private:
    bool isStartUp = false;
    ICameraHolderWPtr m_cameraHolder;
    mutable std::recursive_mutex m_camHolderMutex;

	uint8_t* m_buffer{ nullptr };
	volatile uint32_t m_bufferSize = 0;
	volatile uint32_t m_bufferSize2 = 0;
    uint8_t* m_buffer2{ nullptr };
    QPixmap m_bufPixmap;

	volatile bool m_showLiveView = true;
};