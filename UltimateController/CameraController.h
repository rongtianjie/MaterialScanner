#pragma once

#include "ICameraHolder.h"
#include <vector>
#include <map>


using CameraControllerList = std::vector<ICameraControllerPtr>;


/*!
 * \class CameraController
 *
 * \brief 相机控制器
 */
class CameraController : public ICameraController
{
	Q_OBJECT
public:
    explicit CameraController();
    ~CameraController();

	virtual bool init() override;
	virtual bool release() override;
	virtual bool scan() override;
	virtual void asyncScan() override;

	virtual void setPhotoSavePath(const QString& path) override;

	virtual ICameraHolderPtr at(int index) const override;
	virtual int size() const override;
	virtual CameraDeviceList getCameraDeviceList() const override;
	virtual ICameraHolderPtr findCameraDeviceFromId(const QString& id) const override;
	virtual ICameraHolderPtr findCameraDeviceFromName(const QString& id) const override;

	void addCameraController(const ICameraControllerPtr& controller);
	void removeCameraController(const ICameraControllerPtr& controller);
	const CameraControllerList& getCameraControllerList() const { 
		return m_controllerList; 
	}

	static CameraController* instance();

protected Q_SLOTS:
	void onCameraDeviceChanged(ICameraController* controller);

private:
	CameraControllerList m_controllerList;
};

