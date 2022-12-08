#include "CameraHolderFuji.h"
#include "FujiSDK.h"
#include "GFX100S.h"
#include "XAPIOpt.H"
#include "FujiCommon.h"
#include "CommandTaskThread.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <QTime>
#include <QtDebug>
#include <time.h>
#include <QFile>
#include <QDir>
#include <QMetaObject>
#include <QLockFile>


SaveImageTask::SaveImageTask(const QString& filePath, const QString& name, uint8_t* data, uint64_t size)
{
	m_filePath = filePath;
	m_fileName = name;
	m_data = data;
	m_size = size;
}

SaveImageTask::~SaveImageTask()
{
	SAFE_DELETE_ARRAY(m_data);
}

void SaveImageTask::run() 
{
	m_saveResult = saveImage();
}

bool SaveImageTask::saveImage()
{
	if (m_filePath.isEmpty() || m_fileName.isEmpty())
		return false;

	QDir dir(m_filePath);
	if (!dir.exists())
	{
		dir.mkpath(m_filePath);
	}
	auto filePath = QString();
	QFileInfo fileInfo(m_fileName);
	auto name = fileInfo.baseName();
	auto suffix = fileInfo.suffix();

	auto i = 1;
	while (true)
	{
		filePath = m_filePath + "/" + QString("%1_%2.%3").arg(name).arg(i).arg(suffix);
		if (!QFile::exists(filePath))
		{
			break;
		}

		++i;
	}

	QLockFile lockFile(filePath + "_lock");
	lockFile.lock();

	QFile file(filePath);
	if (!file.open(QFile::WriteOnly))
	{
		lockFile.unlock();
		qCritical() << "save image error:" << filePath;
		return false;
	}

	auto size = file.write((char*)m_data, m_size);
	if (size != m_size)
	{
		qCritical() << "save image size error:" << filePath;
	}

	file.flush();
	file.close();
	lockFile.unlock();

	return true;
}


#define FUJI_SUCCESS(errCode) (XSDK_COMPLETE == (errCode))
#define FUJI_FAIL(errCode) (XSDK_ERROR == (errCode))


// 属性参数列表宏
#define CAP_PROP_LIST(apiA, apiB, prop, propType) \
{ \
auto errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, apiA, apiB, &valueCount, plNumValue);\
if (FUJI_SUCCESS(errCode)) \
{ \
	auto plNumValue = new long[valueCount]; \
	auto errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, apiA, apiB, &valueCount, plNumValue);\
	if (FUJI_SUCCESS(errCode)) \
	{ \
		prop.writable = true; \
		prop.possible = fuji::parse_paragmeter<propType>(plNumValue, valueCount); \
	} \
	SAFE_DELETE_ARRAY(plNumValue); \
} \
else \
{ \
	printError();\
}\
}

//属性函数列表宏
#define CAP_PROP_FUNC_LIST(func, prop, propType) \
{ \
	errCode = m_sdk_fuji->func((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);\
	if (FUJI_SUCCESS(errCode))\
	{\
		plNumValue = new long[valueCount];\
		errCode = m_sdk_fuji->func((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);\
		if (FUJI_SUCCESS(errCode))\
		{\
			prop.writable = true;\
			prop.possible = fuji::parse_paragmeter<propType>(plNumValue, valueCount);\
		}\
		SAFE_DELETE_ARRAY(plNumValue);\
	}\
}


// 设置参数函数宏
#define SET_PROP_FUNC(apiA, apiB, value) \
{\
	if (d_ptr->handler == nullptr) \
	return false; \
	std::lock_guard guard(m_mutex); \
	if (m_isLiveView)\
		stopLiveView(); \
	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, apiA, apiB, (long)value); \
	auto rlt = FUJI_SUCCESS(errCode); \
	if (!rlt)\
		printError(); \
	return rlt; \
}

// 设置具体的参数函数宏
#define SET_SPECIFIC_PROP_FUNC(func, value) \
{ \
if (d_ptr->handler == nullptr) \
return false;\
std::lock_guard guard(m_mutex);\
if (m_isLiveView)\
stopLiveView();\
auto errCode = m_sdk_fuji->func((XSDK_HANDLE)d_ptr->handler, value);\
if (FUJI_FAIL(errCode))\
{\
	printError();\
	return false;\
}\
return true;\
}

// 获取参数函数宏
#define GET_PROP_FUNC(apiA, apiB) \
{\
if (d_ptr->handler == nullptr)\
return 0;\
std::lock_guard guard(m_mutex);\
long value = 0;\
auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, apiA, apiB, &value);\
if (FUJI_FAIL(errCode))\
{\
	printError();\
}\
return value;\
}

// 获取参数
#define GET_PROP_FUNC_BOOL(apiA, apiB, value) \
{\
if (d_ptr->handler == nullptr)\
return false;\
std::lock_guard guard(m_mutex);\
long lValue = 0;\
auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, apiA, apiB, &lValue);\
auto rlt = FUJI_FAIL(errCode);\
if (rlt)\
{\
	printError();\
}\
else\
{ \
	value = lValue;\
}\
return !rlt;\
}

#define GET_SPECIFIC_PROP_FUNC(func) \
{\
if (d_ptr->handler == nullptr)\
return 0;\
std::lock_guard guard(m_mutex);\
long value = 0;\
auto errCode = m_sdk_fuji->func((XSDK_HANDLE)d_ptr->handler, &value);\
if (FUJI_FAIL(errCode))\
{\
	printError();\
	return false;\
}\
return value;\
}


#define MAX_FOCUS_W 31
#define MAX_FOCUS_H 23

class CameraHolderFujiPrivate
{
public:
	explicit CameraHolderFujiPrivate(CameraHolderFuji* ptr) : q_ptr(ptr) {
		commadTask = new CommandTaskThread();
		saveImageTask = new CommandTaskThread();
	}
	~CameraHolderFujiPrivate()
	{
		delete commadTask;
		delete saveImageTask;
	}

	void updateFocusPoint(int focusPoint)
	{
		switch (focusPoint)
		{
		case SDK_FOCUS_POINTS_13X7:
			focusWidth = 13;
			focusHeight = 7;
			break;
		case SDK_FOCUS_POINTS_25X13:
			focusWidth = 25;
			focusHeight = 13;
			break;
		case SDK_FOCUS_POINTS_13X9:
			focusWidth = 13;
			focusHeight = 9;
			break;
		case SDK_FOCUS_POINTS_25X17:
			focusWidth = 25;
			focusHeight = 17;
			break;
		default:
			break;
		}
	}

	uint8_t* getCameraImageData(uint32_t* dataSize, XSDK_ImageInformation& pImgInfo)
	{
		q_ptr->m_mutex.lock();
		if (handler == nullptr)
		{
			q_ptr->m_mutex.unlock();

			return nullptr;
		}

		auto errCode = q_ptr->m_sdk_fuji->m_pfnReadImageInfo((XSDK_HANDLE)handler, &pImgInfo);
		if (FUJI_FAIL(errCode))
		{
			q_ptr->printError();
		}

		if (FUJI_FAIL(errCode) || pImgInfo.lDataSize < 1)
		{
			if (q_ptr->m_isLiveView && mode == ICameraHolder::PcControl)
				q_ptr->startLiveView();

			q_ptr->m_mutex.unlock();
			
			return nullptr;
		}
		auto t2 = clock();
#if 1
		unsigned long lDataSize = pImgInfo.lDataSize;
		*dataSize = lDataSize;
		uint8_t* buf = new uint8_t[lDataSize];

		errCode = q_ptr->m_sdk_fuji->m_pfnReadImage((XSDK_HANDLE)handler, buf, lDataSize);
		//q_ptr->m_sdk_fuji->LeaveCriticalSection();
		q_ptr->m_mutex.unlock();
#else
		unsigned long lDataSize = pImgInfo.lPreviewSize;
		*dataSize = lDataSize;
		uint8_t* buf = new uint8_t[lDataSize];

		errCode = m_sdk_fuji->m_pfnReadPreview((XSDK_HANDLE)d_ptr->handler, buf, lDataSize);
		m_mutex.unlock();
#endif

		if (FUJI_FAIL(errCode))
		{
			SAFE_DELETE_ARRAY(buf);
			return nullptr;
		}

		return  buf;
	}

	void* handler{ nullptr };

	SDK_FocusArea focusArea{ 0, 0, 3 };

	int focusWidth{ 0 };
	int focusHeight{ 0 };
	int liveViewImageSize{ 0 };
	bool isRunFinished{ false };
	bool isRunThread{ true };
	uint32_t imageSize{ 0 };
	uint8_t* imageData{ nullptr };
	int32_t cameraIndex{ 0 };
	ICameraHolder::ControlMode mode{ ICameraHolder::CameraControl };

