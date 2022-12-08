#include "CameraConfig.h"
#include "CameraController.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>


#define PARA_IS_COMPARE(para) \
	if (para != config.para)\
		return false;


CameraConfig::CameraConfig() 
{
}

#if 0
CameraConfig::CameraConfig(const CameraConfig& config)
	: CameraConfig()
{
	*this = config;
}
#endif

CameraConfig::~CameraConfig()
{
}

QString CameraConfig::getFileName() const
{
	QFileInfo fileInfo(m_filePath);

	return fileInfo.baseName();
}

QString CameraConfig::getConfigText() const
{
	return QString();
}

bool CameraConfig::backup(ICameraHolder* getCameraHolder)
{
	if (!getCameraHolder)
	{
		return false;
	}

	m_camBrand = getCameraHolder->getCameraType();
	m_aperture = (int32_t)getCameraHolder->getAperture();
	m_iso = (int32_t)getCameraHolder->getIso();
	m_shutterSpeed = (int32_t)getCameraHolder->getShutterSpeed();
	m_shootMode = (int32_t)getCameraHolder->getShootMode();
	m_focusMode = (int32_t)getCameraHolder->getFocusMode();
	m_autoFocusMode = (int32_t)getCameraHolder->getAutoFocusMode();
	m_focusPoints = (int32_t)getCameraHolder->getFocusPoints();

	bool rlt = false;
	//m_focusArea.clear();
	//float x, y, w, h;
	// rlt = getCameraHolder->getAFAreaPos(x, y, w, h);
	//if (rlt)
	//{
	//	m_focusArea = QPointF(x, y);
	//}

	m_autoFocusIlluminator = getCameraHolder->getAutoFocusIlluminator();
	m_faceDetectionMode = getCameraHolder->getFaceDetectionMode();
	m_eyeDetectionMode = getCameraHolder->getEyeDetectionMode();
	m_exposureProgramMode = getCameraHolder->getExposureProgramMode();
	m_exposureBias = getCameraHolder->getExposureBias();
	m_feLock = getCameraHolder->getFELock();
	m_mfAssistMode = getCameraHolder->getMFAssistMode();
	int value = 0;
	m_afcPriorityMode.clear();
	if (getCameraHolder->getAFCPriorityMode(value))
	{
		m_afcPriorityMode = value;
	}

	m_afsPriorityMode.clear();
	if (getCameraHolder->getAFSPriorityMode(value))
	{
		m_afsPriorityMode = value;
	}

	m_whiteBalance = getCameraHolder->getWhiteBalance();

	int r, b;
	m_wbt.clear();
	rlt = getCameraHolder->getWhiteBalanceTune(r, b);
	if (rlt)
	{
		m_wbt = QPoint(r, b);
	}

	m_zoomValue = getCameraHolder->getZoomValue();
	m_imageSize = getCameraHolder->getImageSize();
	m_imageQuality = (int32_t)getCameraHolder->getImageQuality();


	m_imageZoom.clear();
	if (getCameraHolder->getImageZoom(value))
	{
		m_imageZoom = value;
	}

	m_rawFileCompressionType = getCameraHolder->getRAWFileCompressionType();
	m_rawOutputDepth = getCameraHolder->getRAWOutputDepth();
	m_filmSimulationMode = getCameraHolder->getFilmSimulationMode();
	m_grainEffect = getCameraHolder->getGrainEffect();
	m_shadowing = getCameraHolder->getShadowing();
	m_colorChromeBlue = getCameraHolder->getColorChromeBlue();
	m_smoothSkinEffect = getCameraHolder->getSmoothSkinEffect();
	m_dynamicRange = getCameraHolder->getDynamicRange();
	m_wideDynamicRange = getCameraHolder->getWideDynamicRange();
	m_hightlightTone = getCameraHolder->getHightlightTone();
	m_shadowTone = getCameraHolder->getShadowTone();
	m_colorTemperature = getCameraHolder->getColorTemperature();
	m_colorMode = getCameraHolder->getColorMode();
	m_sharpness = getCameraHolder->getSharpness();
	m_noiseReduction = getCameraHolder->getNoiseReduction();
	m_colorSpace = getCameraHolder->getColorSpace();
	m_meteringMode = getCameraHolder->getMeteringMode();
	m_mediaRecord = getCameraHolder->getMediaRecord();
	m_captureDelay = getCameraHolder->getCaptureDelay();

	return true;
}

