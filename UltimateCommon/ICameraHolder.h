#pragma once

#include "Common.h"
#include "Signal.h"
#include "CameraProperty.h"
#include <string>
#include <list>
#include <vector>
#include <mutex>
#include <functional>
#include <QMetaType>
#include <QObject>

#define PROPERTY_SET_GET(type, propValue, propName, propCapName ) \
const PropertyInfoEntry<type>& propName##Info() const \
{ \
return propValue;\
};\
\
bool set##propCapName(type value)\
{\
	if (propValue.current == value)\
		return true;\
		\
		propValue.current = value;\
		\
	emit propName##Changed();\
		\
	return false;\
};\
\
type get##propCapName() const\
{\
	return propValue.current;\
}


DECLARE_STD_PTR(ICameraHolder)
Q_DECLARE_METATYPE(ICameraHolderPtr)

/*!
 * \class ICameraHolder
 *
 * \brief 相机操作基类
 */
class ULTIMATECOMMON_EXPORT ICameraHolder : public QObject
{
	Q_OBJECT
protected:
	ICameraHolder() { 
		auto id = qRegisterMetaType<ICameraHolderPtr>();
		if (id == -1)
		{
			qRegisterMetaType<ICameraHolderPtr>("ICameraHolderPtr");
		}
	}

public:
	enum FocusStatus
	{
		NoFocus = 0x00,
		FocusSucess,
		FocusFail,
	};
	Q_ENUM(FocusStatus);

	enum ControlMode {
		TwoWayControl = 0x00,	// 双向控制
		PcControl,				// PC控制
		CameraControl,			// 相机控制
	};
	Q_ENUM(ControlMode);

	virtual ~ICameraHolder() {}

Q_SIGNALS:
	void statusChangedSignal(ICameraHolder*, bool);
	void propertyChangeSignal(ICameraHolder*);
	void focusStatusChanged(ICameraHolder*, FocusStatus focusState);
	void error() const;

	void liveViewImageQualityChanged();
	void apertureChanged();
	void isoChanged();
	void shutterSpeedChanged();
	void colorChromeBlueChanged();
	void colorTemperatureChanged();

public:
	virtual CameraBrand getCameraType() const = 0;

	Q_INVOKABLE virtual void setDeviceName(const QString& name) = 0;
	Q_INVOKABLE virtual const QString& getDeviceName() const = 0;

	Q_INVOKABLE virtual const QString& getDeviceId() const = 0;

	// 品牌
	Q_INVOKABLE virtual QString getBrandStr() const = 0;

	/***相机操作****/
	Q_INVOKABLE virtual bool connect() = 0;
	Q_INVOKABLE virtual bool disconnect() = 0;
	Q_INVOKABLE virtual bool isConnected() const = 0;
	Q_INVOKABLE virtual bool release() = 0;

	/***摄像操作****/
	Q_INVOKABLE virtual bool captureImage() = 0;
	Q_INVOKABLE virtual void shootOne() = 0;
	Q_INVOKABLE virtual void shutterToHalf() = 0;
	Q_INVOKABLE virtual void shutterToRelease() = 0;

	virtual void focusPress() {}
	virtual void focusRelease() {}

	Q_INVOKABLE virtual bool captureDown() = 0;
	Q_INVOKABLE virtual bool captureUp() = 0;

	virtual bool setControlMode(ControlMode mode) { return false; };
	virtual ControlMode getControlMode() { return TwoWayControl; };

	Q_INVOKABLE virtual FocusStatus getFocusStatus() const = 0;

	Q_INVOKABLE virtual void setPhotoSavePath(const QString& path) = 0;
	Q_INVOKABLE virtual const QString& getPhotoSavePath() const = 0;

	/***属性操作****/

	// 实时取景
	Q_INVOKABLE virtual bool setLiveView(bool isView) = 0;
	Q_INVOKABLE virtual bool getLiveView() const = 0;
	Q_INVOKABLE virtual uint8_t* getLiveViewBuffer(uint32_t* dataSize) = 0;

	// 实时取景图片质量
	virtual const PropertyInfoEntry<int32_t>& liveViewImageQualityInfo() const = 0;
	virtual bool setLiveViewImageQuality(int value) = 0;
	virtual int getLiveViewImageQuality() const = 0;

	// 光圈
	virtual const PropertyInfoEntry<uint16_t>& apertureInfo() const = 0;
	virtual bool setAperture(uint16_t value) = 0;
	virtual uint16_t getAperture() const = 0;