	// 电量信息
	long bodyBattery;
	long plGripBatteryInfo;
	long plGripBattery2Info;
	long batteryRatio;
	long plGripBatteryRatio;
	long plGripBattery2Ratio;
	long plBodyBattery2Info;
	long plBodyBattery3Info;
	
	CommandTaskThread* commadTask{ nullptr };
	CommandTaskThread* saveImageTask{  nullptr };

protected:
	CameraHolderFuji* q_ptr;
};


CameraHolderFuji::CameraHolderFuji(CFujiSDK* m_sdk_fuji, const QString& deviceId)
	: ICameraHolderEntry(deviceId, CameraBrand::CC_BRAND_FUJI)
	, m_sdk_fuji(m_sdk_fuji)
	, d_ptr(new CameraHolderFujiPrivate(this))
{
	m_thread = new std::thread(std::bind(&CameraHolderFuji::updateImageThread, this));
	m_thread->detach();
}

CameraHolderFuji::~CameraHolderFuji()
{
	d_ptr->isRunThread = false;
	while (d_ptr->isRunFinished)
	{
		QThread::msleep(5);
	}
	delete m_thread;
	delete d_ptr;
}

QString CameraHolderFuji::getBrandStr() const
{
    return "FUJIFILM";
}

void CameraHolderFuji::setDeviceName(const QString& name)
{
	ICameraHolderEntry::setDeviceName(name);

	std::lock_guard guard(m_mutex);
	if (d_ptr->handler == nullptr)
		return;

	if (m_isLiveView)
		stopLiveView();

	auto newName = name.left(32).toStdString();
	char buf[32] = {0};
	memcpy(buf, newName.c_str(), newName.size());
	auto errCode = m_sdk_fuji->m_pfnWriteDeviceName(d_ptr->handler, buf);
	if (FUJI_FAIL(errCode))
		printError();
}

bool CameraHolderFuji::connect()
{
	long lCapabilityBitmap;
	
	XSDK_HANDLE handle = 0;

	char pDevice[8];
	strcpy_s(pDevice, 8, ("ENUM:" + std::to_string(d_ptr->cameraIndex)).c_str());
	auto errCode = m_sdk_fuji->m_pfnOpenEx(pDevice, &handle, &lCapabilityBitmap, NULL);

	m_isConnected = FUJI_SUCCESS(errCode);
	if (!m_isConnected)
	{
		printError();

		return false;
	}

	d_ptr->handler = handle;

	setControlMode(ICameraHolder::PcControl);

	d_ptr->liveViewImageSize = getLiveViewImageSize();

	auto points = getFocusPoints();
	d_ptr->updateFocusPoint(points);

	updateBatteryInfo();

	setLiveSize(SDK_LIVEVIEW_SIZE_L);

	setLiveView(true);

	QThread::msleep(10);

	updateProperties();

	emit statusChangedSignal(this, m_isConnected);

	return m_isConnected;
}

bool CameraHolderFuji::disconnect()
{
	if (d_ptr->handler == nullptr)
		return true;

	std::lock_guard guard(m_mutex);
	long lCapabilityBitmap;
	auto pDevice = m_deviceId.data();
	auto errCode = m_sdk_fuji->m_pfnClose((XSDK_HANDLE)d_ptr->handler);

	if (FUJI_SUCCESS(errCode))
	{
		d_ptr->handler = nullptr;
		m_isConnected = false;
		emit statusChangedSignal(this, m_isConnected);
	}
	else
	{
		printError();
	}

	return FUJI_SUCCESS(errCode);
}

bool CameraHolderFuji::isConnected() const
{
	return m_isConnected;
}

bool CameraHolderFuji::release()
{
	return true;
}

bool CameraHolderFuji::setLiveView(bool isView)
{
#if 0
	return true;
#endif

	if (d_ptr->handler == nullptr)
		return false;

	bool rlt = false;
	std::lock_guard guard(m_mutex);

	if (isView)
	{
		stopLiveView();
		rlt = startLiveView();
	}
	else
	{
#if 0
		rlt = stopLiveView();
#else
		auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_StopLiveView, GFX100S_API_PARAM_StopLiveView);
		rlt = FUJI_SUCCESS(errCode);
		if (!rlt)
			printError();
#endif
	}

	if (rlt)
		m_isLiveView = isView;

	return rlt;
}

bool CameraHolderFuji::getLiveView() const
{
	return m_isLiveView;
}

uint8_t* CameraHolderFuji::getLiveViewBuffer(uint32_t* dataSize)
{
	uint8_t* imageData = nullptr;

	m_mutexData.lock();
	if (d_ptr->imageData)
	{
		imageData = new uint8_t[d_ptr->imageSize];
		memcpy(imageData, d_ptr->imageData, d_ptr->imageSize);
		*dataSize = d_ptr->imageSize;
	}
	m_mutexData.unlock();

	return imageData;
}

bool CameraHolderFuji::setLiveViewImageQuality(int value)
{
	auto func = [=]() {
		if (d_ptr->handler == nullptr)
			return false;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
			stopLiveView();

		long mode = value;
		auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetLiveViewImageQuality,
			GFX100S_API_PARAM_SetLiveViewImageQuality, mode);
		if (FUJI_FAIL(errCode))
			printError();
		else
		{
			ICameraHolderEntry::setLiveViewImageQuality(value);
		}

		return FUJI_SUCCESS(errCode);
	};
	d_ptr->commadTask->pushTaskFunc(func);

	return true;
}

int CameraHolderFuji::getRealLiveViewImageQuality()
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	long mode = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetLiveViewImageQuality,
		GFX100S_API_PARAM_GetLiveViewImageQuality, &mode);
	if (FUJI_FAIL(errCode))
		printError();

	return mode;
}

bool CameraHolderFuji::setLiveViewImageSize(int32_t size)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetLiveViewImageSize,
		GFX100S_API_PARAM_SetImageSize, size);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getLiveViewImageSize() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetLiveViewImageSize,
		GFX100S_API_PARAM_GetImageSize, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setAperture(uint16_t value)
{
	auto func = [=]() {
		if (d_ptr->handler == nullptr)
			return false;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
			stopLiveView();

		auto errCode = m_sdk_fuji->m_pfnSetAperture((XSDK_HANDLE)d_ptr->handler, value);
		if (FUJI_FAIL(errCode))
		{
			printError();
			return false;
		}
		ICameraHolderEntry::setAperture(value);

		return true;
	};

	d_ptr->commadTask->pushTaskFunc(func);

	return true;
}

uint16_t CameraHolderFuji::getRealAperture() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetAperture((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return 0;
	}

	return value;
}

bool CameraHolderFuji::setIso(uint32_t value)
{
	auto func = [=]() {
		if (d_ptr->handler == nullptr)
			return;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
			stopLiveView();

		auto errCode = m_sdk_fuji->m_pfnSetSensitivity((XSDK_HANDLE)d_ptr->handler, value);
		auto rlt = FUJI_SUCCESS(errCode);
		if (!rlt)
			printError();
		else
		{
			ICameraHolderEntry::setIso(value);
		}
	};

	d_ptr->commadTask->pushTaskFunc(func);

	return true;
}

uint32_t CameraHolderFuji::getRealIso() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetSensitivity((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return false;
	}
	return value;
}

bool CameraHolderFuji::setShutterSpeed(int32_t value)
{
	auto func = [=]() {
		if (d_ptr->handler == nullptr)
			return;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
			stopLiveView();

		auto errCode = m_sdk_fuji->m_pfnSetShutterSpeed((XSDK_HANDLE)d_ptr->handler, value, 0);
		auto rlt = FUJI_SUCCESS(errCode);
		if (!rlt)
			printError();
		else
		{
			ICameraHolderEntry::setShutterSpeed(value);
		}
	};
	d_ptr->commadTask->pushTaskFunc(func);

	return true;
}

int32_t CameraHolderFuji::getRealShutterSpeed() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	long value = 0;
	long plBulb = 0;
	auto errCode = m_sdk_fuji->m_pfnGetShutterSpeed((XSDK_HANDLE)d_ptr->handler, &value, &plBulb);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return 0;
	}

	return value;
}

bool CameraHolderFuji::setShootMode(int32_t mode)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	long shootMode = fuji::shoot_mode_to_fuji(mode);
	auto errCode = m_sdk_fuji->m_pfnSetDriveMode((XSDK_HANDLE)d_ptr->handler, shootMode);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getShootMode() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetDriveMode((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return false;
	}
	return fuji::shoot_mode_from_fuji(value);
}

bool CameraHolderFuji::setFocusMode(int32_t mode)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	long shootMode = fuji::focus_mode_to_fuji(mode);
	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetFocusMode, 
		GFX100S_API_PARAM_SetFocusMode, shootMode);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getFocusMode() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetFocusMode, 
		GFX100S_API_PARAM_SetFocusMode, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return fuji::focus_mode_from_fuji(value);
}

