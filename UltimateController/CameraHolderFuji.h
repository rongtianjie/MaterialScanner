#pragma once
#pragma execution_character_set("utf-8")

#include "ICameraHolder.h"
#include "CommandTaskThread.h"
#include <mutex>
#include <QString>


class CFujiSDK;
class CameraHolderFujiPrivate;


/*!
 * \class SaveImageTask
 * \brief 保存图片任务
 */
class SaveImageTask : public Task
{
	Q_OBJECT
public:
	explicit SaveImageTask(const QString& filePath, const QString& name, uint8_t* data, uint64_t size);
	~SaveImageTask();

	virtual int getTaskId() const override { return 2; };

	virtual void run() override;

	bool saveImage();

	bool getResult() const { return m_saveResult; }

private:
	bool m_saveResult{ false };
	QString m_filePath;
	QString m_fileName;
	uint8_t* m_data{ nullptr };
	uint64_t m_size{ 0 };
};
DECLARE_STD_PTR(SaveImageTask);

/*!
 * \class CameraHolderFuji
 *
 * \brief 富士相机控制类
 */
class CameraHolderFuji : public ICameraHolderEntry
{
	Q_OBJECT
	friend class CameraControllerFuji;
	friend class CameraHolderFujiPrivate;
public:
	CameraHolderFuji() = delete;
    explicit CameraHolderFuji(CFujiSDK* m_sdk_fuji, const QString& deviceId);
    ~CameraHolderFuji();

    QString getBrandStr() const override;

	void setDeviceName(const QString& name) override;

	/***相机操作****/
	virtual bool connect() override;
	virtual bool disconnect() override;
	virtual bool isConnected() const override;
	virtual bool release() override;

	/***摄像操作****/
	virtual bool captureImage() override;
	void shootOne() override;
	void shutterToHalf() override;
	void shutterToRelease() override;

	virtual void focusPress() override;
	virtual void focusRelease() override;

	bool captureDown() override;
	bool captureUp() override;

	// 控制模式
	virtual bool setControlMode(ControlMode mode) override;
	virtual ControlMode getControlMode() override;

	void setPhotoSavePath(const QString& path) override;

	/***属性操作****/

	// 实时取景
	virtual bool setLiveView(bool isView) override;
	virtual bool getLiveView() const override;
	uint8_t* getLiveViewBuffer(uint32_t* dataSize) override;

	// 实时取景图片质量
	virtual bool setLiveViewImageQuality(int value) override;
	int getRealLiveViewImageQuality();

	bool setLiveViewImageSize(int32_t size);
	int32_t getLiveViewImageSize() const;

	// 光圈
	virtual bool setAperture(uint16_t value) override;
	uint16_t getRealAperture() const;

	// iso
	virtual bool setIso(uint32_t value) override;
	uint32_t getRealIso() const;

	// 快门速度
	virtual bool setShutterSpeed(int32_t value) override;
	int32_t getRealShutterSpeed() const;

	// 拍摄模式
	virtual bool setShootMode(int32_t mode) override;
	virtual int32_t getShootMode() const override;

	// 聚焦模式
	virtual bool setFocusMode(int32_t mode) override;
	virtual int32_t getFocusMode() const override;

	// 自动对焦模式
	virtual bool setAutoFocusMode(int32_t value) override;
	virtual int32_t getAutoFocusMode() const override;

	// 对焦点
	virtual bool setFocusPoints(int32_t value) override;
	virtual int32_t getFocusPoints() const override;

	// 对焦位置
	bool setAFAreaPos(float x, float y) override;
	bool getAFAreaPos(float& x, float& y, float& w, float& h) override;
	bool getRealAFAreaPos();

	// 自动对焦照明器
	virtual bool setAutoFocusIlluminator(int32_t value) override;
	virtual int32_t getAutoFocusIlluminator() const override;

	// 面部识别
	virtual bool setFaceDetectionMode(int32_t value) override;
	virtual int32_t getFaceDetectionMode() const override;

	// 眼球识别模式
	virtual bool setEyeDetectionMode(int32_t value) override;
	virtual int32_t getEyeDetectionMode() const override;

	// 曝光模式
	virtual bool setExposureProgramMode(int mode) override;
	virtual int getExposureProgramMode() const override;

	// 曝光补偿
	virtual bool setExposureBias(int mode) override;
	virtual int getExposureBias() const override;

	// FELock
	virtual bool setFELock(int32_t value) override;
	virtual int32_t getFELock() const override;

	// MF辅助模式
	virtual bool setMFAssistMode(int32_t value) override;
	virtual int32_t getMFAssistMode() const override;

	// 快门优先级模式
	virtual bool setAFCPriorityMode(int32_t value) override;
	virtual bool getAFCPriorityMode(int32_t& value) const override;

	virtual bool setAFSPriorityMode(int32_t value) override;
	virtual bool getAFSPriorityMode(int32_t& value) const override;

	// AWBLock
	virtual bool setAWBLock(bool value) override { return false; }
	virtual bool isAWBLock() const override { return false; }

	// 白平衡
	virtual bool setWhiteBalance(int value) override;
	virtual int getWhiteBalance() const override;

	// 白平衡移位
	virtual bool setWhiteBalanceTune(int r, int b) override;
	virtual bool getWhiteBalanceTune(int& r, int& b) const override;