	// iso
	virtual const PropertyInfoEntry<uint32_t>& isoInfo() const = 0;
	virtual bool setIso(uint32_t value) = 0;
	virtual uint32_t getIso() const = 0;

	// 快门速度
	virtual const PropertyInfoEntry<int32_t>& shutterSpeedInfo() const = 0;
	virtual bool setShutterSpeed(int32_t value) = 0;
	virtual int32_t getShutterSpeed() const = 0;

	// 拍摄模式
	virtual const PropertyInfoEntry<int32_t>& shootModeInfo() const = 0;
	virtual bool setShootMode(int32_t mode) = 0;
	virtual int32_t getShootMode() const = 0;

	// 聚焦模式
	virtual const PropertyInfoEntry<int32_t>& focusModeInfo() const = 0;
	virtual bool setFocusMode(int32_t mode) = 0;
	virtual int32_t getFocusMode() const = 0;

	// 自动对焦模式
	virtual const PropertyInfoEntry<int32_t>& autoFocusModeInfo() const = 0;
	virtual bool setAutoFocusMode(int32_t value) { return false; }
	virtual int32_t getAutoFocusMode() const { return 0; }

	// 对焦点
	virtual const PropertyInfoEntry<int32_t>& focusPointsInfo() const = 0;
	virtual bool setFocusPoints(int32_t value) { return false; }
	virtual int32_t getFocusPoints() const { return 0; }

	// 对焦位置
	virtual bool setAFAreaPos(float x, float y) = 0;
	virtual bool getAFAreaPos(float& x, float& y, float& w, float& h) = 0;

	// 自动对焦照明器
	virtual const PropertyInfoEntry<int32_t>& autoFocusIlluminatorInfo() const = 0;
	virtual bool setAutoFocusIlluminator(int32_t value) { return false; }
	virtual int32_t getAutoFocusIlluminator() const { return 0; }

	// 面部识别模式
	virtual const PropertyInfoEntry<int32_t>& faceDetectionModeInfo() const = 0;
	virtual bool setFaceDetectionMode(int32_t value) { return false; }
	virtual int32_t getFaceDetectionMode() const { return 0; }

	// 眼球识别模式
	virtual const PropertyInfoEntry<int32_t>& eyeDetectionModeInfo() const = 0;
	virtual bool setEyeDetectionMode(int32_t value) { return false; }
	virtual int32_t getEyeDetectionMode() const { return 0; }

	// 曝光模式
	virtual const PropertyInfoEntry<uint32_t>& exposureProgramModeInfo() const = 0;
	virtual bool setExposureProgramMode(int mode) = 0;
	virtual int getExposureProgramMode() const = 0;

	// 曝光补偿
	virtual const PropertyInfoEntry<int32_t>& exposureBiasInfo() const = 0;
	virtual bool setExposureBias(int value) = 0;
	virtual int getExposureBias() const = 0;

	// FE Lock
	virtual const PropertyInfoEntry<int32_t>& feLockInfo() const = 0;
	virtual bool setFELock(int32_t value) = 0;
	virtual int32_t getFELock() const = 0;

	// MF辅助模式
	virtual const PropertyInfoEntry<int32_t>& mfAssistModeInfo() const = 0;
	virtual bool setMFAssistMode(int32_t value) { return false; }
	virtual int32_t getMFAssistMode() const { return 0; }

	// 快门优先级模式
	virtual const PropertyInfoEntry<int32_t>& shutterPriorityModeInfo() const = 0;
	virtual bool setAFCPriorityMode(int32_t value) { return false; }
	virtual bool getAFCPriorityMode(int32_t& value) const { return false; }

	virtual bool setAFSPriorityMode(int32_t value) { return false; }
	virtual bool getAFSPriorityMode(int32_t& value) const { return false; }

	// AWBLock
	virtual bool setAWBLock(bool isLock) = 0;
	virtual bool isAWBLock() const = 0;

	// 白平衡
	virtual const PropertyInfoEntry<uint16_t>& whiteBalanceInfo() const = 0;
	virtual bool setWhiteBalance(int value) = 0;
	virtual int getWhiteBalance() const = 0;

	// 白平衡移位
	virtual const PropertyInfoEntry<int32_t>& whiteBalanceTuneRInfo() const = 0;
	virtual const PropertyInfoEntry<int32_t>& whiteBalanceTuneBInfo() const = 0;
	virtual bool setWhiteBalanceTune(int r, int b) { return false; }
	virtual bool getWhiteBalanceTune(int& r, int& b) const { return false; }