bool CameraHolderFuji::setAutoFocusMode(int32_t value)
{
	if (d_ptr->handler == nullptr) 
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();
	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetAFMode,
		GFX100S_API_PARAM_SetAFMode, SDK_ITEM_DIRECTION_CURRENT, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getAutoFocusMode() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);

	long angle = 0;
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetAFMode,
		GFX100S_API_PARAM_GetAFMode, SDK_ITEM_DIRECTION_CURRENT, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setFocusPoints(int32_t value)
{
#if 0
	SET_PROP_FUNC(API_CODE_SetFocusPoints, GFX100S_API_PARAM_SetFocusPoints, value);
#else
	if (d_ptr->handler == nullptr || d_ptr->focusWidth == 0 || d_ptr->focusHeight == 0)
		return 0;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
	{
		stopLiveView();
	}

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetFocusPoints,
		GFX100S_API_PARAM_SetFocusPoints, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
	}
	else
	{
		d_ptr->updateFocusPoint(value);
	}

	return rlt;
#endif
}

int32_t CameraHolderFuji::getFocusPoints() const
{
	GET_PROP_FUNC(API_CODE_GetFocusPoints, GFX100S_API_PARAM_GetFocusPoints);
}

void CameraHolderFuji::setPhotoSavePath(const QString& path)
{
    ICameraHolderEntry::setPhotoSavePath(path);
}

bool CameraHolderFuji::setAFAreaPos(float x, float y)
{
	auto func = [=]() {
		if (d_ptr->handler == nullptr || d_ptr->focusWidth == 0 || d_ptr->focusHeight == 0)
			return false;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
		{
			stopLiveView();
		}

		d_ptr->focusArea.h = ((x - 0.1f) / 0.8f - 0.5f) * (d_ptr->focusWidth - 1);
		d_ptr->focusArea.v = (0.5f - (y - 0.13) / 0.74f) * (d_ptr->focusHeight - 1);

		auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetFocusArea,
			GFX100S_API_PARAM_SetFocusArea, SDK_ITEM_DIRECTION_0, &d_ptr->focusArea);
		auto rlt = FUJI_SUCCESS(errCode);
		if (!rlt)
		{
			printError();
		}

		return rlt;
	};

	d_ptr->commadTask->pushTaskFunc(func);

	return true;
}

bool CameraHolderFuji::getAFAreaPos(float& x, float& y, float& w, float& h)
{
	if (d_ptr->focusWidth == 0 || d_ptr->focusHeight == 0)
		return false;

	x = (d_ptr->focusArea.h / (float)(d_ptr->focusWidth - 1) + 0.5f) * 0.8 + 0.1;
	y = (0.5f - d_ptr->focusArea.v / (float)(d_ptr->focusHeight - 1)) * 0.74 + 0.13;
	w = d_ptr->focusArea.size / (float)(MAX_FOCUS_W - 1);
	h = d_ptr->focusArea.size / (float)(MAX_FOCUS_H - 1);

	return true;
}

bool CameraHolderFuji::getRealAFAreaPos()
{
	if (d_ptr->handler == nullptr)
	{
		return false;
	}

	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetFocusArea,
		GFX100S_API_PARAM_GetFocusArea, SDK_ITEM_DIRECTION_0, &d_ptr->focusArea);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
		return false;
	}

	return true;
}

bool CameraHolderFuji::setAutoFocusIlluminator(int32_t value)
{
	SET_PROP_FUNC(API_CODE_GetAFIlluminator, GFX100S_API_PARAM_GetAFIlluminator, value);
}

int32_t CameraHolderFuji::getAutoFocusIlluminator() const
{
	GET_PROP_FUNC(API_CODE_GetAFIlluminator, GFX100S_API_PARAM_GetAFIlluminator);
}

bool CameraHolderFuji::setFaceDetectionMode(int32_t value)
{
	SET_PROP_FUNC(API_CODE_GetFaceDetectionMode, GFX100S_API_PARAM_GetFaceDetectionMode, value);
}

int32_t CameraHolderFuji::getFaceDetectionMode() const
{
	GET_PROP_FUNC(API_CODE_GetFaceDetectionMode, GFX100S_API_PARAM_GetFaceDetectionMode);
}

bool CameraHolderFuji::setEyeDetectionMode(int32_t value)
{
	SET_PROP_FUNC(API_CODE_GetEyeAFMode, GFX100S_API_PARAM_GetEyeAFMode, value);
}

int32_t CameraHolderFuji::getEyeDetectionMode() const
{
	GET_PROP_FUNC(API_CODE_GetEyeAFMode, GFX100S_API_PARAM_GetEyeAFMode);
}

bool CameraHolderFuji::setExposureProgramMode(int mode)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetAEMode((XSDK_HANDLE)d_ptr->handler, mode);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int CameraHolderFuji::getExposureProgramMode() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = -1;
	auto errCode = m_sdk_fuji->m_pfnGetAEMode((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setExposureBias(int mode)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetExposureBias((XSDK_HANDLE)d_ptr->handler, mode);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int CameraHolderFuji::getExposureBias() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = -1;
	auto errCode = m_sdk_fuji->m_pfnGetExposureBias((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setFELock(int32_t value)
{
	SET_PROP_FUNC(API_CODE_GetInterlockAEAFArea, GFX100S_API_PARAM_GetInterlockAEAFArea, value);
}

int32_t CameraHolderFuji::getFELock() const
{
	GET_PROP_FUNC(API_CODE_GetInterlockAEAFArea, GFX100S_API_PARAM_GetInterlockAEAFArea);
}

bool CameraHolderFuji::setMFAssistMode(int32_t value)
{
	SET_PROP_FUNC(API_CODE_GetMFAssistMode, GFX100S_API_PARAM_GetMFAssistMode, value);
}

int32_t CameraHolderFuji::getMFAssistMode() const
{
	GET_PROP_FUNC(API_CODE_GetMFAssistMode, GFX100S_API_PARAM_GetMFAssistMode);
}

bool CameraHolderFuji::setAFCPriorityMode(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetShutterPriorityMode, 
		GFX100S_API_PARAM_SetShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFC, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

bool CameraHolderFuji::getAFCPriorityMode(int32_t& value) const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long priority = -1;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetShutterPriorityMode,
		GFX100S_API_PARAM_GetShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFC, &priority);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
	}
	value = priority;

	return rlt;
}

bool CameraHolderFuji::setAFSPriorityMode(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetShutterPriorityMode,
		GFX100S_API_PARAM_SetShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFS, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

bool CameraHolderFuji::getAFSPriorityMode(int32_t& value) const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long priority = -1;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetShutterPriorityMode,
		GFX100S_API_PARAM_GetShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFS, &priority);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
	}
	value = priority;

	return rlt;
}

bool CameraHolderFuji::setWhiteBalance(int value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetWBMode((XSDK_HANDLE)d_ptr->handler, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int CameraHolderFuji::getWhiteBalance() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetWBMode((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return false;
	}
	return value;
}

bool CameraHolderFuji::setWhiteBalanceTune(int r, int b)
{
	if (d_ptr->handler == nullptr)
		return false;

	auto wbMode = getWhiteBalance();

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetWhiteBalanceTune,
		GFX100S_API_PARAM_SetWhiteBalanceTune, wbMode, r, b);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

bool CameraHolderFuji::getWhiteBalanceTune(int& r, int& b) const
{
	if (d_ptr->handler == nullptr)
		return 0;

	auto wbMode = getWhiteBalance();

	std::lock_guard guard(m_mutex);

	long rValue = 0;
	long bValue = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetWhiteBalanceTune,
		GFX100S_API_PARAM_GetWhiteBalanceTune, wbMode, &rValue, &bValue);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	r = rValue;
	b = bValue;

	return FUJI_SUCCESS(errCode);
}

bool CameraHolderFuji::setFocusDistance(int value)
{
#if 1
	if (d_ptr->handler == nullptr)
		return false;

	auto func = [=]() {
		if (d_ptr->handler == nullptr) 
			return false; 
		std::lock_guard guard(m_mutex); 
		
		if (m_isLiveView)
			stopLiveView(); 

		auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetFocusPos, GFX100S_API_PARAM_SetFocusPos, (long)value);
		auto rlt = FUJI_SUCCESS(errCode); 
		if (!rlt)
		{
			printError(); 

			qCritical() << getDeviceName() << "setFocusDistance failed.";
		}

		return rlt; 
	};
	d_ptr->commadTask->pushTaskFunc(func);
	return true;
#else
	SET_PROP_FUNC(API_CODE_SetFocusPos, GFX100S_API_PARAM_SetFocusPos, value);
#endif
}

