/*
 *  XAPI.h
 *  
 *    FUJIFILM X Series Digital Camera Shooting SDK Header file
 *
 *    Version 1.10.0.0
 *
 *  Copyright (C) 2014-2021 FUJIFILM Corporation.   
 *
 */

#ifndef __XAPI_H__
#define __XAPI_H__

//
// Type defines
//
#ifdef  _WIN32  // for Windows
    #include <windows.h>
    typedef HINSTANCE           LIB_HANDLE;
    typedef HANDLE              XSDK_HANDLE;
    #ifdef  _XSDK_INTERNAL_
        #define XSDK_APIENTRY  __declspec(dllexport)    long
    #else
        #define XSDK_APIENTRY  __declspec(dllimport)    long
    #endif
#else           // for Macintosh
    #import <CoreFoundation/CoreFoundation.h>
    typedef CFBundleRef         LIB_HANDLE;
    typedef UInt8               BYTE;
    typedef char*               LPSTR;
    typedef const char*         LPCSTR;
    typedef void*               PVOID;
    typedef Handle              XSDK_HANDLE;
    typedef long                XSDK_APIENTRY;
#endif  // _WIN32

//
// Structure defines
//

// Device infomation
#pragma pack(1)
typedef struct{
    char    strVendor[256];
    char    strManufacturer[256];
    char    strProduct[256];
    char    strFirmware[256];
    char    strDeviceType[256];
    char    strSerialNo[256];
    char    strFramework[256];
    BYTE    bDeviceId;
    char    strDeviceName[32];
    char    strYNo[32];
} XSDK_DeviceInformation;
#pragma pack()

// Changed Property
#pragma pack(1)
typedef struct{
    long    n1;
    long    n2;
    long    n3;
    long    n4;
    long    n5;
    long    n6;
    long    n7;
    long    n8;
    long    n9;
    long    n10;
} XSDK_DATA_INT;
#pragma pack()

#pragma pack(1)
typedef union {
    XSDK_DATA_INT   n;
    char            c[40];
} XSDK_DATA;
#pragma pack()

#pragma pack(1)
typedef struct{
    long        apiCode;
    XSDK_DATA   data;
} XSDK_PROPERTY;
#pragma pack()

#pragma pack(1)
typedef struct{
    unsigned short  propCnt;
    XSDK_PROPERTY   prop[200];
} XSDK_CHANGEDPROPERTY;
#pragma pack()

// Image information
#pragma pack(1)
typedef struct{
    char        strInternalName[32];
    long        lFormat;
    long        lDataSize;
    long        lImagePixHeight;
    long        lImagePixWidth;
    long        lImageBitDepth;
    long        lPreviewSize;
    XSDK_HANDLE hImage;
} XSDK_ImageInformation;
#pragma pack()

// Lens Info
#pragma pack(1)
typedef struct{
    char    strModel[20];
    char    strProductName[100];
    char    strSerialNo[20];
    long    lISCapability;
    long    lMFCapability;
    long    lZoomPosCapability;
} XSDK_LensInformation;
#pragma pack()

/////////////////////////////////////////////////////////////////////////////////////////////
//  API Code
enum{
    // Initialize/Finalize
    API_CODE_Init                       = 0x1001,
    API_CODE_Exit                       = 0x1002,

    // Enumeration
    API_CODE_Detect                     = 0x1010,

    // Session management
    API_CODE_Close                      = 0x1022,
    API_CODE_PowerOFF                   = 0x1023,
    API_CODE_OpenEx                     = 0x1024,

    // Basic functions
    API_CODE_GetErrorNumber             = 0x1031,
    API_CODE_GetVersionString           = 0x1032,

    // Device Information
    API_CODE_GetDeviceInfo              = 0x1041,
    API_CODE_WriteDeviceName            = 0x1042,
    API_CODE_GetFirmwareVersion         = 0x1043,
    API_CODE_GetLensInfo                = 0x1044,
    API_CODE_GetLensVersion             = 0x1045,

    // Camera Operation Mode
    API_CODE_CapPriorityMode            = 0x1101,
    API_CODE_SetPriorityMode            = 0x1102,
    API_CODE_GetPriorityMode            = 0x1103,

    // Release Control
    API_CODE_CapRelease                 = 0x1111,
    API_CODE_Release                    = 0x1112,
    API_CODE_GetReleaseStatus           = 0x1113,
	API_CODE_GetRecordingStatus			= 0x1114,

    // Image Acquisition 
    API_CODE_GetBufferCapacity          = 0x1200,
    API_CODE_ReadImageInfo              = 0x1201,
    API_CODE_ReadPreview                = 0x1202,
    API_CODE_ReadImage                  = 0x1203,
    API_CODE_DeleteImage                = 0x1204,

    // Exposure control
    API_CODE_CapAEMode                  = 0x1301,
    API_CODE_SetAEMode                  = 0x1302,
    API_CODE_GetAEMode                  = 0x1303,
    API_CODE_CapShutterSpeed            = 0x1304,
    API_CODE_SetShutterSpeed            = 0x1305,
    API_CODE_GetShutterSpeed            = 0x1306,
    API_CODE_CapExposureBias            = 0x1307,
    API_CODE_SetExposureBias            = 0x1308,
    API_CODE_GetExposureBias            = 0x1309,
    API_CODE_CapSensitivity             = 0x1311,
    API_CODE_SetSensitivity             = 0x1312,
    API_CODE_GetSensitivity             = 0x1313,
    API_CODE_CapDynamicRange            = 0x1314,
    API_CODE_SetDynamicRange            = 0x1315,
    API_CODE_GetDynamicRange            = 0x1316,
    API_CODE_CapMeteringMode            = 0x1317,
    API_CODE_SetMeteringMode            = 0x1318,
    API_CODE_GetMeteringMode            = 0x1319,

    API_CODE_CapLensZoomPos             = 0x1321,
    API_CODE_SetLensZoomPos             = 0x1322,
    API_CODE_GetLensZoomPos             = 0x1323,
    API_CODE_CapAperture                = 0x1324,
    API_CODE_SetAperture                = 0x1325,
    API_CODE_GetAperture                = 0x1326,

    API_CODE_CapWBMode                  = 0x1331,
    API_CODE_SetWBMode                  = 0x1332,
    API_CODE_GetWBMode                  = 0x1333,
    API_CODE_CapWBColorTemp             = 0x1334,
    API_CODE_SetWBColorTemp             = 0x1335,
    API_CODE_GetWBColorTemp             = 0x1336,

	API_CODE_CapMovieShutterSpeed		= 0x1340,
	API_CODE_SetMovieShutterSpeed		= 0x1341,
	API_CODE_GetMovieShutterSpeed		= 0x1342,
	API_CODE_CapMovieExposureBias		= 0x1343,
	API_CODE_SetMovieExposureBias		= 0x1344,
	API_CODE_GetMovieExposureBias		= 0x1345,
	API_CODE_CapMovieSensitivity		= 0x1346,
	API_CODE_SetMovieSensitivity		= 0x1347,
	API_CODE_GetMovieSensitivity		= 0x1348,
	API_CODE_CapMovieAperture			= 0x1349,
	API_CODE_SetMovieAperture			= 0x134A,
	API_CODE_GetMovieAperture			= 0x134B,

    // Media control
    API_CODE_CapMediaRecord             = 0x1351,
    API_CODE_SetMediaRecord             = 0x1352,
    API_CODE_GetMediaRecord             = 0x1353,

    // Force Mode
    API_CODE_CapForceMode               = 0x1371,
    API_CODE_SetForceMode               = 0x1372,

    // BackupSettings
    API_CODE_SetBackupSettings          = 0x1375,
    API_CODE_GetBackupSettings          = 0x1376,

	// Drive Mode
	API_CODE_SetDriveMode               = 0x1377,
	API_CODE_GetDriveMode               = 0x1378,
	API_CODE_CapDriveMode               = 0x1379,

	// Mode
	API_CODE_CapMode                    = 0x137A,
	API_CODE_SetMode                    = 0x137B,
	API_CODE_GetMode                    = 0x137C,
    
    //Model Dependent Function Call I/F
    API_CODE_CapProp                    = 0x1401,
    API_CODE_SetProp                    = 0x1402,
    API_CODE_GetProp                    = 0x1403,
};


//
// Constants defines
//

// DSC connect I/F
#define XSDK_DSC_IF_USB                     0x00000001  // USB
#define XSDK_DSC_IF_WIFI_LOCAL              0x00000010  // Wi-Fi within local segment
#define XSDK_DSC_IF_WIFI_IP                 0x00000020  // Wi-Fi specified IPv4 address

// Camera Mode
#define XSDK_DSC_MODE_TETHER                0x0001
#define XSDK_DSC_MODE_RAW                   0x0002
#define XSDK_DSC_MODE_BR                    0x0004
#define XSDK_DSC_MODE_WEBCAM                0x0008

// Priority Mode
#define XSDK_PRIORITY_CAMERA                0x0001
#define XSDK_PRIORITY_PC                    0x0002

// Release Mode
#define XSDK_RELEASE_MASK_ONMODE            0xFF00
#define XSDK_RELEASE_MASK_OFFMODE           0x00FF
// (On Mode)
#define XSDK_RELEASE_SHOOT                  0x0100
#define XSDK_RELEASE_S1ON                   0x0200
#define XSDK_RELEASE_S2                     0x0300
#define XSDK_RELEASE_BULB_ON                0x0400
#define XSDK_RELEASE_REC_START				0x0800
#define XSDK_RELEASE_BULBS2_ON              0x0500
#define XSDK_RELEASE_PIXELSHIFT             0x4000
#define XSDK_RELEASE_CUSWB                  0x8000
#define XSDK_RELEASE_AEON                   0x9000
#define XSDK_RELEASE_AFON                   0x9100
#define XSDK_RELEASE_AFAEON                 0x9200
#define XSDK_RELEASE_AF                     0x9300
#define XSDK_RELEASE_INSTANTAF              0xA000
#define XSDK_RELEASE_WBLON					0x9200

