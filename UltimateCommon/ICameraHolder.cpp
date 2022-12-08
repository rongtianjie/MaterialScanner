#include "ICameraHolder.h"
#include "CommandTaskThread.h"
#include <QThreadPool>



const PropertyInfoEntry<int32_t>& ICameraHolderEntry::shootModeInfo() const
{
	return m_propTab.still_capture_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::focusModeInfo() const
{
	return m_propTab.focus_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::autoFocusModeInfo() const
{
	return m_propTab.auto_focus_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::focusPointsInfo() const
{
	return m_propTab.focus_points;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::autoFocusIlluminatorInfo() const
{
	return m_propTab.auto_focus_illuminator;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::faceDetectionModeInfo() const
{
	return m_propTab.face_detection_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::eyeDetectionModeInfo() const
{
	return m_propTab.eye_detection_mode;
}

const PropertyInfoEntry<uint32_t>& ICameraHolderEntry::exposureProgramModeInfo() const
{
	return m_propTab.exposure_program_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::exposureBiasInfo() const
{
	return m_propTab.exposure_bias;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::feLockInfo() const
{
	return m_propTab.fe_lock;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::mfAssistModeInfo() const
{
	return m_propTab.mf_assist_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::shutterPriorityModeInfo() const
{
	return m_propTab.shutter_priority_mode;
}

const PropertyInfoEntry<uint16_t>& ICameraHolderEntry::whiteBalanceInfo() const
{
	return m_propTab.white_balance;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::whiteBalanceTuneRInfo() const
{
	return m_propTab.white_balance_tune_R;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::whiteBalanceTuneBInfo() const
{
	return m_propTab.white_balance_tune_B;
}

const PropertyInfoEntry<int8_t>& ICameraHolderEntry::zoomSpeedRangeInfo() const
{
	return m_propTab.zoom_speed_range;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::zoomInfo() const
{
	return m_propTab.zoom_operation;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::imageSizeInfo() const
{
	return m_propTab.image_size;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::imageQualityInfo() const
{
	return m_propTab.image_quality;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::imageZoomInfo() const
{
	return m_propTab.image_zoom;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::imageRatioInfo() const
{
	return m_propTab.image_ratio;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::rawFileCompressionTypeInfo() const
{
	return m_propTab.compression_file_format;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::rawOutputDepthInfo() const
{
	return m_propTab.raw_output_depth;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::filmSimulationModeInfo() const
{
	return m_propTab.film_simulation_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::grainEffectInfo() const
{
	return m_propTab.grain_effect;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::shadowingInfo() const
{
	return m_propTab.shadowing;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::colorChromeBlueInfo() const
{
	return m_propTab.color_chrome_blue;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::smoothSkinEffectInfo() const
{
	return m_propTab.smooth_skin_effect;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::dynamicRangeInfo() const
{
	return m_propTab.dynamic_range;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::wideDynamicRangeInfo() const
{
	return m_propTab.wide_dynamic_range;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::hightlightToneInfo() const
{
	return m_propTab.hightlight_tone;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::shadowToneInfo() const
{
	return m_propTab.shadow_tone;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::colorModeInfo() const
{
	return m_propTab.color_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::sharpnessInfo() const
{
	return m_propTab.sharpness;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::noiseReductionInfo() const
{
	return m_propTab.noise_reduction;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::colorSpaceInfo() const
{
	return m_propTab.color_space;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::meteringModeInfo() const
{
	return m_propTab.metering_mode;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::mediaRecordInfo() const
{
	return m_propTab.media_record;
}

const PropertyInfoEntry<int32_t>& ICameraHolderEntry::captureDelayInfo() const
{
	return m_propTab.capture_delay;
}

bool IEntityCameraController::release()
{
	if (isInit())
	{
		m_deviceMutex.lock();
		for (auto& camHolder : m_deviceList)
		{
			camHolder->disconnect();
			camHolder->release();
		}
		m_deviceList.clear();
		m_deviceMutex.unlock();
	}

	return true;
}

void IEntityCameraController::setPhotoSavePath(const QString& path)
{
	ICameraController::setPhotoSavePath(path);
	m_photoPath = path;

	std::lock_guard guard(m_deviceMutex);
	for (auto& device : m_deviceList)
	{
		device->setPhotoSavePath(path);
	}
}

ICameraHolderPtr IEntityCameraController::at(int index) const {
	std::lock_guard guard(m_deviceMutex);
	if (index < 0 || index >= m_deviceList.size())
	{
		return nullptr;
	}

	return m_deviceList[index];
}

int IEntityCameraController::size() const {
	std::lock_guard guard(m_deviceMutex);
	return m_deviceList.size();
}

CameraDeviceList IEntityCameraController::getCameraDeviceList() const {
	std::lock_guard guard(m_deviceMutex);
	return m_deviceList;
}

ICameraHolderPtr IEntityCameraController::findCameraDeviceFromId(const QString& id) const {
	std::lock_guard guard(m_deviceMutex);
	auto iter = std::find_if(m_deviceList.begin(), m_deviceList.end(), [&](const ICameraHolderPtr& cameraHold) {
		return cameraHold->getDeviceId() == id;
		});

	if (iter != m_deviceList.end())
	{
		return (*iter);
	}

	return ICameraHolderPtr();
}

ICameraHolderPtr IEntityCameraController::findCameraDeviceFromName(const QString& name) const
{
	std::lock_guard guard(m_deviceMutex);
	auto iter = std::find_if(m_deviceList.begin(), m_deviceList.end(), [&](const ICameraHolderPtr& cameraHold) {
		return cameraHold->getDeviceName() == name;
		});

	if (iter != m_deviceList.end())
	{
		return (*iter);
	}

	return ICameraHolderPtr();
}

void IEntityCameraController::updateCameraDevice(const std::list<QString>& camNameList)
{
	bool isChanged = false;
	{
		std::lock_guard guard(m_deviceMutex);
		for (auto iter = m_deviceList.begin(); iter != m_deviceList.end(); )
		{
			bool isFind = false;

			auto camIter = std::find(camNameList.begin(), camNameList.end(), (*iter)->getDeviceId());
			if (camIter == camNameList.end())
			{
				(*iter)->disconnect();
				iter = m_deviceList.erase(iter);
				isChanged = true;
				emit cameraDisconnected(*iter);
			}
			else
			{
				++iter;
			}
		}
	}

	if (isChanged)
	{
		emit devicesChangedSingal(this);
	}
}

void IEntityCameraController::addCameraDevice(const ICameraHolderPtr& cameraDev)
{
	if (!cameraDev)
		return;

	m_deviceMutex.lock();
	m_deviceList.push_back(cameraDev);
	m_deviceMutex.unlock();

	connect(cameraDev.get(), &ICameraHolder::statusChangedSignal, this, &IEntityCameraController::onDisconnect, Qt::QueuedConnection);

	emit cameraConnected(cameraDev);
	emit devicesChangedSingal(this);
}

void IEntityCameraController::onDisconnect(ICameraHolder* cam, bool connected)
{
	if (!connected)
	{
		m_deviceMutex.lock();
		auto iter = std::find_if(m_deviceList.begin(), m_deviceList.end(), [=](const ICameraHolderPtr& value) {
			return value.get() == cam;
			});

		if (iter != m_deviceList.end())
		{
			auto remCam = (*iter);
			disconnect(cam, &ICameraHolder::statusChangedSignal, this, &IEntityCameraController::onDisconnect);
			m_deviceList.erase(iter);
			emit cameraDisconnected(remCam);
		}
		m_deviceMutex.unlock();

	}
}

void ICameraController::asyncScan()
{
	CommandTaskThread::instance()->pushTaskFunc([this] {
		this->scan();
	});
}
