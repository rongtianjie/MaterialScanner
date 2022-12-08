//
//  X Series Camera SDK wrapping class
//

#include "FujiSDK.h"
#include "tchar.h"


#ifndef XSDK_SHUTTER_40
#define XSDK_SHUTTER_40 (40317473)
#endif

#ifndef XSDK_SHUTTER_50
#define XSDK_SHUTTER_50 (50796833)
#endif

#ifndef XSDK_SHUTTER_60
#define XSDK_SHUTTER_60 (64000000)
#endif

#ifndef XSDK_SHUTTER_2M
#define XSDK_SHUTTER_2M (64000030)
#endif

#ifndef XSDK_SHUTTER_4M
#define XSDK_SHUTTER_4M (64000060)
#endif

#ifndef XSDK_SHUTTER_8M
#define XSDK_SHUTTER_8M (64000090)
#endif

#ifndef XSDK_SHUTTER_15M
#define XSDK_SHUTTER_15M (64000120)
#endif

#ifndef XSDK_SHUTTER_30M
#define XSDK_SHUTTER_30M (64000150)
#endif

#ifndef XSDK_SHUTTER_60M
#define XSDK_SHUTTER_60M (64000180)
#endif


long CFujiSDK::g_plShutterSpeedList[FUJI_SDK_ELEMENTS_OF_SHUTTER_SPEED] = {
	XSDK_SHUTTER_UNKNOWN,
	XSDK_SHUTTER_1_32000,
	XSDK_SHUTTER_1_25600,
	XSDK_SHUTTER_1_24000,
	XSDK_SHUTTER_1_20000,
	XSDK_SHUTTER_1_16000,
	XSDK_SHUTTER_1_12800,
	XSDK_SHUTTER_1_12000,
	XSDK_SHUTTER_1_10000,
	XSDK_SHUTTER_1_8000,
	XSDK_SHUTTER_1_6400,
	XSDK_SHUTTER_1_6000,
	XSDK_SHUTTER_1_5000,
	XSDK_SHUTTER_1_4000,
	XSDK_SHUTTER_1_3200,
	XSDK_SHUTTER_1_3000,
	XSDK_SHUTTER_1_2500,
	XSDK_SHUTTER_1_2000,
	XSDK_SHUTTER_1_1600,
	XSDK_SHUTTER_1_1500,
	XSDK_SHUTTER_1_1250,
	XSDK_SHUTTER_1_1000,
	XSDK_SHUTTER_1_800,
	XSDK_SHUTTER_1_750,
	XSDK_SHUTTER_1_640,
	XSDK_SHUTTER_1_500,
	XSDK_SHUTTER_1_400,
	XSDK_SHUTTER_1_350,
	XSDK_SHUTTER_1_320,
	XSDK_SHUTTER_1_250,
	XSDK_SHUTTER_1_200,
	XSDK_SHUTTER_1_180,
	XSDK_SHUTTER_1_160,
	XSDK_SHUTTER_1_125,
	XSDK_SHUTTER_1_100,
	XSDK_SHUTTER_1_90,
	XSDK_SHUTTER_1_80,
	XSDK_SHUTTER_1_60,
	XSDK_SHUTTER_1_50,
	XSDK_SHUTTER_1_45,
	XSDK_SHUTTER_1_40,
	XSDK_SHUTTER_1_30,
	XSDK_SHUTTER_1_25,
	XSDK_SHUTTER_1_20H,
	XSDK_SHUTTER_1_20,
	XSDK_SHUTTER_1_15,
	XSDK_SHUTTER_1_13,
	XSDK_SHUTTER_1_10H,
	XSDK_SHUTTER_1_10,
	XSDK_SHUTTER_1_8,
	XSDK_SHUTTER_1_6,
	XSDK_SHUTTER_1_6H,
	XSDK_SHUTTER_1_5,
	XSDK_SHUTTER_1_4,
	XSDK_SHUTTER_1_3,
	XSDK_SHUTTER_1_3H,
	XSDK_SHUTTER_1_2P5,
	XSDK_SHUTTER_1_2,
	XSDK_SHUTTER_1_1P6,
	XSDK_SHUTTER_1_1P5,
	XSDK_SHUTTER_1_1P3,
	XSDK_SHUTTER_1,
	XSDK_SHUTTER_1P3,
	XSDK_SHUTTER_1P5A,
	XSDK_SHUTTER_1P5,
	XSDK_SHUTTER_2,
	XSDK_SHUTTER_2P5,
	XSDK_SHUTTER_3H,
	XSDK_SHUTTER_3,
	XSDK_SHUTTER_4,
	XSDK_SHUTTER_5,
	XSDK_SHUTTER_6H,
	XSDK_SHUTTER_6,
	XSDK_SHUTTER_8,
	XSDK_SHUTTER_10,
	XSDK_SHUTTER_10H,
	XSDK_SHUTTER_13,
	XSDK_SHUTTER_15,
	XSDK_SHUTTER_20,
	XSDK_SHUTTER_20H,
	XSDK_SHUTTER_25,
	XSDK_SHUTTER_30,
	XSDK_SHUTTER_40,
	XSDK_SHUTTER_50,
	XSDK_SHUTTER_60,
	XSDK_SHUTTER_2M,
	XSDK_SHUTTER_4M,
	XSDK_SHUTTER_8M,
	XSDK_SHUTTER_15M,
	XSDK_SHUTTER_30M,
	XSDK_SHUTTER_60M,
	XSDK_SHUTTER_BULB,
	-64000180,
	-64000150,
	-64000120,
	-64000090,
	-64000060,
	-64000030,
	-64000000,
	-59714111,
	-57017517,
	-50796833,
	-48502930,
	-45254833,
	-42224253,
	-40317473,
	-35918785,
	-34296750,
	-32000000,
	-29857055,
	-28508758,
	-25398416,
	-24251465,
	-22627416,
	-21112126,
	-20158736,
	-17959392,
	-17148375,
	-16000000,
	-14928527,
	-14254379,
	-12699208,
	-12125732,
	-11313708,
	-10556063,
	-10079368,
	-8979696,
	-8574187,
	-8000000,
	-7464263,
	-7127189,
	-6349604,
	-6062866,
	-5656854,
	-5278031,
	-5039684,
	-4489848,
	-4287093,
	-4000000,
	-3732131,
	-3563594,
	-3174802,
	-3031433,
	-2828427,
	-2639015,
	-2519842,
	-2244924,
	-2143546,
	-2000000,
	-1866065,
	-1781797,
	-1587401,
	-1515716,
	-1414213,
	-1319507,
	-1259921,
	-1122462,
	-1071773,
	-1000000,
	-933032,
	-890898,
	-793700,
	-757858,
	-707106,
	-659753,
	-629960,
	-561231,
	-535886,
	-500000,
	-466516,
	-445449,
	-396850,
	-378929,
	-353553,
	-329876,
	-314980,
	-280615,
	-267943,
	-250000,
	-233258,
	-222724,
	-198425,
	-189464,
	-176776,
	-164938,
	-157490,
	-140307,
	-133971,
	-125000,
	-116629,
	-111362,
	-99212,
	-94732,
	-88388,
	-82469,
	-78745,
	-70153,
	-66985,
	-62500,
	-58314,
	-55681,
	-49606,
	-47366,
	-44194,
	-41234,
	-39372,
	-35076,
	-33492,
	-31250,
	-29157,
	-27840,
	-24803,
	-23683,
	-22097,
	-20617,
	-19686,
	-17538,
	-16746,
	-15625,
	-14578,
	-13920,
	-12401,
	-11841,
	-11048,
	-10308,
	-9843,
	-8769,
	-8373,
	-7812,
	-7289,
	-6960,
	-6200,
	-5920,
	-5524,
	-5154,
	-4921,
	-4384,
	-4186,
	-3906,
	-3644,
	-3480,
	-3100,
	-2960,
	-2762,
	-2577,
	-2460,
	-2192,
	-2093,
	-1953,
	-1822,
	-1740,
	-1550,
	-1480,
	-1381,
	-1288,
	-1230,
	-1096,
	-1046,
	-976,
	-911,
	-870,
	-775,
	-740,
	-690,
	-644,
	-615,
	-548,
	-523,
	-488,
	-455,
	-435,
	-387,
	-370,
	-345,
	-322,
	-307,
	-274,
	-261,
	-244,
	-227,
	-217,
	-193,
	-185,
	-172,
	-161,
	-153,
	-137,
	-130,
	-122,
	-113,
	-108,
	-96,
	-92,
	-86,
	-80,
	-76,
	-68,
	-65,
	-61,
	-56,
	-54,
	-48,
	-46,
	-43,
	-40,
	-38,
	-34,
	-32,
	-30,
	-28,
	-27,
	-24,
	-23,
	-21,
	-20,
	-19,
	-17,
	-16,
	-15,
};