// (Off Mode)
#define XSDK_RELEASE_N_AFOFF                0x0001
#define XSDK_RELEASE_N_AEOFF                0x0002
#define XSDK_RELEASE_N_AFAEOFF              0x0003
#define XSDK_RELEASE_N_S1OFF                0x0004
#define XSDK_RELEASE_N_AF                   0x0020
#define XSDK_RELEASE_N_INSTANTAF            0x0010
#define XSDK_RELEASE_N_BULBOFF              0x0008
#define XSDK_RELEASE_REC_STOP				0x0080
#define XSDK_RELEASE_N_BULBS2OFF            0x0008
#define XSDK_RELEASE_N_BULBAFOFF            (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFOFF)
#define XSDK_RELEASE_N_BULBAEOFF            (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AEOFF)
#define XSDK_RELEASE_N_BULBAFAEOFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFAEOFF)
#define XSDK_RELEASE_N_BULBS1OFF            (XSDK_RELEASE_N_BULBS2OFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAFS1OFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAES1OFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAFAES1OFF        (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFAEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_CANCEL                 0x000F
#define XSDK_RELEASE_N_WBLOFF				0x0040

// (On and Off Mode)
#define XSDK_RELEASE_SHOOT_S1OFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_SHOOT_AEOFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AEOFF)
#define XSDK_RELEASE_SHOOT_AFOFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AFOFF)
#define XSDK_RELEASE_SHOOT_AFAEOFF          (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AFAEOFF)
#define XSDK_RELEASE_S2_S1OFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AFOFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF)                
#define XSDK_RELEASE_S2_AEOFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AEOFF)                
#define XSDK_RELEASE_S2_AFAEOFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFAEOFF)              
#define XSDK_RELEASE_S2_AFS1OFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AES1OFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AFAES1OFF           (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_REC_START_S1OFF        (XSDK_RELEASE_REC_START | XSDK_RELEASE_N_S1OFF)

// Release AFStatus
#define XSDK_RELEASE_OK                     1           // AF OK or CWB_AE_NOERR
#define XSDK_RELEASE_AF_FAILURE             0           // !AF
#define XSDK_RELEASE_AF_UNCHECK             2           // AF not checked
#define XSDK_RELEASE_AF_NOMOVE              4           // AF no Movement

#define XSDK_RELEASE_CWB_AE_OVER            2           // CWB Exposure Over
#define XSDK_RELEASE_CWB_AE_UNDER           3           // CWB Exposure Under


// Release Status (return in bitmap)
#define XSDK_RELEASE_STATUS_S1              0x0200
#define XSDK_RELEASE_STATUS_BULB            0x0400
#define XSDK_RELEASE_STATUS_AF              0x0800
#define XSDK_RELEASE_STATUS_AEL             0x4000
#define XSDK_RELEASE_STATUS_AFL             0x8000
#define XSDK_RELEASE_STATUS_WBL             0x2000
#define XSDK_RELEASE_STATUS_SHOOTING        0x0100

// Recording Status 
#define XSDK_REC_STATUS_NOT_AVAILABLE       0x0001
#define XSDK_REC_STATUS_STANDBY             0x0002
#define XSDK_REC_STATUS_RECORDING           0x0004
#define XSDK_REC_STATUS_STORING             0x0008

// Image Format(ReadImageInfo)
#define XSDK_IMAGEFORMAT_RAW                1
#define XSDK_IMAGEFORMAT_LIVE               4
#define XSDK_IMAGEFORMAT_NONE               5
#define XSDK_IMAGEFORMAT_JPEG               7
#define XSDK_IMAGEFORMAT_JPEG_90            0x0607
#define XSDK_IMAGEFORMAT_JPEG_180           0x0307
#define XSDK_IMAGEFORMAT_JPEG_270           0x0807
#define XSDK_IMAGEFORMAT_RAW_90             0x0601
#define	XSDK_IMAGEFORMAT_RAW_180            0x0301
#define	XSDK_IMAGEFORMAT_RAW_270            0x0801
#define	XSDK_IMAGEFORMAT_LIVE_90            0x0604
#define	XSDK_IMAGEFORMAT_LIVE_180           0x0304
#define	XSDK_IMAGEFORMAT_LIVE_270           0x0804

// AE mode
#define XSDK_AE_OFF                         0x0001      //  Manual Mode
#define XSDK_AE_APERTURE_PRIORITY           0x0003      //  Aperture Priority Mode
#define XSDK_AE_SHUTTER_PRIORITY            0x0004      //  Shutter Priority Mode
#define XSDK_AE_PROGRAM                     0x0006      //  Program Mode

// Aperture
#define XSDK_IRIS_NONE                      0           // F0 or F-- such as MOUNT ADAPTER
#define XSDK_IRIS_AUTO                      0xFFFF      // AUTO, Only for Get

// ShutterSpeed
#define XSDK_SHUTTER_UNKNOWN                0           //  PB mode or SET-UP menu is in use
#define XSDK_SHUTTER_1_65000                15          //  1/65000"
#define XSDK_SHUTTER_1_64000                15          //  1/64000"
#define XSDK_SHUTTER_1_60000                15          //  1/64000"
#define XSDK_SHUTTER_1_51200                19          //  1/51200"
#define XSDK_SHUTTER_1_50000                19          //  1/53000"
#define XSDK_SHUTTER_1_40000                24          //  1/40000"
#define XSDK_SHUTTER_1_32000                30          //  1/32000"
#define XSDK_SHUTTER_1_25600                38          //  1/25600"
#define XSDK_SHUTTER_1_25000                38          //  1/25000"
#define XSDK_SHUTTER_1_24000                43          //  1/24000"
#define XSDK_SHUTTER_1_20000                48          //  1/20000"
#define XSDK_SHUTTER_1_16000                61          //  1/16000"
#define XSDK_SHUTTER_1_13000                76          //  1/13000"
#define XSDK_SHUTTER_1_12800                76          //  1/12800"
#define XSDK_SHUTTER_1_12000                86          //  1/12000"
#define XSDK_SHUTTER_1_10000                96          //  1/10000"
#define XSDK_SHUTTER_1_8000                 122         //  1/8000"
#define XSDK_SHUTTER_1_6400                 153         //  1/6400"
#define XSDK_SHUTTER_1_6000                 172         //  1/6000"
#define XSDK_SHUTTER_1_5000                 193         //  1/5000"
#define XSDK_SHUTTER_1_4000                 244         //  1/4000"
#define XSDK_SHUTTER_1_3200                 307         //  1/3200"
#define XSDK_SHUTTER_1_3000                 345         //  1/3000"
#define XSDK_SHUTTER_1_2500                 387         //  1/2500"
#define XSDK_SHUTTER_1_2000                 488         //  1/2000"
#define XSDK_SHUTTER_1_1600                 615         //  1/1600"
#define XSDK_SHUTTER_1_1500                 690         //  1/1500"
#define XSDK_SHUTTER_1_1250                 775         //  1/1250"
#define XSDK_SHUTTER_1_1000                 976         //  1/1000"
#define XSDK_SHUTTER_1_800                  1230        //  1/800"
#define XSDK_SHUTTER_1_750                  1381        //  1/750"
#define XSDK_SHUTTER_1_640                  1550        //  1/640"
#define XSDK_SHUTTER_1_500                  1953        //  1/500"
#define XSDK_SHUTTER_1_400                  2460        //  1/400"
#define XSDK_SHUTTER_1_350                  2762        //  1/350"
#define XSDK_SHUTTER_1_320                  3100        //  1/320"
#define XSDK_SHUTTER_1_250                  3906        //  1/250"
#define XSDK_SHUTTER_1_200                  4921        //  1/200"
#define XSDK_SHUTTER_1_180                  5524        //  1/180"
#define XSDK_SHUTTER_1_160                  6200        //  1/160"
#define XSDK_SHUTTER_1_125                  7812        //  1/125"
#define XSDK_SHUTTER_1_100                  9843        //  1/100"
#define XSDK_SHUTTER_1_90                   11048       //  1/90"
#define XSDK_SHUTTER_1_80                   12401       //  1/80"
#define XSDK_SHUTTER_1_60                   15625       //  1/60"
#define XSDK_SHUTTER_1_50                   19686       //  1/50"
#define XSDK_SHUTTER_1_45                   22097       //  1/45"
#define XSDK_SHUTTER_1_40                   24803       //  1/40"
#define XSDK_SHUTTER_1_30                   31250       //  1/30"
#define XSDK_SHUTTER_1_25                   39372       //  1/25"
#define XSDK_SHUTTER_1_20H                  44194       //  1/20"
#define XSDK_SHUTTER_1_20                   49606       //  1/20"
#define XSDK_SHUTTER_1_15                   62500       //  1/15"
#define XSDK_SHUTTER_1_13                   78745       //  1/13"
#define XSDK_SHUTTER_1_10H                  88388       //  1/10"
#define XSDK_SHUTTER_1_10                   99212       //  1/10"
#define XSDK_SHUTTER_1_8                    125000      //  1/8"
#define XSDK_SHUTTER_1_6                    157490      //  1/6"
#define XSDK_SHUTTER_1_6H                   176776      //  1/6"
#define XSDK_SHUTTER_1_5                    198425      //  1/5"
#define XSDK_SHUTTER_1_4                    250000      //  1/4"
#define XSDK_SHUTTER_1_3                    314980      //  1/3"
#define XSDK_SHUTTER_1_3H                   353553      //  1/3"
#define XSDK_SHUTTER_1_2P5                  396850      //  1/2.5"
#define XSDK_SHUTTER_1_2                    500000      //  1/2"
#define XSDK_SHUTTER_1_1P6                  629960      //  1/1.6"
#define XSDK_SHUTTER_1_1P5                  707106      //  1/1.5"
#define XSDK_SHUTTER_1_1P3                  793700      //  1/1.3"
#define XSDK_SHUTTER_1                      1000000     //  1"
#define XSDK_SHUTTER_1P3                    1259921     //  1.3"
#define XSDK_SHUTTER_1P5                    1414213     //  1.5" (X-T1 only)
#define XSDK_SHUTTER_1P5A                   1587401     //  1.5"
#define XSDK_SHUTTER_1P6                    1587401     //  1.6"
#define XSDK_SHUTTER_2                      2000000     //  2"
#define XSDK_SHUTTER_2P5                    2519842     //  2.5"
#define XSDK_SHUTTER_3H                     2828427     //  3"
#define XSDK_SHUTTER_3                      3174802     //  3"
#define XSDK_SHUTTER_4                      4000000     //  4"
#define XSDK_SHUTTER_5                      5039684     //  5"
#define XSDK_SHUTTER_6H                     5656854     //  6"
#define XSDK_SHUTTER_6                      6349604     //  6"
#define XSDK_SHUTTER_6P5                    6349604     //  6"
#define XSDK_SHUTTER_8                      8000000     //  8"
#define XSDK_SHUTTER_10                     10079368    //  10"
#define XSDK_SHUTTER_10H                    11313708    //  10"
#define XSDK_SHUTTER_13                     12699208    //  13"
#define XSDK_SHUTTER_15                     16000000    //  15"
#define XSDK_SHUTTER_20                     20158736    //  20"
#define XSDK_SHUTTER_20H                    22627416    //  20"
#define XSDK_SHUTTER_25                     25398416    //  25"
#define XSDK_SHUTTER_30                     32000000    //  30"
#define XSDK_SHUTTER_35S                    35000000    //  35"
#define XSDK_SHUTTER_40S                    40000000    //  40"
#define XSDK_SHUTTER_40                     40317473    //  40"
#define XSDK_SHUTTER_45S                    45000000    //  45"
#define XSDK_SHUTTER_50S                    50000000    //  50"
#define XSDK_SHUTTER_50                     50796833    //  52"
#define XSDK_SHUTTER_55S                    55000000    //  55"
#define XSDK_SHUTTER_60S                    60000000    //  60"
#define XSDK_SHUTTER_60                     64000000    //  60"
#define XSDK_SHUTTER_80                     80634947    //  80"
#define XSDK_SHUTTER_100                    101593667   //  110"
#define XSDK_SHUTTER_2M                     64000030    //  120"
#define XSDK_SHUTTER_125                    128000000   //  140"
#define XSDK_SHUTTER_160                    161269894   //  170"
#define XSDK_SHUTTER_200                    203187334   //  210"
#define XSDK_SHUTTER_4M                     64000060    //  240"
#define XSDK_SHUTTER_250                    256000000   //  250"
#define XSDK_SHUTTER_320                    322539788   //  320"
#define XSDK_SHUTTER_400                    406374669   //  420"
#define XSDK_SHUTTER_8M                     64000090    //  480"
#define XSDK_SHUTTER_500                    512000000   //  500"
#define XSDK_SHUTTER_640                    645079577   //  640"
#define XSDK_SHUTTER_800                    812749338   //  850"
#define XSDK_SHUTTER_15M                    64000120    //  900"
#define XSDK_SHUTTER_1000                   1024000000  //  1000"
#define XSDK_SHUTTER_1300                   1290159155  //  1300"
#define XSDK_SHUTTER_1600                   1625498677  //  1700"
#define XSDK_SHUTTER_30M                    64000150    //  1800"
#define XSDK_SHUTTER_2000                   2048000000  //  2000"
#define XSDK_SHUTTER_60M                    64000180    //  3600"
#define XSDK_SHUTTER_BULB                   -1          //  BULB

