#include "DeviceControl.h"
#include "UltimateCommon_global.h"
#include "CommandTaskThread.h"
#include "CameraOperation.h"
#include "MainControl.h"
#include "PowerControl.h"
#include "SoftConfig.h"
#include "CameraConfig.h"
#include "ImageCheckThread.h"
#include "resource.h"

#include <QTime>
#include <QtDebug>
#include <QFile>
#include <QApplication>
#include <QThread>
#include <QDir>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLockFile>


// 版本号
//#define VERSION_INFO    "1.0.0.22"

// 电机移动后延时
#define EM_MOVE_DELAY	1500

// 扫描电流值
#define SCANER_ELE_VALUE	2.5

// 单灯点亮电流值
#define SINGLE_ELE_VALUE	0.2

// 全部点亮电流值
#define ALL_ELE_VALUE		0.8

// 电源控制宏
#define POWER_CONTROL	1


AsyncDeviceControl::AsyncDeviceControl()
{
#if 0
	 m_lightList = QList<int>{
		0,1,2,3,4,5,6,7,
		16,17,18,19,20,21,22,23,
		8,9,10,11,12,13,14,15,
		24,25,26,27,28,29,30,31
	};
#elif 1
	// 旋转光源 顺序：1-2-3-4-5-6-7-8
	m_lightList = QList<int>{
		0,1,2,4,5,6,
		8,9,10,12,13,14
	};
#elif 0
	// 测试使用，减少旋转光源 顺序：1-2-3-4-5-6-7-8
	m_lightList = QList<int>{
		0,1,2,3,4,5,6,7
	};
#endif

	// 控制命令线程
	m_comTaskThread = new CommandTaskThread();
	connect(m_comTaskThread, &CommandTaskThread::finished, this, [this]() {
		SAFE_DELETE(m_devControl);
		SAFE_DELETE(m_powerCtrl);
		SAFE_DELETE(m_bottomPowerCtrl);
	}, Qt::DirectConnection);

	auto cameraController = CameraController::instance();

	connect(cameraController, &CameraController::cameraConnected, this, &AsyncDeviceControl::onCameraConnected, Qt::QueuedConnection);
	connect(cameraController, &CameraController::cameraDisconnected, this, &AsyncDeviceControl::onCameraDisconnected, Qt::QueuedConnection);
}

AsyncDeviceControl::~AsyncDeviceControl()
{
	m_calivrationState = false;
	m_resetState = false;
	m_isControlMode = false;
	endBurnAndLap();
	delete m_comTaskThread;
}

void AsyncDeviceControl::setLoopIndex(qint32 value)
{
	if (m_loopIndex == value)
		return;

	m_loopIndex = value;
	emit loopIndexChanged();
}

bool AsyncDeviceControl::hasCamera() const
{
	return CameraController::instance()->size() > 0;
}

ICameraHolderPtr AsyncDeviceControl::getLeftCamera() const
{
	auto cam = CameraController::instance()->findCameraDeviceFromName(LEFT_CAMERA_NAME);
	return cam;
}

ICameraHolderPtr AsyncDeviceControl::getRightCamera() const
{
	auto cam = CameraController::instance()->findCameraDeviceFromName(RIGHT_CAMERA_NAME);
    return cam;
}

int AsyncDeviceControl::getShootCount() const
{
	return m_lightList.count();
}

QString AsyncDeviceControl::numberToStr(qreal value)
{
	return QString::number(value, 'f', 2);
}


QString AsyncDeviceControl::getVersionInfo()
{
    return QString("%1.%2.%3.%4").arg(VER_MAJOR).arg(VER_MINOR).arg(VER_BUILD).arg(VER_REVISION);
}

void AsyncDeviceControl::exchangedCamera()
{
	auto leftCamera = getLeftCamera();
	auto rightCamera = getRightCamera();

	if (leftCamera)
	{
		leftCamera->setDeviceName(RIGHT_CAMERA_NAME);
	}

	if (rightCamera)
	{
		rightCamera->setDeviceName(LEFT_CAMERA_NAME);
	}

	emit leftCameraChanged();
	emit rightCameraChanged();
}

void AsyncDeviceControl::connectDevice()
{
	auto connectFunc = [this]() {
		if (!m_devControl)
		{
			m_devControl = new MainControl();
			connect(m_devControl, &MainControl::disconnected, this, [this]() {
				//disConnectDevice();
				m_devState = false;
				emit deviceStateChanged();

				endBurnAndLap();
			}, Qt::DirectConnection);
		}

		auto comList = MainControl::checkedCom();
		auto index = SoftConfig::instance()->getMainMacEmIndex();
		if (comList.size() > index)
		{
			auto rlt = m_devControl->connect(comList[index]);
			if (rlt && m_devState != rlt)
			{
				m_devState = true;
				emit deviceStateChanged();
			}
		}

#if POWER_CONTROL
		m_powerCtrl = new PowerControl();
		connect(m_powerCtrl, &PowerControl::disconnected, this, [this]() {
		});

		auto lightIndex = SoftConfig::instance()->getLightEmIndex();
		if (comList.size() > lightIndex)
		{
			auto rlt = m_powerCtrl->connect(comList[lightIndex], 1);
			if (rlt)
			{
				m_powerCtrl->setVoltage(73);
				m_powerCtrl->setElectricity(SCANER_ELE_VALUE);
				m_powerCtrl->setOutputState(true);

				if (m_devControl)
				{
					m_devControl->controlAllLight(LightProtocol::Type::LightAuto1, true);
					m_devControl->controlAllLight(LightProtocol::Type::LightAuto2, true);
				}
			}
			else
			{
				qCritical() << "Connect light power error.";
			}
		}
#endif

		setStageLightConnectState(true);
	};
	m_comTaskThread->pushTaskFunc(connectFunc);
}