	// 变焦操作
	virtual bool zoomWide() override { return false; }
	virtual bool zoomTele() override { return false; }
	virtual bool zoomStop() override { return false; }
	virtual bool setZoomValue(int value) override { return false; }
	virtual int getZoomValue() const override { return 0; }

	virtual bool setFocusDistance(int value) override;
	virtual int getFocusDistance() const override;

	// 图片大小
	virtual bool setImageSize(int32_t size) override;
	virtual int32_t getImageSize() const override;

	// 图片质量
	virtual bool setImageQuality(int value) override;
	virtual int getImageQuality() const override;

	// 图片缩放比例
	virtual bool setImageZoom(int32_t value) override;
	virtual bool getImageZoom(int32_t& value) const override;

	// 图片比例
	virtual bool setImageRatio(int32_t size) override { return false; };
	virtual int32_t getImageRatio() const override { return 0; };

	// 图片文件压缩格式
	virtual bool setRAWFileCompressionType(int32_t value) override;
	virtual int32_t getRAWFileCompressionType() const override;

	// 图片文件输出深度
	virtual bool setRAWOutputDepth(int32_t value) override;
	virtual int32_t getRAWOutputDepth() const override;

	// 胶片模拟
	virtual bool setFilmSimulationMode(int32_t value) override;
	virtual int32_t getFilmSimulationMode() const override;

	// 纹理效果
	virtual bool setGrainEffect(int32_t value) override;
	virtual int32_t getGrainEffect() const override;

	// 阴影
	virtual bool setShadowing(int32_t value) override;
	virtual int32_t getShadowing() const override;

	// COLOR CHROME FX BLUE
	virtual bool setColorChromeBlue(int32_t value) override;
	virtual int32_t getColorChromeBlue() const override;

	// 平滑蒙皮效果
	virtual bool setSmoothSkinEffect(int32_t value) override;
	virtual int32_t getSmoothSkinEffect() const override;

	// 动态范围
	virtual bool setDynamicRange(int32_t value) override;
	virtual int32_t getDynamicRange() const override;

	// 宽动态范围
	virtual bool setWideDynamicRange(int32_t value) override;
	virtual int32_t getWideDynamicRange() const override;

	// 突出显示色调
	virtual bool setHightlightTone(int32_t value) override;
	virtual int32_t getHightlightTone() const override;

	// 阴影色调
	virtual bool setShadowTone(int32_t value) override;
	virtual int32_t getShadowTone() const override;

	// 色温
	virtual bool setColorTemperature(int32_t value) override;
	virtual int32_t getColorTemperature() const override;

	// 颜色类型
	virtual bool setColorMode(int32_t value) override;
	virtual int32_t getColorMode() const override;

	// 锐度
	virtual bool setSharpness(int32_t value) override;
	virtual int32_t getSharpness() const override;

	// 降噪
	virtual bool setNoiseReduction(int32_t value) override;
	virtual int32_t getNoiseReduction() const override;

	// 颜色空间
	virtual bool setColorSpace(int32_t value) override;
	virtual int32_t getColorSpace() const override;

	// 测光模式
	virtual bool setMeteringMode(int32_t value) override;
	virtual int32_t getMeteringMode() const override;

	// 相机参数备份
	bool setBackupSettings(uint8_t* data, uint64_t size);
	uint8_t* getbackupSettings(uint64_t& size);

	//保存到sd卡
	bool setMediaRecord(int value) override;
	int getMediaRecord() const override;

	//拍照延时
	virtual bool setCaptureDelay(int value) override;
	virtual int getCaptureDelay() const override;

	// 电量
	virtual bool getBatteryLevel(int32_t& value) const override;
	virtual bool getBatteryState(int32_t& value) const override;

	// 相机信息
	virtual QString getModel() const override;
	virtual QString getSerialNumber() const override;
	virtual QString getTetherConnection() const override;

	// 镜头信息
	virtual QString getLensName() override;
	virtual QString getLensID() override;
	virtual QString getLensModel() override;
	virtual int32_t getRearShutterCount() const override;

    void updateCameraStatus() override;

	virtual void updateProperties() override;

	float getAspectRatioValue() const override;

protected Q_SLOTS:
	void onCameraInvalid();

private:
	void printError() const;

	bool startLiveView();
	bool stopLiveView() const;
	bool stopLiveView2() const;

	bool setLiveSize(int value);

	void updateLensInfo();

	void updateImageThread();

	// 更新电量信息
	void updateBatteryInfo();

private:
	mutable std::recursive_mutex m_mutex;
	mutable std::recursive_mutex m_mutexData;
	CFujiSDK* m_sdk_fuji{ nullptr };

	std::atomic_bool m_isConnected{ false };

	QString m_models;
	QString m_serialNumber;
	QString m_tetherConnection;

	QString m_lensName;				// 镜头名称
	QString m_lensID;				// 镜头ID
	QString m_rearShutterCount;		// 

	CameraHolderFujiPrivate* d_ptr{ nullptr };
	std::thread* m_thread{ nullptr };
};

/*!
 * \class CameraControllerFuji
 *
 * \brief 富士相机控制器
 */
class CameraControllerFuji : public IEntityCameraController
{
public:
    CameraControllerFuji();
    ~CameraControllerFuji();

    virtual bool init() override;
    virtual bool release() override;
    virtual bool scan() override;

	ICameraControllerPtr findCameraFromName(const QString& name);

protected:
	CFujiSDK* m_sdk_fuji{ nullptr };
};
