#pragma once

#include "XAPI.h"


#define FUJI_SDK_ELEMENTS_OF_SHUTTER_SPEED  (319)

enum{
	FUJI_SDK_ERR_OK               = 0,
	FUJI_SDK_ERR_LOADSDK          = -1,
	FUJI_SDK_ERR_SDKVERSION       = -2,
};



class CFujiSDK
{
public:
	CFujiSDK( HMODULE hModule=NULL, LPCTSTR pRelativePath=NULL );
	virtual ~CFujiSDK(void);
	int GetError();
	void EnterCriticalSection();
	void LeaveCriticalSection();
	HMODULE GetHModule();

	static long g_plShutterSpeedList[FUJI_SDK_ELEMENTS_OF_SHUTTER_SPEED];
	static TCHAR g_pptchShutterSpeedText[FUJI_SDK_ELEMENTS_OF_SHUTTER_SPEED][8];

private:
	int                m_iErr;
	CRITICAL_SECTION   m_CriticalSection;
	HMODULE            m_hModule;

public:
	_XSDK_Init                      m_pfnInit;
	_XSDK_Exit                      m_pfnExit;
	_XSDK_Detect                    m_pfnDetect;
	_XSDK_OpenEx                    m_pfnOpenEx;
	_XSDK_Close                     m_pfnClose;
	_XSDK_PowerOFF                  m_pfnPowerOFF;
	_XSDK_GetErrorNumber            m_pfnGetErrorNumber;
	_XSDK_GetVersionString          m_pfnGetVersionString;
	_XSDK_GetDeviceInfo             m_pfnGetDeviceInfo;
	_XSDK_WriteDeviceName           m_pfnWriteDeviceName;
	_XSDK_GetFirmwareVersion        m_pfnGetFirmwareVersion;
	_XSDK_GetLensInfo               m_pfnGetLensInfo;
	_XSDK_GetLensVersion            m_pfnGetLensVersion;
	_XSDK_CapPriorityMode           m_pfnCapPriorityMode;
	_XSDK_SetPriorityMode           m_pfnSetPriorityMode;
	_XSDK_GetPriorityMode           m_pfnGetPriorityMode;
	_XSDK_CapRelease                m_pfnCapRelease;
	_XSDK_Release                   m_pfnRelease;
	_XSDK_GetReleaseStatus          m_pfnGetReleaseStatus;
	_XSDK_ReadImageInfo             m_pfnReadImageInfo;
	_XSDK_ReadImage                 m_pfnReadImage;
	_XSDK_ReadPreview               m_pfnReadPreview;
	_XSDK_DeleteImage               m_pfnDeleteImage;
	_XSDK_GetBufferCapacity         m_pfnGetBufferCapacity;
	_XSDK_CapAEMode                 m_pfnCapAEMode;
	_XSDK_SetAEMode                 m_pfnSetAEMode;
	_XSDK_GetAEMode                 m_pfnGetAEMode;
	_XSDK_CapShutterSpeed           m_pfnCapShutterSpeed;
	_XSDK_SetShutterSpeed           m_pfnSetShutterSpeed;
	_XSDK_GetShutterSpeed           m_pfnGetShutterSpeed;
	_XSDK_CapExposureBias           m_pfnCapExposureBias;
	_XSDK_SetExposureBias           m_pfnSetExposureBias;
	_XSDK_GetExposureBias           m_pfnGetExposureBias;
	_XSDK_CapDynamicRange           m_pfnCapDynamicRange;
	_XSDK_SetDynamicRange           m_pfnSetDynamicRange;
	_XSDK_GetDynamicRange           m_pfnGetDynamicRange;
	_XSDK_CapSensitivity            m_pfnCapSensitivity;
	_XSDK_SetSensitivity            m_pfnSetSensitivity;
	_XSDK_GetSensitivity            m_pfnGetSensitivity;
	_XSDK_CapMeteringMode           m_pfnCapMeteringMode;
	_XSDK_SetMeteringMode           m_pfnSetMeteringMode;
	_XSDK_GetMeteringMode           m_pfnGetMeteringMode;
	_XSDK_CapLensZoomPos            m_pfnCapLensZoomPos;
	_XSDK_GetLensZoomPos            m_pfnGetLensZoomPos;
	_XSDK_SetLensZoomPos            m_pfnSetLensZoomPos;
	_XSDK_CapAperture               m_pfnCapAperture;
	_XSDK_SetAperture               m_pfnSetAperture;
	_XSDK_GetAperture               m_pfnGetAperture;
	_XSDK_CapWBMode                 m_pfnCapWBMode;
	_XSDK_SetWBMode                 m_pfnSetWBMode;
	_XSDK_GetWBMode                 m_pfnGetWBMode;
	_XSDK_CapWBColorTemp            m_pfnCapWBColorTemp;
	_XSDK_SetWBColorTemp            m_pfnSetWBColorTemp;
	_XSDK_GetWBColorTemp            m_pfnGetWBColorTemp;
	_XSDK_CapMediaRecord            m_pfnCapMediaRecord;
	_XSDK_SetMediaRecord            m_pfnSetMediaRecord;
	_XSDK_GetMediaRecord            m_pfnGetMediaRecord;
	_XSDK_CapForceMode              m_pfnCapForceMode;
	_XSDK_SetForceMode              m_pfnSetForceMode;
	_XSDK_SetBackupSettings         m_pfnSetBackupSettings;
	_XSDK_GetBackupSettings         m_pfnGetBackupSettings;
	_XSDK_CapDriveMode              m_pfnCapDriveMode;
	_XSDK_SetDriveMode              m_pfnSetDriveMode;
	_XSDK_GetDriveMode              m_pfnGetDriveMode;
	_XSDK_CapProp					m_pfnCapProp;
	_XSDK_SetProp					m_pfnSetProp;
	_XSDK_GetProp					m_pfnGetProp;
};



