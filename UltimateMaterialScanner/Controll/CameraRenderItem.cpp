#include "CameraRenderItem.h"
#include "CameraTextFormat.h"
#include "SoftConfig.h"
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>



CameraRenderItem::CameraRenderItem(QQuickItem* parent)
	: QQuickPaintedItem(parent)
{
	setAcceptHoverEvents(true);
	setAcceptTouchEvents(true);
	setAcceptedMouseButtons(Qt::AllButtons);
	setFlag(ItemAcceptsInputMethod, true);

	m_curImage = QPixmap(":/resources/Images/no_signal.jpg");

	m_updateImageThread = new UpdateImageThread();
	connect(m_updateImageThread, &UpdateImageThread::updateImage, this, &CameraRenderItem::updateImage, Qt::QueuedConnection);
	m_updateImageThread->start();

	m_batteryTimer = new QTimer(this);
	connect(m_batteryTimer, &QTimer::timeout, this, &CameraRenderItem::updateBatteryInfo);
}

CameraRenderItem::~CameraRenderItem()
{
	m_updateImageThread->requestInterruption();
	m_updateImageThread->wait();
	m_updateImageThread->quit();

	delete m_updateImageThread;
	delete m_showTimer;
}

void CameraRenderItem::setShowCameraImage(bool isShow)
{
	if (m_isShowCamera == isShow)
		return;

	m_isShowCamera = isShow;

	emit showCameraImageChanged();
}

void CameraRenderItem::updateBatteryInfo()
{ 
	auto cam = getCameraHolder();
	if (cam)
	{
		int32_t value;
		if (cam->getBatteryLevel(value))
		{
			auto battery = value / 100.f;
			if (battery != m_batteryValue)
			{
				m_batteryValue = battery;
				emit batteryValueChanged();
			}
		}

		if (cam->getBatteryState(value))
		{
			auto isCharging = (value == 0x000D);
			if (isCharging != m_isCharging)
			{
				m_isCharging = isCharging;
				emit isChargingChanged();
			}
		}
	}
}

void CameraRenderItem::setCameraHolder(const ICameraHolderPtr& cam)
{
	if (m_cameraHolder.lock() == cam)
	{
		return;
	}
	m_cameraHolder = cam;
	m_updateImageThread->setCameraHolder(cam);

	emit cameraHolderChanged();

	if (cam)
	{
		m_distance = cam->getFocusDistance();
		connect(cam.get(), &ICameraHolder::statusChangedSignal, this, [this](ICameraHolder* cam, bool state) {
			if (state == false && m_cameraHolder.lock().get() == cam)
			{
				m_cameraHolder.reset();
				emit hasCameraChanged();
			}
		});

		//connect(cam.get(), &ICameraHolder::focusStatusChanged, this, [this](ICameraHolder*, ICameraHolder::FocusStatus focusState) {
		//	update();
		//	if (focusState == ICameraHolder::FocusSucess)
		//	{
		//		auto cam = getCameraHolder();
		//		m_distance = cam->getFocusDistance();
		//		qDebug() << "Focus distance:" << m_distance;
		//	}
		//	}, Qt::QueuedConnection);
		
		m_batteryTimer->start(10000);
		updateBatteryInfo();
	}
	else
	{
		m_batteryTimer->stop();
	}

	emit hasCameraChanged();
}

void CameraRenderItem::setFocusPosition(int x, int y)
{
	auto camHolder = m_cameraHolder.lock();
    if (!camHolder || !camHolder->getLiveView())
		return;

    if (x > viewport_x && x < (viewport_x + viewport_w) &&
        y > viewport_y && y < (viewport_y + viewport_h))
    {
        float posX = (x - viewport_x) / (float)viewport_w;
        float posY = (y - viewport_y) / (float)viewport_h;

        camHolder->setAFAreaPos(posX, posY);
    }
}

void CameraRenderItem::shootOne()
{
	auto cam = m_cameraHolder.lock();
	if (cam)
	{
		cam->shootOne();
	}
}

void CameraRenderItem::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
	}
	else
	{
		auto cam = m_cameraHolder.lock();
		if (cam)
		{
			cam->focusPress();
		}
	}
}