bool CameraConfig::restore(ICameraHolder* getCameraHolder)
{
	if (!getCameraHolder)
	{
		return false;
	}

	if (getCameraHolder->getCameraType() != m_camBrand)
	{
		return false;
	}

	if (m_aperture.isValid())
	{
		getCameraHolder->setAperture(m_aperture.toInt());
	}

	if (m_iso.isValid())
	{
		getCameraHolder->setIso(m_iso.toInt());
	}

	if (m_shutterSpeed.isValid())
	{
		getCameraHolder->setShutterSpeed(m_shutterSpeed.toInt());
	}

	if (m_shootMode.isValid())
	{
		getCameraHolder->setShootMode(m_shootMode.toInt());
	}

	if (m_focusMode.isValid())
	{
		getCameraHolder->setFocusMode(m_focusMode.toInt());
	}

	if (m_autoFocusMode.isValid())
	{
		getCameraHolder->setAutoFocusMode(m_autoFocusMode.toInt());
	}

	if (m_focusPoints.isValid())
	{
		getCameraHolder->setFocusPoints(m_focusPoints.toInt());
	}

	//if (m_focusArea.isValid())
	//{
	//	auto point = m_focusArea.toPointF();
	//	getCameraHolder->setAFAreaPos(point.x(), point.y());
	//}

	if (m_autoFocusIlluminator.isValid())
	{
		getCameraHolder->setAutoFocusIlluminator(m_autoFocusIlluminator.toInt());
	}

	if (m_faceDetectionMode.isValid())
	{
		getCameraHolder->setFaceDetectionMode(m_faceDetectionMode.toInt());
	}

	if (m_eyeDetectionMode.isValid())
	{
		getCameraHolder->setEyeDetectionMode(m_eyeDetectionMode.toInt());
	}

	if (m_exposureProgramMode.isValid())
	{
		getCameraHolder->setExposureProgramMode(m_exposureProgramMode.toInt());
	}

	if (m_exposureBias.isValid())
	{
		getCameraHolder->setExposureBias(m_exposureBias.toInt());
	}

	if (m_feLock.isValid())
	{
		getCameraHolder->setFELock(m_feLock.toInt());
	}

	if (m_mfAssistMode.isValid())
	{
		getCameraHolder->setMFAssistMode(m_mfAssistMode.toInt());
	}

	if (m_afcPriorityMode.isValid())
	{
		getCameraHolder->setAFCPriorityMode(m_afcPriorityMode.toInt());
	}

	if (m_afsPriorityMode.isValid())
	{
		getCameraHolder->setAFSPriorityMode(m_afsPriorityMode.toInt());
	}

	if (m_whiteBalance.isValid())
	{
		getCameraHolder->setWhiteBalance(m_whiteBalance.toInt());
	}

	if (m_wbt.isValid())
	{
		auto wbt = m_wbt.toPoint();
		getCameraHolder->setWhiteBalanceTune(wbt.x(), wbt.y());
	}

	if (m_zoomValue.isValid())
	{
		getCameraHolder->setZoomValue(m_zoomValue.toInt());
	}

	if (m_imageSize.isValid())
	{
		getCameraHolder->setImageSize(m_imageSize.toInt());
	}

	if (m_imageQuality.isValid())
	{
		getCameraHolder->setImageQuality(m_imageQuality.toInt());
	}

	if (m_imageZoom.isValid())
	{
		getCameraHolder->setImageZoom(m_imageZoom.toInt());
	}

	if (m_rawFileCompressionType.isValid())
	{
		getCameraHolder->setRAWFileCompressionType(m_rawFileCompressionType.toInt());
	}

	if (m_rawOutputDepth.isValid())
	{
		getCameraHolder->setRAWOutputDepth(m_rawOutputDepth.toInt());
	}

	if (m_filmSimulationMode.isValid())
	{
		getCameraHolder->setFilmSimulationMode(m_filmSimulationMode.toInt());
	}

	if (m_grainEffect.isValid())
	{
		getCameraHolder->setGrainEffect(m_grainEffect.toInt());
	}

	if (m_shadowing.isValid())
	{
		getCameraHolder->setShadowing(m_shadowing.toInt());
	}

	if (m_colorChromeBlue.isValid())
	{
		getCameraHolder->setColorChromeBlue(m_colorChromeBlue.toInt());
	}

	if (m_smoothSkinEffect.isValid())
	{
		getCameraHolder->setSmoothSkinEffect(m_smoothSkinEffect.toInt());
	}

	if (m_dynamicRange.isValid())
	{
		getCameraHolder->setDynamicRange(m_dynamicRange.toInt());
	}

	if (m_wideDynamicRange.isValid())
	{
		getCameraHolder->setWideDynamicRange(m_wideDynamicRange.toInt());
	}

	if (m_hightlightTone.isValid())
	{
		getCameraHolder->setHightlightTone(m_hightlightTone.toInt());
	}

	if (m_shadowTone.isValid())
	{
		getCameraHolder->setShadowTone(m_shadowTone.toInt());
	}

	if (m_colorTemperature.isValid())
	{
		getCameraHolder->setColorTemperature(m_colorTemperature.toInt());
	}

	if (m_colorMode.isValid())
	{
		getCameraHolder->setColorMode(m_colorMode.toInt());
	}

	if (m_sharpness.isValid())
	{
		getCameraHolder->setSharpness(m_sharpness.toInt());
	}

	if (m_noiseReduction.isValid())
	{
		getCameraHolder->setNoiseReduction(m_noiseReduction.toInt());
	}

	if (m_colorSpace.isValid())
	{
		getCameraHolder->setColorSpace(m_colorSpace.toInt());
	}

	if (m_meteringMode.isValid())
	{
		getCameraHolder->setMeteringMode(m_meteringMode.toInt());
	}

	if (m_mediaRecord.isValid())
	{
		getCameraHolder->setMediaRecord(m_mediaRecord.toInt());
	}

	if (m_captureDelay.isValid())
	{
		getCameraHolder->setCaptureDelay(m_captureDelay.toInt());
	}

	return true;
}

