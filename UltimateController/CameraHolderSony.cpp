#include "CameraHolderSony.h"
#include <iostream>
#include "Text.h"
#include "MessageDefine.h"
#include "SonyCommon.h"
#include "CrDeviceProperty.h"

using namespace cli;



CameraHolderSony::CameraHolderSony(SCRSDK::ICrCameraObjectInfo* camInfo, const QString& id)
    : ICameraHolderEntry(id, CameraBrand::CC_BRAND_SONY)
{
	m_camInfo = SCRSDK::CreateCameraObjectInfo(
        camInfo->GetName(),
        camInfo->GetModel(),
        camInfo->GetUsbPid(),
        camInfo->GetIdType(),
        camInfo->GetIdSize(),
        camInfo->GetId(),
        camInfo->GetConnectionTypeName(),
        camInfo->GetAdaptorName(),
        camInfo->GetPairingNecessity()
	);
}

CameraHolderSony::~CameraHolderSony()
{
    if (m_camInfo)
    {
        m_camInfo->Release();
    }

    disconnect();
}

QString CameraHolderSony::getBrandStr() const
{
    return "Sony";
}

bool CameraHolderSony::setLiveView(bool isView)
{
	int value = isView ? SCRSDK::CrLiveView_Enable : SCRSDK::CrLiveView_Disable;

	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveViewStatus);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt8);

	auto rlt = setProperty(prop);
	if (rlt)
	{
		m_isLiveView = isView;
	}

	return rlt;
}

bool CameraHolderSony::getLiveView() const
{
	uint16_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveViewStatus, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return (value == SCRSDK::CrLiveView_Enable);
}

uint8_t* CameraHolderSony::getLiveViewBuffer(uint32_t* dataSize)
{
	if (!m_isLiveView)
		return nullptr;

    m_mutex.lock();

    SCRSDK::CrImageInfo pInfo;
    SCRSDK::GetLiveViewImageInfo(m_sonyHandler, &pInfo);
	auto bufSize = pInfo.GetBufferSize();
	CrInt8u* pJpegData = nullptr;

	if (bufSize > 0)
	{
		SCRSDK::CrImageDataBlock* pLiveViewImage = new SCRSDK::CrImageDataBlock();
		pLiveViewImage->SetSize(bufSize);

		CrInt8u* recvBuffer = new CrInt8u[bufSize];
		pLiveViewImage->SetData(recvBuffer);

		auto err = SCRSDK::GetLiveViewImage(m_sonyHandler, pLiveViewImage);
		if (CR_FAILED(err))
		{
			if (err == SCRSDK::CrWarning_Frame_NotUpdated) {
				std::cout << "Warning. GetLiveView Frame NotUpdate\n";
			}
			else if (err == SCRSDK::CrError_Memory_Insufficient) {
				std::cout << "Warning. GetLiveView Memory insufficient\n";
			}
		}
		else
		{
			CrInt32u size = pLiveViewImage->GetImageSize();
			*dataSize = size;
			//std::cout << "get sony jpeg buffer lDataSize: " << size << std::endl;
			pJpegData = new CrInt8u[size];
			memcpy(pJpegData, pLiveViewImage->GetImageData(), size);
		}

		delete[] recvBuffer;
		delete pLiveViewImage;
		//getFocusInfo();
	}

    m_mutex.unlock();

	return pJpegData;
}

bool CameraHolderSony::setLiveViewImageQuality(int value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int CameraHolderSony::getLiveViewImageQuality() const
{
	uint16_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
	});

	return value;
}

bool CameraHolderSony::setAperture(uint16_t value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

   return setProperty(prop);
}

uint16_t CameraHolderSony::getAperture() const
{
    uint16_t value = 0;

    getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber, [&](SCRSDK::CrDeviceProperty& prop) {
        value = prop.GetCurrentValue();
        });

	return value;
}

bool CameraHolderSony::setIso(uint32_t value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt32Array);

	return setProperty(prop);
}

uint32_t CameraHolderSony::getIso() const
{
	uint16_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setShutterSpeed(int32_t value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt32Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getShutterSpeed() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setShootMode(int32_t mode)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_DriveMode);
    int32_t value = shoot_mode_to_sony(mode);

	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt32Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getShootMode() const
{
    int32_t value = (int32_t)ShootMode::Shoot_Single;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_DriveMode, [&](SCRSDK::CrDeviceProperty& prop) {
		int propertyValue = prop.GetCurrentValue();
		value = shoot_mode_from_sony(propertyValue);
	});

	return value;
}

bool CameraHolderSony::setFocusMode(int32_t mode)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusMode);
	int32_t value = focus_mode_to_sony(mode);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getFocusMode() const
{
	int32_t value = (int32_t)FocusMode::Focus_Unkown;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusMode, [&](SCRSDK::CrDeviceProperty& prop) {
		int propertyValue = prop.GetCurrentValue();
		value = focus_mode_from_sony(propertyValue);
		});

	return value;
}