void CameraRenderItem::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		setFocusPosition(event->x(), event->y());
	}
	else
	{
		auto cam = m_cameraHolder.lock();
		if (cam)
		{
			cam->focusRelease();
		}
	}
}

void CameraRenderItem::updateImage(QPixmap image)
{
	m_curImage = image;
	update();
}

void CameraRenderItem::paint(QPainter* painter)
{
	auto camHolder = m_cameraHolder.lock();

	// draw camera info
	if (nullptr != camHolder)
	{
		//auto t1 = clock();
		auto size = this->size();
		auto bgRect = QRect(0, 0, size.width(), size.height());
		painter->fillRect(bgRect, QColor(0, 0, 0));

		calculateViewportPos();

		painter->setPen(QPen(Qt::red));
		painter->setFont(QFont("Times", 150));

		// 绘制相机画面
		painter->drawPixmap(viewport_x, viewport_y, viewport_w, viewport_h, m_curImage);

		auto t2 = clock();
		//qDebug() << "time 1 : " << t2 - t1;

		// 绘制标准线
		{
			painter->setPen(QPen(Qt::black));
			auto point1 = QPoint(viewport_x, viewport_y + viewport_h / 2);
			auto point2 = QPoint(viewport_x + viewport_w, viewport_y + viewport_h / 2);
			painter->drawLine(point1, point2);

			auto point3 = QPoint(viewport_x + viewport_w/2, viewport_y);
			auto point4 = QPoint(viewport_x + viewport_w / 2, viewport_y + viewport_h);
			painter->drawLine(point3, point4);
		}
		auto t3 = clock();
		//qDebug() << "time 1 : " << t3 - t2;

		// 绘制常见信息
		painter->setPen(QPen(Qt::white));
		auto font = painter->font();
		font.setFamily("Microsoft YaHei");
		font.setPixelSize(18);
		font.setBold(true);
		painter->setFont(font);

		auto w = viewport_w / 4;
		QRect propRect(viewport_x, bgRect.height() - 60, w, 60);

		QString text;

		text = CameraTextFormat::instance()->format_shutter_speed(camHolder->getShutterSpeed());
		painter->drawText(propRect, Qt::AlignVCenter | Qt::AlignRight, text);

		propRect.translate({ w, 0 });
		text = CameraTextFormat::instance()->format_f_number(camHolder->getAperture());
		painter->drawText(propRect, Qt::AlignCenter, text);

		if (SoftConfig::instance()->getShowFocusValue())
		{
			propRect.translate({ w, 0 });
			m_distance = camHolder->getFocusDistance();
			text = QString::number(m_distance);
			painter->drawText(propRect, Qt::AlignVCenter | Qt::AlignLeft, text);
		}
		
		propRect.translate({ w, 0 });
		text = CameraTextFormat::instance()->format_iso_sensitivity(camHolder->getIso());
		painter->drawText(propRect, Qt::AlignVCenter | Qt::AlignLeft, text);

		drawFocusPoint(painter, camHolder);

		//auto te = clock();
		//qDebug() << "time e : " << te - t1;
	}
}

void CameraRenderItem::drawFocusPoint(QPainter* painter, const ICameraHolderPtr& camHolder)
{
	if (m_isShowCamera && camHolder->getLiveView())
	{
		float afX = 0.f, afY = 0.f, afW = 0.f, afH = 0.f;
		auto rlt = camHolder->getAFAreaPos(afX, afY, afW, afH);
		if (rlt)
		{
			QPen pen = painter->pen();
			pen.setWidth(2);

			auto states = camHolder->getFocusStatus();
			switch (states)
			{
			case ICameraHolder::NoFocus:
				pen.setBrush(QColor("white"));
				break;
			case ICameraHolder::FocusSucess:
				pen.setBrush(QColor("green"));
				break;
			case ICameraHolder::FocusFail:
				pen.setBrush(QColor("red"));
				break;
			default:
				break;
			}

			painter->setPen(pen);
			auto offsetX = viewport_x + viewport_w * (afX - afW / 2);
			auto offsetY = viewport_y + viewport_h * (afY - afH / 2);

			QRectF rect(offsetX, offsetY, viewport_w * afW, viewport_h * afH);

			auto wl = rect.width() * 0.3;
			auto hl = rect.height() * 0.3;

			auto pt = rect.topLeft();
			painter->drawLine(pt, pt + QPointF{ 0, hl });
			painter->drawLine(pt, pt + QPointF{ wl, 0 });

			pt = rect.topRight();
			painter->drawLine(pt, pt + QPointF{ 0, hl });
			painter->drawLine(pt, pt + QPointF{ -wl, 0 });

			pt = rect.bottomLeft();
			painter->drawLine(pt, pt + QPointF{ 0, -hl });
			painter->drawLine(pt, pt + QPointF{ wl, 0 });

			pt = rect.bottomRight();
			painter->drawLine(pt, pt + QPointF{ 0, -hl });
			painter->drawLine(pt, pt + QPointF{ -wl, 0 });

			pt = rect.center();
			painter->drawPoint(pt);
		}
	}
}

