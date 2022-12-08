#pragma once

#include "CameraProperty.h"
#include "UltimateCommon_global.h"
#include <QObject>
#include <QVariant>


class ICameraHolder;

#if 0
template<typename T>
class ItemValue
{
public:
	bool isValided{ false };
	T value{ 0 };

	void operator=(T v) {
		isValided = true;
		value = v;
	}
};

using ItemValueInt = ItemValue<int32_t>;
using ItemValueIFloat = ItemValue<float>;
#endif

/*!
 * \class CameraConfig
 *
 * \brief 相机参数类
 */
class CameraConfig : public QObject
{
	Q_OBJECT

public:
	CameraConfig();
	//CameraConfig(const CameraConfig& config);
	~CameraConfig();

	QString getFileName() const;

	QString getConfigText() const;

	bool backup(ICameraHolder* getCameraHolder);

	bool restore(ICameraHolder* getCameraHolder);

	const QString& getFilePath() const { return m_filePath; }

#if 1
	void operator = (const CameraConfig& config);
	bool operator == (const CameraConfig& config) const;
	bool operator != (const CameraConfig& config) const;
#endif

	bool saveFile(const QString& filePath);
	bool readFile(const QString& filePath);

	CameraBrand getBrand() const {
		return m_camBrand;
	}

	bool isValid() const { 
		return m_camBrand != CameraBrand::CC_BRAND_UNKOWN && m_camBrand != CameraBrand::CC_BRAND_ALL; 
	}

protected:
	QString m_filePath;
	CameraBrand m_camBrand;
	QVariant m_aperture;
	QVariant m_iso;
	QVariant m_shutterSpeed;
	QVariant m_shootMode;
	QVariant m_focusMode;
	QVariant m_autoFocusMode;
	QVariant m_focusPoints;
	//QVariant m_focusArea;
	QVariant m_autoFocusIlluminator;
	QVariant m_faceDetectionMode;
	QVariant m_eyeDetectionMode;
	QVariant m_exposureProgramMode;
	QVariant m_exposureBias;
	QVariant m_feLock;
	QVariant m_mfAssistMode;
	QVariant m_afcPriorityMode;
	QVariant m_afsPriorityMode;
	QVariant m_whiteBalance;
	QVariant m_wbt;
	QVariant m_zoomValue;
	QVariant m_imageSize;
	QVariant m_imageQuality;
	QVariant m_imageZoom;
	QVariant m_rawFileCompressionType;
	QVariant m_rawOutputDepth;
	QVariant m_filmSimulationMode;
	QVariant m_grainEffect;
	QVariant m_shadowing;
	QVariant m_colorChromeBlue;
	QVariant m_smoothSkinEffect;
	QVariant m_dynamicRange;
	QVariant m_wideDynamicRange;
	QVariant m_hightlightTone;
	QVariant m_shadowTone;
	QVariant m_colorTemperature;
	QVariant m_colorMode;
	QVariant m_sharpness;
	QVariant m_noiseReduction;
	QVariant m_colorSpace;
	QVariant m_meteringMode;
	QVariant m_mediaRecord;
	QVariant m_captureDelay;
};
DECLARE_STD_PTR(CameraConfig);
Q_DECLARE_METATYPE(CameraConfigPtr);