#if 1
void CameraConfig::operator=(const CameraConfig& config)
{
	m_filePath = config.m_filePath;
	m_camBrand = config.m_camBrand;
	m_aperture = config.m_aperture;
	m_iso = config.m_iso;
	m_shutterSpeed = config.m_shutterSpeed;
	m_shootMode = config.m_shootMode;
	m_focusMode = config.m_focusMode;
	m_autoFocusMode = config.m_autoFocusMode;
	m_focusPoints = config.m_focusPoints;
	//m_focusArea = config.m_focusArea;
	m_autoFocusIlluminator = config.m_autoFocusIlluminator;
	m_faceDetectionMode = config.m_faceDetectionMode;
	m_eyeDetectionMode = config.m_eyeDetectionMode;
	m_exposureProgramMode = config.m_exposureProgramMode;
	m_exposureBias = config.m_exposureBias;
	m_feLock = config.m_feLock;
	m_mfAssistMode = config.m_mfAssistMode;
	m_afcPriorityMode = config.m_afcPriorityMode;
	m_afsPriorityMode = config.m_afsPriorityMode;
	m_whiteBalance = config.m_whiteBalance;
	m_wbt = config.m_wbt;
	m_zoomValue = config.m_zoomValue;
	m_imageSize = config.m_imageSize;
	m_imageQuality = config.m_imageQuality;
	m_imageZoom = config.m_imageZoom;
	m_rawFileCompressionType = config.m_rawFileCompressionType;
	m_rawOutputDepth = config.m_rawOutputDepth;
	m_filmSimulationMode = config.m_filmSimulationMode;
	m_grainEffect = config.m_grainEffect;
	m_shadowing = config.m_shadowing;
	m_colorChromeBlue = config.m_colorChromeBlue;
	m_smoothSkinEffect = config.m_smoothSkinEffect;
	m_dynamicRange = config.m_dynamicRange;
	m_wideDynamicRange = config.m_wideDynamicRange;
	m_hightlightTone = config.m_hightlightTone;
	m_shadowTone = config.m_shadowTone;
	m_colorTemperature = config.m_colorTemperature;
	m_colorMode = config.m_colorMode;
	m_sharpness = config.m_sharpness;
	m_noiseReduction = config.m_noiseReduction;
	m_colorSpace = config.m_colorSpace;
	m_meteringMode = config.m_meteringMode;
	m_mediaRecord = config.m_mediaRecord;
	m_captureDelay = config.m_captureDelay;
}