void CameraHolderSony::setPhotoSavePath(const QString& path)
{
	ICameraHolderEntry::setPhotoSavePath(path);

    if (m_sonyHandler != 0 && !m_photoSavePath.isEmpty())
    {
		auto wPath = cli::toWstring(util::toStr(m_photoSavePath));
		auto suffix = cli::toWstring(util::toStr(getDeviceName()) + "_");
		SCRSDK::CrError err = SCRSDK::SetSaveInfo(m_sonyHandler, (CrChar*)wPath.c_str(), (CrChar*)suffix.c_str(), -1);

        if (CR_FAILED(err))
        {
			std::cout << "SetSaveInfo: error code " << err << cli::get_message_desc(err) << std::endl;
		}
    }

    //return err == SCRSDK::CrError_None;
}

bool CameraHolderSony::setAFAreaPos(float x, float y)
{
    SCRSDK::CrDeviceProperty prop;
    prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusArea);
    prop.SetCurrentValue(SCRSDK::CrFocusArea::CrFocusArea_Flexible_Spot_M);
    prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

    auto err_prop = SCRSDK::SetDeviceProperty(m_sonyHandler, &prop);
    if (CR_FAILED(err_prop))
    {
        std::cout << "Set FocusArea FAILED : " << err_prop << cli::get_message_desc(err_prop) << std::endl;
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

	x *= 639;
	y *= 479;
    executePosXY(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AF_Area_Position, x, y);

	return true;
}

bool CameraHolderSony::getAFAreaPos(float& x, float& y, float& w, float& h)
{
	auto rlt = getLiveViewProperty(SCRSDK::CrLiveViewPropertyCode::CrLiveViewProperty_AF_Area_Position, [&](SCRSDK::CrLiveViewProperty& prop) {
		auto pinfo = (SCRSDK::CrFocusFrameInfo*)(prop.GetValue());

		x = pinfo->xNumerator / (float)pinfo->xDenominator;
		y = pinfo->yNumerator / (float)pinfo->yDenominator;

		w = pinfo->width / (float)pinfo->xDenominator;
		h = pinfo->height / (float)pinfo->yDenominator;

		//std::cout << "Area position:" << "(" << x << ", " << y << ")" << std::endl;
	});

	return rlt;
}

bool CameraHolderSony::setExposureProgramMode(int mode)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ExposureProgramMode);
	prop.SetCurrentValue(mode);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int CameraHolderSony::getExposureProgramMode() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ExposureProgramMode, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
	});

	return value;
}

bool CameraHolderSony::setFELock(int32_t value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FEL);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getFELock() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FEL, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setAWBLock(bool isLock)
{
	int value = isLock ? SCRSDK::CrLockIndicator_Unlocked : SCRSDK::CrLockIndicator_Locked;
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AWBL);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

bool CameraHolderSony::isAWBLock() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AWBL, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return (value == SCRSDK::CrLockIndicator_Unlocked);
}

bool CameraHolderSony::setWhiteBalance(int value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_WhiteBalance);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int CameraHolderSony::getWhiteBalance() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_WhiteBalance, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::zoomWide()
{
	return setZoomValue(SCRSDK::CrZoomOperation_Wide);
}

bool CameraHolderSony::zoomTele()
{
	return setZoomValue(SCRSDK::CrZoomOperation_Tele);
}

bool CameraHolderSony::zoomStop()
{
	return setZoomValue(SCRSDK::CrZoomOperation_Stop);
}

bool CameraHolderSony::setZoomValue(int value)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Operation);
	prop.SetCurrentValue(value);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int CameraHolderSony::getZoomValue() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Operation, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setImageSize(int32_t size)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ImageSize);
	prop.SetCurrentValue(size);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getImageSize() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ImageSize, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setImageRatio(int32_t size)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AspectRatio);
	prop.SetCurrentValue(size);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getImageRatio() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AspectRatio, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::setRAWFileCompressionType(int32_t format)
{
	SCRSDK::CrDeviceProperty prop;
	prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_RAW_FileCompressionType);
	prop.SetCurrentValue(format);
	prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16Array);

	return setProperty(prop);
}

int32_t CameraHolderSony::getRAWFileCompressionType() const
{
	int32_t value = 0;

	getProperty(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_RAW_FileCompressionType, [&](SCRSDK::CrDeviceProperty& prop) {
		value = prop.GetCurrentValue();
		});

	return value;
}