TCHAR CFujiSDK::g_pptchShutterSpeedText[FUJI_SDK_ELEMENTS_OF_SHUTTER_SPEED][8] = {
	_T(""),
	_T("1/32000"),
	_T("1/25000"),
	_T("1/24000"),
	_T("1/20000"),
	_T("1/16000"),
	_T("1/13000"),
	_T("1/12000"),
	_T("1/10000"),
	_T("1/8000"),
	_T("1/6400"),
	_T("1/6000"),
	_T("1/5000"),
	_T("1/4000"),
	_T("1/3200"),
	_T("1/3000"),
	_T("1/2500"),
	_T("1/2000"),
	_T("1/1600"),
	_T("1/1500"),
	_T("1/1250"),
	_T("1/1000"),
	_T("1/800"),
	_T("1/750"),
	_T("1/640"),
	_T("1/500"),
	_T("1/400"),
	_T("1/350"),
	_T("1/320"),
	_T("1/250"),
	_T("1/200"),
	_T("1/180"),
	_T("1/160"),
	_T("1/125"),
	_T("1/100"),
	_T("1/90"),
	_T("1/80"),
	_T("1/60"),
	_T("1/50"),
	_T("1/45"),
	_T("1/40"),
	_T("1/30"),
	_T("1/25"),
	_T("1/20"),
	_T("1/20"),
	_T("1/15"),
	_T("1/13"),
	_T("1/10"),
	_T("1/10"),
	_T("1/8"),
	_T("1/6"),
	_T("1/6"),
	_T("1/5"),
	_T("1/4"),
	_T("1/3"),
	_T("1/3"),
	_T("1/2.5"),
	_T("1/2"),
	_T("1/1.6"),
	_T("1/1.5"),
	_T("1/1.3"),
	_T("1\""),
	_T("1.3\""),
	_T("1.5\""),
	_T("1.5\""),
	_T("2\""),
	_T("2.5\""),
	_T("3\""),
	_T("3\""),
	_T("4\""),
	_T("5\""),
	_T("6\""),
	_T("6.5\""),
	_T("8\""),
	_T("10\""),
	_T("10\""),
	_T("13\""),
	_T("15\""),
	_T("20\""),
	_T("20\""),
	_T("25\""),
	_T("30\""),
	_T("40\""),
	_T("50\""),
	_T("60\""),
	_T("2'00\""),
	_T("4'00\""),
	_T("8'00\""),
	_T("15'00\""),
	_T("30'00\""),
	_T("60'00\""),
	_T("BULB"),
	_T("60'00\""),
	_T("30'00\""),
	_T("15'00\""),
	_T("8'00\""),
	_T("4'00\""),
	_T("2'00\""),
	_T("60\""),  // _T("60\""),
	_T("58\""),
	_T("56\""),
	_T("50\""),  // _T("52\""),
	_T("50\""),
	_T("45\""),
	_T("42\""),
	_T("40\""),
	_T("38\""),
	_T("34\""),
	_T("30\""),
	_T("28\""),
	_T("27\""),
	_T("26\""),
	_T("25\""),
	_T("20\""),
	_T("20\""),
	_T("20\""),
	_T("18\""),
	_T("17\""),
	_T("15\""),
	_T("15\""),
	_T("14\""),
	_T("13\""),
	_T("12\""),
	_T("10\""),
	_T("10\""),
	_T("10\""),
	_T("9\""),
	_T("8.5\""),
	_T("8\""),
	_T("7.5\""),
	_T("7\""),
	_T("6.5\""),
	_T("6\""),
	_T("6\""),
	_T("5.3\""),
	_T("5\""),
	_T("4.5\""),
	_T("4.3\""),
	_T("4\""),
	_T("3.7\""),
	_T("3.5\""),
	_T("3.2\""),
	_T("3.1\""),
	_T("3\""),
	_T("2.6\""),
	_T("2.5\""),
	_T("2.3\""),
	_T("2.1\""),
	_T("2\""),
	_T("1.9\""),
	_T("1.8\""),
	_T("1.7\""),
	_T("1.6\""),
	_T("1.5\""),
	_T("1.4\""),
	_T("1.3\""),
	_T("1.2\""),
	_T("1.1\""),
	_T("1\""),
	_T("1/1.1"),
	_T("1/1.2"),
	_T("1/1.3"),
	_T("1/1.4"),
	_T("1/1.5"),
	_T("1/1.6"),
	_T("1/1.7"),
	_T("1/1.8"),
	_T("1/1.9"),
	_T("1/2"),
	_T("1/2.1"),
	_T("1/2.3"),
	_T("1/2.5"),
	_T("1/2.6"),
	_T("1/2.8"),
	_T("1/3"),
	_T("1/3.1"),
	_T("1/3.2"),
	_T("1/3.5"),
	_T("1/4"),
	_T("1/4.3"),
	_T("1/4.5"),
	_T("1/5"),
	_T("1/5.3"),
	_T("1/6"),
	_T("1/6"),
	_T("1/6.5"),
	_T("1/7"),
	_T("1/7.5"),
	_T("1/8"),
	_T("1/8.5"),
	_T("1/9"),
	_T("1/10"),
	_T("1/10"),
	_T("1/10"),
	_T("1/12"),
	_T("1/13"),
	_T("1/14"),
	_T("1/15"),
	_T("1/15"),
	_T("1/17"),
	_T("1/18"),
	_T("1/20"),
	_T("1/20"),
	_T("1/20"),
	_T("1/25"),
	_T("1/26"),
	_T("1/27"),
	_T("1/28"),
	_T("1/30"),
	_T("1/34"),
	_T("1/38"),
	_T("1/40"),
	_T("1/42"),
	_T("1/45"),
	_T("1/50"),
	_T("1/52"),
	_T("1/56"),
	_T("1/58"),
	_T("1/60"),
	_T("1/70"),
	_T("1/75"),
	_T("1/80"),
	_T("1/85"),
	_T("1/90"),
	_T("1/100"),
	_T("1/105"),
	_T("1/110"),
	_T("1/120"),
	_T("1/125"),
	_T("1/140"),
	_T("1/150"),
	_T("1/160"),
	_T("1/170"),
	_T("1/180"),
	_T("1/200"),
	_T("1/210"),
	_T("1/220"),
	_T("1/240"),
	_T("1/250"),
	_T("1/280"),
	_T("1/300"),
	_T("1/320"),
	_T("1/340"),
	_T("1/350"),
	_T("1/400"),
	_T("1/420"),
	_T("1/450"),
	_T("1/480"),
	_T("1/500"),
	_T("1/550"),
	_T("1/600"),
	_T("1/640"),
	_T("1/680"),
	_T("1/750"),
	_T("1/800"),
	_T("1/850"),
	_T("1/900"),
	_T("1/950"),
	_T("1/1000"),
	_T("1/1100"),
	_T("1/1250"),
	_T("1/1300"),
	_T("1/1400"),
	_T("1/1500"),
	_T("1/1600"),
	_T("1/1700"),
	_T("1/1800"),
	_T("1/1900"),
	_T("1/2000"),
	_T("1/2200"),
	_T("1/2400"),
	_T("1/2500"),
	_T("1/2700"),
	_T("1/2900"),
	_T("1/3000"),
	_T("1/3200"),
	_T("1/3500"),
	_T("1/3800"),
	_T("1/4000"),
	_T("1/4400"),
	_T("1/4700"),
	_T("1/5000"),
	_T("1/5400"),
	_T("1/5800"),
	_T("1/6000"),
	_T("1/6400"),
	_T("1/7000"),
	_T("1/7500"),
	_T("1/8000"),
	_T("1/8500"),
	_T("1/9000"),
	_T("1/10000"),
	_T("1/10500"),
	_T("1/11000"),
	_T("1/12000"),
	_T("1/13000"),
	_T("1/14000"),
	_T("1/15000"),
	_T("1/16000"),
	_T("1/17000"),
	_T("1/18000"),
	_T("1/20000"),
	_T("1/22000"),
	_T("1/23000"),
	_T("1/25000"),
	_T("1/26000"),
	_T("1/28000"),
	_T("1/30000"),
	_T("1/32000"),
	_T("1/35000"),
	_T("1/38000"),
	_T("1/40000"),
	_T("1/43000"),
	_T("1/46000"),
	_T("1/50000"),
	_T("1/53000"),
	_T("1/57000"),
	_T("1/60000"),
	_T("1/64000"),
};