bool CameraConfig::operator==(const CameraConfig& config) const
{
	PARA_IS_COMPARE(m_camBrand);
	PARA_IS_COMPARE(m_aperture);
	PARA_IS_COMPARE(m_iso);
	PARA_IS_COMPARE(m_shutterSpeed);
	PARA_IS_COMPARE(m_shootMode);
	PARA_IS_COMPARE(m_focusMode);
	PARA_IS_COMPARE(m_autoFocusMode);
	PARA_IS_COMPARE(m_focusPoints);
	//PARA_IS_COMPARE(m_focusArea);
	PARA_IS_COMPARE(m_autoFocusIlluminator);
	PARA_IS_COMPARE(m_faceDetectionMode);
	PARA_IS_COMPARE(m_eyeDetectionMode);
	PARA_IS_COMPARE(m_exposureProgramMode);
	PARA_IS_COMPARE(m_exposureBias);
	PARA_IS_COMPARE(m_feLock);
	PARA_IS_COMPARE(m_mfAssistMode);
	PARA_IS_COMPARE(m_afcPriorityMode);
	PARA_IS_COMPARE(m_afsPriorityMode);
	PARA_IS_COMPARE(m_whiteBalance);
	PARA_IS_COMPARE(m_wbt);
	PARA_IS_COMPARE(m_zoomValue);
	PARA_IS_COMPARE(m_imageSize);
	PARA_IS_COMPARE(m_imageQuality);
	PARA_IS_COMPARE(m_imageZoom);
	PARA_IS_COMPARE(m_rawFileCompressionType);
	PARA_IS_COMPARE(m_rawOutputDepth);
	PARA_IS_COMPARE(m_filmSimulationMode);
	PARA_IS_COMPARE(m_grainEffect);
	PARA_IS_COMPARE(m_shadowing);
	PARA_IS_COMPARE(m_colorChromeBlue);
	PARA_IS_COMPARE(m_smoothSkinEffect);
	PARA_IS_COMPARE(m_dynamicRange);
	PARA_IS_COMPARE(m_wideDynamicRange);
	PARA_IS_COMPARE(m_hightlightTone);
	PARA_IS_COMPARE(m_shadowTone);
	PARA_IS_COMPARE(m_colorTemperature);
	PARA_IS_COMPARE(m_colorMode);
	PARA_IS_COMPARE(m_sharpness);
	PARA_IS_COMPARE(m_noiseReduction);
	PARA_IS_COMPARE(m_colorSpace);
	PARA_IS_COMPARE(m_meteringMode);
	PARA_IS_COMPARE(m_mediaRecord);
	PARA_IS_COMPARE(m_captureDelay);

	return true;
}

bool CameraConfig::operator != (const CameraConfig& config) const
{
    auto rlt = CameraConfig::operator ==(config);
	return !rlt;
}

#endif

