#include "SoftConfig.h"
#include "DeviceControl.h"
#include "ICameraHolder.h"
#include "DeviceControl.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QDir>
#include <QJsonArray>



SoftConfig::SoftConfig(QObject* parent)
{
}

void SoftConfig::setTestSavePath(const QString& path)
{
	if (path == m_testSavePath)
		return;

	m_testSavePath = path;

    emit testSavePathChanged();
}

bool SoftConfig::readFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
		return false;

	auto configData = file.readAll();
	file.close();

	QJsonParseError error;

	auto jsonDoc = QJsonDocument::fromJson(configData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		return false;
	}

	auto rootObj = jsonDoc.object();

	{
		auto scanSettings = rootObj["scanSettings"].toObject();

		auto scanSavePath = scanSettings["scanSavePath"].toString();
		setScanSavePath(scanSavePath);

		auto scanSaveName = scanSettings["scanSaveName"].toString();
		setScanSaveName(scanSaveName);
	}

	{
		auto testSettingsObj = rootObj["testSettingsObj"].toObject();

		auto testSavePath = testSettingsObj["testSavePath"].toString();
		setTestSavePath(testSavePath);
	}

	{
		auto shotSettingsObj = rootObj["shotSettingsObj"].toObject();
		if (!shotSettingsObj.isEmpty())
		{
			auto x = shotSettingsObj["x"].toInt(0);
			setX(x);

			auto y = shotSettingsObj["y"].toInt(200);
			setY(y);

			auto z = shotSettingsObj["z"].toInt(1000);
			setZ(z);

            auto endShootOne = shotSettingsObj["endShootOne"].toBool(false);
            setEndShootOne(endShootOne);

			auto focusDisIndex = shotSettingsObj["focusDisIndex"].toInt(0);
			setFocusDisIndex(focusDisIndex);

			auto scanModel = (ScanMode)shotSettingsObj["scanMode"].toInt(ScanMode::NormalMode);
			setScanMode(scanModel);

			auto stageElecValue = shotSettingsObj["stageLightElec"].toDouble(0.f);
			setStageLightElectricity(stageElecValue);
		}
	}

	// 串口索引
	{
		auto portObj = rootObj["SerialPortIndex"].toObject();
		if (!portObj.isEmpty())
		{
			m_lightEmIndex = portObj["lightEmIndex"].toInt(0);
			m_bottomLightEmIndex = portObj["bottomLightEmIndex"].toInt(1);
			m_mainMacIndex = portObj["mainMacIndex"].toInt(2);
		}
	}

	// 发布功能配置
	{
		auto uiShowObj = rootObj["UiShowState"].toObject();
		auto inSideTestUi = uiShowObj["inSideTestUi"].toBool(false);
		setShowInsideTestUi(inSideTestUi);
		m_showFocusValue = uiShowObj["showFocusValue"].toBool(false);
		m_grayBoardTest = uiShowObj["grayBoardTest"].toBool(false);
		m_isRelease = uiShowObj["isRelease"].toBool(true);
		m_isOutDebugInfo = uiShowObj["isOutDebugInfo"].toBool(false);
		m_isAutoAnalysis = uiShowObj["isAutoAnalysis"].toBool(false);
		m_analysisResolution = uiShowObj["analysisResolution"].toInt(1);
	}

	auto shootSet = rootObj["shootingSettings"].toObject();

	m_configSavePath = filePath;

	return true;
}

bool SoftConfig::writeFile(const QString& filePath) const
{
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly))
		return false;

	QJsonDocument jsonDoc;
	QJsonObject rootObj;

	{
		QJsonObject scanSettingsObj;

		scanSettingsObj["scanSavePath"] = m_scanSavePath;
		scanSettingsObj["scanSaveName"] = m_scanSaveName;

		rootObj["scanSettings"] = scanSettingsObj;
	}

	{
		QJsonObject testSettingsObj;

		testSettingsObj["testSavePath"] = m_testSavePath;

		rootObj["testSettingsObj"] = testSettingsObj;
	}

	{
		QJsonObject shotSettingsObj;

		shotSettingsObj["x"] = (int)m_x;
		shotSettingsObj["y"] = (int)m_y;
		shotSettingsObj["z"] = (int)m_z;
		shotSettingsObj["endShootOne"] = m_endShootOne;
		shotSettingsObj["focusDisIndex"] = m_focusDisIndex;
		shotSettingsObj["scanMode"] = (int)m_scanMode;
		shotSettingsObj["stageLightElec"] = m_stageLightElec;

		rootObj["shotSettingsObj"] = shotSettingsObj;
	}

	// 串口索引
	{
		QJsonObject portObj;
		portObj["lightEmIndex"] = m_lightEmIndex;
		portObj["bottomLightEmIndex"] = m_bottomLightEmIndex;
		portObj["mainMacIndex"] = m_mainMacIndex;
		
		rootObj["SerialPortIndex"] = portObj;
	}

	// 界面显示状态
	{
		QJsonObject uiShowObj;
		uiShowObj["inSideTestUi"] = m_insideTestUi;
		uiShowObj["showFocusValue"] = m_showFocusValue;
		uiShowObj["grayBoardTest"] = m_grayBoardTest;
		uiShowObj["isRelease"] = m_isRelease;
		uiShowObj["isOutDebugInfo"] = m_isOutDebugInfo;
		uiShowObj["isAutoAnalysis"] = m_isAutoAnalysis;
		uiShowObj["analysisResolution"] = m_analysisResolution;
		
		rootObj["UiShowState"] = uiShowObj;
	}
	
	jsonDoc.setObject(rootObj);

	auto configData = jsonDoc.toJson(QJsonDocument::Indented);
	file.write(configData);
	file.flush();
	file.close();

	m_configSavePath = filePath;

	return true;
}