//
// hModule : EXE --> NULL
//           DLL --> hModule @ DLL Attach
// pRelativePath : Relative path to the SDK
//           Same folder as EXE/DLL, NULL can be assigned
//
CFujiSDK::CFujiSDK( HMODULE hModule, LPCTSTR pRelativePath )
{
	::InitializeCriticalSection( &m_CriticalSection );

	TCHAR pPathAdd[ MAX_PATH ];
	if( pRelativePath != NULL ){
		if( pRelativePath[ 0 ] != _T('\\') ){
			wsprintf( pPathAdd, _T("\\%s"), pRelativePath );
		} else {
			lstrcpy( pPathAdd, pRelativePath );
		}
		if( pPathAdd[ lstrlen( pPathAdd ) -1 ] != _T('\\') ){
			lstrcat( pPathAdd, _T("\\") );
		}
	} else {
		lstrcpy( pPathAdd, _T("\\") );
	}

	// Get My path
	TCHAR  pFilePath[ MAX_PATH ];
	::GetModuleFileName( hModule, pFilePath, MAX_PATH );

	// 
	TCHAR* pTmpPtr;
	pTmpPtr = _tcsrchr( pFilePath, _T('\\') );
	wsprintf(pTmpPtr, _T("%sdlls\\fuji\\XAPI.DLL"), pPathAdd);
	
	m_iErr = FUJI_SDK_ERR_OK;

	m_hModule = ::LoadLibrary( pFilePath );
	if( NULL == m_hModule ){
		// ERROR
		m_iErr = FUJI_SDK_ERR_LOADSDK;
		m_pfnExit = NULL;
	} else {

		m_pfnInit                      = (_XSDK_Init                      )::GetProcAddress( m_hModule, "XSDK_Init" );
		m_pfnExit                      = (_XSDK_Exit                      )::GetProcAddress( m_hModule, "XSDK_Exit" );
		m_pfnDetect                    = (_XSDK_Detect                    )::GetProcAddress( m_hModule, "XSDK_Detect" );
		m_pfnOpenEx                    = (_XSDK_OpenEx                    )::GetProcAddress( m_hModule, "XSDK_OpenEx" );
		m_pfnClose                     = (_XSDK_Close                     )::GetProcAddress( m_hModule, "XSDK_Close" );
		m_pfnPowerOFF                  = (_XSDK_PowerOFF                  )::GetProcAddress( m_hModule, "XSDK_PowerOFF" );
		m_pfnGetErrorNumber            = (_XSDK_GetErrorNumber            )::GetProcAddress( m_hModule, "XSDK_GetErrorNumber" );
		m_pfnGetVersionString          = (_XSDK_GetVersionString          )::GetProcAddress( m_hModule, "XSDK_GetVersionString" );
		m_pfnGetDeviceInfo             = (_XSDK_GetDeviceInfo             )::GetProcAddress( m_hModule, "XSDK_GetDeviceInfo" );
		m_pfnWriteDeviceName           = (_XSDK_WriteDeviceName           )::GetProcAddress( m_hModule, "XSDK_WriteDeviceName" );
		m_pfnGetFirmwareVersion        = (_XSDK_GetFirmwareVersion        )::GetProcAddress( m_hModule, "XSDK_GetFirmwareVersion" );
		m_pfnGetLensInfo               = (_XSDK_GetLensInfo               )::GetProcAddress( m_hModule, "XSDK_GetLensInfo" );
		m_pfnGetLensVersion            = (_XSDK_GetLensVersion            )::GetProcAddress( m_hModule, "XSDK_GetLensVersion" );
		m_pfnCapPriorityMode           = (_XSDK_CapPriorityMode           )::GetProcAddress( m_hModule, "XSDK_CapPriorityMode" );
		m_pfnSetPriorityMode           = (_XSDK_SetPriorityMode           )::GetProcAddress( m_hModule, "XSDK_SetPriorityMode" );
		m_pfnGetPriorityMode           = (_XSDK_GetPriorityMode           )::GetProcAddress( m_hModule, "XSDK_GetPriorityMode" );
		m_pfnCapRelease                = (_XSDK_CapRelease                )::GetProcAddress( m_hModule, "XSDK_CapRelease" );
		m_pfnRelease                   = (_XSDK_Release                   )::GetProcAddress( m_hModule, "XSDK_Release" );

		m_pfnGetReleaseStatus          = (_XSDK_GetReleaseStatus          )::GetProcAddress( m_hModule, "XSDK_GetReleaseStatus");

		m_pfnReadImageInfo             = (_XSDK_ReadImageInfo             )::GetProcAddress( m_hModule, "XSDK_ReadImageInfo" );
		m_pfnReadImage                 = (_XSDK_ReadImage                 )::GetProcAddress( m_hModule, "XSDK_ReadImage" );
		m_pfnReadPreview               = (_XSDK_ReadPreview               )::GetProcAddress( m_hModule, "XSDK_ReadPreview" );
		m_pfnDeleteImage               = (_XSDK_DeleteImage               )::GetProcAddress( m_hModule, "XSDK_DeleteImage" );
		m_pfnGetBufferCapacity         = (_XSDK_GetBufferCapacity         )::GetProcAddress( m_hModule, "XSDK_GetBufferCapacity" );
		m_pfnCapAEMode                 = (_XSDK_CapAEMode                 )::GetProcAddress( m_hModule, "XSDK_CapAEMode" );
		m_pfnSetAEMode                 = (_XSDK_SetAEMode                 )::GetProcAddress( m_hModule, "XSDK_SetAEMode" );
		m_pfnGetAEMode                 = (_XSDK_GetAEMode                 )::GetProcAddress( m_hModule, "XSDK_GetAEMode" );
		m_pfnCapShutterSpeed           = (_XSDK_CapShutterSpeed           )::GetProcAddress( m_hModule, "XSDK_CapShutterSpeed" );
		m_pfnSetShutterSpeed           = (_XSDK_SetShutterSpeed           )::GetProcAddress( m_hModule, "XSDK_SetShutterSpeed" );
		m_pfnGetShutterSpeed           = (_XSDK_GetShutterSpeed           )::GetProcAddress( m_hModule, "XSDK_GetShutterSpeed" );
		m_pfnCapExposureBias           = (_XSDK_CapExposureBias           )::GetProcAddress( m_hModule, "XSDK_CapExposureBias" );
		m_pfnSetExposureBias           = (_XSDK_SetExposureBias           )::GetProcAddress( m_hModule, "XSDK_SetExposureBias" );
		m_pfnGetExposureBias           = (_XSDK_GetExposureBias           )::GetProcAddress( m_hModule, "XSDK_GetExposureBias" );
		m_pfnCapDynamicRange           = (_XSDK_CapDynamicRange           )::GetProcAddress( m_hModule, "XSDK_CapDynamicRange" );
		m_pfnSetDynamicRange           = (_XSDK_SetDynamicRange           )::GetProcAddress( m_hModule, "XSDK_SetDynamicRange" );
		m_pfnGetDynamicRange           = (_XSDK_GetDynamicRange           )::GetProcAddress( m_hModule, "XSDK_GetDynamicRange" );
		m_pfnCapSensitivity            = (_XSDK_CapSensitivity            )::GetProcAddress( m_hModule, "XSDK_CapSensitivity" );
		m_pfnSetSensitivity            = (_XSDK_SetSensitivity            )::GetProcAddress( m_hModule, "XSDK_SetSensitivity" );
		m_pfnGetSensitivity            = (_XSDK_GetSensitivity            )::GetProcAddress( m_hModule, "XSDK_GetSensitivity" );
		m_pfnCapMeteringMode           = (_XSDK_CapMeteringMode           )::GetProcAddress( m_hModule, "XSDK_CapMeteringMode" );
		m_pfnSetMeteringMode           = (_XSDK_SetMeteringMode           )::GetProcAddress( m_hModule, "XSDK_SetMeteringMode" );
		m_pfnGetMeteringMode           = (_XSDK_GetMeteringMode           )::GetProcAddress( m_hModule, "XSDK_GetMeteringMode" );
		m_pfnCapLensZoomPos            = (_XSDK_CapLensZoomPos            )::GetProcAddress( m_hModule, "XSDK_CapLensZoomPos" );
		m_pfnGetLensZoomPos            = (_XSDK_GetLensZoomPos            )::GetProcAddress( m_hModule, "XSDK_GetLensZoomPos" );
		m_pfnSetLensZoomPos            = (_XSDK_SetLensZoomPos            )::GetProcAddress( m_hModule, "XSDK_SetLensZoomPos" );
		m_pfnCapAperture               = (_XSDK_CapAperture               )::GetProcAddress( m_hModule, "XSDK_CapAperture" );
		m_pfnSetAperture               = (_XSDK_SetAperture               )::GetProcAddress( m_hModule, "XSDK_SetAperture" );
		m_pfnGetAperture               = (_XSDK_GetAperture               )::GetProcAddress( m_hModule, "XSDK_GetAperture" );

		m_pfnCapWBMode                 = (_XSDK_CapWBMode                 )::GetProcAddress( m_hModule, "XSDK_CapWBMode" );
		m_pfnSetWBMode                 = (_XSDK_SetWBMode                 )::GetProcAddress( m_hModule, "XSDK_SetWBMode" );
		m_pfnGetWBMode                 = (_XSDK_GetWBMode                 )::GetProcAddress( m_hModule, "XSDK_GetWBMode" );
		m_pfnCapWBColorTemp            = (_XSDK_CapWBColorTemp            )::GetProcAddress( m_hModule, "XSDK_CapWBColorTemp" );
		m_pfnSetWBColorTemp            = (_XSDK_SetWBColorTemp            )::GetProcAddress( m_hModule, "XSDK_SetWBColorTemp" );
		m_pfnGetWBColorTemp            = (_XSDK_GetWBColorTemp            )::GetProcAddress( m_hModule, "XSDK_GetWBColorTemp" );
		m_pfnCapMediaRecord            = (_XSDK_CapMediaRecord            )::GetProcAddress( m_hModule, "XSDK_CapMediaRecord" );
		m_pfnSetMediaRecord            = (_XSDK_SetMediaRecord            )::GetProcAddress( m_hModule, "XSDK_SetMediaRecord" );
		m_pfnGetMediaRecord            = (_XSDK_GetMediaRecord            )::GetProcAddress( m_hModule, "XSDK_GetMediaRecord" );

		m_pfnCapForceMode              = (_XSDK_CapForceMode              )::GetProcAddress( m_hModule, "XSDK_CapForceMode" );
		m_pfnSetForceMode              = (_XSDK_SetForceMode              )::GetProcAddress( m_hModule, "XSDK_SetForceMode" );

		m_pfnSetBackupSettings         = (_XSDK_SetBackupSettings         )::GetProcAddress(m_hModule, "XSDK_SetBackupSettings");
		m_pfnGetBackupSettings         = (_XSDK_GetBackupSettings         )::GetProcAddress(m_hModule, "XSDK_GetBackupSettings");

		m_pfnCapDriveMode = (_XSDK_CapDriveMode)::GetProcAddress(m_hModule, "XSDK_CapDriveMode");
		m_pfnSetDriveMode = (_XSDK_SetDriveMode)::GetProcAddress(m_hModule, "XSDK_SetDriveMode");
		m_pfnGetDriveMode = (_XSDK_GetDriveMode)::GetProcAddress(m_hModule, "XSDK_GetDriveMode");

		m_pfnCapProp                   = (_XSDK_CapProp                   )::GetProcAddress( m_hModule, "XSDK_CapProp" );
		m_pfnSetProp                   = (_XSDK_SetProp                   )::GetProcAddress( m_hModule, "XSDK_SetProp" );
		m_pfnGetProp                   = (_XSDK_GetProp                   )::GetProcAddress( m_hModule, "XSDK_GetProp" );


		if( ( m_pfnInit == NULL )
		    || ( m_pfnExit == NULL )
		    || ( m_pfnDetect == NULL )
			|| ( m_pfnOpenEx == NULL )
		    || ( m_pfnClose == NULL )
		    || ( m_pfnPowerOFF == NULL )
		    || ( m_pfnGetErrorNumber == NULL )
		    || ( m_pfnGetVersionString == NULL )
		    || ( m_pfnGetDeviceInfo == NULL )
		    || ( m_pfnWriteDeviceName == NULL )
		    || ( m_pfnGetFirmwareVersion == NULL )
		    || ( m_pfnGetLensInfo == NULL )
		    || ( m_pfnGetLensVersion == NULL )
		    || ( m_pfnCapPriorityMode == NULL )
		    || ( m_pfnSetPriorityMode == NULL )
		    || ( m_pfnGetPriorityMode == NULL )
		    || ( m_pfnCapRelease == NULL )
		    || ( m_pfnRelease == NULL )
		    || ( m_pfnGetReleaseStatus == NULL)
		    || ( m_pfnReadImageInfo == NULL )
		    || ( m_pfnReadImage == NULL )
		    || ( m_pfnReadPreview == NULL )
		    || ( m_pfnDeleteImage == NULL )
		    || ( m_pfnGetBufferCapacity == NULL )
		    || ( m_pfnCapAEMode == NULL )
		    || ( m_pfnSetAEMode == NULL )
		    || ( m_pfnGetAEMode == NULL )
		    || ( m_pfnCapShutterSpeed == NULL )
		    || ( m_pfnSetShutterSpeed == NULL )
		    || ( m_pfnGetShutterSpeed == NULL )
		    || ( m_pfnCapExposureBias == NULL )
		    || ( m_pfnSetExposureBias == NULL )
		    || ( m_pfnGetExposureBias == NULL )
		    || ( m_pfnCapDynamicRange == NULL )
		    || ( m_pfnSetDynamicRange == NULL )
		    || ( m_pfnGetDynamicRange == NULL )
		    || ( m_pfnCapSensitivity == NULL )
		    || ( m_pfnSetSensitivity == NULL )
		    || ( m_pfnGetSensitivity == NULL )
		    || ( m_pfnCapMeteringMode == NULL )
		    || ( m_pfnSetMeteringMode == NULL )
		    || ( m_pfnGetMeteringMode == NULL )
		    || ( m_pfnCapLensZoomPos == NULL )
		    || ( m_pfnGetLensZoomPos == NULL )
		    || ( m_pfnSetLensZoomPos == NULL )
		    || ( m_pfnCapAperture == NULL )
		    || ( m_pfnSetAperture == NULL )
		    || ( m_pfnGetAperture == NULL )
		    || ( m_pfnCapWBMode == NULL )
		    || ( m_pfnSetWBMode == NULL )
		    || ( m_pfnGetWBMode == NULL )
		    || ( m_pfnCapWBColorTemp == NULL )
		    || ( m_pfnSetWBColorTemp == NULL )
		    || ( m_pfnGetWBColorTemp == NULL )
		    || ( m_pfnCapMediaRecord == NULL )
		    || ( m_pfnSetMediaRecord == NULL )
		    || ( m_pfnGetMediaRecord == NULL )
			|| ( m_pfnCapForceMode == NULL )
			|| ( m_pfnSetForceMode == NULL )
			|| ( m_pfnSetBackupSettings == NULL )
		    || ( m_pfnGetBackupSettings == NULL )
			|| ( m_pfnSetDriveMode == NULL )
			|| ( m_pfnGetDriveMode == NULL )
		    || ( m_pfnCapProp == NULL )
		    || ( m_pfnSetProp == NULL )
		    || ( m_pfnGetProp == NULL ) ){

			 ::FreeLibrary( m_hModule );
			m_hModule = NULL;
				
			// ERROR
			m_iErr = FUJI_SDK_ERR_SDKVERSION;
			m_pfnExit = NULL;

		} else {
			// Success

		}

	}
	
}

CFujiSDK::~CFujiSDK(void)
{
	::DeleteCriticalSection( &m_CriticalSection );

	if (NULL != m_hModule){
		try{
			::FreeLibrary(m_hModule);
		}
		catch (...) {

		}
		m_hModule = NULL;
	}
}

int CFujiSDK::GetError()
{
	return m_iErr;
}


HMODULE CFujiSDK::GetHModule()
{
	return m_hModule;
}


void CFujiSDK::EnterCriticalSection()
{
	::EnterCriticalSection( &m_CriticalSection );
}

void CFujiSDK::LeaveCriticalSection()
{
	::LeaveCriticalSection( &m_CriticalSection );
}