// S1 Lock ShutterSpeed (1/10EV Step)
#define XSDK_SS_2000                        -2048000000 //  2000"
#define XSDK_SS_1900                        -1910851566 //  1900"
#define XSDK_SS_1800                        -1824560574 //  1800"
#define XSDK_SS_1700                        -1625498677 //  1700"
#define XSDK_SS_1600                        -1552093764 //  1600"
#define XSDK_SS_1500                        -1448154687 //  1500"
#define XSDK_SS_1400                        -1351176100 //  1400"
#define XSDK_SS_1300                        -1290159155 //  1300"
#define XSDK_SS_1200                        -1149401137 //  1200"
#define XSDK_SS_1100                        -1097496025 //  1100"
#define XSDK_SS_1000                        -1024000000 //  1000"
#define XSDK_SS_950                         -955425783  //  950"
#define XSDK_SS_900                         -912280287  //  900"
#define XSDK_SS_850                         -812749338  //  850"
#define XSDK_SS_800                         -776046882  //  800"
#define XSDK_SS_750                         -724077343  //  750"
#define XSDK_SS_680                         -675588050  //  680"
#define XSDK_SS_640                         -645079577  //  640"
#define XSDK_SS_600                         -574700568  //  600"
#define XSDK_SS_550                         -548748012  //  550"
#define XSDK_SS_500                         -512000000  //  500"
#define XSDK_SS_480                         -477712891  //  480"
#define XSDK_SS_450                         -456140143  //  450"
#define XSDK_SS_420                         -406374669  //  420"
#define XSDK_SS_400                         -388023441  //  400"
#define XSDK_SS_350                         -362038671  //  350"
#define XSDK_SS_340                         -337794025  //  340"
#define XSDK_SS_320                         -322539788  //  320"
#define XSDK_SS_300                         -287350284  //  300"
#define XSDK_SS_280                         -274374006  //  280"
#define XSDK_SS_250                         -256000000  //  250"
#define XSDK_SS_240                         -238856445  //  240"
#define XSDK_SS_220                         -228070071  //  220"
#define XSDK_SS_210                         -203187334  //  210"
#define XSDK_SS_200                         -194011720  //  200"
#define XSDK_SS_180                         -181019335  //  180"
#define XSDK_SS_170                         -161269894  //  170"
#define XSDK_SS_160                         -143675142  //  160"
#define XSDK_SS_150                         -137187003  //  150"
#define XSDK_SS_140                         -128000000  //  140"
#define XSDK_SS_125                         -119428222  //  125"
#define XSDK_SS_120                         -114035035  //  120"
#define XSDK_SS_110                         -101593667  //  110"
#define XSDK_SS_100                         -97005860   //  100"
#define XSDK_SS_90                          -90509667   //  90"
#define XSDK_SS_85                          -84448506   //  85"
#define XSDK_SS_80                          -80634947   //  80"
#define XSDK_SS_75                          -71837571   //  75"
#define XSDK_SS_70                          -68593501   //  70"
#define XSDK_SS_60                          -64000000   //  60"
#define XSDK_SS_58                          -59714111   //  58"
#define XSDK_SS_56                          -57017517   //  56"
#define XSDK_SS_52                          -50796833   //  52"
#define XSDK_SS_50                          -48502930   //  50"
#define XSDK_SS_45                          -45254833   //  45"
#define XSDK_SS_42                          -42224253   //  42"
#define XSDK_SS_40                          -40317473   //  40"
#define XSDK_SS_38                          -35918785   //  38"
#define XSDK_SS_34                          -34296750   //  34"
#define XSDK_SS_30                          -32000000   //  30乭
#define XSDK_SS_28                          -29857055   //  28乭
#define XSDK_SS_27                          -28508758   //  27乭
#define XSDK_SS_26                          -25398416   //  26乭
#define XSDK_SS_25                          -24251465   //  25乭
#define XSDK_SS_20                          -22627416   //  20乭
#define XSDK_SS_18                          -17959392   //  18乭
#define XSDK_SS_17                          -17148375   //  17乭
#define XSDK_SS_15                          -16000000   //  15乭
#define XSDK_SS_14                          -14254379   //  14乭
#define XSDK_SS_13                          -12699208   //  13乭
#define XSDK_SS_12                          -12125732   //  12乭
#define XSDK_SS_10                          -11313708   //  10乭
#define XSDK_SS_9                           -8979696    //  9乭
#define XSDK_SS_8P5                         -8574187    //  8.5乭
#define XSDK_SS_8                           -8000000    //  8乭
#define XSDK_SS_7P5                         -7464263    //  7.5乭
#define XSDK_SS_7                           -7127189    //  7乭
#define XSDK_SS_6P5                         -6349604    //  6.5乭
#define XSDK_SS_6                           -6062866    //  6乭
#define XSDK_SS_5P3                         -5278031    //  5.3乭
#define XSDK_SS_5                           -5039684    //  5乭
#define XSDK_SS_4P5                         -4489848    //  4.5乭
#define XSDK_SS_4P3                         -4287093    //  4.3乭
#define XSDK_SS_4                           -4000000    //  4乭
#define XSDK_SS_3P7                         -3732131    //  3.7乭
#define XSDK_SS_3P5                         -3563594    //  3.5乭
#define XSDK_SS_3P2                         -3174802    //  3.2乭
#define XSDK_SS_3P1                         -3031433    //  3.1乭
#define XSDK_SS_3                           -2828427    //  3乭
#define XSDK_SS_2P6                         -2639015    //  2.6乭
#define XSDK_SS_2P5                         -2519842    //  2.5乭
#define XSDK_SS_2P3                         -2244924    //  2.3乭
#define XSDK_SS_2P1                         -2143546    //  2.1乭
#define XSDK_SS_2                           -2000000    //  2乭
#define XSDK_SS_1P9                         -1866065    //  1.9乭
#define XSDK_SS_1P8                         -1781797    //  1.8乭
#define XSDK_SS_1P7                         -1587401    //  1.7乭
#define XSDK_SS_1P6                         -1515716    //  1.6乭
#define XSDK_SS_1P5                         -1414213    //  1.5乭
#define XSDK_SS_1P4                         -1319507    //  1.4乭
#define XSDK_SS_1P3                         -1259921    //  1.3乭
#define XSDK_SS_1P2                         -1122462    //  1.2乭
#define XSDK_SS_1P1                         -1071773    //  1.1乭
#define XSDK_SS_1                           -1000000    //  1乭
#define XSDK_SS_1_1P1                       -933032     //  1/1.1"
#define XSDK_SS_1_1P2                       -890898     //  1/1.2"
#define XSDK_SS_1_1P3                       -793700     //  1/1.3"
#define XSDK_SS_1_1P4                       -757858     //  1/1.4"
#define XSDK_SS_1_1P5                       -707106     //  1/1.5"
#define XSDK_SS_1_1P6                       -659753     //  1/1.6"
#define XSDK_SS_1_1P7                       -629960     //  1/1.7"
#define XSDK_SS_1_1P8                       -561231     //  1/1.8"
#define XSDK_SS_1_1P9                       -535886     //  1/1.9"
#define XSDK_SS_1_2                         -500000     //  1/2"
#define XSDK_SS_1_2P1                       -466516     //  1/2.1"
#define XSDK_SS_1_2P3                       -445449     //  1/2.3"
#define XSDK_SS_1_2P5                       -396850     //  1/2.5"
#define XSDK_SS_1_2P6                       -378929     //  1/2.6"
#define XSDK_SS_1_2P8                       -353553     //  1/2.8"
#define XSDK_SS_1_3                         -329876     //  1/3"
#define XSDK_SS_1_3P1                       -314980     //  1/3.1"
#define XSDK_SS_1_3P2                       -280615     //  1/3.2"
#define XSDK_SS_1_3P5                       -267943     //  1/3.5"
#define XSDK_SS_1_4                         -250000     //  1/4"
#define XSDK_SS_1_4P3                       -233258     //  1/4.3"
#define XSDK_SS_1_4P5                       -222724     //  1/4.5"
#define XSDK_SS_1_5                         -198425     //  1/5"
#define XSDK_SS_1_5P3                       -189464     //  1/5.3"
#define XSDK_SS_1_6                         -176776     //  1/6"
#define XSDK_SS_1_6P5                       -157490     //  1/6.5"
#define XSDK_SS_1_7                         -140307     //  1/7"
#define XSDK_SS_1_7P5                       -133971     //  1/7.5"
#define XSDK_SS_1_8                         -125000     //  1/8"
#define XSDK_SS_1_8P5                       -116629     //  1/8.5"
#define XSDK_SS_1_9                         -111362     //  1/9"
#define XSDK_SS_1_10                        -99212      //  1/10"
#define XSDK_SS_1_12                        -82469      //  1/12"
#define XSDK_SS_1_13                        -78745      //  1/13"
#define XSDK_SS_1_14                        -70153      //  1/14"
#define XSDK_SS_1_15                        -66985      //  1/15"
#define XSDK_SS_1_17                        -58314      //  1/17"
#define XSDK_SS_1_18                        -55681      //  1/18"
#define XSDK_SS_1_20                        -49606      //  1/20"
#define XSDK_SS_1_25                        -41234      //  1/25"
#define XSDK_SS_1_26                        -39372      //  1/26"
#define XSDK_SS_1_27                        -35076      //  1/27"
#define XSDK_SS_1_28                        -33492      //  1/28"
#define XSDK_SS_1_30                        -31250      //  1/30"
#define XSDK_SS_1_34                        -29157      //  1/34"
#define XSDK_SS_1_38                        -27840      //  1/38"
#define XSDK_SS_1_40                        -24803      //  1/40"
#define XSDK_SS_1_42                        -23683      //  1/42"
#define XSDK_SS_1_45                        -22097      //  1/45"
#define XSDK_SS_1_50                        -20617      //  1/50"
#define XSDK_SS_1_52                        -19686      //  1/52"
#define XSDK_SS_1_56                        -17538      //  1/56"
#define XSDK_SS_1_58                        -16746      //  1/58"
#define XSDK_SS_1_60                        -15625      //  1/60"
#define XSDK_SS_1_70                        -14578      //  1/70"
#define XSDK_SS_1_75                        -13920      //  1/75"
#define XSDK_SS_1_80                        -12401      //  1/80"
#define XSDK_SS_1_85                        -11841      //  1/85"
#define XSDK_SS_1_90                        -11048      //  1/90"
#define XSDK_SS_1_100                       -10308      //  1/100"
#define XSDK_SS_1_105                       -9843       //  1/105"
#define XSDK_SS_1_110                       -8769       //  1/110"
#define XSDK_SS_1_120                       -8373       //  1/120"
#define XSDK_SS_1_125                       -7812       //  1/125"
#define XSDK_SS_1_140                       -7289       //  1/140"
#define XSDK_SS_1_150                       -6960       //  1/150"
#define XSDK_SS_1_160                       -6200       //  1/160"
#define XSDK_SS_1_170                       -5920       //  1/170"
#define XSDK_SS_1_180                       -5524       //  1/180"
#define XSDK_SS_1_200                       -5154       //  1/200"
#define XSDK_SS_1_210                       -4921       //  1/210"
#define XSDK_SS_1_220                       -4384       //  1/220"
#define XSDK_SS_1_240                       -4186       //  1/240"
#define XSDK_SS_1_250                       -3906       //  1/250"
#define XSDK_SS_1_280                       -3644       //  1/280"
#define XSDK_SS_1_300                       -3480       //  1/300"
#define XSDK_SS_1_320                       -3100       //  1/320"
#define XSDK_SS_1_340                       -2960       //  1/340"
#define XSDK_SS_1_350                       -2762       //  1/350"
#define XSDK_SS_1_400                       -2577       //  1/400"
#define XSDK_SS_1_420                       -2460       //  1/420"
#define XSDK_SS_1_450                       -2192       //  1/450"
#define XSDK_SS_1_480                       -2093       //  1/480"
#define XSDK_SS_1_500                       -1953       //  1/500"
#define XSDK_SS_1_550                       -1822       //  1/550"
#define XSDK_SS_1_600                       -1740       //  1/600"
#define XSDK_SS_1_640                       -1550       //  1/640"
#define XSDK_SS_1_680                       -1480       //  1/680"
#define XSDK_SS_1_750                       -1381       //  1/750"
#define XSDK_SS_1_800                       -1288       //  1/800"
#define XSDK_SS_1_850                       -1230       //  1/850"
#define XSDK_SS_1_900                       -1096       //  1/900"
#define XSDK_SS_1_950                       -1046       //  1/950"
#define XSDK_SS_1_1000                      -976        //  1/1000"
#define XSDK_SS_1_1100                      -911        //  1/1100"
#define XSDK_SS_1_1250                      -870        //  1/1250"
#define XSDK_SS_1_1300                      -775        //  1/1300"
#define XSDK_SS_1_1400                      -740        //  1/1400"
#define XSDK_SS_1_1500                      -690        //  1/1500"
#define XSDK_SS_1_1600                      -644        //  1/1600"
#define XSDK_SS_1_1700                      -615        //  1/1700"
#define XSDK_SS_1_1800                      -548        //  1/1800"
#define XSDK_SS_1_1900                      -523        //  1/1900"
#define XSDK_SS_1_2000                      -488        //  1/2000"
#define XSDK_SS_1_2200                      -455        //  1/2200"
#define XSDK_SS_1_2400                      -435        //  1/2400"
#define XSDK_SS_1_2500                      -387        //  1/2500"
#define XSDK_SS_1_2700                      -370        //  1/2700"
#define XSDK_SS_1_2900                      -345        //  1/2900"
#define XSDK_SS_1_3000                      -322        //  1/3000"
#define XSDK_SS_1_3200                      -307        //  1/3200"
#define XSDK_SS_1_3500                      -274        //  1/3500"
#define XSDK_SS_1_3800                      -261        //  1/3800"
#define XSDK_SS_1_4000                      -244        //  1/4000"
#define XSDK_SS_1_4400                      -227        //  1/4400"
#define XSDK_SS_1_4700                      -217        //  1/4700"
#define XSDK_SS_1_5000                      -193        //  1/5000"
#define XSDK_SS_1_5400                      -185        //  1/5400"
#define XSDK_SS_1_5800                      -172        //  1/5800"
#define XSDK_SS_1_6000                      -161        //  1/6000"
#define XSDK_SS_1_6400                      -153        //  1/6400"
#define XSDK_SS_1_7000                      -137        //  1/7000"
#define XSDK_SS_1_7500                      -130        //  1/7500"
#define XSDK_SS_1_8000                      -122        //  1/8000"
#define XSDK_SS_1_8500                      -113        //  1/8500"
#define XSDK_SS_1_9000                      -108        //  1/9000"
#define XSDK_SS_1_10000                     -96         //  1/10000"
#define XSDK_SS_1_10500                     -92         //  1/10500"
#define XSDK_SS_1_11000                     -86         //  1/11000"
#define XSDK_SS_1_12000                     -80         //  1/12000"
#define XSDK_SS_1_13000                     -76         //  1/13000"
#define XSDK_SS_1_14000                     -68         //  1/14000"
#define XSDK_SS_1_15000                     -65         //  1/15000"
#define XSDK_SS_1_16000                     -61         //  1/16000"
#define XSDK_SS_1_17000                     -56         //  1/17000"
#define XSDK_SS_1_18000                     -54         //  1/18000"
#define XSDK_SS_1_20000                     -48         //  1/20000"
#define XSDK_SS_1_22000                     -46         //  1/22000"
#define XSDK_SS_1_23000                     -43         //  1/23000"
#define XSDK_SS_1_25000                     -40         //  1/25000"
#define XSDK_SS_1_26000                     -38         //  1/26000"
#define XSDK_SS_1_28000                     -34         //  1/28000"
#define XSDK_SS_1_30000                     -32         //  1/30000"
#define XSDK_SS_1_32000                     -30         //  1/32000"
#define XSDK_SS_1_35000                     -28         //  1/35000"
#define XSDK_SS_1_38000                     -27         //  1/38000"
#define XSDK_SS_1_40000                     -24         //  1/40000"
#define XSDK_SS_1_43000                     -23         //  1/43000"
#define XSDK_SS_1_46000                     -21         //  1/46000"
#define XSDK_SS_1_50000                     -20         //  1/50000"
#define XSDK_SS_1_53000                     -19         //  1/53000"
#define XSDK_SS_1_57000                     -17         //  1/57000"
#define XSDK_SS_1_60000                     -16         //  1/60000"
#define XSDK_SS_1_64000                     -15         //  1/64000"