void CameraRenderItem::calculateViewportPos()
{
	auto camHolder = m_cameraHolder.lock();
	auto size = this->size();
	float aspectRatio = camHolder == nullptr ? 1.5 : camHolder->getAspectRatioValue();

	// basic size
	float ratio = size.width() / (float)size.height();
	if (ratio > aspectRatio)
	{
		viewport_h = size.height();
		viewport_w = viewport_h * aspectRatio;
		viewport_x = (int)((size.width() - viewport_w) / 2);
		viewport_y = 0;
	}
	else
	{
		viewport_x = 0;
		viewport_w = size.width();
		viewport_h = viewport_w / aspectRatio;
		viewport_y = (size.height() - viewport_h) / 2;
	}
}

BatteryItem::BatteryItem(QQuickItem* parent) 
	: QQuickPaintedItem(parent)
{
}

BatteryItem::~BatteryItem()
{
}

void BatteryItem::setBatteryValue(float value)
{
	if (m_batteryValue != value)
	{
		m_batteryValue = value;
		emit batteryValueChanged();
		update();
	}
}

void BatteryItem::setIsCharging(bool value)
{
	if (m_isCharging != value)
	{
		m_isCharging = value;
		emit isChargingChanged();
		update();
	}
}

void BatteryItem::paint(QPainter* painter)
{
	painter->save();

	painter->setRenderHint(QPainter::Antialiasing);
	auto h = width() * 0.4;
	auto h2 = h * 0.4;
	auto w2 = 0.67 * h2;
	auto w = width() - w2 - 4;
	
	QRectF rect(2, (height() - h) / 2, w, h);

	{
		QPen pen(QBrush("#7c7c7c"), 2);
		painter->setPen(pen);
		// 矩形框
		QPainterPath painterPath;
		auto radius = rect.height() * 0.3;
		painterPath.addRoundedRect(rect, radius, radius);
		painter->drawPath(painterPath);
	}

	{
		// 右边的椭圆点
		QRectF rectPoint(4 + w - w2 / 2, (height() - h2) / 2, w2, h2);
		QPainterPath painterPath;
		painterPath.moveTo(rectPoint.center());
		painterPath.arcTo(rectPoint, 90, -180);
		painter->fillPath(painterPath, QBrush("#7c7c7c"));
	}
	
	{
		// 电量
		QPen pen(QBrush("#7c7c7c"), 0);
		painter->setPen(pen);

		auto rect2 = rect.adjusted(3, 3, -3, -3);
		QPainterPath painterPath;
		auto radius = rect2.height() * 0.3;
		painterPath.addRoundedRect(rect2, radius, radius);
		QPainterPath painterPath2;
		rect2.setWidth(rect2.width() * m_batteryValue);
		painterPath2.addRect(rect2);

		QString color;
		if (m_batteryValue > 0.2)
			color = "#00ff00";
		else if (m_batteryValue > 0.1)
			color = "#ffd900";
		else
			color = "#ff0000";

		painter->fillPath(painterPath.intersected(painterPath2), QBrush(QColor(color)));
	}
	
	{
		// 电池电量文本
		QPen pen(QBrush("white"), 2);
		auto font = painter->font();
		font.setBold(true);
		font.setFamily("Microsoft YaHei");
		font.setPixelSize(13);
		painter->setFont(font);
		painter->setPen(pen);
		
		int value = m_batteryValue * 100;
		painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(value));
	}
	
	painter->restore();
}
