#pragma once

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QThread>
#include "ICameraHolder.h"

class QTimer;


/*!
 * \class UpdateImageThread
 *
 * \brief 更新图片线程
 */
class UpdateImageThread : public QThread
{
	Q_OBJECT
public:
	using QThread::QThread;

Q_SIGNALS:
	void updateImage(QPixmap image);

public:
	void setCameraHolder(const ICameraHolderPtr& cam) {
		std::lock_guard guard(m_mutex);
		m_cameraHolder = cam;
	}

protected:
	void run() override
	{
		uint8_t* oldImgData = nullptr;

		while (!isInterruptionRequested())
		{
			m_mutex.lock();
			auto camHolder = m_cameraHolder.lock();
			m_mutex.unlock();
			if (camHolder && camHolder->getLiveView())
			{
				QPixmap pixmap;
				uint32_t size = 0;
				uint8_t* imgData = camHolder->getLiveViewBuffer(&size);
				if (oldImgData != imgData)
				{
					if (imgData)
					{
						auto t1 = clock();
						pixmap.loadFromData((uchar*)imgData, size, "jpg");
						SAFE_DELETE_ARRAY(imgData);
						auto t2 = clock();
						//qDebug() << "load image time : " << t2 - t1;
					}
					else
					{
						pixmap = QPixmap(":/resources/Images/no_signal.jpg");
					}
					oldImgData = imgData;

					emit updateImage(pixmap);
				}
				
				QThread::msleep(1);
			}
			else
			{
				QThread::msleep(10);
			}
		}
	}

public:
	std::recursive_mutex m_mutex;
	ICameraHolderWPtr m_cameraHolder;
};

/*!
 * \class CameraRenderItem
 *
 * \brief 相机渲染控件
 */
class CameraRenderItem : public QQuickPaintedItem
{
	Q_OBJECT

	Q_PROPERTY(bool isShowCameraImage READ isShowCameraImage WRITE setShowCameraImage NOTIFY showCameraImageChanged);
	Q_PROPERTY(ICameraHolderPtr cameraHolder READ getCameraHolder WRITE setCameraHolder NOTIFY cameraHolderChanged);
	Q_PROPERTY(bool hasCamera READ hasCamera NOTIFY hasCameraChanged);
	Q_PROPERTY(float batteryValue READ getBatteryValue NOTIFY batteryValueChanged);
	Q_PROPERTY(bool isCharging READ getIsCharging NOTIFY isChargingChanged);

public:
	CameraRenderItem(QQuickItem* parent = nullptr);
	~CameraRenderItem();

Q_SIGNALS:
	void showCameraImageChanged();
	void hasCameraChanged();
	void cameraHolderChanged();
	void batteryValueChanged();
	void isChargingChanged();

public:
	Q_INVOKABLE void setShowCameraImage(bool isShow);
	Q_INVOKABLE bool isShowCameraImage() const { return m_isShowCamera; }

	float getBatteryValue() const { return m_batteryValue; }
	bool getIsCharging() const { return m_isCharging; }

	bool hasCamera() const {
		return !m_cameraHolder.expired();
	}

	void setCameraHolder(const ICameraHolderPtr& cam);
	ICameraHolderPtr getCameraHolder() const {
		return m_cameraHolder.lock();
	}

	Q_INVOKABLE void setFocusPosition(int x, int y);

	Q_INVOKABLE void shootOne();


	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

public Q_SLOTS:
	void updateImage(QPixmap image);

protected:
	void paint(QPainter* painter) override;

	void drawFocusPoint(QPainter* painter, const ICameraHolderPtr& camHolder);

	void calculateViewportPos();

protected Q_SLOTS:
	void updateBatteryInfo();

private:
	int viewport_x{ 0 }, viewport_y{ 0 }, viewport_w{ 0 }, viewport_h{ 0 };
	bool m_isShowCamera{ false };
	ICameraHolderWPtr m_cameraHolder;
	QTimer* m_showTimer{ nullptr };
	UpdateImageThread* m_updateImageThread{ nullptr };
	QPixmap m_curImage;
	int m_distance{ 0 };

	bool m_isCharging{ false };		// 是否在充电
	float m_batteryValue{ 0.0f };	// 电池电量0 ~ 1
	QTimer* m_batteryTimer{ nullptr };
};





/*!
 * \class BatteryItem
 *
 * \brief 电池电量控件
 */
class BatteryItem : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(float batteryValue READ getBatteryValue WRITE setBatteryValue NOTIFY batteryValueChanged);
	Q_PROPERTY(bool isCharging READ getIsCharging WRITE setIsCharging NOTIFY isChargingChanged);
		
public:
	BatteryItem(QQuickItem* parent = nullptr);
	~BatteryItem();

Q_SIGNALS:
	void batteryValueChanged();
	void isChargingChanged();
	
public:
	void setBatteryValue(float value);
	float getBatteryValue() const { return m_batteryValue; }

	void setIsCharging(bool value);
	bool getIsCharging() const { return m_isCharging; }

private:
	void paint(QPainter* painter) override;

private:
	bool m_isCharging{ false };		// 是否在充电
	float m_batteryValue{ 0.0f };	// 电池电量0 ~ 1
};