// ExposureBias
#define XSDK_EXPOSURE_BIAS_P5P00            150         // +5EV
#define XSDK_EXPOSURE_BIAS_P4P67            140         // +4 2/3EV
#define XSDK_EXPOSURE_BIAS_P4P33            130         // +4 1/3EV
#define XSDK_EXPOSURE_BIAS_P4P00            120         // +4EV
#define XSDK_EXPOSURE_BIAS_P3P67            110         // +3 2/3EV
#define XSDK_EXPOSURE_BIAS_P3P33            100         // +3 1/3EV
#define XSDK_EXPOSURE_BIAS_P3P00             90         // +3EV
#define XSDK_EXPOSURE_BIAS_P2P67             80         // +2 2/3EV
#define XSDK_EXPOSURE_BIAS_P2P33             70         // +2 1/3EV
#define XSDK_EXPOSURE_BIAS_P2P00             60         // +2EV
#define XSDK_EXPOSURE_BIAS_P1P67             50         // +1 2/3EV
#define XSDK_EXPOSURE_BIAS_P1P33             40         // +1 1/3EV
#define XSDK_EXPOSURE_BIAS_P1P00             30         // +1EV
#define XSDK_EXPOSURE_BIAS_P0P67             20         // +2/3EV
#define XSDK_EXPOSURE_BIAS_P0P33             10         // +1/3EV
#define XSDK_EXPOSURE_BIAS_0                  0         // 亇0EV
#define XSDK_EXPOSURE_BIAS_M0P33            -10         // -1/3EV
#define XSDK_EXPOSURE_BIAS_M0P67            -20         // -2/3EV
#define XSDK_EXPOSURE_BIAS_M1P00            -30         // -1EV
#define XSDK_EXPOSURE_BIAS_M1P33            -40         // -1 1/3EV
#define XSDK_EXPOSURE_BIAS_M1P67            -50         // -1 2/3EV
#define XSDK_EXPOSURE_BIAS_M2P00            -60         // -2EV
#define XSDK_EXPOSURE_BIAS_M2P33            -70         // -2 1/3EV
#define XSDK_EXPOSURE_BIAS_M2P67            -80         // -2 2/3EV
#define XSDK_EXPOSURE_BIAS_M3P00            -90         // -3EV
#define XSDK_EXPOSURE_BIAS_M3P33           -100         // -3 1/3EV
#define XSDK_EXPOSURE_BIAS_M3P67           -110         // -3 2/3EV
#define XSDK_EXPOSURE_BIAS_M4P00           -120         // -4EV
#define XSDK_EXPOSURE_BIAS_M4P33           -130         // -4 1/3EV
#define XSDK_EXPOSURE_BIAS_M4P67           -140         // -4 2/3EV
#define XSDK_EXPOSURE_BIAS_M5P00           -150         // -5EV