int CameraHolderFuji::getFocusDistance() const
{
	GET_PROP_FUNC(API_CODE_GetFocusPos, GFX100S_API_PARAM_GetFocusPos);
}

bool CameraHolderFuji::setImageSize(int32_t size)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetImageSize,
		GFX100S_API_PARAM_SetImageSize, size);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getImageSize() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetImageSize,
		GFX100S_API_PARAM_GetImageSize, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setImageQuality(int value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetImageQuality,
		GFX100S_API_PARAM_SetImageQuality, value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int CameraHolderFuji::getImageQuality() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetImageQuality,
		GFX100S_API_PARAM_GetImageQuality, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setImageZoom(int32_t value)
{
	SET_PROP_FUNC(API_CODE_SetThroughImageZoom, GFX100S_API_PARAM_SetThroughImageZoom, value);
}

bool CameraHolderFuji::getImageZoom(int32_t& value) const
{
	GET_PROP_FUNC_BOOL(API_CODE_GetThroughImageZoom, GFX100S_API_PARAM_GetThroughImageZoom, value);
}

void CameraHolderFuji::shutterToHalf()
{
	if (d_ptr->handler == nullptr)
		return;

	auto shutterHalf = [this]() {
		std::lock_guard guard(m_mutex);
		long plReleaseStatus;
		auto errCode = m_sdk_fuji->m_pfnGetReleaseStatus((XSDK_HANDLE)d_ptr->handler, &plReleaseStatus);

		if (FUJI_FAIL(errCode))
		{
			printError();
			return;
		}

		//std::cout << "pre half plReleaseStatus: " << plReleaseStatus << std::endl;

		long lShotOpt = 1;
		long lStatus;

		if (m_isLiveView)
			stopLiveView();

		errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_S1ON, &lShotOpt, &lStatus);
		if (FUJI_FAIL(errCode))
		{
			printError();

			return;
		}

		std::cout << "ShutterToHalf: " << lStatus << std::endl;

		m_focusStatus = (lStatus == XSDK_RELEASE_OK) ? FocusStatus::FocusSucess : FocusStatus::FocusFail;
		emit focusStatusChanged(this, m_focusStatus);
	};

	d_ptr->commadTask->pushTask(std::make_shared<FunctionTask>(shutterHalf));
}

void CameraHolderFuji::shutterToRelease()
{
	if (d_ptr->handler == nullptr)
		return;

	auto shutterRelease = [this]() {
		long plReleaseStatus;

		std::lock_guard guard(m_mutex);
		if (m_isLiveView)
			stopLiveView();

		auto errCode = m_sdk_fuji->m_pfnGetReleaseStatus((XSDK_HANDLE)d_ptr->handler, &plReleaseStatus);

		if (FUJI_FAIL(errCode))
		{
			printError();
		}
		else
		{
			//std::cout << "pre release plReleaseStatus: " << plReleaseStatus << std::endl;

			//if (XSDK_RELEASE_STATUS_S1 == plReleaseStatus)
			{
				long lShotOpt = 1;
				long lStatus;

				auto errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_N_S1OFF, &lShotOpt, &lStatus);

				if (FUJI_FAIL(errCode))
				{
					printError();
				}

				std::cout << "ShutterToRelease: " << lStatus << std::endl;
			}
		}

		m_focusStatus =  FocusStatus::NoFocus;
		emit focusStatusChanged(this, m_focusStatus);
	};

	d_ptr->commadTask->pushTask(std::make_shared<FunctionTask>(shutterRelease));
}

void CameraHolderFuji::focusPress()
{
	auto func = [this]() {
		std::lock_guard guard(m_mutex);

		if (d_ptr->handler == nullptr)
			return;

		long plReleaseStatus;
		long lShotOpt = 1;
		long lStatus;

		int errCode = 0;
		if (m_isLiveView)
			stopLiveView();

		errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_INSTANTAF, &lShotOpt, &lStatus);
		if (FUJI_FAIL(errCode))
		{
			printError();

			return;
		}

		std::cout << "ShutterToHalf: " << lStatus << std::endl;

		m_focusStatus = (lStatus == XSDK_RELEASE_OK) ? FocusStatus::FocusSucess : FocusStatus::FocusFail;
		emit focusStatusChanged(this, m_focusStatus);
	};
	d_ptr->commadTask->pushTask(std::make_shared<FunctionTask>(func));
}

void CameraHolderFuji::focusRelease()
{
	auto func = [this]() {
		long plReleaseStatus;

		std::lock_guard guard(m_mutex);

		if (d_ptr->handler == nullptr)
			return;
		if (m_isLiveView)
			stopLiveView();

		long lShotOpt = 1;
		long lStatus;

		auto errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_N_INSTANTAF, &lShotOpt, &lStatus);

		if (FUJI_FAIL(errCode))
		{
			printError();
		}
		else
		{
			std::cout << "ShutterToRelease: " << lStatus << std::endl;

			m_focusStatus = FocusStatus::NoFocus;
			emit focusStatusChanged(this, m_focusStatus);
		}
	};
	d_ptr->commadTask->pushTask(std::make_shared<FunctionTask>(func));
}


bool CameraHolderFuji::captureImage()
{
	int errCode = 0;

	if (m_isLiveView)
		stopLiveView();

	if (captureDown())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(35));

		captureUp();
	}

	return FUJI_SUCCESS(errCode);
}

void CameraHolderFuji::shootOne()
{
	auto func = [this]() {
		auto isCapture = captureImage();
	};

	d_ptr->commadTask->pushTask(std::make_shared<FunctionTask>(func));
}

bool CameraHolderFuji::captureDown()
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	long lShotOpt = 1;
	long lStatus;
	auto errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_S1ON, &lShotOpt, &lStatus);
	auto rlt = FUJI_FAIL(errCode);

	if (rlt)
		printError();

	return !rlt;
}

bool CameraHolderFuji::captureUp()
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	long lShotOpt = 1;
	long lStatus;
	auto errCode = m_sdk_fuji->m_pfnRelease((XSDK_HANDLE)d_ptr->handler, XSDK_RELEASE_S2_S1OFF, &lShotOpt, &lStatus);
	auto rlt = FUJI_FAIL(errCode);
	if (rlt)
	{
		printError();
	}

	return !rlt;
}

bool CameraHolderFuji::setControlMode(CameraHolderFuji::ControlMode mode)
{
	if (d_ptr->handler == nullptr || mode == TwoWayControl)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView2();

	long priormode = (mode == PcControl) ? XSDK_PRIORITY_PC : XSDK_PRIORITY_CAMERA;

	auto errCode = m_sdk_fuji->m_pfnSetPriorityMode(d_ptr->handler, priormode);
	if (FUJI_FAIL(errCode))
		printError();
	else
		d_ptr->mode = mode;

	return FUJI_SUCCESS(errCode);
}

CameraHolderFuji::ControlMode CameraHolderFuji::getControlMode()
{
	if (d_ptr->handler == nullptr)
		return d_ptr->mode;

	long priormode;
	auto errCode = m_sdk_fuji->m_pfnGetPriorityMode(d_ptr->handler, &priormode);
	if (FUJI_FAIL(errCode))
		printError();
	else
	{
		d_ptr->mode = (priormode == XSDK_PRIORITY_PC) ? PcControl : CameraControl;
	}

	return d_ptr->mode;
}