bool CameraHolderSony::connect()
{
    SCRSDK::CrError err = SCRSDK::Connect(m_camInfo, this, (SCRSDK::CrDeviceHandle*)&m_sonyHandler);

    auto rlt = CR_SUCCEEDED(err);
    if (!rlt)
    {
        std::cout << "Sony connect error, Error Code " << err << cli::get_message_desc(err) << std::endl;
    }

    return rlt;
}

bool CameraHolderSony::disconnect()
{
    bool rlt = true;
    if (m_sonyHandler != 0)
    {
        SCRSDK::CrError errCode = SCRSDK::Disconnect(m_sonyHandler);
        rlt = CR_SUCCEEDED(errCode);
		if (!rlt)
		{
			std::cout << "Sony disconnect error, Error Code " << errCode << cli::get_message_desc(errCode) << std::endl;
		}
    }

    return true;
}

bool CameraHolderSony::isConnected() const
{
    return m_connected;
}

bool CameraHolderSony::release()
{
    auto finalize_status = SCRSDK::ReleaseDevice(m_sonyHandler);
    m_sonyHandler = 0;
	if (CR_FAILED(finalize_status)) {
		std::cout << "Finalize device failed to initialize." << " error code " << finalize_status << std::endl;
		return false;
	}

    return true;
}

void CameraHolderSony::shutterToHalf()
{
    m_mutex.lock();

    SCRSDK::CrDeviceProperty prop;
    prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_S1);
    prop.SetCurrentValue(SCRSDK::CrLockIndicator::CrLockIndicator_Locked);
    prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16);
    SCRSDK::SetDeviceProperty(m_sonyHandler, &prop);

    m_mutex.unlock();
}

void CameraHolderSony::shutterToRelease()
{
    m_mutex.lock();

    SCRSDK::CrDeviceProperty prop;
    prop.SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_S1);
    prop.SetCurrentValue(SCRSDK::CrLockIndicator::CrLockIndicator_Unlocked);
    prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt16);
    SCRSDK::SetDeviceProperty(m_sonyHandler, &prop);
    m_mutex.unlock();
}

bool CameraHolderSony::captureImage()
{
	shootOne();

	return true;
}

void CameraHolderSony::shootOne()
{
    if (getFocusMode() == (int32_t)FocusMode::Focus_MF)
    {
		captureOne();
    }
    else
    {
        // focus first
        shutterToHalf();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		captureOne();

		shutterToRelease();
	}
}

bool CameraHolderSony::captureDown()
{
    auto errCode = SCRSDK::SendCommand(m_sonyHandler, SCRSDK::CrCommandId::CrCommandId_Release, SCRSDK::CrCommandParam_Down);
	return CR_SUCCEEDED(errCode);
}

bool CameraHolderSony::captureUp()
{
	auto errCode = SCRSDK::SendCommand(m_sonyHandler, SCRSDK::CrCommandId::CrCommandId_Release, SCRSDK::CrCommandParam_Up);
	return CR_SUCCEEDED(errCode);
}