	// 变焦操作
	virtual const PropertyInfoEntry<int8_t>& zoomSpeedRangeInfo() const = 0;
	virtual const PropertyInfoEntry<int32_t>& zoomInfo() const = 0;
	virtual bool zoomWide() = 0;
	virtual bool zoomTele() = 0;
	virtual bool zoomStop() = 0;
	virtual bool setZoomValue(int value) = 0;
	virtual int getZoomValue() const = 0;

	// 对焦位置
	virtual bool setFocusDistance(int value) = 0;
	virtual int getFocusDistance() const = 0;

	// 图片大小
	virtual const PropertyInfoEntry<int32_t>& imageSizeInfo() const = 0;
	virtual bool setImageSize(int32_t size) = 0;
	virtual int32_t getImageSize() const = 0;

	// 图片质量
	virtual const PropertyInfoEntry<int32_t>& imageQualityInfo() const = 0;
	virtual bool setImageQuality(int value) = 0;
	virtual int getImageQuality() const = 0;

	// 图片缩放比例
	virtual const PropertyInfoEntry<int32_t>& imageZoomInfo() const = 0;
	virtual bool setImageZoom(int32_t value) { return false; }
	virtual bool getImageZoom(int32_t& value) const { return false; }

	// 图片比例
	virtual const PropertyInfoEntry<int32_t>& imageRatioInfo() const = 0;
	virtual bool setImageRatio(int32_t size) = 0;
	virtual int32_t getImageRatio() const = 0;

	// 图片文件压缩格式
	virtual const PropertyInfoEntry<int32_t>& rawFileCompressionTypeInfo() const = 0;
	virtual bool setRAWFileCompressionType(int32_t value) = 0;
	virtual int32_t getRAWFileCompressionType() const = 0;

	// 图片文件输出深度
	virtual const PropertyInfoEntry<int32_t>& rawOutputDepthInfo() const = 0;
	virtual bool setRAWOutputDepth(int32_t value) { return false; }
	virtual int32_t getRAWOutputDepth() const { return 0; }

	// 胶片模拟
	virtual const PropertyInfoEntry<int32_t>& filmSimulationModeInfo() const = 0;
	virtual bool setFilmSimulationMode(int32_t value) { return false; }
	virtual int32_t getFilmSimulationMode() const { return 0; }

	// 纹理效果
	virtual const PropertyInfoEntry<int32_t>& grainEffectInfo() const = 0;
	virtual bool setGrainEffect(int32_t value) { return false; }
	virtual int32_t getGrainEffect() const { return 0; }

	// 阴影
	virtual const PropertyInfoEntry<int32_t>& shadowingInfo() const = 0;
	virtual bool setShadowing(int32_t value) { return false; }
	virtual int32_t getShadowing() const { return 0; }

	// COLOR CHROME FX BLUE
	virtual const PropertyInfoEntry<int32_t>& colorChromeBlueInfo() const = 0;
	virtual bool setColorChromeBlue(int32_t value) { return false; }
	virtual int32_t getColorChromeBlue() const { return 0; }

	// 平滑蒙皮效果
	virtual const PropertyInfoEntry<int32_t>& smoothSkinEffectInfo() const = 0;
	virtual bool setSmoothSkinEffect(int32_t value) { return false; }
	virtual int32_t getSmoothSkinEffect() const { return 0; }

	// 动态范围
	virtual const PropertyInfoEntry<int32_t>& dynamicRangeInfo() const = 0;
	virtual bool setDynamicRange(int32_t value) { return false; }
	virtual int32_t getDynamicRange() const { return 0; }

	// 宽动态范围
	virtual const PropertyInfoEntry<int32_t>& wideDynamicRangeInfo() const = 0;
	virtual bool setWideDynamicRange(int32_t value) { return false; }
	virtual int32_t getWideDynamicRange() const { return 0; }

	// 突出显示色调
	virtual const PropertyInfoEntry<int32_t>& hightlightToneInfo() const = 0;
	virtual bool setHightlightTone(int32_t value) { return false; }
	virtual int32_t getHightlightTone() const { return 0; }

	// 阴影色调
	virtual const PropertyInfoEntry<int32_t>& shadowToneInfo() const = 0;
	virtual bool setShadowTone(int32_t value) { return false; }
	virtual int32_t getShadowTone() const { return 0; }