// Sensitivity
#define XSDK_SENSITIVITY_ISO50              50          // ISO 50
#define XSDK_SENSITIVITY_ISO60              60          // ISO 60
#define XSDK_SENSITIVITY_ISO80              80          // ISO 80
#define XSDK_SENSITIVITY_ISO100             100         // ISO 100
#define XSDK_SENSITIVITY_ISO125             125         // ISO 125
#define XSDK_SENSITIVITY_ISO160             160         // ISO 160
#define XSDK_SENSITIVITY_ISO200             200         // ISO 200
#define XSDK_SENSITIVITY_ISO250             250         // ISO 250
#define XSDK_SENSITIVITY_ISO320             320         // ISO 320
#define XSDK_SENSITIVITY_ISO400             400         // ISO 400
#define XSDK_SENSITIVITY_ISO500             500         // ISO 500
#define XSDK_SENSITIVITY_ISO640             640         // ISO 640
#define XSDK_SENSITIVITY_ISO800             800         // ISO 800
#define XSDK_SENSITIVITY_ISO1000            1000        // ISO 1000
#define XSDK_SENSITIVITY_ISO1250            1250        // ISO 1250
#define XSDK_SENSITIVITY_ISO1600            1600        // ISO 1600
#define XSDK_SENSITIVITY_ISO2000            2000        // ISO 2000
#define XSDK_SENSITIVITY_ISO2500            2500        // ISO 2500
#define XSDK_SENSITIVITY_ISO3200            3200        // ISO 3200
#define XSDK_SENSITIVITY_ISO4000            4000        // ISO 4000
#define XSDK_SENSITIVITY_ISO5000            5000        // ISO 5000
#define XSDK_SENSITIVITY_ISO6400            6400        // ISO 6400
#define XSDK_SENSITIVITY_ISO8000            8000        // ISO 8000
#define XSDK_SENSITIVITY_ISO10000           10000       // ISO 10000
#define XSDK_SENSITIVITY_ISO12800           12800       // ISO 12800
#define XSDK_SENSITIVITY_ISO16000           16000       // ISO 16000
#define XSDK_SENSITIVITY_ISO20000           20000       // ISO 20000
#define XSDK_SENSITIVITY_ISO25600           25600       // ISO 25600
#define XSDK_SENSITIVITY_ISO32000           32000       // ISO 32000
#define XSDK_SENSITIVITY_ISO40000           40000       // ISO 40000
#define XSDK_SENSITIVITY_ISO51200           51200       // ISO 51200
#define XSDK_SENSITIVITY_ISO64000           64000       // ISO 64000
#define XSDK_SENSITIVITY_ISO80000           80000       // ISO 80000
#define XSDK_SENSITIVITY_ISO102400          102400      // ISO 102400
#define XSDK_SENSITIVITY_AUTO_1             -1          // ISO AUTO (1)
#define XSDK_SENSITIVITY_AUTO_2             -2          // ISO AUTO (2)
#define XSDK_SENSITIVITY_AUTO_3             -3          // ISO AUTO (3)
#define XSDK_SENSITIVITY_AUTO_4             -4          // ISO AUTO (4)
#define XSDK_SENSITIVITY_AUTO400            -400        // ISO AUTO 400
#define XSDK_SENSITIVITY_AUTO800            -800        // ISO AUTO 800
#define XSDK_SENSITIVITY_AUTO1600           -1600       // ISO AUTO 1600
#define XSDK_SENSITIVITY_AUTO3200           -3200       // ISO AUTO 3200
#define XSDK_SENSITIVITY_AUTO6400           -6400       // ISO AUTO 6400

// D Range
#define XSDK_DRANGE_AUTO                    0xFFFF      //  AUTO
#define XSDK_DRANGE_100                     100         //  100%
#define XSDK_DRANGE_200                     200         //  200%
#define XSDK_DRANGE_400                     400         //  400%
#define XSDK_DRANGE_800                     800         //  800%

// Exposure bias
#define XSDK_AEBIAS_MIN                     -150        //  Min. value  ( -5.0EV)
#define XSDK_AEBIAS_MAX                      150        //  Max. value  ( +5.0EV)

// Metering Mode
#define XSDK_METERING_AVERAGE               0x0001
#define XSDK_METERING_MULTI                 0x0003
#define XSDK_METERING_CENTER                0x0004
#define XSDK_METERING_CENTER_WEIGHTED       0x0002

// Force Mode
#define XSDK_FORCESHOOTSTANDBY_SHOOT        0x0001      // Shoot Mode
#define XSDK_FORCESHOOTSTANDBY_PLAYBACK     0x0002      // Playback Mode