void CameraHolderSony::updateCameraStatus()
{
    SCRSDK::CrDeviceProperty* prop = new SCRSDK::CrDeviceProperty();
    CrInt32 numofProperties = 0;
    //prop->SetCode(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AF_Area_Position);

    SCRSDK::GetDeviceProperties(m_sonyHandler, &prop, &numofProperties);

    if (prop)
    {
        for (CrInt32 n = 0; n < numofProperties; n++)
        {
            switch (prop[n].GetCode())
            {
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusIndication:
            {
				auto currentvalue = static_cast<SCRSDK::CrFocusIndicator>(prop[n].GetCurrentValue());

                switch (currentvalue)
                {
                case SCRSDK::CrFocusIndicator_Unlocked:
					m_focusStatus = FocusStatus::NoFocus;
                    break;
                case SCRSDK::CrFocusIndicator_Focused_AF_S:
				case SCRSDK::CrFocusIndicator_Focused_AF_C:
				case SCRSDK::CrFocusIndicator_TrackingSubject_AF_C:
					m_focusStatus = FocusStatus::FocusSucess;
                    break;
                case SCRSDK::CrFocusIndicator_NotFocused_AF_S:
				case SCRSDK::CrFocusIndicator_NotFocused_AF_C:
					m_focusStatus = FocusStatus::FocusFail;
                    break;
                default:
                    break;
                }
            }
            break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusMode:
            {
                CrInt16u currentvalue = static_cast<CrInt16u>(prop[n].GetCurrentValue());
                //std::cout << "FocusMode: " << currentvalue << std::endl;
                switch (currentvalue)
                {
                case SCRSDK::CrFocusMode::CrFocus_MF:
					m_focusMode = FocusMode::Focus_MF;
                    break;
                case SCRSDK::CrFocusMode::CrFocus_AF_S:
					m_focusMode = FocusMode::Focus_AF_S;
                    break;
                case SCRSDK::CrFocusMode::CrFocus_AF_C:
					m_focusMode = FocusMode::Focus_AF_C;
                    break;
                default:
                    break;
                }
            }
            break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusArea:
            {
                CrInt16u currentvalue = static_cast<CrInt16u>(prop[n].GetCurrentValue());
                //std::cout << "FocusArea: " << currentvalue << std::endl;
            }
            break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AF_Area_Position:
			{
				auto currentvalue = prop[n].GetCurrentValue();
				std::cout << "FocusAreaPosition: " << currentvalue << std::endl;
			}
				break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_StillImageStoreDestination:
            {
                CrInt16u currentvalue = static_cast<CrInt16u>(prop[n].GetCurrentValue());
                //std::cout << "StillImageStoreDestination: " << currentvalue << std::endl;
            }
            break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed:
            {
                //int nval = prop[n].GetValueSize() / sizeof(std::uint32_t);
                //std::cout << "ShutterSpeed nval: " << nval << std::endl;
                uint32_t currentvalue = static_cast<std::uint32_t>(prop[n].GetCurrentValue());
                //m_shutterSpeed = format_shutter_speed(currentvalue);
                //std::cout << "ShutterSpeed: " << format_shutter_speed(currentvalue) << std::endl;
            }
            break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_DriveMode:
            {
                //CrInt16u currentvalue = static_cast<CrInt16u>(prop[n].GetCurrentValue());
                //std::cout << "DriveMode: " << currentvalue << std::endl;
                uint32_t currentvalue = static_cast<std::uint32_t>(prop[n].GetCurrentValue());
                //std::cout << "DriveMode: " << currentvalue << std::endl;
            }
            break;
            case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_AspectRatio:
            {
				m_aspectRatio = static_cast<CrInt16u>(prop[n].GetCurrentValue());
#if 0
                switch (currentvalue)
                {
                case SCRSDK::CrAspectRatioIndex::CrAspectRatio_3_2:
                    m_aspectRatio = AspectRatio::AspectRatio_3_2;
                    break;
                case SCRSDK::CrAspectRatioIndex::CrAspectRatio_16_9:
                    m_aspectRatio = AspectRatio::AspectRatio_16_9;
                    break;
                case SCRSDK::CrAspectRatioIndex::CrAspectRatio_4_3:
                    m_aspectRatio = AspectRatio::AspectRatio_4_3;
                    break;
                case SCRSDK::CrAspectRatioIndex::CrAspectRatio_1_1:
                    m_aspectRatio = AspectRatio::AspectRatio_1_1;
                    break;
                default:
                    break;
                }
#endif
            }
            break;
            default:
                break;
            }
        }

        //std::cout << std::endl;
    }
    SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);
}

void CameraHolderSony::updateProperties()
{
	load_properties();

#if 0
	m_propTab.compression_file_format.writable = true;

	std::vector<int32_t> listCapMode{
		(int32_t)SCRSDK::CrRAWFileCompressionType::CrRAWFile_Uncompression,
		(int32_t)SCRSDK::CrRAWFileCompressionType::CrRAWFile_Compression,
	};

	m_propTab.compression_file_format.possible.swap(listCapMode);
#endif
}


float CameraHolderSony::getAspectRatioValue() const
{
	float ratio = 1.0f;
	switch (m_aspectRatio)
	{
	case SCRSDK::CrAspectRatioIndex::CrAspectRatio_3_2:
		ratio = 1.5f;
		break;
	case SCRSDK::CrAspectRatioIndex::CrAspectRatio_16_9:
		ratio = 1.7778f;
		break;
	case SCRSDK::CrAspectRatioIndex::CrAspectRatio_4_3:
		ratio = 1.3333f;
		break;
	case SCRSDK::CrAspectRatioIndex::CrAspectRatio_1_1:
		ratio = 1.0f;
		break;
	default:
		break;
	}

	return ratio;
}

bool CameraHolderSony::getProperty(SCRSDK::CrDeviceProperty& value) const
{
	SCRSDK::CrDeviceProperty* prop = nullptr;
	CrInt32 numofProperties = 0;
	SCRSDK::GetDeviceProperties(m_sonyHandler, &prop, &numofProperties);
	if (prop)
	{
		for (CrInt32 n = 0; n < numofProperties; n++)
		{
			if (value.GetCode() == prop[n].GetCode())
			{
                value = prop[n];

				SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);

				return true;
			}
		}

		SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);
	}

	return false;
}