	// 色温
	virtual const PropertyInfoEntry<int32_t>& colorTemperatureInfo() const = 0;
	virtual bool setColorTemperature(int32_t value) { return false; }
	virtual int32_t getColorTemperature() const { return 0; }

	// 颜色类型
	virtual const PropertyInfoEntry<int32_t>& colorModeInfo() const = 0;
	virtual bool setColorMode(int32_t value) { return false; }
	virtual int32_t getColorMode() const { return 0; }

	// 锐度
	virtual const PropertyInfoEntry<int32_t>& sharpnessInfo() const = 0;
	virtual bool setSharpness(int32_t value) { return false; }
	virtual int32_t getSharpness() const { return 0; }

	// 降噪
	virtual const PropertyInfoEntry<int32_t>& noiseReductionInfo() const = 0;
	virtual bool setNoiseReduction(int32_t value) { return false; }
	virtual int32_t getNoiseReduction() const { return 0; }

	// 颜色空间
	virtual const PropertyInfoEntry<int32_t>& colorSpaceInfo() const = 0;
	virtual bool setColorSpace(int32_t value) { return false; }
	virtual int32_t getColorSpace() const { return 0; }

	// 测光模式
	virtual const PropertyInfoEntry<int32_t>& meteringModeInfo() const = 0;
	virtual bool setMeteringMode(int32_t value) { return false; }
	virtual int32_t getMeteringMode() const { return 0; }

	// 保存到sd卡
	virtual const PropertyInfoEntry<int32_t>& mediaRecordInfo() const = 0;
	virtual bool setMediaRecord(int value) { return false; }
	virtual int getMediaRecord() const { return -1; }

	//拍照延时
	virtual const PropertyInfoEntry<int32_t>& captureDelayInfo() const = 0;
	virtual bool setCaptureDelay(int value) { return false; }
	virtual int getCaptureDelay() const { return -1; }

	// 电量
	virtual bool getBatteryLevel(int32_t& value) const { return false; }
	virtual bool getBatteryState(int32_t& value) const { return false; }

	// 相机信息
	virtual QString getModel() const { return QString(); }
	virtual QString getSerialNumber() const { return QString(); }
	virtual QString getTetherConnection() const { return QString(); }

	// 镜头信息
	virtual QString getLensName() { return QString(); }
	virtual QString getLensID() { return QString(); }
	virtual QString getLensModel() { return ""; }
	virtual int32_t getRearShutterCount() const { return 0; }

    virtual void updateCameraStatus() = 0;

	virtual float getAspectRatioValue() const = 0;

	virtual void updateProperties() = 0;


};


using CameraDeviceList = std::vector<ICameraHolderPtr>;


class ICameraHolderEntry : public ICameraHolder
{
	Q_OBJECT
protected:
	ICameraHolderEntry(const QString& deviceId, CameraBrand camType)
		: m_camType(camType)
		, m_deviceId(deviceId)
	{
	}

public:
	CameraBrand getCameraType() const override { return m_camType; }

	void setDeviceName(const QString& name) override { m_deviceName = name; }
	const QString& getDeviceName() const override { return m_deviceName; }

	const QString& getDeviceId() const override { return m_deviceId; }

	virtual FocusStatus getFocusStatus() const override { return m_focusStatus; };

	virtual void setPhotoSavePath(const QString& path) override { m_photoSavePath = path; }
	virtual const QString& getPhotoSavePath() const override { return m_photoSavePath; }

	PROPERTY_SET_GET(uint16_t, m_propTab.f_number, aperture, Aperture);

	PROPERTY_SET_GET(uint32_t, m_propTab.iso_sensitivity, iso, Iso);

	PROPERTY_SET_GET(int32_t, m_propTab.shutter_speed, shutterSpeed, ShutterSpeed);

	//PROPERTY_SET_GET(int32_t, m_propTab.shutter_speed, shutterSpeed, ShutterSpeed);

	virtual const PropertyInfoEntry<int32_t>& shootModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& focusModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& autoFocusModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& focusPointsInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& autoFocusIlluminatorInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& faceDetectionModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& eyeDetectionModeInfo() const override;

	PROPERTY_SET_GET(int32_t, m_propTab.live_view_image_quality, liveViewImageQuality, LiveViewImageQuality);