// DRIVE MODE
#define XSDK_DRIVE_MODE_INVALID             0xFFFF
#define XSDK_DRIVE_MODE_CH                  0x0002
#define XSDK_DRIVE_MODE_CL                  0x0003
#define XSDK_DRIVE_MODE_S                   0x0004      // S
#define XSDK_DRIVE_MODE_MULTI_EXPOSURE      0x0005
#define XSDK_DRIVE_MODE_ADVFILTER           0x0006
#define XSDK_DRIVE_MODE_PANORAMA            0x0007
#define XSDK_DRIVE_MODE_MOVIE               0x0008      // MOVIE
#define XSDK_DRIVE_MODE_HDR                 0x0009
#define XSDK_DRIVE_MODE_BKT_AE              0x000A
#define XSDK_DRIVE_MODE_BKT_ISO             0x000B
#define XSDK_DRIVE_MODE_BKT_FILMSIMULATION  0x000C
#define XSDK_DRIVE_MODE_BKT_WHITEBALANCE    0x000D
#define XSDK_DRIVE_MODE_BKT_DYNAMICRANGE    0x000E
#define XSDK_DRIVE_MODE_BKT_FOCUS           0x000F
#define XSDK_DRIVE_MODE_PIXELSHIFTMULTISHOT 0x0010

// MODE
#define XSDK_MODE_STILL_C0                  0x0001
#define XSDK_MODE_STILL_C1                  0x0002
#define XSDK_MODE_STILL_C2                  0x0003
#define XSDK_MODE_STILL_C3                  0x0004
#define XSDK_MODE_STILL_C4                  0x0005
#define XSDK_MODE_STILL_C5                  0x0006
#define XSDK_MODE_STILL_C6                  0x0007
#define XSDK_MODE_STILL_C7                  0x0008

#define XSDK_MODE_STILL_ADVFILTER           0x0081
#define XSDK_MODE_STILL_SP                  0x00B1
#define XSDK_MODE_STILL_AUTO                0x00F0

#define XSDK_MODE_MOVIE_C0                  0x0101
#define XSDK_MODE_MOVIE_C1                  0x0102
#define XSDK_MODE_MOVIE_C2                  0x0103
#define XSDK_MODE_MOVIE_C3                  0x0104
#define XSDK_MODE_MOVIE_C4                  0x0105
#define XSDK_MODE_MOVIE_C5                  0x0106
#define XSDK_MODE_MOVIE_C6                  0x0107
#define XSDK_MODE_MOVIE_C7                  0x0108

// White balance
#define XSDK_WB_AUTO                        0x0002
#define	XSDK_WB_AUTO_WHITE_PRIORITY			0x8020
#define	XSDK_WB_AUTO_AMBIENCE_PRIORITY		0x8021
#define XSDK_WB_DAYLIGHT                    0x0004
#define XSDK_WB_INCANDESCENT                0x0006
#define XSDK_WB_UNDER_WATER                 0x0008
#define XSDK_WB_FLUORESCENT1                0x8001
#define XSDK_WB_FLUORESCENT2                0x8002
#define XSDK_WB_FLUORESCENT3                0x8003
#define XSDK_WB_SHADE                       0x8006
#define XSDK_WB_COLORTEMP                   0x8007
#define XSDK_WB_CUSTOM1                     0x8008
#define XSDK_WB_CUSTOM2                     0x8009
#define XSDK_WB_CUSTOM3                     0x800A
#define XSDK_WB_CUSTOM4                     0x800B
#define XSDK_WB_CUSTOM5                     0x800C

// White balance Color Temptune
#define XSDK_WB_COLORTEMP_2500              2500
#define XSDK_WB_COLORTEMP_2550              2550
#define XSDK_WB_COLORTEMP_2650              2650
#define XSDK_WB_COLORTEMP_2700              2700
#define XSDK_WB_COLORTEMP_2800              2800
#define XSDK_WB_COLORTEMP_2850              2850
#define XSDK_WB_COLORTEMP_2950              2950
#define XSDK_WB_COLORTEMP_3000              3000
#define XSDK_WB_COLORTEMP_3100              3100
#define XSDK_WB_COLORTEMP_3200              3200
#define XSDK_WB_COLORTEMP_3300              3300
#define XSDK_WB_COLORTEMP_3400              3400
#define XSDK_WB_COLORTEMP_3600              3600
#define XSDK_WB_COLORTEMP_3700              3700
#define XSDK_WB_COLORTEMP_3800              3800
#define XSDK_WB_COLORTEMP_4000              4000
#define XSDK_WB_COLORTEMP_4200              4200
#define XSDK_WB_COLORTEMP_4300              4300
#define XSDK_WB_COLORTEMP_4500              4500
#define XSDK_WB_COLORTEMP_4800              4800
#define XSDK_WB_COLORTEMP_5000              5000
#define XSDK_WB_COLORTEMP_5300              5300
#define XSDK_WB_COLORTEMP_5600              5600
#define XSDK_WB_COLORTEMP_5900              5900
#define XSDK_WB_COLORTEMP_6300              6300
#define XSDK_WB_COLORTEMP_6700              6700
#define XSDK_WB_COLORTEMP_7100              7100
#define XSDK_WB_COLORTEMP_7700              7700
#define XSDK_WB_COLORTEMP_8300              8300
#define XSDK_WB_COLORTEMP_9100              9100
#define XSDK_WB_COLORTEMP_10000             10000
#define XSDK_WB_COLORTEMP_CURRENT           0

// Media Record
#define XSDK_MEDIAREC_RAWJPEG               0x0001
#define XSDK_MEDIAREC_RAW                   0x0002
#define XSDK_MEDIAREC_JPEG                  0x0003
#define XSDK_MEDIAREC_OFF                   0x0004

// Capability
#define XSDK_YES                            0x0001
#define XSDK_NO                             0x0000

#define XSDK_ON                             0x0001
#define XSDK_OFF                            0x0002

// Movie Sensitivity
#define XSDK_SENSITIVITY_MOVIE_AUTO         -10


//
// Error code (XSDK_GetErrorNumber)
//
enum {
    XSDK_ERRCODE_NOERR                = 0x00000000,
    XSDK_ERRCODE_SEQUENCE             = 0x00001001,
    XSDK_ERRCODE_PARAM                = 0x00001002,
    XSDK_ERRCODE_INVALID_CAMERA       = 0x00001003,
    XSDK_ERRCODE_LOADLIB              = 0x00001004,
    XSDK_ERRCODE_UNSUPPORTED          = 0x00001005,
    XSDK_ERRCODE_BUSY                 = 0x00001006,
    XSDK_ERRCODE_AF_TIMEOUT           = 0x00001007,
    XSDK_ERRCODE_SHOOT_ERROR          = 0x00001008,
    XSDK_ERRCODE_FRAME_FULL           = 0x00001009,
    XSDK_ERRCODE_STANDBY              = 0x00001010,
    XSDK_ERRCODE_NODRIVER             = 0x00001011,
    XSDK_ERRCODE_NO_MODEL_MODULE      = 0x00001012,
    XSDK_ERRCODE_API_NOTFOUND         = 0x00001013,
    XSDK_ERRCODE_API_MISMATCH         = 0x00001014,
    XSDK_ERRCODE_INVALID_USBMODE      = 0x00001015,
	XSDK_ERRCODE_FORCEMODE_BUSY       = 0x00001016,
    XSDK_ERRCODE_COMMUNICATION        = 0x00002001,
    XSDK_ERRCODE_TIMEOUT              = 0x00002002,
    XSDK_ERRCODE_COMBINATION          = 0x00002003,
    XSDK_ERRCODE_WRITEERROR           = 0x00002004,
    XSDK_ERRCODE_CARDFULL             = 0x00002005,
    XSDK_ERRCODE_HARDWARE             = 0x00003001,
    XSDK_ERRCODE_INTERNAL             = 0x00009001,
    XSDK_ERRCODE_MEMFULL              = 0x00009002,
    XSDK_ERRCODE_UNKNOWN              = 0x00009100
};

//  SDK API RETURN VALUE
enum{
    XSDK_COMPLETE   =   0,
    XSDK_ERROR      = -1
};


#ifndef _WIN32
#pragma export on
#endif