bool CameraHolderSony::getProperty(SCRSDK::CrDevicePropertyCode propertyCode, PROPERTY_SET_CK callback) const
{
	SCRSDK::CrDeviceProperty* prop = nullptr;
    uint32_t errorCode;
	CrInt32 numofProperties = 0;
    if (propertyCode == SCRSDK::CrDeviceProperty_Undefined)
    {
		SCRSDK::GetDeviceProperties(m_sonyHandler, &prop, &numofProperties);
	}
    else
    {
		SCRSDK::GetSelectDeviceProperties(m_sonyHandler, propertyCode, &errorCode, &prop, &numofProperties);
    }

	if (prop)
	{
		for (CrInt32 n = 0; n < numofProperties; n++)
		{
			if (propertyCode == prop[n].GetCode())
			{

                if (callback)
                {
                    callback(prop[n]);
                }

				SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);

				return true;
			}
		}

		SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);
	}

	return false;
}

bool CameraHolderSony::getLiveViewProperty(SCRSDK::CrLiveViewPropertyCode propertyCode, LIVEVIEW_PROP_SET_CK callback) const
{
	SCRSDK::CrLiveViewProperty* prop = nullptr;
	uint32_t errorCode;
	CrInt32 numofProperties = 0;
#if 1
	SCRSDK::GetLiveViewProperties(m_sonyHandler, &prop, &numofProperties);
#else
	SCRSDK::GetSelectLiveViewProperties(m_sonyHandler, propertyCode, &errorCode, &prop, &numofProperties);
#endif

	if (prop)
	{
		for (CrInt32 n = 0; n < numofProperties; n++)
		{
			if (propertyCode == prop[n].GetCode())
			{

				if (callback)
				{
					callback(prop[n]);
				}

				SCRSDK::ReleaseLiveViewProperties(m_sonyHandler, prop);

				return true;
			}
		}

		SCRSDK::ReleaseLiveViewProperties(m_sonyHandler, prop);
	}

	return false;
}

bool CameraHolderSony::setProperty(SCRSDK::CrDeviceProperty& value) const
{
	auto errorCode = SCRSDK::SetDeviceProperty(m_sonyHandler, &value);
	if (CR_FAILED(errorCode))
	{
        std::cout << "Set property eror: " << errorCode << cli::get_message_desc(errorCode) << std::endl;
	}

    return CR_SUCCEEDED(errorCode);
}

void CameraHolderSony::setProperty(SCRSDK::CrDevicePropertyCode code, unsigned int val, SCRSDK::CrDataType dataType)
{
    int retryTimes = 0;
    int maxRetryTimes = 5;

    while (retryTimes < maxRetryTimes && !compareProperty(code, val))
    {
        std::cout << "set property times: " << retryTimes << std::endl;
        SCRSDK::CrDeviceProperty prop;
        prop.SetCode(code);
        prop.SetCurrentValue(static_cast<CrInt64u>(val));
        prop.SetValueType(dataType);
        SCRSDK::CrError err = SCRSDK::SetDeviceProperty(m_sonyHandler, &prop);
        if (err == SCRSDK::CrError_None)
        {
            return;
        }

        retryTimes++;
    }
}

bool CameraHolderSony::compareProperty(SCRSDK::CrDevicePropertyCode code, unsigned int val)
{
    SCRSDK::CrDeviceProperty* prop = nullptr;
    CrInt32 numofProperties = 0;
    SCRSDK::GetDeviceProperties(m_sonyHandler, &prop, &numofProperties);
    if (prop)
    {
        for (CrInt32 n = 0; n < numofProperties; n++)
        {
            if (code == prop[n].GetCode())
            {
                CrInt32u currentvalue = static_cast<std::uint32_t>(prop[n].GetCurrentValue());
                std::cout << "compare [" << currentvalue << "]-[" << val << "]" << std::endl;

                SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);
                
                return (currentvalue == val);
            }
        }

		SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop);
    }

    return false;
}

void CameraHolderSony::captureOne()
{
	captureDown();
	// Wait, then send shutter up
	std::this_thread::sleep_for(std::chrono::milliseconds(35));
	captureUp();
}

void CameraHolderSony::executePosXY(CrInt16u code, int x, int y)
{
    std::cout << "in " << x << "  " << y << std::endl;

    if (x < 0 || x > 639 || y < 0 || y > 479)
    {
        return;
    }

    int x_y = x << 16 | y;

    SCRSDK::CrDeviceProperty prop;
    prop.SetCode(code);
    prop.SetCurrentValue(x_y);
    prop.SetValueType(SCRSDK::CrDataType::CrDataType_UInt32);

    SCRSDK::SetDeviceProperty(m_sonyHandler, &prop);
}