bool CameraHolderFuji::setRAWFileCompressionType(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetRAWCompression,
		GFX100S_API_PARAM_SetRAWCompression, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getRAWFileCompressionType() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetRAWCompression,
		GFX100S_API_PARAM_GetRAWCompression, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setRAWOutputDepth(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetRAWOutputDepth,
		GFX100S_API_PARAM_SetRAWOutputDepth, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getRAWOutputDepth() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetRAWOutputDepth,
		GFX100S_API_PARAM_GetRAWOutputDepth, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setFilmSimulationMode(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetFilmSimulationMode,
		GFX100S_API_PARAM_SetFilmSimulationMode, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getFilmSimulationMode() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetFilmSimulationMode,
		GFX100S_API_PARAM_GetFilmSimulationMode, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setGrainEffect(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetGrainEffect,
		GFX100S_API_PARAM_SetGrainEffect, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getGrainEffect() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetGrainEffect,
		GFX100S_API_PARAM_GetGrainEffect, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setShadowing(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetShadowing,
		GFX100S_API_PARAM_SetShadowing, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getShadowing() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetShadowing,
		GFX100S_API_PARAM_GetShadowing, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setColorChromeBlue(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetColorChromeBlue,
		GFX100S_API_PARAM_SetColorChromeBlue, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getColorChromeBlue() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetColorChromeBlue,
		GFX100S_API_PARAM_GetColorChromeBlue, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setSmoothSkinEffect(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetSmoothSkinEffect,
		GFX100S_API_PARAM_SetSmoothSkinEffect, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getSmoothSkinEffect() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetSmoothSkinEffect,
		GFX100S_API_PARAM_GetSmoothSkinEffect, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setDynamicRange(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetDynamicRange((XSDK_HANDLE)d_ptr->handler, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getDynamicRange() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetDynamicRange((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setWideDynamicRange(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetWideDynamicRange,
		GFX100S_API_PARAM_SetWideDynamicRange, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getWideDynamicRange() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetWideDynamicRange,
		GFX100S_API_PARAM_GetWideDynamicRange, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setHightlightTone(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetHighLightTone,
		GFX100S_API_PARAM_SetHighLightTone, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getHightlightTone() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetHighLightTone,
		GFX100S_API_PARAM_GetHighLightTone, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setShadowTone(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetShadowTone,
		GFX100S_API_PARAM_SetShadowTone, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getShadowTone() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetShadowTone,
		GFX100S_API_PARAM_GetShadowTone, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setColorTemperature(int32_t value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetWBColorTemp((XSDK_HANDLE)d_ptr->handler, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int32_t CameraHolderFuji::getColorTemperature() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);
	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetWBColorTemp((XSDK_HANDLE)d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setColorMode(int32_t value)
{
	SET_PROP_FUNC(API_CODE_SetColorMode, GFX100S_API_PARAM_SetColorMode, value);
}

int32_t CameraHolderFuji::getColorMode() const
{
	GET_PROP_FUNC(API_CODE_GetColorMode, GFX100S_API_PARAM_GetColorMode);
}

bool CameraHolderFuji::setSharpness(int32_t value)
{
	SET_PROP_FUNC(API_CODE_SetSharpness, GFX100S_API_PARAM_SetSharpness, value);
}

int32_t CameraHolderFuji::getSharpness() const
{
	GET_PROP_FUNC(API_CODE_GetSharpness, GFX100S_API_PARAM_GetSharpness);
}

bool CameraHolderFuji::setNoiseReduction(int32_t value)
{
	SET_PROP_FUNC(API_CODE_SetNoiseReduction, GFX100S_API_PARAM_SetNoiseReduction, value);
}

int32_t CameraHolderFuji::getNoiseReduction() const
{
	GET_PROP_FUNC(API_CODE_GetNoiseReduction, GFX100S_API_PARAM_GetNoiseReduction);
}

bool CameraHolderFuji::setColorSpace(int32_t value)
{
	SET_PROP_FUNC(API_CODE_SetColorSpace, GFX100S_API_PARAM_SetColorSpace, value);
}

int32_t CameraHolderFuji::getColorSpace() const
{
	GET_PROP_FUNC(API_CODE_GetColorSpace, GFX100S_API_PARAM_GetColorSpace);
}

bool CameraHolderFuji::setMeteringMode(int32_t value)
{
	SET_SPECIFIC_PROP_FUNC(m_pfnSetMeteringMode, value);
}

int32_t CameraHolderFuji::getMeteringMode() const
{
	GET_SPECIFIC_PROP_FUNC(m_pfnGetMeteringMode);
}

bool CameraHolderFuji::setBackupSettings(uint8_t* data, uint64_t size)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetBackupSettings(d_ptr->handler, size, data);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return false;
	}

	return true;
}

uint8_t* CameraHolderFuji::getbackupSettings(uint64_t& size)
{
	if (d_ptr->handler == nullptr)
		return nullptr;

	long bufSize = 0;

	auto errCode = m_sdk_fuji->m_pfnGetBackupSettings(d_ptr->handler, &bufSize, nullptr);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return nullptr;
	}

	uint8_t* buf = new uint8_t[bufSize + 1];
	errCode = m_sdk_fuji->m_pfnGetBackupSettings(d_ptr->handler, &bufSize, buf);
	if (FUJI_FAIL(errCode))
	{
		printError();
		SAFE_DELETE_ARRAY(buf);
		return nullptr;
	}
	size = bufSize;

	return buf;
}

bool CameraHolderFuji::setMediaRecord(int value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetMediaRecord(d_ptr->handler, value);
	if (FUJI_FAIL(errCode))
	{
		printError();
		return false;
	}

	return true;
}

int CameraHolderFuji::getMediaRecord() const
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetMediaRecord(d_ptr->handler, &value);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::setCaptureDelay(int value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetCaptureDelay,
		GFX100S_API_PARAM_SetCaptureDelay, (long)value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
		printError();

	return rlt;
}

int CameraHolderFuji::getCaptureDelay() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);

	long value = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetCaptureDelay,
		GFX100S_API_PARAM_GetCaptureDelay, &value);
	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
	}

	return value;
}

bool CameraHolderFuji::getBatteryLevel(int32_t& value) const
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	value = d_ptr->batteryRatio;

	return true;
}

bool CameraHolderFuji::getBatteryState(int32_t& value) const
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	value = d_ptr->bodyBattery;

	return true;
}

QString CameraHolderFuji::getModel() const
{
	return m_models;
}

QString CameraHolderFuji::getSerialNumber() const
{
	return m_serialNumber;
}

QString CameraHolderFuji::getTetherConnection() const
{
	return m_tetherConnection;
}

QString CameraHolderFuji::getLensName()
{
	updateLensInfo();
	return m_lensName;
}

QString CameraHolderFuji::getLensID()
{
	updateLensInfo();
	return m_lensID;
}

QString CameraHolderFuji::getLensModel()
{
	auto lensName = getLensName();
	auto index = lensName.indexOf("m");
	if (lensName.size() > 6 && index > 2)
	{
		return lensName.mid(2, index - 2);
	}

	return "";
}

int32_t CameraHolderFuji::getRearShutterCount() const
{
	if (d_ptr->handler == nullptr)
		return 0;

	std::lock_guard guard(m_mutex);

	long plShutterCount = 0;
	long plTotalShutterCount = 0;
	long plExchangeCount = 0;
	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_GetShutterCount,
		GFX100S_API_PARAM_GetShutterCount, &plShutterCount, &plTotalShutterCount, &plExchangeCount);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}

	return plTotalShutterCount;
}

void CameraHolderFuji::updateCameraStatus()
{
}

void CameraHolderFuji::updateProperties()
{
	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

#if 0
	//getImageSize();
	uint64_t size = 0;
	auto data = getbackupSettings(size);

	SAFE_DELETE_ARRAY(data);
#endif

	getRealAFAreaPos();

	bool rlt = false;
	int errCode = 0;

	long lNumZoomPos = 0; 
	long lZoomPos = 0;
	errCode = m_sdk_fuji->m_pfnCapLensZoomPos((XSDK_HANDLE)d_ptr->handler, &lNumZoomPos, nullptr, nullptr, nullptr);
	if (FUJI_SUCCESS(errCode) && lNumZoomPos > 0)
	{
		long* plZoomPos = new long[lNumZoomPos];
		errCode = m_sdk_fuji->m_pfnCapLensZoomPos((XSDK_HANDLE)d_ptr->handler, &lNumZoomPos, plZoomPos, nullptr, nullptr);

		lZoomPos = plZoomPos[0];

		SAFE_DELETE_ARRAY(plZoomPos);
	}

	long valueCount = 0;
	long* plNumValue = nullptr;

	if (FUJI_SUCCESS(errCode))
	{
		// 光圈
		errCode = m_sdk_fuji->m_pfnCapAperture((XSDK_HANDLE)d_ptr->handler, lZoomPos, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			plNumValue = new long[valueCount];
			errCode = m_sdk_fuji->m_pfnCapAperture((XSDK_HANDLE)d_ptr->handler, lZoomPos, &valueCount, plNumValue);
			if (FUJI_SUCCESS(errCode))
			{
				m_propTab.f_number.writable = rlt;
				m_propTab.f_number.possible = fuji::parse_paragmeter<uint16_t>(plNumValue, valueCount);
			}
			SAFE_DELETE_ARRAY(plNumValue);
		}
	}

	ICameraHolderEntry::setAperture(getRealAperture());

	// ISO
	errCode = m_sdk_fuji->m_pfnCapSensitivity((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);
	if (FUJI_SUCCESS(errCode))
	{
		plNumValue = new long[valueCount];
		errCode = m_sdk_fuji->m_pfnCapSensitivity((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			m_propTab.iso_sensitivity.writable = true;
			m_propTab.iso_sensitivity.possible = fuji::parse_paragmeter<uint32_t>(plNumValue, valueCount);
		}
		SAFE_DELETE_ARRAY(plNumValue);
	}
	m_propTab.iso_sensitivity.current = getRealIso();

	// 快门速度
	{
		long lBulbCapable;
		errCode = m_sdk_fuji->m_pfnCapShutterSpeed((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue, &lBulbCapable);
		if (FUJI_SUCCESS(errCode))
		{
			plNumValue = new long[valueCount];
			errCode = m_sdk_fuji->m_pfnCapShutterSpeed((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue, &lBulbCapable);
			if (FUJI_SUCCESS(errCode))
			{
				m_propTab.shutter_speed.writable = true;
                m_propTab.shutter_speed.possible = fuji::parse_paragmeter<int32_t>(plNumValue, valueCount);
			}
			SAFE_DELETE_ARRAY(plNumValue);
		}

		m_propTab.shutter_speed.current = getRealShutterSpeed();
	}

	// 拍照模式
#if 1
	errCode = m_sdk_fuji->m_pfnCapDriveMode((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);
	if (FUJI_SUCCESS(errCode))
	{
		plNumValue = new long[valueCount];
		errCode = m_sdk_fuji->m_pfnCapDriveMode((XSDK_HANDLE)d_ptr->handler, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			m_propTab.still_capture_mode.writable = true;
			m_propTab.still_capture_mode.possible = fuji::parse_shoot_mode(plNumValue, valueCount);
		}
		SAFE_DELETE_ARRAY(plNumValue);
	}
#else
	m_propTab.still_capture_mode.writable = true;

	std::vector<int32_t> listCapMode{
		(int32_t)ShootMode::Shoot_Single,
	};

	m_propTab.still_capture_mode.possible = listCapMode;
#endif

	// 对焦模式
#if 0
	CAP_PROP_LIST(API_CODE_CapFocusMode, GFX100S_API_PARAM_CapFocusMode, m_propTab.focus_mode, int32_t);
#else
	{
		long valueCount = 0;
		errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapFocusMode, GFX100S_API_PARAM_CapFocusMode, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			plNumValue = new long[valueCount];
			errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapFocusMode, GFX100S_API_PARAM_CapFocusMode, &valueCount, plNumValue);
			if (FUJI_SUCCESS(errCode))
			{
				m_propTab.focus_mode.writable = true;
				m_propTab.focus_mode.possible = fuji::parse_focus_mode(plNumValue, valueCount);
			}
			SAFE_DELETE_ARRAY(plNumValue);
		}
	}
#endif

	// 自动对焦模式
#if 0
	CAP_PROP_LIST(API_CODE_CapAFMode, GFX100S_API_PARAM_CapAFMode, m_propTab.auto_focus_mode, int32_t);
#else
	{
		errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapAFMode, GFX100S_API_PARAM_CapAFMode, SDK_ITEM_DIRECTION_CURRENT, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			plNumValue = new long[valueCount];
			errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapAFMode, GFX100S_API_PARAM_CapAFMode, SDK_ITEM_DIRECTION_CURRENT, &valueCount, plNumValue);
			if (FUJI_SUCCESS(errCode))
			{
				m_propTab.auto_focus_mode.writable = true;
				m_propTab.auto_focus_mode.possible = fuji::parse_paragmeter<int32_t>(plNumValue, valueCount);
			}
			SAFE_DELETE_ARRAY(plNumValue);
		}
		else
		{
			printError();
		}
	}
#endif

	// 对焦点
	CAP_PROP_LIST(API_CODE_CapFocusPoints, GFX100S_API_PARAM_CapFocusPoints, m_propTab.focus_points, int32_t);

	// 自动对焦照明器
	CAP_PROP_LIST(API_CODE_CapAFIlluminator, GFX100S_API_PARAM_CapAFIlluminator, m_propTab.auto_focus_illuminator, int32_t);

	// 面部识别模式
	CAP_PROP_LIST(API_CODE_CapFaceDetectionMode, GFX100S_API_PARAM_CapFaceDetectionMode, m_propTab.face_detection_mode, int32_t);

	// FE lock
	CAP_PROP_LIST(API_CODE_CapInterlockAEAFArea, GFX100S_API_PARAM_CapInterlockAEAFArea, m_propTab.fe_lock, int32_t);

	// MF辅助模式
	CAP_PROP_LIST(API_CODE_CapMFAssistMode, GFX100S_API_PARAM_CapMFAssistMode, m_propTab.mf_assist_mode, int32_t);

	// 快门优先级模式
#if 0
	CAP_PROP_LIST(API_CODE_CapShutterPriorityMode, GFX100S_API_PARAM_CapShutterPriorityMode, m_propTab.shutter_priority_mode, int32_t);
#else
	{
		errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapShutterPriorityMode, 
			GFX100S_API_PARAM_CapShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFC, &valueCount, plNumValue);
		if (FUJI_SUCCESS(errCode))
		{
			plNumValue = new long[valueCount];
			errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapShutterPriorityMode, 
				GFX100S_API_PARAM_CapShutterPriorityMode, SDK_ITEM_AFPRIORITY_AFC, &valueCount, plNumValue);
			if (FUJI_SUCCESS(errCode))
			{
				m_propTab.shutter_priority_mode.writable = true;
				m_propTab.shutter_priority_mode.possible = fuji::parse_paragmeter<int32_t>(plNumValue, valueCount);
			}
			SAFE_DELETE_ARRAY(plNumValue);
		}
		else
		{
			printError();
		}
	}
#endif

	// 眼球识别模式
	CAP_PROP_LIST(API_CODE_CapEyeAFMode, GFX100S_API_PARAM_CapEyeAFMode, m_propTab.eye_detection_mode, int32_t);

	// 实时预览质量
#if 1
	{
		m_propTab.live_view_image_quality.writable = true;

		std::vector<int32_t> listCapMode{
			SDK_LIVEVIEW_QUALITY_FINE,
			SDK_LIVEVIEW_QUALITY_BASIC,
		};

		m_propTab.live_view_image_quality.possible.swap(listCapMode);
	}
#else
	CAP_PROP_LIST(API_CODE_CapLiveViewImageQuality, GFX100S_API_PARAM_CapLiveViewImageQuality, m_propTab.live_view_image_quality, uint16_t);
#endif

	// 对焦距离
	{
		long size = sizeof(SDK_FOCUS_POS_CAP);
		SDK_FOCUS_POS_CAP focusPosInfo;
		errCode = m_sdk_fuji->m_pfnCapProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CapFocusPos,
			GFX100S_API_PARAM_CapFocusPos, &size, &focusPosInfo);
		if (FUJI_SUCCESS(errCode))
		{
			m_propTab.zoom_operation.writable = true;
			std::vector<int32_t> listCapMode{
				(int32_t)focusPosInfo.lFocusPlsINF,
				(int32_t)focusPosInfo.lFocusPlsMOD,
				(int32_t)focusPosInfo.lMinDriveStepMFDriveEndThresh,
			};

			m_propTab.zoom_operation.possible.swap(listCapMode);
		}
		else
		{
			printError();
		}
	}

	// 图片缩放
#if 0
	CAP_PROP_LIST(API_CODE_CapThroughImageZoom, GFX100S_API_PARAM_CapThroughImageZoom, m_propTab.image_zoom, int32_t);
#else
	m_propTab.image_zoom.writable = true;
	std::vector<int32_t> listCapMode{
		(int32_t)SDK_THROUGH_ZOOM_10,
		(int32_t)SDK_THROUGH_ZOOM_25,
		(int32_t)SDK_THROUGH_ZOOM_40,
		(int32_t)SDK_THROUGH_ZOOM_80,
		(int32_t)SDK_THROUGH_ZOOM_160,
		(int32_t)SDK_THROUGH_ZOOM_240,
	};

	m_propTab.image_zoom.possible.swap(listCapMode);
#endif

	// 曝光模式
	CAP_PROP_FUNC_LIST(m_pfnCapAEMode, m_propTab.exposure_program_mode, uint32_t);

	// 白平衡
	CAP_PROP_FUNC_LIST(m_pfnCapWBMode, m_propTab.white_balance, uint16_t);

	// 白平衡偏移
	{ 
		m_propTab.white_balance_tune_R.writable = true;

		m_propTab.white_balance_tune_R.possible.clear();
		for (auto i = SDK_WB_R_SHIFT_MIN; i <= SDK_WB_R_SHIFT_MAX; i += GFX100S_WB_R_SHIFT_STEP)
		{
			m_propTab.white_balance_tune_R.possible.push_back(i);
		}

		m_propTab.white_balance_tune_B.writable = true;

		m_propTab.white_balance_tune_B.possible.clear();
		for (auto i = SDK_WB_B_SHIFT_MIN; i <= SDK_WB_B_SHIFT_MAX; i += GFX100S_WB_B_SHIFT_STEP)
		{
			m_propTab.white_balance_tune_B.possible.push_back(i);
		}
	}

	//图片质量
	CAP_PROP_LIST(API_CODE_CapImageQuality, GFX100S_API_PARAM_CapImageQuality, m_propTab.image_quality, int32_t);

	// 图片文件压缩格式
	CAP_PROP_LIST(API_CODE_CapRAWCompression, GFX100S_API_PARAM_CapRAWCompression, m_propTab.compression_file_format, int32_t);

	// 图片文件输出深度
	{
		m_propTab.raw_output_depth.writable = true;

		std::vector<int32_t> listCapMode{
			(int32_t)SDK_RAWOUTPUTDEPTH_14BIT,
			(int32_t)SDK_RAWOUTPUTDEPTH_16BIT,
		};

		m_propTab.raw_output_depth.possible.swap(listCapMode);
	}

	// 胶片模拟
	CAP_PROP_LIST(API_CODE_CapFilmSimulationMode, GFX100S_API_PARAM_CapFilmSimulationMode, m_propTab.film_simulation_mode, int32_t);

	// 纹理效果
	CAP_PROP_LIST(API_CODE_CapGrainEffect, GFX100S_API_PARAM_CapGrainEffect, m_propTab.grain_effect, int32_t);

	// 阴影
	CAP_PROP_LIST(API_CODE_CapShadowing, GFX100S_API_PARAM_CapShadowing, m_propTab.shadowing, int32_t);

	// color chrome fx blue 
	CAP_PROP_LIST(API_CODE_CapColorChromeBlue, GFX100S_API_PARAM_CapColorChromeBlue, m_propTab.color_chrome_blue, int32_t);

	// 平滑蒙皮效果 
	CAP_PROP_LIST(API_CODE_CapSmoothSkinEffect, GFX100S_API_PARAM_CapSmoothSkinEffect, m_propTab.smooth_skin_effect, int32_t);

	// 动态范围
	CAP_PROP_FUNC_LIST(m_pfnCapDynamicRange, m_propTab.dynamic_range, int32_t);

	// 宽动态范围
	CAP_PROP_LIST(API_CODE_CapWideDynamicRange, GFX100S_API_PARAM_CapWideDynamicRange, m_propTab.wide_dynamic_range, int32_t);

	// 突出显示色调
	CAP_PROP_LIST(API_CODE_CapHighLightTone, GFX100S_API_PARAM_CapHighLightTone, m_propTab.hightlight_tone, int32_t);

	// 阴影色调
	CAP_PROP_LIST(API_CODE_CapShadowTone, GFX100S_API_PARAM_CapShadowTone, m_propTab.shadow_tone, int32_t);
	
	// 色温
	CAP_PROP_FUNC_LIST(m_pfnCapWBColorTemp, m_propTab.color_temperature, int32_t);

	// 颜色类型
	CAP_PROP_LIST(API_CODE_CapColorMode, GFX100S_API_PARAM_CapColorMode, m_propTab.color_mode, int32_t);

	// 锐度
	CAP_PROP_LIST(API_CODE_CapSharpness, GFX100S_API_PARAM_CapSharpness, m_propTab.sharpness, int32_t);

	// 降噪
	CAP_PROP_LIST(API_CODE_CapNoiseReduction, GFX100S_API_PARAM_CapNoiseReduction, m_propTab.noise_reduction, int32_t);
	
	// 颜色空间
	CAP_PROP_LIST(API_CODE_CapColorSpace, GFX100S_API_PARAM_CapColorSpace, m_propTab.color_space, int32_t);

	// 测光模式
	CAP_PROP_FUNC_LIST(m_pfnCapMeteringMode, m_propTab.metering_mode, int32_t);

	// 保存SD卡类型
	CAP_PROP_FUNC_LIST(m_pfnCapMediaRecord, m_propTab.media_record, int32_t);

	// 拍照延时
	CAP_PROP_LIST(API_CODE_CapCaptureDelay, GFX100S_API_PARAM_CapCaptureDelay, m_propTab.capture_delay, int32_t)

	// 曝光补偿
	CAP_PROP_FUNC_LIST(m_pfnCapExposureBias, m_propTab.exposure_bias, int32_t);

}

float CameraHolderFuji::getAspectRatioValue() const
{
	float ratio = 1.0f;
	switch (d_ptr->liveViewImageSize)
	{
	case SDK_IMAGESIZE_S_3_2:
	case SDK_IMAGESIZE_L_3_2:
	case SDK_IMAGESIZE_M_3_2:
		ratio = 1.5f;
		break;
	case SDK_IMAGESIZE_S_16_9:
	case SDK_IMAGESIZE_L_16_9:
	case SDK_IMAGESIZE_M_16_9:
		ratio = 1.7778f;
		break;
	case SDK_IMAGESIZE_S_4_3:
	case SDK_IMAGESIZE_L_4_3:
	case SDK_IMAGESIZE_M_4_3:
		ratio = 1.3333f;
		break;
	case SDK_IMAGESIZE_S_1_1:
	case SDK_IMAGESIZE_L_1_1:
	case SDK_IMAGESIZE_M_1_1:
		ratio = 1.0f;
		break;
	case SDK_IMAGESIZE_S_65_24:
	case SDK_IMAGESIZE_L_65_24:
	case SDK_IMAGESIZE_M_65_24:
		ratio = 2.7083f;
		break;
	case SDK_IMAGESIZE_S_5_4:
	case SDK_IMAGESIZE_L_5_4:
	case SDK_IMAGESIZE_M_5_4:
		ratio = 1.25f;
		break;
	case SDK_IMAGESIZE_S_7_6:
	case SDK_IMAGESIZE_L_7_6:
	case SDK_IMAGESIZE_M_7_6:
		ratio = 1.1667f;
		break;
	default:
		break;
	}

	return ratio;
}

void CameraHolderFuji::onCameraInvalid()
{
#if 1
	disconnect();
#else
	if (d_ptr->handler == nullptr)
		return;

	long lCapabilityBitmap;
	auto pDevice = m_deviceId.data();
	auto errCode = m_sdk_fuji->m_pfnClose((XSDK_HANDLE)d_ptr->handler);

	if (FUJI_SUCCESS(errCode))
	{
		d_ptr->handler = nullptr;
		m_isConnected = false;
		//emit statusChangedSignal(this, m_isConnected);
	}
	//else
	//{
	//	printError();
	//}

	int connectTimes = 0;
	bool rlt = false;

	while (connectTimes < 1)
	{
		rlt = connect();
		if (rlt)
			break;
		++connectTimes;
	}

	if (!rlt)
		emit statusChangedSignal(this, m_isConnected);
#endif
}

void CameraHolderFuji::printError() const
{
	if (d_ptr->handler == nullptr)
		return;

    long lAPICode, lErrCode;
    m_sdk_fuji->m_pfnGetErrorNumber((XSDK_HANDLE)d_ptr->handler, &lAPICode, &lErrCode);

	if (lErrCode == XSDK_ERRCODE_PARAM)
	{
		emit error();
	}
	else if (lErrCode == XSDK_ERRCODE_COMMUNICATION)
	{
		d_ptr->commadTask->pushTaskFunc(std::bind(&CameraHolderFuji::onCameraInvalid, const_cast<CameraHolderFuji*>(this)));
	}
	
	if (lErrCode != XSDK_ERRCODE_BUSY)
	{
		qCritical() << getDeviceName() << "lAPICode: " << QString::number(lAPICode, 16) << " lErrCode: " << QString::number(lErrCode, 16);
	}
#if 0
	std::cout.setf(std::ios::hex);
#endif
}

bool CameraHolderFuji::startLiveView()
{
	if (d_ptr->handler == nullptr)
		return false;

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_StartLiveView, 
		GFX100S_API_PARAM_StartLiveView);
	if (FUJI_FAIL(errCode))
		printError();

	return FUJI_SUCCESS(errCode);
}

bool CameraHolderFuji::stopLiveView() const
{
	//return false;
	if (d_ptr->handler == nullptr)
		return false;

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_StopLiveView, GFX100S_API_PARAM_StopLiveView);
	if (FUJI_FAIL(errCode))
		printError();

	return FUJI_SUCCESS(errCode);
}

bool CameraHolderFuji::stopLiveView2() const
{
	if (d_ptr->handler == nullptr)
		return false;

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_StopLiveView, GFX100S_API_PARAM_StopLiveView);
	if (FUJI_FAIL(errCode))
		printError();

	return FUJI_SUCCESS(errCode);
}

