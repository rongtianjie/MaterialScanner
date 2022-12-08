#include "CameraOperation.h"
#include "CameraController.h"
#include "CameraTextFormat.h"
#include "CameraConfig.h"
#include "SoftConfig.h"
#include "DeviceControl.h"
#include <QThread>
#include <QApplication>




CameraOperation::CameraOperation(QObject *parent)
	: QObject(parent)
{
}

CameraOperation::~CameraOperation()
{
}

void CameraOperation::setCameraHolder(const ICameraHolderPtr& cam)
{
	if (m_cameraHolder.lock() == cam)
	{
		return;
	}
	m_cameraHolder = cam;

	if (cam)
	{
		connect(cam.get(), &ICameraHolder::error, this, &CameraOperation::updatePropertyInfo);
	}

	emit cameraHolderChanged();
	emit updatePropertyInfo();
}

void CameraOperation::updateShutterSpeedModel(PropertyModel* model)
{
    model->clear();
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		const auto& info = camHolder->shutterSpeedInfo();
		model->setIsModify(info.writable);

		for (const auto& value : info.possible)
		{
			auto text = CameraTextFormat::instance()->format_shutter_speed(value);
			model->push_back({ text, (int)value });
		}

		auto value = camHolder->getShutterSpeed();
		auto iter = std::find(info.possible.begin(), info.possible.end(), value);
		if (iter != info.possible.end())
		{
			model->setCurrentIndex(iter - info.possible.begin());
		}
	}
}

void CameraOperation::setShutterSpeed(int value)
{
	const auto & camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		cam->setShutterSpeed(value);
	}
}

int CameraOperation::getShutterSpeed() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		value = camHolder->getShutterSpeed();
	}

	return value;
}

void CameraOperation::updateIsoModel(PropertyModel* model)
{
	model->clear();
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		const auto& info = camHolder->isoInfo();
		model->setIsModify(info.writable);

		for (const auto& value : info.possible)
		{
			auto text = CameraTextFormat::instance()->format_iso_sensitivity(value);
			model->push_back({ text, (int)value });
		}

		auto value = camHolder->getIso();
		auto iter = std::find(info.possible.begin(), info.possible.end(), value);
		if (iter != info.possible.end())
		{
			model->setCurrentIndex(iter - info.possible.begin());
		}
	}
}

void CameraOperation::setIso(int value)
{
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		cam->setIso(value);
	}
}

int CameraOperation::getIso() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		value = camHolder->getIso();
	}

	return value;
}

void CameraOperation::updateApertureModel(PropertyModel* model)
{
	model->clear();
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		const auto& info = camHolder->apertureInfo();
		model->setIsModify(info.writable);

		for (const auto& value : info.possible)
		{
			auto text = CameraTextFormat::instance()->format_f_number(value);
			model->push_back({ text, (int)value });
		}

		auto value = camHolder->getAperture();
		auto iter = std::find(info.possible.begin(), info.possible.end(), value);
		if (iter != info.possible.end())
		{
			model->setCurrentIndex(iter - info.possible.begin());
		}
	}
}

void CameraOperation::setAperture(int value)
{
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		cam->setAperture(value);
	}
}

int CameraOperation::getAperture() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		value = camHolder->getAperture();
	}

	return value;
}

void CameraOperation::updateWhiteBalanceModel(PropertyModel* model)
{
	model->clear();
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		const auto& info = camHolder->whiteBalanceInfo();
		model->setIsModify(info.writable);

		for (const auto& value : info.possible)
		{
			auto text = CameraTextFormat::instance()->format_white_balance(value);
			model->push_back({ text, (int)value });
		}

		auto value = camHolder->getWhiteBalance();
		auto iter = std::find(info.possible.begin(), info.possible.end(), value);
		if (iter != info.possible.end())
		{
			model->setCurrentIndex(iter - info.possible.begin());
		}
	}
}

void CameraOperation::setWhiteBalance(int value)
{
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		auto rlt = cam->setWhiteBalance(value);
		if (!rlt)
		{
		}
	}
}

int CameraOperation::getWhiteBalance() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0); 
	if (camHolder)
	{
		value = camHolder->getWhiteBalance();
	}

	return value;
}

bool CameraOperation::setControlMode(ICameraHolder::ControlMode mode)
{
#if 0
	bool rlt = true;
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		if (!cam->setControlMode(mode))
		{
			rlt = false;
		}
	}
	return rlt;
#else
	const auto& camA = AsyncDeviceControl::instance()->getLeftCamera();
	if (camA && camA->setControlMode(mode) == false)
	{
		qCritical() << "Mid camera setControlMode failed.";
		return false;
	}

	const auto& camB = AsyncDeviceControl::instance()->getRightCamera();
	if (camB && camB->setControlMode(mode) == false)
	{
		qCritical() << "Side camera setControlMode failed.";
		return false;
	}

	return true;
#endif
}

void CameraOperation::shootOne()
{
    const auto& camList = CameraController::instance()->getCameraDeviceList();

    for (const auto& cam : camList)
    {
        cam->shootOne();
    }
}