bool SoftConfig::readFile()
{
	return readFile(m_configSavePath);
}

bool SoftConfig::writeFile() const
{
	return writeFile(m_configSavePath);
}

void SoftConfig::setConfigFilePath(const QString& filePath)
{
	if (m_configSavePath == filePath)
		return;

    QFileInfo fileInfo(filePath);
    auto dir = fileInfo.absoluteDir();
    if(!dir.exists())
    {
        auto path = fileInfo.absolutePath();
        dir.mkpath(path);
    }

	m_configSavePath = filePath;
}

void SoftConfig::setScanSavePath(const QString& path) 
{
	if (m_scanSavePath == path)
		return;

	m_scanSavePath = path; 
	emit scanSavePathChanged();
}

void SoftConfig::setScanSaveName(const QString& path) 
{
	if (m_scanSaveName == path)
		return;

	m_scanSaveName = path; 
	emit scanSaveNameChanged();
}

void SoftConfig::setX(qint32 value)
{ 
	if (m_x == value)
		return;

	m_x = value; 
	emit xChanged();
}

void SoftConfig::setY(qint32 value)
{ 
	if (m_y == value)
		return;

	m_y = value; 
	emit yChanged();
}

void SoftConfig::setZ(qint32 value)
{ 
	if (m_z == value)
		return;

	m_z = value; 
	emit zChanged();
}

void SoftConfig::setEndShootOne(bool value)
{
    if (m_endShootOne == value)
        return;

    m_endShootOne = value;
    emit endShootOneChanged();
}

void SoftConfig::setFocusDisIndex(int value)
{
	if (m_focusDisIndex == value)
		return;

	m_focusDisIndex = value;
	emit focusDisIndexChanged();
}

// 扫描模式

void SoftConfig::setScanMode(ScanMode mode) {
	if (m_scanMode != mode)
	{
		m_scanMode = mode;
		emit scanModeChanged();
	}
}

void SoftConfig::setStageLightElectricity(float value)
{
	if (m_stageLightElec != value)
	{
		m_stageLightElec = value;
	}
}

void SoftConfig::setShowInsideTestUi(bool value)
{
	if (m_insideTestUi != value)
	{
		m_insideTestUi = value;
		emit showInsideTestUiChanged();
	}
}

CameraFocusDistance::CameraFocusDistance(QObject* parent)
{
	// 50镜头
	m_focDisInfo["50"] = {
		{ 3620, 3644, 3650, 3659 },
		{ 3461, 3480, 3508, 3515 }

	};

	// 120镜头
	m_focDisInfo["120"] = {
		{ -13, 62, 104, 149 },
		{ -242, -172, -142, -96 }
	};
}

bool CameraFocusDistance::readFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
		return false;

	auto configData = file.readAll();
	file.close();

	QJsonParseError error;

	auto jsonDoc = QJsonDocument::fromJson(configData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		return false;
	}

	m_focDisInfo.clear();
	auto rootObj = jsonDoc.object();

	for (auto iter = rootObj.begin(); iter != rootObj.end(); ++iter)
	{
		QString key = iter.key();
		auto lensObj = iter.value().toObject();
		
		FocusDistanceList focusDis;
		{
			QJsonArray midArray = lensObj[LEFT_CAMERA_NAME].toArray();

			for (const auto& obj : midArray)
			{
				focusDis.midCamera.push_back(obj.toInt());
			}
		}

		{
			QJsonArray rightArray = lensObj[RIGHT_CAMERA_NAME].toArray();

			for (const auto& obj : rightArray)
			{
				focusDis.rightCamera.push_back(obj.toInt());
			}
		}

		m_focDisInfo[key] = focusDis;
	}
	
	return true;
}