bool CameraHolderFuji::setLiveSize(int value)
{
	if (d_ptr->handler == nullptr)
		return false;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	auto errCode = m_sdk_fuji->m_pfnSetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_SetLiveViewImageSize,
		GFX100S_API_PARAM_SetLiveViewImageSize, value);
	if (FUJI_FAIL(errCode))
		printError();

	return FUJI_SUCCESS(errCode);
}

void CameraHolderFuji::updateLensInfo()
{
	if (d_ptr->handler == nullptr)
		return;

	std::lock_guard guard(m_mutex);
	if (m_isLiveView)
		stopLiveView();

	XSDK_LensInformation lensInfo;

	auto errCode = m_sdk_fuji->m_pfnGetLensInfo((XSDK_HANDLE)d_ptr->handler, &lensInfo);
	if (FUJI_FAIL(errCode))
	{
		printError();
	}
	else
	{
		m_lensName = lensInfo.strProductName;
		m_lensID = lensInfo.strSerialNo;	
	}
}

void CameraHolderFuji::updateImageThread()
{
	d_ptr->isRunFinished = true;
	while (d_ptr->isRunThread)
	{
		if (d_ptr->handler == nullptr)
		{
			QThread::msleep(35);
			continue;
		}
		
		updateBatteryInfo();
		uint32_t bufSize = 0;

		auto t1 = clock();
		XSDK_ImageInformation imageInfo;

#if 1
		auto buf = d_ptr->getCameraImageData(&bufSize, imageInfo);
#else
		uint8_t* buf = nullptr;
		m_mutex.lock();
		auto errCode = m_sdk_fuji->m_pfnReadImageInfo((XSDK_HANDLE)d_ptr->handler, &imageInfo);
		if (FUJI_FAIL(errCode) || imageInfo.lDataSize < 1)
		{
			if (m_isLiveView || d_ptr->mode == ICameraHolder::CameraControl)
				startLiveView();
			m_mutex.unlock();

			QThread::msleep(10);
			continue;
		}
		m_mutex.unlock();

		bufSize = imageInfo.lDataSize;
		buf = new uint8_t[bufSize];

		m_mutex.lock();
		errCode = m_sdk_fuji->m_pfnReadImage((XSDK_HANDLE)d_ptr->handler, buf, bufSize);
		//q_ptr->m_sdk_fuji->LeaveCriticalSection();
		m_mutex.unlock();
#endif
		if (buf)
		{
			int format = imageInfo.lFormat & 0x00FF;
			if (format != XSDK_IMAGEFORMAT_LIVE)
			{
				auto t3 = clock();
				qDebug() << getDeviceName() << "time_read_image_data : " << t3 - t1;

				QString savePath = getPhotoSavePath();
				if (!savePath.isEmpty())
				{
					auto name = util::toQStr(std::string(imageInfo.strInternalName));
					QFileInfo fileInfo(name);
					auto suffix = fileInfo.suffix();
					auto devName = getDeviceName();

					if (!devName.isEmpty())
					{
						savePath += ("/" + devName);
					}
					else
					{
						devName = fileInfo.baseName();
					}

					devName = QString("%1.%2").arg(devName, suffix);

					d_ptr->saveImageTask->pushTask(std::make_shared<SaveImageTask>(savePath, devName, buf, bufSize));
					buf = nullptr;
				}
			}
			else
			{
				m_mutexData.lock();
				std::swap(d_ptr->imageData, buf);
				d_ptr->imageSize = bufSize;
				m_mutexData.unlock();
			}
		}

		SAFE_DELETE_ARRAY(buf);

		QThread::msleep(10);
	}
	d_ptr->isRunFinished = false;
}

