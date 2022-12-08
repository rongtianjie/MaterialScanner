#pragma once

#include "ICameraHolder.h"
#include "CameraRemote_SDK.h"
#include "IDeviceCallback.h"
#include "Text.h"
#include "ConnectionInfo.h"
#include <mutex>
#include <functional>
#include <QString>

using PROPERTY_SET_CK = std::function<void(SCRSDK::CrDeviceProperty&)>;
using LIVEVIEW_PROP_SET_CK = std::function<void(SCRSDK::CrLiveViewProperty&)>;

/*!
 * \class CameraHolderSony
 *
 * \brief 索尼相机
 */
class CameraHolderSony : public ICameraHolderEntry, public SCRSDK::IDeviceCallback
{
	//friend class CameraControllerSony;

public:
	CameraHolderSony() = delete;
	explicit CameraHolderSony(SCRSDK::ICrCameraObjectInfo* camInfo, const QString& id);
	~CameraHolderSony();

	QString getBrandStr() const override;
	void setPhotoSavePath(const QString& path) override;

	/***相机操作****/
	bool connect() override;
	bool disconnect() override;
	virtual bool isConnected() const override;
	virtual bool release() override;

	/***摄像操作****/
	virtual bool captureImage() override;
	void shootOne() override;
	void shutterToHalf() override;
	void shutterToRelease() override;
	bool captureDown() override;
	bool captureUp() override;

	/***属性操作****/

	// 实时取景
	virtual bool setLiveView(bool isView) override;
	virtual bool getLiveView() const override;
	uint8_t* getLiveViewBuffer(uint32_t* dataSize) override;

	// 实时取景图片质量
	virtual bool setLiveViewImageQuality(int value) override;
	virtual int getLiveViewImageQuality() const override;

	// 光圈
	virtual bool setAperture(uint16_t value) override;
	virtual uint16_t getAperture() const  override;

	// iso
	virtual bool setIso(uint32_t value) override;
	virtual uint32_t getIso() const override;

	// 快门速度
	virtual bool setShutterSpeed(int32_t value) override;
	int32_t getShutterSpeed() const override;

	// 拍摄模式
	bool setShootMode(int32_t mode) override;
	virtual int32_t getShootMode() const override;

	// 聚焦模式
	bool setFocusMode(int32_t mode) override;
	virtual int32_t getFocusMode() const override;

	// 对焦位置
	bool setAFAreaPos(float x, float y) override;
	virtual bool getAFAreaPos(float& x, float& y, float& w, float& h) override;

	// 曝光模式
	virtual bool setExposureProgramMode(int mode) override;
	virtual int getExposureProgramMode() const override;

	// 曝光补偿
	virtual bool setExposureBias(int mode) override { return false; }
	virtual int getExposureBias() const override { return -1; }

	// FELock
	virtual bool setFELock(int32_t isLock) override;
	virtual int32_t getFELock() const override;

	// AWBLock
	virtual bool setAWBLock(bool isLock) override;
	virtual bool isAWBLock() const override;

	// 白平衡
	virtual bool setWhiteBalance(int value) override;
	virtual int getWhiteBalance() const override;

	// 变焦操作
	virtual bool zoomWide() override;
	virtual bool zoomTele() override;
	virtual bool zoomStop() override;
	virtual bool setZoomValue(int value) override;
	virtual int getZoomValue() const override;

	virtual bool setFocusDistance(int value) override { return false; }
	virtual int getFocusDistance() const override { return 0; }

	// 图片大小
	virtual bool setImageSize(int32_t size) override;
	virtual int32_t getImageSize() const override;

	// 图片质量
	virtual bool setImageQuality(int value) override { return false; }
	virtual int getImageQuality() const override { return false; }

	// 图片比例
	virtual bool setImageRatio(int32_t size) override;
	virtual int32_t getImageRatio() const override;

	// 图片文件压缩格式
	virtual bool setRAWFileCompressionType(int32_t format) override;
	virtual int32_t getRAWFileCompressionType() const override;

    void updateCameraStatus() override;

	virtual void updateProperties() override;

	float getAspectRatioValue() const override;

public:
	/***回调函数****/
	void OnConnected(SCRSDK::DeviceConnectionVersioin version) override;
	void OnDisconnected(CrInt32u error) override;
	void OnPropertyChanged() override;
	void OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
	void OnLvPropertyChanged() override;
	void OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
	void OnCompleteDownload(CrChar* filename) override;
	void OnNotifyContentsTransfer(CrInt32u notify, SCRSDK::CrContentHandle handle, CrChar* filename = 0) override;
	void OnWarning(CrInt32u warning) override;
	void OnError(CrInt32u error) override;

private:
	bool getProperty(SCRSDK::CrDeviceProperty& value) const;
	bool getProperty(SCRSDK::CrDevicePropertyCode code, PROPERTY_SET_CK callback) const;
	bool getLiveViewProperty(SCRSDK::CrLiveViewPropertyCode code, LIVEVIEW_PROP_SET_CK callback) const;
	bool setProperty(SCRSDK::CrDeviceProperty& value) const;

    void setProperty(SCRSDK::CrDevicePropertyCode code, unsigned int val, SCRSDK::CrDataType dataType);
    bool compareProperty(SCRSDK::CrDevicePropertyCode code, unsigned int val);

	void captureOne();

    void executePosXY(CrInt16u code, int x, int y);

	void load_properties(CrInt32u num = 0, CrInt32u* codes = nullptr);

protected:
	SCRSDK::ICrCameraObjectInfo* m_camInfo{ nullptr };
	std::atomic<bool> m_connected{ false };
	std::atomic_int m_aspectRatio;

    int64_t m_sonyHandler{ 0 };
	std::recursive_mutex m_mutex;
};
DECLARE_STD_PTR(CameraHolderSony)


/*!
 * \class CameraControllerSony
 *
 * \brief 索尼相机控制器
 */
class CameraControllerSony : public IEntityCameraController
{
public:
    CameraControllerSony();
    ~CameraControllerSony() = default;

	virtual bool init() override;
	virtual bool release() override;
	virtual bool scan() override;
};
