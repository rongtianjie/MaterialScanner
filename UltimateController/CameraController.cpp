#include "CameraController.h"
#include "CommandTaskThread.h"
#include <iostream>
#include <string>


CameraController::CameraController()
    : ICameraController(CameraBrand::CC_BRAND_ALL)
{

}

CameraController::~CameraController()
{
}

bool CameraController::init()
{
    for (const auto& controller : m_controllerList)
    {
        auto isInit = controller->init();
        if (!isInit)
        {
            std::cout << "Controller type : " << std::to_string((int)controller->getControllerType()) 
                << " init error." << std::endl;
        }
    }

    return true;
}

bool CameraController::release()
{
	for (const auto& controller : m_controllerList)
	{
		auto isInit = controller->release();
		if (!isInit)
		{
			std::cout << "Controller type : " << std::to_string((int)controller->getControllerType())
				<< " release error." << std::endl;
		}
	}

	return true;
}

bool CameraController::scan()
{
	for (const auto& controller : m_controllerList)
	{
		auto isInit = controller->scan();
		if (!isInit)
		{
			std::cout << "Controller type : " << std::to_string((int)controller->getControllerType())
				<< " scan error." << std::endl;
		}
	}

	return true;
}

void CameraController::asyncScan()
{
	for (const auto& controller : m_controllerList)
	{
		controller->asyncScan();
	}
}

void CameraController::setPhotoSavePath(const QString& path)
{
	for (const auto& controller : m_controllerList)
	{
		controller->setPhotoSavePath(path);
	}
}

ICameraHolderPtr CameraController::at(int index) const
{
	auto count = 0;
	for (auto i = 0; i < m_controllerList.size(); ++i)
	{
		auto controller = m_controllerList[i];
		auto holder = controller->at(index - count);
		if (holder)
		{
			return holder;
		}
		count += controller->size();
	}

	return nullptr;
}

int CameraController::size() const
{
	int count = 0;

	for (const auto& controller : m_controllerList)
	{
		count += controller->size();
	}

	return count;
}

CameraDeviceList CameraController::getCameraDeviceList() const
{
	CameraDeviceList allDevList;

	for (const auto& controller : m_controllerList)
	{
		auto devList = controller->getCameraDeviceList();

		std::copy(devList.begin(), devList.end(), std::back_inserter(allDevList));
	}

    return allDevList;
}

ICameraHolderPtr CameraController::findCameraDeviceFromId(const QString& id) const
{
	ICameraHolderPtr camHolder;
	for (const auto& controller : m_controllerList)
	{
		camHolder = controller->findCameraDeviceFromId(id);
		if (camHolder)
		{
			break;
		}
	}

	return camHolder;
}

ICameraHolderPtr CameraController::findCameraDeviceFromName(const QString& id) const
{
	ICameraHolderPtr camHolder;
	for (const auto& controller : m_controllerList)
	{
		camHolder = controller->findCameraDeviceFromName(id);
		if (camHolder)
		{
			break;
		}
	}

	return camHolder;
}

void CameraController::addCameraController(const ICameraControllerPtr& controller)
{
    removeCameraController(controller);
	connect(controller.get(), &ICameraController::devicesChangedSingal, this, &CameraController::onCameraDeviceChanged);
	//connect(controller.get(), &ICameraController::cameraConnected, this, &CameraController::cameraConnected, Qt::QueuedConnection);
	connect(controller.get(), &ICameraController::cameraConnected, this, [this](ICameraHolderPtr camera) {
		emit cameraConnected(camera);
		});

	connect(controller.get(), &ICameraController::cameraDisconnected, this, &CameraController::cameraDisconnected);
    m_controllerList.push_back(controller);
}

void CameraController::removeCameraController(const ICameraControllerPtr& controller)
{
	auto iter = std::remove(m_controllerList.begin(), m_controllerList.end(), controller);
	if (iter != m_controllerList.end())
	{
        disconnect(controller.get(), &ICameraController::devicesChangedSingal, this, &CameraController::onCameraDeviceChanged);
		disconnect(controller.get(), &ICameraController::cameraConnected, this, &CameraController::cameraConnected);
		disconnect(controller.get(), &ICameraController::cameraDisconnected, this, &CameraController::cameraDisconnected);
	}
}

CameraController* CameraController::instance()
{
	static CameraController controller;

	return &controller;
}

void CameraController::onCameraDeviceChanged(ICameraController* controller)
{
    emit devicesChangedSingal(this);
}