void CameraHolderFuji::updateBatteryInfo()
{
	if (d_ptr->handler == nullptr)
		return;
	
	std::lock_guard guard(m_mutex);

	auto errCode = m_sdk_fuji->m_pfnGetProp((XSDK_HANDLE)d_ptr->handler, API_CODE_CheckBatteryInfo,
		GFX100S_API_PARAM_CheckBatteryInfo, &(d_ptr->bodyBattery), &(d_ptr->plGripBatteryInfo), 
		&d_ptr->plGripBattery2Info, &d_ptr->batteryRatio, &d_ptr->plGripBatteryRatio, 
		&d_ptr->plGripBattery2Ratio, &d_ptr->plBodyBattery2Info, &d_ptr->plBodyBattery3Info);

	auto rlt = FUJI_SUCCESS(errCode);
	if (!rlt)
	{
		printError();
	}
}


/*!
 * \class CameraControllerFuji
 *
 * \brief 富士相机控制器
 */
CameraControllerFuji::CameraControllerFuji()
    : IEntityCameraController(CameraBrand::CC_BRAND_FUJI)
{
}

CameraControllerFuji::~CameraControllerFuji()
{
	release();

	SAFE_DELETE(m_sdk_fuji);
}

bool CameraControllerFuji::init()
{
	std::lock_guard guard(m_deviceMutex);
	if (isInit())
		return true;

	try 
    {
        m_sdk_fuji = new CFujiSDK(NULL, "");
	}
	catch (...) 
    {
        m_sdk_fuji = NULL;
	}

	if (m_sdk_fuji == NULL) 
    {
		return false;
	}
	else 
    {
		if (FUJI_SDK_ERR_OK != m_sdk_fuji->GetError()) 
        {
			setIsInit(false);
            SAFE_DELETE(m_sdk_fuji);

            return false;
		}
	}

	if (XSDK_COMPLETE == m_sdk_fuji->m_pfnInit((LIB_HANDLE)(NULL)))
	{
		setIsInit(true);
	}

    return isInit();
}