bool CameraConfig::saveFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly))
	{
		return false;
	}

	QJsonDocument jsonDoc;
	QJsonObject rootObj;

	rootObj["CameraBrand"] = (int32_t)m_camBrand;
	if (m_aperture.isValid())
	{
		rootObj["Aperture"] = m_aperture.toInt();
	}

	if (m_iso.isValid())
	{
		rootObj["Iso"] = m_iso.toInt();
	}

	if (m_shutterSpeed.isValid())
	{
		rootObj["ShutterSpeed"] = m_shutterSpeed.toInt();
	}

	if (m_shootMode.isValid())
	{
		rootObj["ShootMode"] = m_shootMode.toInt();
	}

	if (m_focusMode.isValid())
	{
		rootObj["FocusMode"] = m_focusMode.toInt();
	}

	if (m_autoFocusMode.isValid())
	{
		rootObj["AutoFocusMode"] = m_autoFocusMode.toInt();
	}

	if (m_focusPoints.isValid())
	{
		rootObj["FocusPoints"] = m_focusPoints.toInt();
	}

	//if (m_focusArea.isValid())
	//{
	//	auto point = m_focusArea.toPointF();
	//	QJsonObject areaPosObj;
	//	areaPosObj["x"] = point.x();
	//	areaPosObj["y"] = point.y();
	//	rootObj["AFAreaPos"] = areaPosObj;
	//}

	if (m_autoFocusIlluminator.isValid())
	{
		rootObj["AutoFocusIlluminator"] = m_autoFocusIlluminator.toInt();
	}

	if (m_faceDetectionMode.isValid())
	{
		rootObj["FaceDetectionMode"] = m_faceDetectionMode.toInt();
	}

	if (m_eyeDetectionMode.isValid())
	{
		rootObj["EyeDetectionMode"] = m_eyeDetectionMode.toInt();
	}

	if (m_exposureProgramMode.isValid())
	{
		rootObj["ExposureProgramMode"] = m_exposureProgramMode.toInt();
	}

	if (m_exposureBias.isValid())
	{
		rootObj["ExposureBias"] = m_exposureBias.toInt();
	}

	if (m_feLock.isValid())
	{
		rootObj["FELock"] = m_feLock.toInt();
	}

	if (m_mfAssistMode.isValid())
	{
		rootObj["MFAssistMode"] = m_mfAssistMode.toInt();
	}

	if (m_afcPriorityMode.isValid())
	{
		rootObj["AFCPriorityMode"] = m_afcPriorityMode.toInt();
	}

	if (m_afsPriorityMode.isValid())
	{
		rootObj["AFSPriorityMode"] = m_afsPriorityMode.toInt();
	}

	if (m_whiteBalance.isValid())
	{
		rootObj["WhiteBalance"] = m_whiteBalance.toInt();
	}

	if (m_wbt.isValid())
	{
		auto v = m_wbt.toPoint();
		QJsonObject jsonObj;
		jsonObj["r"] = v.x();
		jsonObj["b"] = v.y();

		rootObj["WhiteBalanceTune"] = jsonObj;
	}

	if (m_zoomValue.isValid())
	{
		rootObj["ZoomValue"] = m_zoomValue.toInt();
	}

	if (m_imageSize.isValid())
	{
		rootObj["ImageSize"] = m_imageSize.toInt();
	}

	if (m_imageQuality.isValid())
	{
		rootObj["ImageQuality"] = m_imageQuality.toInt();
	}

	if (m_imageZoom.isValid())
	{
		rootObj["ImageZoom"] = m_imageZoom.toInt();
	}

	if (m_rawFileCompressionType.isValid())
	{
		rootObj["RAWFileCompressionType"] = m_rawFileCompressionType.toInt();
	}

	if (m_rawOutputDepth.isValid())
	{
		rootObj["RAWOutputDepth"] = m_rawOutputDepth.toInt();
	}

	if (m_filmSimulationMode.isValid())
	{
		rootObj["FilmSimulationMode"] = m_filmSimulationMode.toInt();
	}

	if (m_grainEffect.isValid())
	{
		rootObj["GrainEffect"] = m_grainEffect.toInt();
	}

	if (m_shadowing.isValid())
	{
		rootObj["Shadowing"] = m_shadowing.toInt();
	}

	if (m_colorChromeBlue.isValid())
	{
		rootObj["ColorChromeBlue"] = m_colorChromeBlue.toInt();
	}

	if (m_smoothSkinEffect.isValid())
	{
		rootObj["SmoothSkinEffect"] = m_smoothSkinEffect.toInt();
	}

	if (m_dynamicRange.isValid())
	{
		rootObj["DynamicRange"] = m_dynamicRange.toInt();
	}

	if (m_wideDynamicRange.isValid())
	{
		rootObj["WideDynamicRange"] = m_wideDynamicRange.toInt();
	}

	if (m_hightlightTone.isValid())
	{
		rootObj["HightlightTone"] = m_hightlightTone.toInt();
	}

	if (m_shadowTone.isValid())
	{
		rootObj["ShadowTone"] = m_shadowTone.toInt();
	}

	if (m_colorTemperature.isValid())
	{
		rootObj["ColorTemperature"] = m_colorTemperature.toInt();
	}

	if (m_colorMode.isValid())
	{
		rootObj["ColorMode"] = m_colorMode.toInt();
	}

	if (m_sharpness.isValid())
	{
		rootObj["Sharpness"] = m_sharpness.toInt();
	}

	if (m_noiseReduction.isValid())
	{
		rootObj["NoiseReduction"] = m_noiseReduction.toInt();
	}

	if (m_colorSpace.isValid())
	{
		rootObj["ColorSpace"] = m_colorSpace.toInt();
	}

	if (m_meteringMode.isValid())
	{
		rootObj["MeteringMode"] = m_meteringMode.toInt();
	}

	if (m_mediaRecord.isValid())
	{
		rootObj["MediaRecord"] = m_mediaRecord.toInt();
	}

	if (m_captureDelay.isValid())
	{
		rootObj["CaptureDelay"] = m_captureDelay.toInt();
	}

	jsonDoc.setObject(rootObj);

	auto data = jsonDoc.toJson(QJsonDocument::Indented);
	file.write(data);
	file.flush();

	file.close();

	m_filePath = filePath;

	return true;
}