bool CameraFocusDistance::writeFile(const QString& filePath) const
{
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly))
		return false;

	QJsonDocument jsonDoc;
	QJsonObject rootObj;

	for (auto iter = m_focDisInfo.begin(); iter != m_focDisInfo.end(); ++iter)
	{
		QJsonObject lensObj;

		{
			QJsonArray midArray;
			for (const auto& distance : iter->midCamera)
			{
				midArray.push_back(distance);
			}
			lensObj[LEFT_CAMERA_NAME] = midArray;
		}

		{
			QJsonArray rightArray;
			for (const auto& distance : iter->rightCamera)
			{
				rightArray.push_back(distance);
			}
			lensObj[RIGHT_CAMERA_NAME] = rightArray;
		}
		
		rootObj[iter.key()] = lensObj;
	}

	jsonDoc.setObject(rootObj);

	auto configData = jsonDoc.toJson(QJsonDocument::Indented);
	file.write(configData);
	file.flush();
	file.close();

	m_configSavePath = filePath;

	return true;
}

bool CameraFocusDistance::readFile()
{
	return readFile(m_configSavePath);
}

bool CameraFocusDistance::writeFile() const
{
	return writeFile(m_configSavePath);
}

const FocusDistanceList& CameraFocusDistance::getFocusDistanceList(const QString& lensModel)
{
	return m_focDisInfo[lensModel];
}

bool ScanCameraConfig::backup(ICameraHolder* camHandle)
{
	if (!camHandle)
	{
		return false;
	}

	m_lensModel = camHandle->getLensModel();
	m_camBrand = camHandle->getCameraType();
	m_aperture = (int32_t)camHandle->getAperture();
	m_iso = (int32_t)camHandle->getIso();
	m_shutterSpeed = (int32_t)camHandle->getShutterSpeed();
	m_colorTemperature = camHandle->getColorTemperature();
	m_softVersion = AsyncDeviceControl::instance()->getVersionInfo();

	return true;
}

bool ScanCameraConfig::saveFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly))
	{
		return false;
	}

	QJsonDocument jsonDoc;
	QJsonObject rootObj;

	rootObj["CameraBrand"] = (int32_t)m_camBrand;
	rootObj["LensModel"] = m_lensModel;
	rootObj["FocusDisIndex"] = m_focusDisIndex;
	rootObj["Aperture"] = m_aperture;
	rootObj["Iso"] = m_iso;
	rootObj["ShutterSpeed"] = m_shutterSpeed;
	rootObj["ColorTemperature"] = m_colorTemperature;
	rootObj["SoftVersion"] = m_softVersion;

	jsonDoc.setObject(rootObj);

	auto data = jsonDoc.toJson(QJsonDocument::Indented);
	file.write(data);
	file.flush();

	file.close();

	return true;
}

bool ScanCameraConfig::readFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
		return false;

	auto data = file.readAll();
	file.close();

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(data, &error);

	if (error.error != QJsonParseError::NoError)
		return false;

	auto rootObj = doc.object();

	if (rootObj.contains("CameraBrand"))
		m_camBrand = (CameraBrand)rootObj["CameraBrand"].toInt();
	else
		return false;

	m_aperture = rootObj["Aperture"].toInt();
	m_focusDisIndex = rootObj["FocusDisIndex"].toInt();
	m_iso = rootObj["Iso"].toInt();
	m_shutterSpeed = rootObj["ShutterSpeed"].toInt();
	m_colorTemperature = rootObj["ColorTemperature"].toInt();
	m_lensModel = rootObj["LensModel"].toString();
	m_softVersion = rootObj["SoftVersion"].toString();

	return true;
}

void ScanCameraConfig::operator=(const ScanCameraConfig& config)
{
	m_camBrand = config.m_camBrand;
	m_lensModel = config.m_lensModel;
	m_focusDisIndex = config.m_focusDisIndex;
	m_aperture = config.m_aperture;
	m_iso = config.m_iso;
	m_shutterSpeed = config.m_shutterSpeed;
	m_colorTemperature = config.m_colorTemperature;
	m_softVersion = config.m_softVersion;
}

bool ScanCameraConfig::operator==(const ScanCameraConfig& config) const
{
	if (m_camBrand != config.m_camBrand || 
		m_lensModel != config.m_lensModel ||
		m_focusDisIndex != config.m_focusDisIndex ||
		m_aperture != config.m_aperture ||
		m_iso != config.m_iso ||
		m_shutterSpeed != config.m_shutterSpeed ||
		m_colorTemperature != config.m_colorTemperature ||
		m_softVersion != config.m_softVersion )
		return false;

	return true;
}

bool ScanCameraConfig::operator!=(const ScanCameraConfig& config) const
{
	auto rlt = ScanCameraConfig::operator==(config);
	return !rlt;
}