bool CameraControllerFuji::release()
{
	IEntityCameraController::release();

    if (!isInit())
    {
        return true;
    }

	if (m_sdk_fuji && XSDK_COMPLETE == m_sdk_fuji->m_pfnExit())
	{
		setIsInit(false);
	}

    return !isInit();
}

bool CameraControllerFuji::scan()
{
    //std::lock_guard guard(m_deviceMutex);
	if (!isInit() || !m_sdk_fuji)
        return false;

	IEntityCameraController::release();

	long lNumCameras = 0;
    if (XSDK_COMPLETE == m_sdk_fuji->m_pfnDetect(XSDK_DSC_IF_USB, NULL, NULL, &lNumCameras))
    {
        std::cout << "fuji dect succ: " << lNumCameras << std::endl;

		std::vector<XSDK_DeviceInformation> devInfoList;
		std::vector<XSDK_HANDLE> devHandleList;

        for (int i = 0; i < lNumCameras; i++)
        {
            char pDevice[8];
            strcpy_s(pDevice, 8, ("ENUM:" + std::to_string(i)).c_str());
            long lCapabilityBitmap;
            XSDK_HANDLE hCam;
            auto errCode = m_sdk_fuji->m_pfnOpenEx(pDevice, &hCam, &lCapabilityBitmap, NULL);
            if (XSDK_COMPLETE == errCode)
            {
                // get device name
                XSDK_DeviceInformation pDevInfo;

                if (XSDK_COMPLETE == m_sdk_fuji->m_pfnGetDeviceInfo(hCam, &pDevInfo))
                {
                    std::cout << "fuji device name: " << pDevInfo.strDeviceName << std::endl;
                    std::cout << "fuji strSerialNo: " << pDevInfo.strSerialNo << std::endl;
                    std::cout << "fuji strFirmware: " << pDevInfo.strFirmware << std::endl;
                    std::cout << "fuji strProduct: " << pDevInfo.strProduct << std::endl;

                    devInfoList.emplace_back(std::move(pDevInfo));
                    devHandleList.push_back(hCam);
					m_sdk_fuji->m_pfnClose(hCam);
				}
                else
                {
					m_sdk_fuji->m_pfnClose(hCam);
					std::cout << "fuji get name error" << std::endl;
                }
            }
        }

		std::list<QString> camNameList;

        for (auto i = 0; i < devInfoList.size(); ++i)
        {
            auto& pDevInfo = devInfoList[i];

            auto deviceId = std::string("Fuji_") + std::string((char*)pDevInfo.strSerialNo);
            camNameList.emplace_back(std::move(util::toQStr(deviceId)));
        }

		updateCameraDevice(camNameList);

        for (auto i= 0; i < devInfoList.size(); ++i)
        {
			auto& pDevInfo = devInfoList[i];
			auto& hCam = devHandleList[i];

			auto deviceId = QString("Fuji_") + QString((char*)pDevInfo.strSerialNo);

			auto cameraHold = findCameraDeviceFromId(deviceId);
			if (!cameraHold)
			{
				// create camera holder
				auto cam = std::make_shared<CameraHolderFuji>(m_sdk_fuji, deviceId);
				cameraHold = cam;

                cam->d_ptr->cameraIndex = i;
				cam->m_models = pDevInfo.strProduct;
				cam->m_serialNumber = pDevInfo.strSerialNo;
				cam->m_tetherConnection = pDevInfo.strFramework;
				
				auto devName = util::toQStr(std::string(pDevInfo.strDeviceName));

				char version[256];
				m_sdk_fuji->m_pfnGetVersionString(version);
				std::cout << " Version: " << version << std::endl;

				cam->setDeviceName((devName));
				cam->setPhotoSavePath(getPhotoSavePath());

				cameraHold->connect();

				addCameraDevice(cam);
			}
        }
    }

	return true;
}