	virtual const PropertyInfoEntry<uint32_t>& exposureProgramModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& exposureBiasInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& feLockInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& mfAssistModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& shutterPriorityModeInfo() const override;
	virtual const PropertyInfoEntry<uint16_t>& whiteBalanceInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& whiteBalanceTuneRInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& whiteBalanceTuneBInfo() const override;
	virtual const PropertyInfoEntry<int8_t>& zoomSpeedRangeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& zoomInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& imageSizeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& imageQualityInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& imageZoomInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& imageRatioInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& rawFileCompressionTypeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& rawOutputDepthInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& filmSimulationModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& grainEffectInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& shadowingInfo() const override;

	virtual const PropertyInfoEntry<int32_t>& colorChromeBlueInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& smoothSkinEffectInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& dynamicRangeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& wideDynamicRangeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& hightlightToneInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& shadowToneInfo() const override;

	PROPERTY_SET_GET(int32_t, m_propTab.color_temperature, colorTemperature, ColorTemperature);

	virtual const PropertyInfoEntry<int32_t>& colorModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& sharpnessInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& noiseReductionInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& colorSpaceInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& meteringModeInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& mediaRecordInfo() const override;
	virtual const PropertyInfoEntry<int32_t>& captureDelayInfo() const override;

protected:
	QString m_deviceId;         // 设备ID，唯一标识符
	QString m_deviceName;       // 设备名
	QString m_photoSavePath;    // 图片保存路径

	CameraBrand m_camType;          // 相机类型
	FocusMode m_focusMode;
	std::atomic_bool m_isLiveView{ true };
	FocusStatus m_focusStatus{ NoFocus };

	mutable PropertyInfoTable m_propTab;
};
DECLARE_STD_PTR(ICameraHolderEntry)
Q_DECLARE_METATYPE(ICameraHolderEntryPtr)

/*!
 * \class ICameraController
 *
 * \brief 相机控制基类
 */
class ICameraController : public QObject
{
	Q_OBJECT
protected:
	explicit ICameraController(CameraBrand camType) : m_camType(camType)
	{
	}
	virtual ~ICameraController() {}

public:
	bool isInit() const {
		return m_isInit;
	}
	CameraBrand getControllerType() {
        return m_camType;
	}

	Q_INVOKABLE virtual bool init() = 0;
	Q_INVOKABLE virtual bool release() = 0;
	Q_INVOKABLE virtual bool scan() = 0;
	Q_INVOKABLE virtual void asyncScan();

	Q_INVOKABLE virtual void setPhotoSavePath(const QString& path) {}

	Q_INVOKABLE virtual ICameraHolderPtr at(int index) const = 0;
	Q_INVOKABLE virtual int size() const = 0;
	Q_INVOKABLE virtual CameraDeviceList getCameraDeviceList() const = 0;
	Q_INVOKABLE virtual ICameraHolderPtr findCameraDeviceFromId(const QString& id) const = 0;
	Q_INVOKABLE virtual ICameraHolderPtr findCameraDeviceFromName(const QString& name) const = 0;

Q_SIGNALS:
	void devicesChangedSingal(ICameraController*);

	void cameraConnected(ICameraHolderPtr camera);
	void cameraDisconnected(ICameraHolderPtr camera);

protected:
	void setIsInit(bool value) { m_isInit = value; }

private:
    bool m_isInit{ false };
    CameraBrand m_camType;
};
DECLARE_STD_PTR(ICameraController)


/*!
 * \class IEntityCameraController
 *
 * \brief 实体相机控制器
 */
class IEntityCameraController : public ICameraController
{
	Q_OBJECT
public:
    IEntityCameraController(CameraBrand camType) : ICameraController(camType) {}
	virtual ~IEntityCameraController() {
		m_deviceList.clear();
	}

	bool release() override;

	virtual void setPhotoSavePath(const QString& path);
	const QString& getPhotoSavePath() const { return m_photoPath; }

	virtual ICameraHolderPtr at(int index) const override;

	virtual int size() const override;

	virtual CameraDeviceList getCameraDeviceList() const override;

	virtual ICameraHolderPtr findCameraDeviceFromId(const QString& id) const override;

	virtual ICameraHolderPtr findCameraDeviceFromName(const QString& name) const override;

protected:
	virtual void updateCameraDevice(const std::list<QString>& camNameList);

	void addCameraDevice(const ICameraHolderPtr& cameraDev);

protected Q_SLOTS:
	void onDisconnect(ICameraHolder*, bool connected);

protected:
	mutable std::recursive_mutex m_deviceMutex;

private:
	QString m_photoPath;

	CameraDeviceList m_deviceList;
};