//
// Export API defines
//
#ifdef  __cplusplus
extern "C" {
#endif  // _cplusplus

// Initialize/Finalize
XSDK_APIENTRY XSDK_Init( LIB_HANDLE hLib );
XSDK_APIENTRY XSDK_Exit( void );

// Enumeration
XSDK_APIENTRY XSDK_Detect( long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount );

// Session management
XSDK_APIENTRY XSDK_Close( XSDK_HANDLE hCamera );
XSDK_APIENTRY XSDK_PowerOFF( XSDK_HANDLE hCamera );
XSDK_APIENTRY XSDK_OpenEx( LPSTR pDevice, XSDK_HANDLE* phCamera, long* plCameraMode, void* pOption );

// Basic functions
XSDK_APIENTRY XSDK_GetErrorNumber( XSDK_HANDLE hCamera, long* plAPICode, long* plERRCode );
XSDK_APIENTRY XSDK_GetVersionString( LPSTR pVersionString );

// Device Information
XSDK_APIENTRY XSDK_GetDeviceInfo( XSDK_HANDLE hCamera, XSDK_DeviceInformation* pDevInfo );
XSDK_APIENTRY XSDK_WriteDeviceName( XSDK_HANDLE hCamera, LPCSTR pDeviceName );
XSDK_APIENTRY XSDK_GetFirmwareVersion( XSDK_HANDLE hCamera, LPSTR pFirmwareVersion );
XSDK_APIENTRY XSDK_GetLensInfo( XSDK_HANDLE hCamera, XSDK_LensInformation* pLensInfo );
XSDK_APIENTRY XSDK_GetLensVersion( XSDK_HANDLE hCamera, LPSTR pLensVersion );

// Camera Operation Mode
XSDK_APIENTRY XSDK_CapPriorityMode( XSDK_HANDLE hCamera, long* plNumPriorityMode, long* plPriorityMode );
XSDK_APIENTRY XSDK_SetPriorityMode( XSDK_HANDLE hCamera, long lPriorityMode );
XSDK_APIENTRY XSDK_GetPriorityMode( XSDK_HANDLE hCamera, long* plPriorityMode );

// Release Control
XSDK_APIENTRY XSDK_CapRelease( XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode );
XSDK_APIENTRY XSDK_Release( XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus );
XSDK_APIENTRY XSDK_GetReleaseStatus( XSDK_HANDLE hCamera, long* plReleaseStatus );
XSDK_APIENTRY XSDK_GetRecordingStatus( XSDK_HANDLE hCamera, long* plStatus );

// Image acquisition
XSDK_APIENTRY XSDK_GetBufferCapacity( XSDK_HANDLE hCamera, long* plShootFrameNum, long* plTotalFrameNum );
XSDK_APIENTRY XSDK_ReadImageInfo( XSDK_HANDLE hCamera, XSDK_ImageInformation* pImgInfo );
XSDK_APIENTRY XSDK_ReadPreview(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
XSDK_APIENTRY XSDK_ReadImage(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
XSDK_APIENTRY XSDK_DeleteImage( XSDK_HANDLE hCamera );

// Exposure control
XSDK_APIENTRY XSDK_CapAEMode( XSDK_HANDLE hCamera, long* plNumAEMode, long* plAEMode );
XSDK_APIENTRY XSDK_SetAEMode( XSDK_HANDLE hCamera, long lAEMode );
XSDK_APIENTRY XSDK_GetAEMode( XSDK_HANDLE hCamera, long* plAEMode );

XSDK_APIENTRY XSDK_CapShutterSpeed( XSDK_HANDLE hCamera, long* plNumShutterSpeed, long* plShutterSpeed, long* plBulbCapable );
XSDK_APIENTRY XSDK_SetShutterSpeed( XSDK_HANDLE hCamera, long lShutterSpeed, long lBulb );
XSDK_APIENTRY XSDK_GetShutterSpeed( XSDK_HANDLE hCamera, long* plShutterSpeed, long* plBulb );

XSDK_APIENTRY XSDK_CapExposureBias( XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias );
XSDK_APIENTRY XSDK_SetExposureBias( XSDK_HANDLE hCamera, long lExposureBias );
XSDK_APIENTRY XSDK_GetExposureBias( XSDK_HANDLE hCamera, long* plExposureBias );

XSDK_APIENTRY XSDK_CapSensitivity( XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity );
XSDK_APIENTRY XSDK_SetSensitivity( XSDK_HANDLE hCamera, long lSensitivity );
XSDK_APIENTRY XSDK_GetSensitivity( XSDK_HANDLE hCamera, long* plSensitivity );

XSDK_APIENTRY XSDK_CapDynamicRange( XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange );
XSDK_APIENTRY XSDK_SetDynamicRange( XSDK_HANDLE hCamera, long lDynamicRange );
XSDK_APIENTRY XSDK_GetDynamicRange( XSDK_HANDLE hCamera, long* plDynamicRange );

XSDK_APIENTRY XSDK_CapMeteringMode( XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode );
XSDK_APIENTRY XSDK_SetMeteringMode( XSDK_HANDLE hCamera, long lMeteringMode );
XSDK_APIENTRY XSDK_GetMeteringMode( XSDK_HANDLE hCamera, long* plMeteringMode);

XSDK_APIENTRY XSDK_CapLensZoomPos( XSDK_HANDLE hCamera, long* plNumZoomPos, long* plZoomPos, long* plFocusLength, long* pl35mmFocusLength );
XSDK_APIENTRY XSDK_SetLensZoomPos( XSDK_HANDLE hCamera, long lZoomPos );
XSDK_APIENTRY XSDK_GetLensZoomPos( XSDK_HANDLE hCamera, long* plZoomPos );

XSDK_APIENTRY XSDK_CapAperture( XSDK_HANDLE hCamera, long lZoomPos, long* plNumAperture, long* plFNumber );
XSDK_APIENTRY XSDK_SetAperture( XSDK_HANDLE hCamera, long lFNumber );
XSDK_APIENTRY XSDK_GetAperture( XSDK_HANDLE hCamera, long* plFNumber);

XSDK_APIENTRY XSDK_CapWBMode( XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode );
XSDK_APIENTRY XSDK_SetWBMode( XSDK_HANDLE hCamera, long lWBMode );
XSDK_APIENTRY XSDK_GetWBMode( XSDK_HANDLE hCamera, long* plWBMode );

XSDK_APIENTRY XSDK_CapWBColorTemp( XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp );
XSDK_APIENTRY XSDK_SetWBColorTemp( XSDK_HANDLE hCamera, long lColorTemp );
XSDK_APIENTRY XSDK_GetWBColorTemp( XSDK_HANDLE hCamera, long* plColorTemp);

XSDK_APIENTRY XSDK_CapMovieShutterSpeed(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long long* pllShutterSpeed);
XSDK_APIENTRY XSDK_SetMovieShutterSpeed(XSDK_HANDLE hCamera, long long llShutterSpeed);
XSDK_APIENTRY XSDK_GetMovieShutterSpeed(XSDK_HANDLE hCamera, long long* pllShutterSpeed);
XSDK_APIENTRY XSDK_CapMovieExposureBias(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
XSDK_APIENTRY XSDK_SetMovieExposureBias(XSDK_HANDLE hCamera, long lExposureBias);
XSDK_APIENTRY XSDK_GetMovieExposureBias(XSDK_HANDLE hCamera, long* plExposureBias);
XSDK_APIENTRY XSDK_CapMovieSensitivity(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
XSDK_APIENTRY XSDK_SetMovieSensitivity(XSDK_HANDLE hCamera, long lSensitivity);
XSDK_APIENTRY XSDK_GetMovieSensitivity(XSDK_HANDLE hCamera, long* plSensitivity);
XSDK_APIENTRY XSDK_CapMovieAperture(XSDK_HANDLE hCamera, long* plNumAperture, long* plFNumber);
XSDK_APIENTRY XSDK_SetMovieAperture(XSDK_HANDLE hCamera, long lFNumber);
XSDK_APIENTRY XSDK_GetMovieAperture(XSDK_HANDLE hCamera, long* plFNumber);

XSDK_APIENTRY XSDK_CapMediaRecord( XSDK_HANDLE hCamera, long* plNumMediaRecord, long* plMediaRecord );
XSDK_APIENTRY XSDK_SetMediaRecord( XSDK_HANDLE hCamera, long lMediaRecord );
XSDK_APIENTRY XSDK_GetMediaRecord( XSDK_HANDLE hCamera, long* plMediaRecord );

// Force Mode
XSDK_APIENTRY XSDK_CapForceMode( XSDK_HANDLE hCamera, long* plNumForceMode, long* plForceMode );
XSDK_APIENTRY XSDK_SetForceMode( XSDK_HANDLE hCamera, long lForceMode );

// Drive Mode
XSDK_APIENTRY XSDK_CapDriveMode( XSDK_HANDLE hCamera, long* plNum, long* pFunctionMode );
XSDK_APIENTRY XSDK_SetDriveMode( XSDK_HANDLE hCamera, long functionMode );
XSDK_APIENTRY XSDK_GetDriveMode( XSDK_HANDLE hCamera, long* pFunctionMode );

// Mode
XSDK_APIENTRY XSDK_CapMode( XSDK_HANDLE hCamera, long* plNum, long* plMode );
XSDK_APIENTRY XSDK_SetMode( XSDK_HANDLE hCamera, long lMode );
XSDK_APIENTRY XSDK_GetMode( XSDK_HANDLE hCamera, long* plMode );

// BackupSettings
XSDK_APIENTRY XSDK_SetBackupSettings( XSDK_HANDLE hCamera, long lSize, unsigned char* pBackup);
XSDK_APIENTRY XSDK_GetBackupSettings( XSDK_HANDLE hCamera, long* plSize, unsigned char* pBackup );

// Model Dependent SDK API Call I/F
XSDK_APIENTRY XSDK_CapProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
XSDK_APIENTRY XSDK_SetProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
XSDK_APIENTRY XSDK_GetProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );

#ifdef  __cplusplus
}
#endif  // _cplusplus

#ifndef _WIN32
#pragma export off
#endif

//
// Import function types define
//
// Initialize/Finalize
typedef XSDK_APIENTRY  (*_XSDK_Init)(LIB_HANDLE hLib);
typedef XSDK_APIENTRY  (*_XSDK_Exit)(void);

// Enumeration
typedef XSDK_APIENTRY  (*_XSDK_Detect)(long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount);

// Session management
typedef XSDK_APIENTRY  (*_XSDK_Close)(XSDK_HANDLE hCamera);
typedef XSDK_APIENTRY  (*_XSDK_PowerOFF)(XSDK_HANDLE hCamera);
typedef XSDK_APIENTRY  (*_XSDK_OpenEx)( LPSTR pDevice, XSDK_HANDLE* phCamera, long* plCameraMode, void* pOption );

// Basic functions
typedef XSDK_APIENTRY  (*_XSDK_GetErrorNumber)(XSDK_HANDLE hCamera, long* plAPICode, long* plERRCode);
typedef XSDK_APIENTRY  (*_XSDK_GetVersionString)(LPSTR pVersionString);

// Device Information
typedef XSDK_APIENTRY  (*_XSDK_GetDeviceInfo)(XSDK_HANDLE hCamera,  XSDK_DeviceInformation* pDevInfo);
typedef XSDK_APIENTRY  (*_XSDK_WriteDeviceName)(XSDK_HANDLE hCamera, LPCSTR pDeviceName);
typedef XSDK_APIENTRY  (*_XSDK_GetFirmwareVersion)(XSDK_HANDLE  hCamera, LPSTR pFirmwareVersion);
typedef XSDK_APIENTRY  (*_XSDK_GetLensInfo)(XSDK_HANDLE hCamera, XSDK_LensInformation* pLensInfo);
typedef XSDK_APIENTRY  (*_XSDK_GetLensVersion)(XSDK_HANDLE hCamera, LPSTR pLensVersion);

// Camera Operation Mode
typedef XSDK_APIENTRY  (*_XSDK_CapPriorityMode)(XSDK_HANDLE hCamera, long* plNumPriorityMode, long* plPriorityMode);
typedef XSDK_APIENTRY  (*_XSDK_SetPriorityMode)(XSDK_HANDLE hCamera, long lPriorityMode);
typedef XSDK_APIENTRY  (*_XSDK_GetPriorityMode)(XSDK_HANDLE hCamera, long*  plPriorityMode);

// Release Control
typedef XSDK_APIENTRY  (*_XSDK_CapRelease)(XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode);
typedef XSDK_APIENTRY  (*_XSDK_Release)(XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus);
typedef XSDK_APIENTRY  (*_XSDK_GetReleaseStatus)(XSDK_HANDLE hCamera, long* plReleaseStatus);
typedef XSDK_APIENTRY  (*_XSDK_GetRecordingStatus)(XSDK_HANDLE hCamera, long* plStatus);

// Image acquisition
typedef XSDK_APIENTRY  (*_XSDK_GetBufferCapacity)(XSDK_HANDLE hCamera, long* plShootFrameNum, long* plTotalFrameNum);
typedef XSDK_APIENTRY  (*_XSDK_ReadImageInfo)(XSDK_HANDLE hCamera,  XSDK_ImageInformation* pImgInfo);
typedef XSDK_APIENTRY  (*_XSDK_ReadPreview)(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
typedef XSDK_APIENTRY  (*_XSDK_ReadImage)(XSDK_HANDLE hCamera,  unsigned char* pData, unsigned long lDataSize);
typedef XSDK_APIENTRY  (*_XSDK_DeleteImage)(XSDK_HANDLE hCamera);

// Exposure control
typedef XSDK_APIENTRY  (*_XSDK_CapAEMode)(XSDK_HANDLE hCamera, long* plNumAEMode, long* plAEMode);
typedef XSDK_APIENTRY  (*_XSDK_SetAEMode)(XSDK_HANDLE hCamera,  long lAEMode);
typedef XSDK_APIENTRY  (*_XSDK_GetAEMode)(XSDK_HANDLE hCamera,  long* plAEMode);
typedef XSDK_APIENTRY  (*_XSDK_CapShutterSpeed)(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long* plShutterSpeed, long* plBulbCapable);
typedef XSDK_APIENTRY  (*_XSDK_SetShutterSpeed)(XSDK_HANDLE hCamera, long lShutterSpeed, long lBulb);
typedef XSDK_APIENTRY  (*_XSDK_GetShutterSpeed)(XSDK_HANDLE hCamera, long* plShutterSpeed, long* plBulb);
typedef XSDK_APIENTRY  (*_XSDK_CapExposureBias)(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_SetExposureBias)(XSDK_HANDLE hCamera, long lExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_GetExposureBias)(XSDK_HANDLE hCamera, long*  plExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_CapSensitivity)(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_SetSensitivity)(XSDK_HANDLE  hCamera, long lSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_GetSensitivity)(XSDK_HANDLE  hCamera, long* plSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_CapDynamicRange)(XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_SetDynamicRange)(XSDK_HANDLE hCamera, long lDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_GetDynamicRange)(XSDK_HANDLE hCamera, long* plDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_CapMeteringMode)(XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode);
typedef XSDK_APIENTRY  (*_XSDK_SetMeteringMode)(XSDK_HANDLE hCamera, long lMeteringMode);
typedef XSDK_APIENTRY  (*_XSDK_GetMeteringMode)(XSDK_HANDLE hCamera, long*  plMeteringMode);

typedef XSDK_APIENTRY  (*_XSDK_CapLensZoomPos)(XSDK_HANDLE hCamera, long* plNumZoomPos, long* plZoomPos, long* plFocusLength, long* pl35mmFocusLength);
typedef XSDK_APIENTRY  (*_XSDK_SetLensZoomPos)(XSDK_HANDLE hCamera, long lZoomPos);
typedef XSDK_APIENTRY  (*_XSDK_GetLensZoomPos)(XSDK_HANDLE hCamera, long* plZoomPos);
typedef XSDK_APIENTRY  (*_XSDK_CapAperture)(XSDK_HANDLE hCamera, long lZoomPos, long* plNumAperture, long* plFNumber);
typedef XSDK_APIENTRY  (*_XSDK_SetAperture)(XSDK_HANDLE hCamera, long lFNumber);
typedef XSDK_APIENTRY  (*_XSDK_GetAperture)(XSDK_HANDLE hCamera, long*  plFNumber);


typedef XSDK_APIENTRY  (*_XSDK_CapWBMode)(XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode);
typedef XSDK_APIENTRY  (*_XSDK_SetWBMode)(XSDK_HANDLE hCamera, long lWBMode);
typedef XSDK_APIENTRY  (*_XSDK_GetWBMode)(XSDK_HANDLE hCamera, long* plWBMode);
typedef XSDK_APIENTRY  (*_XSDK_CapWBColorTemp)(XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp);
typedef XSDK_APIENTRY  (*_XSDK_SetWBColorTemp)(XSDK_HANDLE hCamera, long lColorTemp);
typedef XSDK_APIENTRY  (*_XSDK_GetWBColorTemp)(XSDK_HANDLE hCamera, long* plColorTemp);

typedef XSDK_APIENTRY (*_XSDK_CapMovieShutterSpeed)(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long long* pllShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_SetMovieShutterSpeed)(XSDK_HANDLE hCamera, long long llShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_GetMovieShutterSpeed)(XSDK_HANDLE hCamera, long long* pllShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_CapMovieExposureBias)(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
typedef XSDK_APIENTRY (*_XSDK_SetMovieExposureBias)(XSDK_HANDLE hCamera, long lExposureBias);
typedef XSDK_APIENTRY (*_XSDK_GetMovieExposureBias)(XSDK_HANDLE hCamera, long* plExposureBias);
typedef XSDK_APIENTRY (*_XSDK_CapMovieSensitivity)(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
typedef XSDK_APIENTRY (*_XSDK_SetMovieSensitivity)(XSDK_HANDLE hCamera, long lSensitivity);
typedef XSDK_APIENTRY (*_XSDK_GetMovieSensitivity)(XSDK_HANDLE hCamera, long* plSensitivity);
typedef XSDK_APIENTRY (*_XSDK_CapMovieAperture)(XSDK_HANDLE hCamera, long* plNumAperture, long* plFNumber);
typedef XSDK_APIENTRY (*_XSDK_SetMovieAperture)(XSDK_HANDLE hCamera, long lFNumber);
typedef XSDK_APIENTRY (*_XSDK_GetMovieAperture)(XSDK_HANDLE hCamera, long* plFNumber);

// Media control
typedef XSDK_APIENTRY  (*_XSDK_CapMediaRecord)(XSDK_HANDLE hCamera, long* plNumMediaRecord, long* plMediaRecord);
typedef XSDK_APIENTRY  (*_XSDK_SetMediaRecord)(XSDK_HANDLE hCamera, long lMediaRecord);
typedef XSDK_APIENTRY  (*_XSDK_GetMediaRecord)(XSDK_HANDLE hCamera, long* plMediaRecord);

// Force Mode
typedef XSDK_APIENTRY  (*_XSDK_CapForceMode)(XSDK_HANDLE hCamera, long* plNumForceMode, long* plForceMode);
typedef XSDK_APIENTRY  (*_XSDK_SetForceMode)(XSDK_HANDLE hCamera, long lForceMode);

// Drive Mode
typedef XSDK_APIENTRY  (*_XSDK_CapDriveMode)( XSDK_HANDLE hCamera, long* plNum, long* pFunctionMode );
typedef XSDK_APIENTRY  (*_XSDK_SetDriveMode)( XSDK_HANDLE hCamera, long functionMode );
typedef XSDK_APIENTRY  (*_XSDK_GetDriveMode)( XSDK_HANDLE hCamera, long* pFunctionMode );

// Mode
typedef XSDK_APIENTRY  (*_XSDK_CapMode)( XSDK_HANDLE hCamera, long* plNum, long* plMode );
typedef XSDK_APIENTRY  (*_XSDK_SetMode)( XSDK_HANDLE hCamera, long lMode );
typedef XSDK_APIENTRY  (*_XSDK_GetMode)( XSDK_HANDLE hCamera, long* plMode );

// BackupSettings
typedef XSDK_APIENTRY  (*_XSDK_SetBackupSettings)( XSDK_HANDLE hCamera, long lSize, unsigned char* pBackup);
typedef XSDK_APIENTRY  (*_XSDK_GetBackupSettings)( XSDK_HANDLE hCamera, long* plSize, unsigned char* pBackup );

// Model Dependent SDK API Call I/F
typedef XSDK_APIENTRY  (*_XSDK_CapProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
typedef XSDK_APIENTRY  (*_XSDK_SetProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
typedef XSDK_APIENTRY  (*_XSDK_GetProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );

#endif  // __XAPI_H__