void CameraHolderSony::load_properties(CrInt32u num, CrInt32u* codes)
{
	std::int32_t nprop = 0;
	SCRSDK::CrDeviceProperty* prop_list = nullptr;

	//m_propTab.media_slot1_quick_format_enable_status.writable = false;
	//m_propTab.media_slot2_quick_format_enable_status.writable = false;

	SCRSDK::CrError status = SCRSDK::CrError_Generic;
	if (0 == num) {
		// Get all
		status = SCRSDK::GetDeviceProperties(m_sonyHandler, &prop_list, &nprop);
	}
	else {
		// Get difference
		status = SCRSDK::GetSelectDeviceProperties(m_sonyHandler, num, codes, &prop_list, &nprop);
	}

	if (CR_FAILED(status)) {
		std::cout << "Failed to get device properties.\n";
		return;
	}

	if (prop_list && nprop > 0) {
		// Got properties list
		for (std::int32_t i = 0; i < nprop; ++i) {
			auto prop = prop_list[i];
			int nval = 0;

			switch (prop.GetCode()) {
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_SdkControlMode:
				m_propTab.sdk_mode.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.sdk_mode.current = static_cast<std::uint32_t>(prop.GetCurrentValue());
				//m_modeSDK = (SCRSDK::CrSdkControlMode)m_propTab.sdk_mode.current;
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.f_number.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.f_number.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_f_number(prop.GetValues(), nval);
					m_propTab.f_number.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity:
				nval = prop.GetValueSize() / sizeof(std::uint32_t);
				m_propTab.iso_sensitivity.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.iso_sensitivity.current = static_cast<std::uint32_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_iso_sensitivity(prop.GetValues(), nval);
					m_propTab.iso_sensitivity.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed:
				nval = prop.GetValueSize() / sizeof(std::uint32_t);
				m_propTab.shutter_speed.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.shutter_speed.current = static_cast<std::uint32_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_shutter_speed(prop.GetValues(), nval);
					m_propTab.shutter_speed.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_PriorityKeySettings:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.position_key_setting.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.position_key_setting.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (nval != m_propTab.position_key_setting.possible.size()) {
					auto parsed_values = parse_position_key_setting(prop.GetValues(), nval);
					m_propTab.position_key_setting.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ExposureProgramMode:
				nval = prop.GetValueSize() / sizeof(std::uint32_t);
				m_propTab.exposure_program_mode.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.exposure_program_mode.current = static_cast<std::uint32_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_exposure_program_mode(prop.GetValues(), nval);
					m_propTab.exposure_program_mode.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_DriveMode:
				nval = prop.GetValueSize() / sizeof(std::uint32_t);
				m_propTab.still_capture_mode.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.still_capture_mode.current = static_cast<std::uint32_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_still_capture_mode(prop.GetValues(), nval);
					m_propTab.still_capture_mode.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusMode:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.focus_mode.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.focus_mode.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_focus_mode(prop.GetValues(), nval);
					m_propTab.focus_mode.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusArea:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.focus_area.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.focus_area.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_focus_area(prop.GetValues(), nval);
					m_propTab.focus_area.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.live_view_image_quality.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.live_view_image_quality.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto view = parse_live_view_image_quality(prop.GetValues(), nval);
					m_propTab.live_view_image_quality.possible.swap(view);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveViewStatus:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.live_view_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.live_view_status.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.media_slot1_full_format_enable_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.media_slot1_full_format_enable_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.media_slot1_full_format_enable_status.possible.size()) {
					auto parsed_values = parse_media_slotx_format_enable_status(prop.GetValues(), nval);
					m_propTab.media_slot1_full_format_enable_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.media_slot2_full_format_enable_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.media_slot2_full_format_enable_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.media_slot2_full_format_enable_status.possible.size()) {
					auto parsed_values = parse_media_slotx_format_enable_status(prop.GetValues(), nval);
					m_propTab.media_slot2_full_format_enable_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.media_slot1_quick_format_enable_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.media_slot1_quick_format_enable_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.media_slot1_quick_format_enable_status.possible.size()) {
					auto parsed_values = parse_media_slotx_format_enable_status(prop.GetValues(), nval);
					m_propTab.media_slot1_quick_format_enable_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.media_slot2_quick_format_enable_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.media_slot2_quick_format_enable_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.media_slot2_quick_format_enable_status.possible.size()) {
					auto parsed_values = parse_media_slotx_format_enable_status(prop.GetValues(), nval);
					m_propTab.media_slot2_quick_format_enable_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_WhiteBalance:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.white_balance.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.white_balance.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_white_balance(prop.GetValues(), nval);
					m_propTab.white_balance.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_CustomWB_Capture_Standby:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.customwb_capture_stanby.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.customwb_capture_stanby.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (nval != m_propTab.white_balance.possible.size()) {
					auto parsed_values = parse_customwb_capture_stanby(prop.GetValues(), nval);
					m_propTab.customwb_capture_stanby.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.customwb_capture_stanby_cancel.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.customwb_capture_stanby_cancel.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (nval != m_propTab.customwb_capture_stanby_cancel.possible.size()) {
					auto parsed_values = parse_customwb_capture_stanby_cancel(prop.GetValues(), nval);
					m_propTab.customwb_capture_stanby_cancel.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_CustomWB_Capture_Operation:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.customwb_capture_operation.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.customwb_capture_operation.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_customwb_capture_operation(prop.GetValues(), nval);
					m_propTab.customwb_capture_operation.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_CustomWB_Execution_State:
				nval = prop.GetValueSize() / sizeof(std::uint16_t);
				m_propTab.customwb_capture_execution_state.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.customwb_capture_execution_state.current = static_cast<std::uint16_t>(prop.GetCurrentValue());
				if (nval != m_propTab.customwb_capture_execution_state.possible.size()) {
					auto parsed_values = parse_customwb_capture_execution_state(prop.GetValues(), nval);
					m_propTab.customwb_capture_execution_state.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Operation_Status:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.zoom_operation_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.zoom_operation_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.zoom_operation_status.possible.size()) {
					auto parsed_values = parse_zoom_operation_status(prop.GetValues(), nval);
					m_propTab.zoom_operation_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Setting:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.zoom_setting_type.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.zoom_setting_type.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_zoom_setting_type(prop.GetValues(), nval);
					m_propTab.zoom_setting_type.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Type_Status:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.zoom_types_status.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.zoom_types_status.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.zoom_types_status.possible.size()) {
					auto parsed_values = parse_zoom_types_status(prop.GetValues(), nval);
					m_propTab.zoom_types_status.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Operation:
				nval = prop.GetValueSize() / sizeof(std::int8_t);
				m_propTab.zoom_operation.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.zoom_operation.current = static_cast<std::int8_t>(prop.GetCurrentValue());
				if (nval != m_propTab.zoom_operation.possible.size()) {
					auto parsed_values = parse_zoom_operation(prop.GetValues(), nval);
					m_propTab.zoom_operation.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Zoom_Speed_Range:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.zoom_speed_range.writable = prop.IsSetEnableCurrentValue();
				if (0 < nval) {
					auto parsed_values = parse_zoom_speed_range(prop.GetValues(), nval);
					m_propTab.zoom_speed_range.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ZoomAndFocusPosition_Save:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.save_zoom_and_focus_position.writable = prop.IsSetEnableCurrentValue();
				if (0 < nval) {
					auto parsed_values = parse_save_zoom_and_focus_position(prop.GetValues(), nval);
					m_propTab.save_zoom_and_focus_position.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ZoomAndFocusPosition_Load:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.load_zoom_and_focus_position.writable = prop.IsSetEnableCurrentValue();
				if (0 < nval) {
					auto parsed_values = parse_load_zoom_and_focus_position(prop.GetValues(), nval);
					m_propTab.load_zoom_and_focus_position.possible.swap(parsed_values);
				}
				break;
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Remocon_Zoom_Speed_Type:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.remocon_zoom_speed_type.writable = prop.IsSetEnableCurrentValue();
				//m_propTab.remocon_zoom_speed_type.current = static_cast<std::uint8_t>(prop.GetCurrentValue());
				if (0 < nval) {
					auto parsed_values = parse_remocon_zoom_speed_type(prop.GetValues(), nval);
					m_propTab.remocon_zoom_speed_type.possible.swap(parsed_values);
				}
				break;

			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusIndication:
			{
				auto currentvalue = static_cast<SCRSDK::CrFocusIndicator>(prop.GetCurrentValue());

				if (currentvalue == SCRSDK::CrFocusIndicator_Focused_AF_S)
				{
					m_focusStatus = FocusStatus::FocusSucess;
				}
				else if (currentvalue == SCRSDK::CrFocusIndicator_NotFocused_AF_S)
				{
					m_focusStatus = FocusStatus::FocusFail;
				}
				else
				{
					m_focusStatus = FocusStatus::NoFocus;
				}

				focusStatusChanged(this, m_focusStatus);
				break;
			}
			case SCRSDK::CrDevicePropertyCode::CrDeviceProperty_RAW_FileCompressionType:
				nval = prop.GetValueSize() / sizeof(std::uint8_t);
				m_propTab.compression_file_format.writable = prop.IsSetEnableCurrentValue();
				if (0 < nval) {
					auto parsed_values = parse_property_item_list<uint16_t, int32_t>(prop.GetValues(), nval);
					m_propTab.compression_file_format.possible.swap(parsed_values);
				}
				break;
			default:
				break;
			}
		}
		SCRSDK::ReleaseDeviceProperties(m_sonyHandler, prop_list);
	}
}



/***回调函数****/
void CameraHolderSony::OnConnected(SCRSDK::DeviceConnectionVersioin version)
{
    m_connected = true;

    setPhotoSavePath(m_photoSavePath);

    statusChangedSignal(this, m_connected);

    std::cout << "Connected: " << util::toStr(getDeviceName()) << " version:" << version << std::endl;
}

void CameraHolderSony::OnDisconnected(CrInt32u error)
{
	m_connected = false;

	statusChangedSignal(this, m_connected);
	std::cout << "Disconnected: " << util::toStr(getDeviceName()) << " error:" << error << cli::get_message_desc(error) << std::endl;
}

void CameraHolderSony::OnPropertyChanged()
{
    //std::cout << "Property changed." << std::endl;
    updateCameraStatus();

	emit propertyChangeSignal(this);
}

void CameraHolderSony::OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
	std::cout << "Property changed:" << num << " " << *codes << cli::get_message_desc(*codes) << std::endl;
	load_properties(num, codes);
}

void CameraHolderSony::OnLvPropertyChanged()
{
}

void CameraHolderSony::OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
}

void CameraHolderSony::OnCompleteDownload(CrChar* filename)
{
    //std::cout << "download: " << filename << std::endl;
}

void CameraHolderSony::OnNotifyContentsTransfer(CrInt32u notify, SCRSDK::CrContentHandle handle, CrChar* filename)
{
}

void CameraHolderSony::OnWarning(CrInt32u warning)
{
    std::cout << "sony camera callback warning: " << warning << cli::get_message_desc(warning) << std::endl;
    if (SCRSDK::CrWarning_Exposure_Started == warning)
    {
        std::cout << "Exposure_Started: " << std::endl;
    }
}

void CameraHolderSony::OnError(CrInt32u error)
{
    std::cout << "sony camera callback error: " << error << cli::get_message_desc(error) << std::endl;
}

CameraControllerSony::CameraControllerSony()
    : IEntityCameraController(CameraBrand::CC_BRAND_SONY)
{
}

bool CameraControllerSony::init()
{
	auto rlt = true;
	if (!isInit())
    {
		auto rlt = SCRSDK::Init(0);
		setIsInit(rlt);
    }

    return rlt;
}

bool CameraControllerSony::release()
{
	IEntityCameraController::release();
	auto rlt = true;
	if (isInit())
	{
		rlt = SCRSDK::Release();
		setIsInit(!rlt);
	}

    return rlt;
}

bool CameraControllerSony::scan()
{
	if (!isInit())
	{
		std::cout << "sony sdk init failed" << std::endl;
		return false;
	}
	SCRSDK::ICrEnumCameraObjectInfo* sdk_sony = nullptr;

	SCRSDK::CrError err = SCRSDK::EnumCameraObjects(&sdk_sony);
	if (sdk_sony == NULL)
	{
		std::cout << "sony camera not found" << std::endl;
		return false;
	}

	CrInt32u cntOfCamera = sdk_sony->GetCount();

	std::cout << "sony dect succ: " << cntOfCamera << std::endl;

    std::list<QString> camNameList;

	for (CrInt32u n = 0; n < cntOfCamera; n++)
	{
		SCRSDK::ICrCameraObjectInfo* pobj = (SCRSDK::ICrCameraObjectInfo*)(sdk_sony->GetCameraObjectInfo(n));
		auto deviceId = std::string("Sony_") + std::string((char*)pobj->GetId(), strlen((char*)pobj->GetId()));

        camNameList.emplace_back(std::move(util::toQStr(deviceId)));
	}

    updateCameraDevice(camNameList);

	for (CrInt32u n = 0; n < cntOfCamera; n++)
	{
		SCRSDK::ICrCameraObjectInfo* pobj = (SCRSDK::ICrCameraObjectInfo*)(sdk_sony->GetCameraObjectInfo(n));

		std::cout << "name: " << cli::toString((wchar_t*)pobj->GetName()) << std::endl;
		std::cout << "GetAdaptorName: " << cli::toString((wchar_t*)pobj->GetAdaptorName()) << std::endl;
		std::cout << "GetId: " << cli::toString((wchar_t*)pobj->GetId()) << std::endl;
		std::cout << "GetGuid: " << cli::toString((wchar_t*)pobj->GetGuid()) << std::endl;

		auto id = cli::toString((wchar_t*)pobj->GetId());
		auto deviceName = std::string("Sony_") + cli::toString((wchar_t*)pobj->GetModel()) + "(" + id + ")";
		auto deviceId = std::string("Sony_") + id;
        auto cameraHold = findCameraDeviceFromId(util::toQStr(deviceId));

        if (!cameraHold)
        {
			// create camera holder
			auto cam = std::make_shared<CameraHolderSony>(pobj, util::toQStr(deviceId));

			cam->setDeviceName(util::toQStr(deviceName));
			cam->connect();
			cam->setPhotoSavePath(getPhotoSavePath());

			addCameraDevice(cam);
        }
	}

    sdk_sony->Release();

    return true;
}