bool CameraConfig::readFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}

	auto data = file.readAll();

	file.close();

	QJsonParseError error;

	auto doc = QJsonDocument::fromJson(data, &error);

	if (error.error != QJsonParseError::NoError)
	{
		return false;
	}

	auto rootObj = doc.object();

	if (rootObj.contains("CameraBrand"))
	{
		m_camBrand = (CameraBrand)rootObj["CameraBrand"].toInt();
	}
	else
	{
		return false;
	}

	m_aperture = rootObj["Aperture"].toVariant();
	m_iso = rootObj["Iso"].toVariant();
	m_shutterSpeed = rootObj["ShutterSpeed"].toVariant();
	m_shootMode = rootObj["ShootMode"].toVariant();
	m_focusMode = rootObj["FocusMode"].toVariant();
	m_autoFocusMode = rootObj["AutoFocusMode"].toVariant();
	m_focusPoints = rootObj["FocusPoints"].toVariant();

	//auto posObj = rootObj["AFAreaPos"].toObject();
	//m_focusArea = QPointF(posObj["x"].toDouble(), posObj["y"].toDouble());

	m_autoFocusIlluminator = rootObj["AutoFocusIlluminator"].toVariant();
	m_faceDetectionMode = rootObj["FaceDetectionMode"].toVariant();
	m_eyeDetectionMode = rootObj["EyeDetectionMode"].toVariant();
	m_exposureProgramMode = rootObj["ExposureProgramMode"].toVariant();
	m_exposureBias = rootObj["ExposureBias"].toVariant();
	m_feLock = rootObj["FELock"].toVariant();
	m_mfAssistMode = rootObj["MFAssistMode"].toVariant();
	m_afcPriorityMode = rootObj["AFCPriorityMode"].toVariant();
	m_afsPriorityMode = rootObj["AFSPriorityMode"].toVariant();
	m_whiteBalance = rootObj["WhiteBalance"].toVariant();

	{
		auto wbtObj = rootObj["WhiteBalanceTune"].toObject();
		m_wbt = QPoint(wbtObj["x"].toInt(), wbtObj["y"].toInt());
	}

	m_zoomValue = rootObj["ZoomValue"].toVariant();
	m_imageSize = rootObj["ImageSize"].toVariant();
	m_imageQuality = rootObj["ImageQuality"].toVariant();
	m_imageZoom = rootObj["ImageZoom"].toVariant();
	m_rawFileCompressionType = rootObj["RAWFileCompressionType"].toVariant();
	m_rawOutputDepth = rootObj["RAWOutputDepth"].toVariant();
	m_filmSimulationMode = rootObj["FilmSimulationMode"].toVariant();
	m_grainEffect = rootObj["GrainEffect"].toVariant();
	m_shadowing = rootObj["Shadowing"].toVariant();
	m_colorChromeBlue = rootObj["ColorChromeBlue"].toVariant();
	m_smoothSkinEffect = rootObj["SmoothSkinEffect"].toVariant();
	m_dynamicRange = rootObj["DynamicRange"].toVariant();
	m_wideDynamicRange = rootObj["WideDynamicRange"].toVariant();
	m_hightlightTone = rootObj["HightlightTone"].toVariant();
	m_shadowTone = rootObj["ShadowTone"].toVariant();
	m_colorTemperature = rootObj["ColorTemperature"].toVariant();
	m_colorMode = rootObj["ColorMode"].toVariant();
	m_sharpness = rootObj["Sharpness"].toVariant();
	m_noiseReduction = rootObj["NoiseReduction"].toVariant();
	m_colorSpace = rootObj["ColorSpace"].toVariant();
	m_meteringMode = rootObj["MeteringMode"].toVariant();
	m_mediaRecord = rootObj["MediaRecord"].toVariant();
	m_captureDelay = rootObj["CaptureDelay"].toVariant();

	m_filePath = filePath;
	return true;
}