void CameraOperation::setColorTemperature(int value)
{
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		auto rlt = cam->setColorTemperature(value);
		if (!rlt)
		{
			emit colorTemperatureChanged();
		}
	}
}

int CameraOperation::getColorTemperature() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		value = camHolder->getColorTemperature();
	}

	return value;
}

void CameraOperation::updateMediaRecordModel(PropertyModel* model)
{
	model->clear();
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		const auto& info = camHolder->mediaRecordInfo();
		model->setIsModify(info.writable);

		for (const auto& value : info.possible)
		{
			auto text = CameraTextFormat::instance()->format_media_record(value);
			model->push_back({ text, (int)value });
		}

		auto value = camHolder->getMediaRecord();
		auto iter = std::find(info.possible.begin(), info.possible.end(), value);
		if (iter != info.possible.end())
		{
			model->setCurrentIndex(iter - info.possible.begin());
		}
	}
}

void CameraOperation::setMediaRecord(int value)
{
	const auto& camList = CameraController::instance()->getCameraDeviceList();

	for (const auto& cam : camList)
	{
		cam->setMediaRecord(value);
	}
}

int CameraOperation::getMediaRecord() const
{
	int value = -1;
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		value = camHolder->getMediaRecord();
	}

	return value;
}

bool CameraOperation::backupParameter()
{
	auto camHolder = CameraController::instance()->at(0);
	if (camHolder)
	{
		CameraConfig config;
		config.backup(camHolder.get());
		auto defaultCameraPara = QApplication::applicationDirPath() + "/configs/defaultCameraPara.json";
		return config.saveFile(defaultCameraPara);
	}

	return false;
}

int CameraOperation::restoreParameter()
{
	int errorCode = 0;
	CameraConfig config;
	auto defaultCameraPara = QApplication::applicationDirPath() + "/configs/defaultCameraPara.json";
	auto rlt = config.readFile(defaultCameraPara);

	if (rlt)
	{
        auto camA = AsyncDeviceControl::instance()->getLeftCamera();
		if (camA && !config.restore(camA.get()))
		{
			errorCode = -1;
		}

        auto camB = AsyncDeviceControl::instance()->getRightCamera();
		if (camB && !config.restore(camB.get()))
		{
			errorCode = -2;
		}

		emit AsyncDeviceControl::instance()->updatePropertyInfo();
	}

	return errorCode;
}

bool CameraOperation::checkBatteryLevel()
{
	int32_t value;
	const auto& camA = AsyncDeviceControl::instance()->getLeftCamera();
	if (camA && camA->getBatteryLevel(value) && value < 10)
	{
		return false;
	}

	const auto& camB = AsyncDeviceControl::instance()->getRightCamera();
	if (camB && camB->getBatteryLevel(value) && value < 10)
	{
		return false;
	}
	
	return true;
}

bool CameraOperation::checkCameraPara()
{
	CameraConfig config;
	auto defaultCameraPara = QApplication::applicationDirPath() + "/configs/defaultCameraPara.json";
	auto rlt = config.readFile(defaultCameraPara);

	if (rlt)
	{
		const auto& camA = AsyncDeviceControl::instance()->getLeftCamera();
		if (camA)
		{
			CameraConfig configA;
			configA.backup(camA.get());

			if (configA != config)
			{
				return false;
			}
		}

		const auto& camB = AsyncDeviceControl::instance()->getRightCamera();
		if (camB)
		{
			CameraConfig configB;
			configB.backup(camA.get());

			if (configB != config)
			{
				return false;
			}
		}
	}

	return true;
}

QString CameraOperation::getLensModel() const
{
	QString lensModel;
	const auto& camA = AsyncDeviceControl::instance()->getLeftCamera();
	if (camA)
		lensModel = camA->getLensModel();

	const auto& camB = AsyncDeviceControl::instance()->getRightCamera();
	if (camB)
		lensModel = camB->getLensModel();

	return lensModel;
}

bool CameraOperation::setFocusDistance(int index)
{
	if (index < 0)
		return false;

	bool rltMid = false, rltRight = false;

	auto lensModel = getLensModel();
	const auto& disList = CameraFocusDistance::instance()->getFocusDistanceList(lensModel);

	const auto& camA = AsyncDeviceControl::instance()->getLeftCamera();
	if (camA && disList.midCamera.size() > index)
	{
		auto distance = disList.midCamera.at(index);
		rltMid = camA->setFocusDistance(distance);
		if (!rltMid)
		{
			qCritical() << "Mid camera setFocusDistance failed. distance = " << distance;
		}
	}

	const auto& camB = AsyncDeviceControl::instance()->getRightCamera();
	if (camB && disList.rightCamera.size() > index)
	{
		auto distance = disList.rightCamera.at(index);
		rltRight = camB->setFocusDistance(distance);
		if (!rltRight)
		{
			qCritical() << "Side camera setFocusDistance failed. distance = " << distance;
		}
	}

	return rltMid && rltRight;
}