void AsyncDeviceControl::disConnectDevice()
{
	auto func = [this]() {
		if (!m_devState)
			return;
		
		if (m_devControl)
		{
			m_devControl->disconnect();
			m_devState = false;
			emit deviceStateChanged();

			endBurnAndLap();
		}
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::controlCamera()
{
	auto func = [this]() {
		if (m_devControl)
		{
			setControlModeSync(ICameraHolder::CameraControl);

			m_devControl->controlCamera();

			setControlModeSync(ICameraHolder::PcControl);
		}
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::controlLightFromIndex(qint32 id, bool isOpen)
{
	if (id < 0 || id >= m_lightList.size())
		return;

	controlLight(m_lightList[id], isOpen);
}

void AsyncDeviceControl::controlLight(qint32 index, bool isOpen)
{
	auto func = [=]() {
		if (m_devControl)
		{
			if (m_powerCtrl)
				m_powerCtrl->setElectricity(isOpen ? SINGLE_ELE_VALUE : SCANER_ELE_VALUE);

#if 1
			auto rlt = m_devControl->controlLight(index, isOpen, false);
#else
			if (index < 0)
			{
				return false;
			}

			int addressIndex = index / 16 * 8 + index % 8 + 1;
			int lightIndex = index % 16 / 8;
			auto rlt = controlLightSub(addressIndex, lightIndex, isOpen);
#endif // 0
			qDebug() << "controlLight rlt : " << rlt;

			if (rlt)
			{
				emit lightStateChanged(index, isOpen);
			}
		}
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::testLightAdc(qint32 index)
{
	auto func = [=]() {
		testAdc(index);
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::openAllLight()
{
	auto func = [=]() {
		controlAllLight(true);
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::closeAllLight()
{
	auto func = [=]() {
		controlAllLight(false);
		
	};

	m_comTaskThread->pushTaskFunc(func);
}


void AsyncDeviceControl::shoot(qint32 index)
{
	auto func = [=]() {
		auto x = SoftConfig::instance()->getX();
		auto y = SoftConfig::instance()->getY();
        shootImage(index, x, y);
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::allLightshoot()
{
	auto func = [=]() {
		if (!m_devControl->isConnected())
			return;

		auto x = SoftConfig::instance()->getX();
		auto y = SoftConfig::instance()->getY();

		setControlModeSync(ICameraHolder::CameraControl);

		allLightShoot(x, y);

		setControlModeSync(ICameraHolder::PcControl);
	};
	m_comTaskThread->pushTaskFunc(func);
}

QString AsyncDeviceControl::checkScanSavePath()
{
	QString errorText;

	const auto& path = SoftConfig::instance()->getScanSavePath();

	// 检测扫描保存路径是否存在，并自动加后缀
    auto name = SoftConfig::instance()->getScanSaveName();

	if (path.isEmpty() || name.isEmpty())
	{
		errorText = tr("File store not set.");
		CameraController::instance()->setPhotoSavePath("");
	}
	else
	{
        QString savePath = path + "/" + name;
		auto newName = name;
        int i = 1;
        QDir dir;
        while(dir.exists(savePath))
        {
			newName = QString("%1_%2").arg(name).arg(i);
            savePath = QString("%1/%2").arg(path).arg(newName);
			++i;
        }
        SoftConfig::instance()->setScanSaveName(name);
		m_scanPathName = newName;
        CameraController::instance()->setPhotoSavePath(savePath);
		saveScanCameraPara(savePath);
	}

	// 检测此盘符剩余容量
	auto drivePath = path.left(3);
	auto freeSpaceSize = util::getDiskFreeSpace(drivePath) / 1024.f/1024.f/1024.f;
	if (freeSpaceSize < 30)
	{
		errorText = tr("The disk space is not enough.");
	}

	return errorText;
}

void AsyncDeviceControl::startBurnAndLap(bool isOnlyCtrlLight)
{
	if (m_isBurnAndLaping || !m_devControl->isConnected())
		return;

	m_isBurnAndLaping = true;

	auto func = [=]() {
		if (m_powerCtrl)
			m_powerCtrl->setElectricity(SCANER_ELE_VALUE);

		if (m_isOnlyCtrlLight != isOnlyCtrlLight)
		{
			m_isOnlyCtrlLight = isOnlyCtrlLight;
			emit isOnlyCtrlLightChanged();
		}
		setLoopIndex(0);

		m_isBurnAndLap = true;
		emit burnAndLapChanged();

		// 检测扫描路径，设置对焦值
		if (!m_isOnlyCtrlLight)
		{
			auto errorText = checkScanSavePath();
			if (!errorText.isEmpty())
			{
				emit scanTipInfo(errorText);
			}

#if 1
			auto index = SoftConfig::instance()->getFocusDisIndex();
			CameraOperation::instance()->setFocusDistance(index);
#endif
			// 拷贝标定文件到图片路径下
			if (m_scanPathName.isEmpty() == false)
			{
				auto lensModel = CameraOperation::instance()->getLensModel();
				auto savePath = SoftConfig::instance()->getScanSavePath();
				auto parPath = QApplication::applicationDirPath() + "/stereo_main/conf";
				auto parFile = QString("%1/cam_%2.json").arg(parPath).arg(lensModel);
				auto newParFile = QString("%1/%2/cam_%3.json").arg(savePath).arg(m_scanPathName).arg(lensModel);
				QFile::remove(newParFile);
				QFile::copy(parFile, newParFile);
			}
		}

		setControlModeSync(ICameraHolder::CameraControl);

		auto x = SoftConfig::instance()->getX();
		auto y = SoftConfig::instance()->getY();
		auto z = SoftConfig::instance()->getZ();
		auto endShootOne = SoftConfig::instance()->getEndShootOne();

		if (!m_isOnlyCtrlLight)
		{
			if (SoftConfig::instance()->getGrayBoardTest())
			{
				// 灰度板测试
				grayBoardTestShooting(x, y, z, endShootOne);
			}
			else
			{
#if 0
				// 扫描拍摄
				scanShooting(x, y, z, endShootOne);
#else
				auto hasBottomLight = SoftConfig::instance()->getScanMode() == SoftConfig::BottomLightMode;
				// 扫描拍摄并检测图片
				scanShootingAndCheckImage(x, y, z, endShootOne, hasBottomLight);
#endif
			}
		}
		else
		{
			// 灯光跑圈
			lightRunningCircle(x, y, z);
		}

		setControlModeSync(ICameraHolder::PcControl);

		const auto& testSavePath = SoftConfig::instance()->getTestSavePath();
		CameraController::instance()->setPhotoSavePath(testSavePath);

		m_isBurnAndLap = false;
		emit burnAndLapChanged();
		m_isBurnAndLaping = false;
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::endBurnAndLap()
{
	m_isBurnAndLaping = false;
}

void AsyncDeviceControl::setEMRotationAngle(float angle)
{
	auto func = [=]() {
		if (!m_devControl->isConnected())
			return;

		auto rlt = m_devControl->setEMRotationAngle(angle);

		auto times = 0;
		while (rlt && times < 2)
		{
			auto recProto = m_devControl->readData();
			auto emProt = dynamic_cast<ElectricMachineryProtocol*>(recProto);
			if (emProt)
			{
				SAFE_DELETE(recProto);
				break;
			}
			SAFE_DELETE(recProto);
			QThread::msleep(100);
			++times;
		}
	};
	m_comTaskThread->pushTaskFunc(func);
	qDebug() << "setEMRotationAngle " << angle;
}

void AsyncDeviceControl::setEMRotationSpeed(int speed)
{
	auto func = [=]() {
		if (!m_devControl->isConnected())
			return;

		auto rlt = m_devControl->setEMRotationSpeed(speed);
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::resetEM()
{
	auto func = [=]() {
		resetEMSync();
	};
	m_comTaskThread->pushTaskFunc(func);
}

int AsyncDeviceControl::lightNumber(int index)
{
	auto newIndex = (index % 12 + 1) / 7 + index % 6 * 2 + 1 + (index / 12) * 12;

	return newIndex;
}

int AsyncDeviceControl::lightIdToIndex(int id)
{
	auto index = m_lightList.indexOf(id);
	return index;
}

bool AsyncDeviceControl::isConnectedStageLight()
{
	if (m_bottomPowerCtrl)
		return m_bottomPowerCtrl->isConnected();

	return false;
}

void AsyncDeviceControl::setStageLightConnectState(bool isConnect)
{
	auto func = [=]() {
		if (!m_bottomPowerCtrl)
		{
			m_bottomPowerCtrl = new PowerControl();

			connect(m_bottomPowerCtrl, &PowerControl::disconnected, this, [this]() {
				m_bottomPowerCtrl->disconnect();

				emit stageLightConnectStateChanged();

#if 0
				auto comList = MainControl::checkedCom();

				auto bottomEmIndex = SoftConfig::instance()->getBottomLightEmIndex();
				if (comList.size() > bottomEmIndex)
					m_bottomPowerCtrl->connect(comList[bottomEmIndex], 2);
#endif
			}, Qt::QueuedConnection);
		}

		auto comList = MainControl::checkedCom();

		auto bottomEmIndex = SoftConfig::instance()->getBottomLightEmIndex();
		if (comList.size() > bottomEmIndex)
		{
			auto rlt = m_bottomPowerCtrl->connect(comList[bottomEmIndex], 2);
			if (rlt)
			{
				m_bottomPowerCtrl->setVoltage(46);
				auto elecValue = SoftConfig::instance()->getStageLightElectricity();
				m_bottomPowerCtrl->setElectricity(elecValue);
				m_bottomPowerCtrl->setOutputState(false);
			
				emit stageLightConnectStateChanged();
			}
			else
			{
				qCritical() << "Connect bottom light power error.";
			}
		}
	};
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::setStageLightState(bool state)
{
	auto func = [=]() {
		ctrlStageLight(state);	
	};
	m_comTaskThread->pushTaskFunc(func);
}

bool AsyncDeviceControl::getStageLightState()
{
	if (m_bottomPowerCtrl)
	{
		return m_bottomPowerCtrl->getOutputState();
	}

	return false;
}

void AsyncDeviceControl::setStageLightElectricity(float value)
{
	//qInfo() << "Electricity:" << value;
	auto func = [=]() {
		if (m_bottomPowerCtrl)
		{
			auto rlt = m_bottomPowerCtrl->setElectricity(value);
			if (rlt)
			{
				SoftConfig::instance()->setStageLightElectricity(value);
				//emit stageLightElectricityChanged();
			}
		}
	};
	m_comTaskThread->pushTaskFunc(func);
}

float AsyncDeviceControl::getStageLightElectricity()
{
	if (m_bottomPowerCtrl)
	{
		auto value = m_bottomPowerCtrl->getElectricity();
		//qInfo() << "value : " << value;

		return value;
	}

	return 0.0f;
}

void AsyncDeviceControl::cameraCalibration()
{
	if (m_calivrationState)
		return;

	setCalibrationState(true);

#if 1
	auto func = [=]() {
		controlAllLight(false);
		resetEMSync();

		if (!m_devControl->isConnected())
		{
			setCalibrationState(false);
			emit calibrationFinished(false);

			return;
		}

		auto x = SoftConfig::instance()->getX();
		auto y = SoftConfig::instance()->getY();
		auto z = SoftConfig::instance()->getZ();

		auto calibPath = QApplication::applicationDirPath() + "/Calibration/Images";
		QDir dir(calibPath);
		dir.removeRecursively();
		CameraController::instance()->setPhotoSavePath(calibPath);

		bool rlt = true;

		setControlModeSync(ICameraHolder::CameraControl);

		for (auto i = 0; i < 4; ++i)
		{
			rlt = CameraOperation::instance()->setFocusDistance(i);
			if (!rlt)
				break;

			allLightShoot(x, y);

			QThread::msleep(z);
		}

		setControlModeSync(ICameraHolder::PcControl);

		if (rlt)
		{
			QThread::msleep(2000);
			rlt = calibration(calibPath);
		}

		//dir.removeRecursively();

		const auto& testSavePath = SoftConfig::instance()->getTestSavePath();
		CameraController::instance()->setPhotoSavePath(testSavePath);
		setCalibrationState(false);

		emit calibrationFinished(rlt);
	};
#else
	auto func = [=]() {
		auto calibPath = QApplication::applicationDirPath() + "/Calibration/Images";
		auto ret = calibration(calibPath);

		setCalibrationState(false);

		emit calibrationFinished(ret);
	};
#endif
	
	m_comTaskThread->pushTaskFunc(func);
}

void AsyncDeviceControl::controlAllLight(bool isOpen)
{
	if (isOpen)
	{
		if (m_powerCtrl)
			m_powerCtrl->setElectricity(ALL_ELE_VALUE);

		if (m_devControl)
		{
			for (auto i = 0; i < 3; i++)
			{
				m_devControl->controlAllLight(LightProtocol::Type::Light1, true);
				m_devControl->controlAllLight(LightProtocol::Type::Light2, true);
			}
			emit allLightOpened();
		}
	}
	else
	{
		if (m_devControl)
		{
			for (auto i = 0; i < 3; i++)
			{
				m_devControl->controlAllLight(LightProtocol::Type::Light1, false);
				m_devControl->controlAllLight(LightProtocol::Type::Light2, false);
			}

			emit allLightClosed();
		}

		if (m_powerCtrl)
			m_powerCtrl->setElectricity(SCANER_ELE_VALUE);
	}
}

void AsyncDeviceControl::setCalibrationState(bool state)
{
	if (m_calivrationState != state)
	{
		m_calivrationState = state;
		emit calibrationStateChanged();
	}
}

bool AsyncDeviceControl::calibration(const QString& calibPath)
{
	auto ret = true;

	QDir midDir(calibPath + "/" + LEFT_CAMERA_NAME);
	QDir rightDir(calibPath + "/" + RIGHT_CAMERA_NAME);
	auto midPathList = midDir.entryInfoList(QStringList() << "*.raf", QDir::Files, QDir::Name);
	auto rightPathList = rightDir.entryInfoList(QStringList() << "*.raf", QDir::Files, QDir::Name);

	if (midPathList.size() == 4 && rightPathList.size() == 4)
	{
		std::vector<QProcess*> processList;
		auto calibration_path = QApplication::applicationDirPath() + "/Calibration";
#if 1
		auto lensModel = CameraOperation::instance()->getLensModel();
		if (lensModel.isEmpty())
		{
			return false;
		}
#else
		auto lensModel = "120";
#endif


		for (auto i = 0; i < 4; ++i)
		{
			auto midPath = midPathList[i].absoluteFilePath();
			auto rightPath = rightPathList[i].absoluteFilePath();

			auto calibration_file = QString("%1/cam_%2_%3.json").arg(calibration_path).arg(lensModel).arg(i);
			auto process = new QProcess();
			auto praList = QStringList() << midPath << rightPath << 
				calibration_file << QString::number(i) << calibPath;
			process->setWorkingDirectory(calibration_path);
			process->start(calibration_path + "/recalibration.exe", praList);
			processList.push_back(process);
		}

		std::map<int, bool> processStateMap;
		for (auto i = 0; i < 4; ++i)
		{
			processStateMap[i] = false;
		}

		while (true)
		{
			if (processStateMap.empty())
				break;
			
			for (auto iter = processStateMap.begin(); iter != processStateMap.end(); )
			{
				if (false == iter->second)
				{
					int index = iter->first;
					iter->second = processList[index]->waitForFinished(index);
					
					if (iter->second)
					{
						auto exitCode = processList[index]->exitCode();
						
						if (exitCode != 0)
							ret = false;
					}
				}

				if (iter->second)
				{
					iter = processStateMap.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	
		qDeleteAll(processList);

		if (ret)
		{
			// 组合标定参数
			auto parPath = QApplication::applicationDirPath() + "/stereo_main/conf";
			auto parFile = QString("%1/cam_%2.json").arg(parPath).arg(lensModel);
			QFile parJsonFile(parFile);
			
			ret = parJsonFile.open(QFile::WriteOnly);
			
			if (ret)
			{
				QJsonArray parArray;
				
				QJsonParseError error;
				
				for (auto i = 0; i < 4; ++i)
				{
					auto calibration_file = QString("%1/cam_%2_%3.json").arg(calibration_path).arg(lensModel).arg(i);
					QFile jsonFile(calibration_file);

					if (jsonFile.open(QFile::ReadOnly) == false)
					{
						ret = false;
						break;
					}
					
					auto jsonDoc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
					jsonFile.close();

					if (error.error != QJsonParseError::NoError)
					{
						ret = false;
						break;
					}

					auto json = jsonDoc.object();
					parArray.push_back(json);
				}

				if (ret)
				{
					QJsonDocument parJsonDoc;
					QJsonObject rootObj;

					rootObj["camera_matrices"] = parArray;
					parJsonDoc.setObject(rootObj);
					auto data = parJsonDoc.toJson(QJsonDocument::Indented);

					parJsonFile.write(data);	
				}
				
				parJsonFile.close();
			}

			//QFile::remove(parFile);
			//ret = QFile::copy(calibration_file, parFile);
		}
	}
	else
	{
		ret = false;
	}
	
	return ret;
}

void AsyncDeviceControl::resetEMSync()
{
	if (!m_devControl->isConnected())
		return;

	m_resetState = true;
	auto rlt = m_devControl->resetEM();

	while (m_resetState)
	{
		auto recProto = m_devControl->readData(100);
		auto emProt = dynamic_cast<EMControlProtocol*>(recProto);
		if (emProt)
		{
			SAFE_DELETE(recProto);
			break;
		}
		SAFE_DELETE(recProto);
	}
}

void AsyncDeviceControl::allLightShoot(int x, int y)
{
	if (x >= 0)
	{
		for (size_t i = 0; i < 2; i++)
		{
			m_devControl->controlAllLight(LightProtocol::Type::LightAuto1, true);
			m_devControl->controlAllLight(LightProtocol::Type::LightAuto2, true);
		}

		QThread::msleep(x);
		m_devControl->controlCamera();
	}
	else
	{
		m_devControl->controlCamera();
		QThread::msleep(-x);

		for (size_t i = 0; i < 2; i++)
		{
			m_devControl->controlAllLight(LightProtocol::Type::LightAuto1, true);
			m_devControl->controlAllLight(LightProtocol::Type::LightAuto2, true);
		}
	}

	QThread::msleep(y);

	for (size_t i = 0; i < 2; i++)
	{
		m_devControl->controlAllLight(LightProtocol::Type::LightAuto1, false);
		m_devControl->controlAllLight(LightProtocol::Type::LightAuto2, false);
	}
}

void AsyncDeviceControl::shootImage(qint32 index, quint32 x, quint32 y, bool isOnlyCtrlLight)
{
	if (index < 0)
	{
		return;
	}

	auto rlt = m_devControl->controlLight(index, true, true);
	if (rlt)
		emit lightStateChanged(index, true);

	QThread::msleep(x);

	if (!isOnlyCtrlLight)
	{
		m_devControl->controlCamera();
	}

	QThread::msleep(y);

	rlt = m_devControl->controlLight(index, false, true);
	if (rlt)
	{
		emit lightStateChanged(index, false);
	}
}

void AsyncDeviceControl::testAdc(qint32 index)
{
	if (index < 0 || !m_devControl)
	{
		return;
	}

	int addressIndex = index / 16 * 8 + index % 8 + 1;
	int lightIndex = (index + 1) % 16 / 8;

	auto isOpen = true;
	auto lightProt = m_devControl->controlLight(addressIndex, lightIndex, true, false);
	if (lightProt)
		emit lightStateChanged(index, true);

#if 1
	if (lightProt)
	{
		auto addTextFile = QApplication::applicationDirPath() + "/configs/adcTest.txt";
		QFile file(addTextFile);

		QTextStream out(&file);

		auto isFileOpened = file.open(QFile::WriteOnly | QFile::Append);
		out << "address:" << "\t" << lightProt->getAddress() << "\t"
			<< (lightProt->getType() == LightProtocol::Type::LightAuto1 ? "light1" : "light2") << "\t"
			<< (isOpen ? "open" : "close") << "\n";

		for (auto i = 0; i < 50; ++i)
		{
			QThread::msleep(20);

			auto adcValue = quint16(0);
            auto rlt = m_devControl->readAdc(addressIndex, lightIndex == 0 ? AdcProtocol::Type::ADC3 : AdcProtocol::Type::ADC2, adcValue);

			if (isFileOpened)
			{
				if (lightProt->getType() == LightProtocol::Type::Light2)
				{
					out << (i + 1) << "\t" << "adc:" << "\t" << adcValue << "\n";
				}
				else
				{
					qDebug() << "adcData = nullptr";
				}
			}

		}

		if (isFileOpened)
		{
			out << "\n";

			file.flush();
			file.close();
		}
		else
		{
			auto q = 1;
		}
	}

	SAFE_DELETE(lightProt);

	lightProt = m_devControl->controlLight(addressIndex, lightIndex, false, false);
	if (lightProt)
		emit lightStateChanged(index, false);
	SAFE_DELETE(lightProt);
#endif
}

bool AsyncDeviceControl::controlLightSub(quint8 addressIndex, quint8 lightIndex, bool isOpen)
{
	QByteArray pData;
	pData.append((char)addressIndex);
	pData.append((char)0x05);
	pData.append((char)0x01);
	pData.append(lightIndex == 0 ? (char)0x2e : (char)0x2f);
	pData.append(isOpen ? (char)0xFF : (char)0x00);
	pData.append((char)0x00);
	quint16 crc = util::checksum16(pData);
	pData.append((char*) &crc, 2);
	auto text = pData.toHex();

	auto rlt = m_devControl->writeData(pData);

	return rlt;
}

bool AsyncDeviceControl::waitTime(int time, volatile const bool& runState)
{
	auto t1 = clock();
	auto rlt = false;

    while (runState)
	{
		QThread::msleep(1);
		auto t2 = clock();
		if (t2 - t1 >= time)
		{
			rlt = true;
			break;
		}
	}

	return rlt;
}

void AsyncDeviceControl::setControlModeSync(ICameraHolder::ControlMode mode)
{
	while (m_isControlMode)
	{
		auto rlt = CameraOperation::instance()->setControlMode(mode);
		if (rlt)
			break;
		else
			QThread::msleep(10);
	}
}

bool AsyncDeviceControl::ctrlStageLight(bool state)
{
	if (!m_bottomPowerCtrl)
		return false;

	auto rlt = m_bottomPowerCtrl->setOutputState(state);
	auto t1 = clock();

	while (true)
	{
		QThread::msleep(10);
		m_bottomPowerCtrl->syncInfo();
		if (m_bottomPowerCtrl->getOutputState() == state)
		{
			rlt = true;
			break;
		}

		auto t2 = clock();
		if (t2 - t1 > 2000)
		{
			rlt = false;
			break;
		}
	}
	emit stageLightStateChanged();
	return rlt;
}

bool AsyncDeviceControl::saveScanCameraPara(const QString& savePath)
{
	auto camHandle = getLeftCamera();
	if (!camHandle)
		camHandle = getRightCamera();

	if (!camHandle)
		return false;

	QDir dir(savePath);
	if (!dir.exists())
	{
		dir.mkpath(savePath);
	}
	auto path = savePath + "/" + SCAN_CAMERA_CONFIG;
	ScanCameraConfig config;
	config.backup(camHandle.get());
	config.setFocusDistanceIndex(SoftConfig::instance()->getFocusDisIndex());
	return config.saveFile(path);
}

void AsyncDeviceControl::scanShooting(int x, int y, int z, bool endShootOne)
{
	controlAllLight(false);
	ctrlStageLight(false);

	m_devControl->resetEM();

#if 1
	while (m_isBurnAndLaping)
	{
		auto recProto = m_devControl->readData();
		auto emProt = dynamic_cast<EMControlProtocol*>(recProto);
		if (emProt)
		{
			SAFE_DELETE(recProto);
			break;
		}
		SAFE_DELETE(recProto);
	}
#endif
	int rotationTimes = 180 / m_rotationAngle;

	for (auto i = 0; i < rotationTimes; ++i)
	{
		if (!m_isBurnAndLaping)
			break;

		for (const auto& lightIndex : m_lightList)
		{
			if (!m_isBurnAndLaping)
				break;

			auto index = lightIndex;

			int addressIndex = index / 16 * 8 + index % 8 + 1;
			int lightIndex = (index + 1) % 16 / 8;

			bool rlt = false;
			if (x >= 0)
			{
				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);

				if (!m_isOnlyCtrlLight)
				{
					if (waitTime(x, m_isBurnAndLaping) == false)
						break;

					m_devControl->controlCamera();
				}
			}
			else
			{
				if (!m_isOnlyCtrlLight)
				{
					m_devControl->controlCamera();

					if (waitTime(qAbs(x), m_isBurnAndLaping) == false)
						break;
				}

				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);
			}

			if (waitTime(y, m_isBurnAndLaping) == false)
				break;

			rlt = m_devControl->controlLight(index, false, true);
			if (rlt)
			{
				emit lightStateChanged(index, false);
			}

			if (waitTime(z, m_isBurnAndLaping) == false)
				break;

			setLoopIndex(m_loopIndex + 1);
		}

		if (m_isBurnAndLaping && i < (rotationTimes - 1))
		{
			float angle = 0;
			float rotationAngle = (i + 1) * m_rotationAngle;
			auto rlt = m_devControl->setEMRotationAngle(rotationAngle);
			do {
				if (rlt)
				{
					m_devControl->getEMRotationAngle(angle);
				}

			} while (std::abs(angle - rotationAngle) > 2 && m_isBurnAndLaping);

			waitTime(EM_MOVE_DELAY, m_isBurnAndLaping);
		}
	}

	if (m_isBurnAndLaping)
	{
		if (SoftConfig::instance()->getScanMode() == SoftConfig::BottomLightMode)
		{
			ctrlStageLight(true);
			waitTime(100, m_isBurnAndLaping);
			m_devControl->controlCamera();
			waitTime(y, m_isBurnAndLaping);
			ctrlStageLight(false);
		}

		if (endShootOne)
			m_devControl->controlCamera();
	}
}

void AsyncDeviceControl::scanShootingAndCheckImage(int x, int y, int z, bool endShootOne, bool hasBottomLight)
{
	controlAllLight(false);
	ctrlStageLight(false);

	m_devControl->resetEM();
#if 1
	while (m_isBurnAndLaping)
	{
		auto recProto = m_devControl->readData();
		auto emProt = dynamic_cast<EMControlProtocol*>(recProto);
		if (emProt)
		{
			SAFE_DELETE(recProto);
			break;
		}
		SAFE_DELETE(recProto);
	}
#endif
	int rotationTimes = 180 / m_rotationAngle;

	auto totalCount = rotationTimes * m_lightList.size();
	if (hasBottomLight)
		totalCount += 2;
	auto savePath = SoftConfig::instance()->getScanSavePath();
	auto scanPath = QString("%1/%2").arg(savePath).arg(m_scanPathName);
	//auto scanPath1 = QString("%1/%2").arg(savePath).arg(u8"杯垫+树叶+青苔-1~2cm");

	ImageCheckThread midImageCheck(LEFT_CAMERA_NAME);
	ImageCheckThread rightImageCheck(RIGHT_CAMERA_NAME);
	midImageCheck.startCheck(scanPath, totalCount);
	rightImageCheck.startCheck(scanPath, totalCount);

#if 1
	for (auto i = 0; i < rotationTimes; ++i)
	{
		if (!m_isBurnAndLaping)
			break;

		for (const auto& lightIndex : m_lightList)
		{
			if (!m_isBurnAndLaping)
				break;

			auto index = lightIndex;


			bool rlt = false;
			if (x >= 0)
			{
				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);

				if (waitTime(x, m_isBurnAndLaping) == false)
					break;

				m_devControl->controlCamera();
			}
			else
			{
				m_devControl->controlCamera();

				if (waitTime(qAbs(x), m_isBurnAndLaping) == false)
					break;

				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);
			}

			if (waitTime(y, m_isBurnAndLaping) == false)
				break;

			rlt = m_devControl->controlLight(index, false, true);
			if (rlt)
			{
				emit lightStateChanged(index, false);
			}

			if (waitTime(z, m_isBurnAndLaping) == false)
				break;

			setLoopIndex(m_loopIndex + 1);
		}

		if (m_isBurnAndLaping && i < (rotationTimes - 1))
		{
			float angle = 0;
			float rotationAngle = (i + 1) * m_rotationAngle;
			auto rlt = m_devControl->setEMRotationAngle(rotationAngle);
			do {
				if (rlt)
				{
					m_devControl->getEMRotationAngle(angle);
				}

			} while (std::abs(angle - rotationAngle) > 2 && m_isBurnAndLaping);

			waitTime(EM_MOVE_DELAY, m_isBurnAndLaping);
		}
	}

	if (m_isBurnAndLaping)
	{
		if (hasBottomLight)
		{
			ctrlStageLight(true);
			waitTime(100, m_isBurnAndLaping);
			m_devControl->controlCamera();
			waitTime(y, m_isBurnAndLaping);
			ctrlStageLight(false);
		}

		if (endShootOne)
			m_devControl->controlCamera();
	}
#endif

	while ((midImageCheck.isChecking() || rightImageCheck.isChecking()) && m_isBurnAndLaping)
		QThread::msleep(100);

	// 重新扫描拍摄
	if (m_isBurnAndLaping)
	{
		auto midList = midImageCheck.getErrorImageResout();
		auto rightList = rightImageCheck.getErrorImageResout();
		midList.insert(rightList.begin(), rightList.end());
		
		if (midList.empty() == false)
		{
			auto iter = midList.end();
			while (true)
			{
				--iter;
				rescanShooting(scanPath, x, y, *iter);
				
				if (iter == midList.begin())
					break;
			}
		}
	}
}

void AsyncDeviceControl::lightRunningCircle(int x, int y, int z)
{
	for (const auto& lightIndex : m_lightList)
	{
		if (!m_isBurnAndLaping)
			break;

		auto index = lightIndex;

		int addressIndex = index / 16 * 8 + index % 8 + 1;
		int lightIndex = (index + 1) % 16 / 8;

		bool rlt = false;
		if (x >= 0)
		{
			rlt = m_devControl->controlLight(index, true, true);
			if (rlt)
				emit lightStateChanged(index, true);

			if (!m_isOnlyCtrlLight)
			{
				if (waitTime(x, m_isBurnAndLaping) == false)
					break;

				m_devControl->controlCamera();
			}
		}
		else
		{
			if (!m_isOnlyCtrlLight)
			{
				m_devControl->controlCamera();

				if (waitTime(qAbs(x), m_isBurnAndLaping) == false)
					break;
			}

			rlt = m_devControl->controlLight(index, true, true);
			if (rlt)
				emit lightStateChanged(index, true);
		}

		if (waitTime(y, m_isBurnAndLaping) == false)
			break;

		rlt = m_devControl->controlLight(index, false, true);
		if (rlt)
		{
			emit lightStateChanged(index, false);
		}

		if (waitTime(z, m_isBurnAndLaping) == false)
			break;

		setLoopIndex(m_loopIndex + 1);
	}
}

void AsyncDeviceControl::grayBoardTestShooting(int x, int y, int z, bool endShootOne)
{
	for (auto i = 0; i < 11; ++i)
	{
		if (!m_isBurnAndLaping)
			break;

		for (const auto& lightIndex : m_lightList)
		{
			if (!m_isBurnAndLaping)
				break;

			auto index = lightIndex;

			int addressIndex = index / 16 * 8 + index % 8 + 1;
			int lightIndex = (index + 1) % 16 / 8;

			bool rlt = false;
			if (x >= 0)
			{
				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);

				if (!m_isOnlyCtrlLight)
				{
					if (waitTime(x, m_isBurnAndLaping) == false)
						break;

					m_devControl->controlCamera();
				}
			}
			else
			{
				if (!m_isOnlyCtrlLight)
				{
					m_devControl->controlCamera();

					if (waitTime(qAbs(x), m_isBurnAndLaping) == false)
						break;
				}

				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);
			}

			if (waitTime(y, m_isBurnAndLaping) == false)
				break;

			rlt = m_devControl->controlLight(index, false, true);
			if (rlt)
			{
				emit lightStateChanged(index, false);
			}

			if (waitTime(z, m_isBurnAndLaping) == false)
				break;

			setLoopIndex(m_loopIndex + 1);
		}

		m_devControl->controlCamera();

		waitTime(120000, m_isBurnAndLaping);
	}

	if (endShootOne)
	{
		m_devControl->controlCamera();
	}
}


void AsyncDeviceControl::rescanShooting(const QString& scanSavePath, int x, int y, int index)
{
	int rotationTimes = 180 / m_rotationAngle;
	int posIndex = index / m_lightList.size();
	int lightIndex = (index - 1) % m_lightList.size();

	auto lightId = m_lightList[lightIndex];

	auto midFilePath = QString("%1/%2/%2_%3.raf").arg(scanSavePath).arg(LEFT_CAMERA_NAME).arg(index);
	auto rightFilePath = QString("%1/%2/%2_%3.raf").arg(scanSavePath).arg(RIGHT_CAMERA_NAME).arg(index);

	bool rlt = false;
	while (m_isBurnAndLaping)
	{
		qCritical() << "rescan shooting index = " << index;
		QFile::remove(midFilePath);
		QFile::remove(rightFilePath);

		if (index < 96)
		{
			if (m_isBurnAndLaping && posIndex < (rotationTimes - 1))
			{
				float angle = 0;
				float rotationAngle = posIndex * m_rotationAngle;
				auto rlt = m_devControl->setEMRotationAngle(rotationAngle);
				do {
					if (rlt)
					{
						m_devControl->getEMRotationAngle(angle);
					}

				} while (std::abs(angle - rotationAngle) > 2 && m_isBurnAndLaping);

				waitTime(EM_MOVE_DELAY, m_isBurnAndLaping);
			}

			if (x >= 0)
			{
				rlt = m_devControl->controlLight(lightId, true, true);
				if (rlt)
					emit lightStateChanged(lightId, true);

				if (!m_isOnlyCtrlLight)
				{
					QThread::msleep(x);

					m_devControl->controlCamera();
				}
			}
			else
			{
				if (!m_isOnlyCtrlLight)
				{
					m_devControl->controlCamera();

					QThread::msleep(qAbs(x));
				}

				rlt = m_devControl->controlLight(index, true, true);
				if (rlt)
					emit lightStateChanged(index, true);
			}

			QThread::msleep(qAbs(y));

			rlt = m_devControl->controlLight(index, false, true);
			if (rlt)
			{
				emit lightStateChanged(index, false);
			}
		}
		else
		{
			ctrlStageLight(true);
			waitTime(100, m_isBurnAndLaping);
			m_devControl->controlCamera();
			waitTime(y, m_isBurnAndLaping);
			ctrlStageLight(false);
		}

		QLockFile midLockFile(midFilePath + "_lock");
		QLockFile rightLockFile(rightFilePath + "_lock");
		auto midRet = 0;
		auto rightRet = 0;
		while (m_isBurnAndLaping && (midRet == 0 || rightRet == 0) )
		{
			if (midRet == 0 && QFile::exists(midFilePath) && midLockFile.tryLock())
			{
				midLockFile.unlock();
				midRet = ImageCheckThread::testRawFileIsOk(midFilePath);
			}

			if (rightRet == 0 && QFile::exists(rightFilePath) && rightLockFile.tryLock())
			{
				rightLockFile.unlock();
				rightRet = ImageCheckThread::testRawFileIsOk(rightFilePath);
			}
		}

		if (getLeftCamera() == nullptr || getRightCamera() == nullptr ||
			(midRet == 1 && rightRet == 1))
		{
			break;
		}
	}
}

void AsyncDeviceControl::onCameraConnected(ICameraHolderPtr camera)
{
	auto devName = camera->getDeviceName();
    if (devName.isEmpty() || (devName != LEFT_CAMERA_NAME && devName != RIGHT_CAMERA_NAME))
	{
		devName = LEFT_CAMERA_NAME;
	}

	if (devName == LEFT_CAMERA_NAME)
	{
		auto dev = CameraController::instance()->findCameraDeviceFromName(devName);
		if (dev && dev != camera)
		{
			devName = RIGHT_CAMERA_NAME;
		}
	}

	if (devName == RIGHT_CAMERA_NAME)
	{
		auto dev = CameraController::instance()->findCameraDeviceFromName(devName);
		if (dev && dev != camera)
		{
			devName = camera->getDeviceName();
		}
	}

	if (devName != camera->getDeviceName())
	{
		camera->setDeviceName(devName);
	}

	if (devName == LEFT_CAMERA_NAME)
	{
		emit leftCameraChanged();
	}
	else if (devName == RIGHT_CAMERA_NAME)
	{
		emit rightCameraChanged();
	}

	auto lensName = camera->getLensName();
	auto lensId = camera->getLensID();
	//qInfo() << devName << lensName << lensId;

	auto func = [=]() {
		CameraConfig config;
		auto defaultCameraPara = QApplication::applicationDirPath() + "/configs/defaultCameraPara.json";
		auto rlt = config.readFile(defaultCameraPara);

		if (rlt)
		{
			rlt = config.restore(camera.get());
		}

		if (!rlt)
		{
			qCritical() << tr("Restore %1 camera para error.").arg(devName);
		}
	};
	m_comTaskThread->pushTaskFunc(func);

	emit updatePropertyInfo();
	emit hasCameraChanged();
}

void AsyncDeviceControl::onCameraDisconnected(ICameraHolderPtr camera)
{
	auto devName = camera->getDeviceName();
	if (devName == LEFT_CAMERA_NAME)
	{
		emit leftCameraChanged();
	}
	else if (devName == RIGHT_CAMERA_NAME)
	{
		emit rightCameraChanged();
	}

	emit hasCameraChanged();
}

