/*
HSV SDK Camera Library

HSVCL.h
*/

#ifndef HSVCL_H_
#define HSVCL_H_

#pragma once

#ifndef CAMERALIBAPI
	#ifdef WIN32
		#define CAMERALIBAPI __declspec(dllimport)
	#else // WIN32
		#define CAMERALIBAPI
	#endif // WIN32
#endif // CAMERALIBAPI

#include "hsvcl_ver.h"
#include "SensingInformation.h"

///////////////////////////////////////////
// OS dependent part
///////////////////////////////////////////

#include <Windows.h>

#define CLHANDLE HANDLE
#define CLHANDLE_NULL ((void*)0)


///////////////////////////////////////////
// Constants
///////////////////////////////////////////

// version 02.00.0000
#define LIBRARY_VERSION ((CL_MAJOR_VERSION << 24) + (CL_MINOR_VERSION << 16) + CL_BUILD_NUMBER)


#define DEVICE_IDENTIFIER_MAX_LENGTH (256)
#define CUNAME_MAX_LENGTH (15)

#define CUAPPLICATION_DEFAULT_ADDRESS_V1	(0x30001000)		// V1
#define CUAPPLICATION_DEFAULT_ADDRESS_K2	(0x7E001000)		// K2
#define CUAPPLICATION_DEFAULT_ADDRESS		CUAPPLICATION_DEFAULT_ADDRESS_K2

#define MAX_IMAGE_SI_FPS (3840)
#define MAX_GAIN_RANGE (65535)

///////////////////////////////////////////
// Structures and Enumerators
///////////////////////////////////////////

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4214)
#endif

typedef struct _ClVersionInfo
{
	unsigned char major;
	unsigned char minor;
	unsigned short revision;
} ClVersionInfo;

// Bus Type
typedef enum _ClPortType
{
	PORT_NOT_SET,
	PORT_USB20			= 0x100,
	PORT_USB20_MSC		= PORT_USB20,
	PORT_USB30			= 0x2A0,
	PORT_USB30_CYPRESS	= PORT_USB30,
	PORT_ETHERNET		= 0x300,

	PORT_NAMEDPIPE		= 0x1000,
	PORT_NAMEDPIPELAST	= 0x1fff,

	PORT_UNKNOWN = 0xffff
} ClPortType;

typedef enum _ClPacketTypeBitmap
{
	PACKETTYPEBITMAP_RESPONSE_FROM_CU		= 0x0001,
	PACKETTYPEBITMAP_RESPONSE_FROM_HSV382	= 0x0002,
	PACKETTYPEBITMAP_RESPONSE_FROM_SENSOR	= 0x0002,
	PACKETTYPEBITMAP_SI						= 0x0004,
	PACKETTYPEBITMAP_IMAGE					= 0x0008,
	PACKETTYPEBITMAP_BULK_SI				= 0x0010,
	PACKETTYPEBITMAP_BULK_IMAGE				= 0x0020,
	PACKETTYPEBITMAP_NOTIFICATION			= 0x0040,
	PACKETTYPEBITMAP_EVENT_LOG				= 0x0080,
	PACKETTYPEBITMAP_RESPONSE_FROM_INTERNAL	= 0x0100,
} ClPacketTypeBitmap;

// Data type on Data Channel
#define CLDATACHANNEL_DATATYPE_SI       (0x00000001)
#define CLDATACHANNEL_DATATYPE_IMAGE    (0x00000002)
#define CLDATACHANNEL_DATATYPE_EVENTLOG (0x00000004)

// Configuration key enumerator
typedef enum _ClCameraConfigKey
{
	CLP_UNKNOWN,
	CLP_IMAGEMODE,
	CLP_IMAGEOUTPUTMODE,
	CLP_ANALOGGAIN,
	CLP_INPUTGAIN_R,
	CLP_INPUTGAIN_G,
	CLP_INPUTGAIN_B,
	CLP_DRAWTRACKINGFRAME,
	CLP_ENABLETRACKING,
	CLP_ALUSTATE,
	CLP_ALUPROGRAMNUMBER,
	CLP_TARGETTRACKINGMODE,
	CLP_NOISEREDUCTION,
	CLP_RESERVED1,
	CLP_EXPOSUREMODE,
	CLP_SHUTTERVALUE,
	CLP_FDGAIN,
	CLP_DIGITALGAIN,
	CLP_WHITEBALANCEMODE,

	CLP_IMAGETRANSFERRATE = 0x100,
	CLP_SITRANSFERRATE,

} ClCameraConfigKey;

typedef struct _ClCameraConfigAll
{
	unsigned long imageMode;
	unsigned long imageOutputMode;
	unsigned long imageTransferRate;
	unsigned long siTransferRate;
	unsigned long inputGainR;
	unsigned long inputGainG;
	unsigned long inputGainB;
	unsigned long analogGain;
	unsigned long drawTrackingFrame;
	unsigned long enableTracking;
	unsigned long aluState;
	unsigned long aluProgramNumber;
	unsigned long targetTrackingMode;
	unsigned long noiseReduction;
	unsigned long exposureMode;
	unsigned long shutterValue;
	unsigned long fdGain;
	unsigned long digitalGain;
	unsigned long whitebalanceMode;
} ClCameraConfigAll;

typedef enum _ClSensorRegisterCategory
{
	CATEGORY_VERSION,
	CATEGORY_CONFIG,
	CATEGORY_GPIO_CTRL,
	CATEGORY_IO_CFG,
	CATEGORY_SW_AD,
	CATEGORY_SYSCTRL,
	CATEGORY_SYS_COM,
	CATEGORY_SYS_SOUT,
	CATEGORY_CMD_COM,
	CATEGORY_OTP,
	CATEGORY_EXT_ENC,
	CATEGORY_SG_CTRL,
	CATEGORY_SG_COM,
	CATEGORY_SCENE_CFG,
	CATEGORY_SCENE_DATA0,
	CATEGORY_SCENE_DATA1,
	CATEGORY_SCENE_DATA2,
	CATEGORY_ADJ,
	CATEGORY_ADJ_COM,
	CATEGORY_AUTOCTRL,
	CATEGORY_AUTOCOM,
	CATEGORY_AE,
	CATEGORY_AE_COM,
	CATEGORY_AE_DGRM,
	CATEGORY_AE_FLC_COM,
	CATEGORY_AE_FLC,
	CATEGORY_AE_FLCDTCT_OUT,
	CATEGORY_AE_FRM,
	CATEGORY_AE_OPD_EXT,
	CATEGORY_AE_OPD_OUT,
	CATEGORY_AE_OPD,
	CATEGORY_AE_SOUT,
	CATEGORY_AE_WD_COM,
	CATEGORY_AE_MIRIS_COM,
	CATEGORY_AE_F,
	CATEGORY_AE_COM_F,
	CATEGORY_AE_DGRM_F,
	CATEGORY_AE_FRM_F,
	CATEGORY_AE_OPD_F,
	CATEGORY_AE_SOUT_F,
	CATEGORY_AWB,
	CATEGORY_AWB_ADJ,
	CATEGORY_AWB_FRM,
	CATEGORY_AWB_EXTFRM,
	CATEGORY_AWB_EXPFRM,
	CATEGORY_AWB_USERTYPE,
	CATEGORY_AWB_OPD,
	CATEGORY_AWB_WBG,
	CATEGORY_AWB_SUPER_KELVIN,
	CATEGORY_AWB_BB,
	CATEGORY_AWB_MWB,
	CATEGORY_AWB_OPD_R_OUT,
	CATEGORY_AWB_OPD_G_OUT,
	CATEGORY_AWB_OPD_B_OUT,
	CATEGORY_AWB_OPD_EXT,
	CATEGORY_AWB_COM,
	CATEGORY_AWB_SOUT,
	CATEGORY_SENSCOM,
	CATEGORY_SENSCTRL,
	CATEGORY_SENS,
	CATEGORY_MODE,
	CATEGORY_MIRROR,
	CATEGORY_CONST0,
	CATEGORY_CONST1,
	CATEGORY_FW_CMN,
	CATEGORY_LOG_SET,
	CATEGORY_LOG_DATA,
	CATEGORY_LOG_SETTING,
	CATEGORY_LOG_OUT,
	CATEGORY_AED_SRAM,
	CATEGORY_CU_CUSTOM1_REG,
	CATEGORY_CU_CUSTOM2_REG,
	CATEGORY_CU_CUSTOM3_REG,
	CATEGORY_CU_CUSTOM4_REG,
	CATEGORY_CU_CUSTOM7_REG,
	CATEGORY_CU_CUSTOM8_REG,
	CATEGORY_CU_CUSTOM10_REG,
	CATEGORY_CU_CUSTOM12_REG,
	CATEGORY_RP_REG,
	CATEGORY_RP_INT,
	CATEGORY_RP_ALU,
	CATEGORY_RP_TEMP,
	CATEGORY_RP_FRAME1,
	CATEGORY_RP_FRAME2,
	CATEGORY_RP_FRAME3,
	CATEGORY_RP_FRAME4,
	CATEGORY_RP_FRAME5,
	CATEGORY_RP_FRAME6,
	CATEGORY_RP_FRAME7,
	CATEGORY_RP_FRAME8,
	CATEGORY_RC_REG,
	CATEGORY_SG_REG,
	CATEGORY_SG_INT,
	CATEGORY_CK_REG,
	CATEGORY_RG_MIF_REG,
	CATEGORY_RG_CLMIF_REG,
	CATEGORY_RG_REGIF_REG,
	CATEGORY_SYTOP_OTPIF0,
	CATEGORY_SYTOP_OTPIF1,
	CATEGORY_SYTOP_OTPDIRECT,
	CATEGORY_SYTOP_TIMER0,
	CATEGORY_SYTOP_TIMER1,
	CATEGORY_SYTOP_GPIO,
	CATEGORY_SYTOP_UART,
	CATEGORY_SYTOP_I2C_MASTER,
	CATEGORY_SYTOP_WDT,
	CATEGORY_SYTOP_SREG,
	CATEGORY_SYTOP_SPI0_SIOCS,
	CATEGORY_SYTOP_SPI0_CTRLIF,
	CATEGORY_SYTOP_SPI1_FLASH,
	CATEGORY_SYTOP_SPI2_CAMSIO,
	CATEGORY_SYTOP_SPI2_CAMSIOCS,
	CATEGORY_SYTOP_SPI2_ADDCS,
	CATEGORY_SYTOP_DEBUG,
	CATEGORY_SYTOP_I2C_CIS,
} ClSensorRegisterCategory;


// Image output mode
#define CLOUTPUTMODE_1BIT0  (0x00000001)
#define CLOUTPUTMODE_1BIT1  (0x00000002)
#define CLOUTPUTMODE_4BITHD (0x00000100)
#define CLOUTPUTMODE_4BITDU (0x00000200)
#define CLOUTPUTMODE_8BITHD (0x00001000)
#define CLOUTPUTMODE_8BITDU (0x00002000)
#define CLOUTPUTMODE_10BITDU (0x00002000)
#define CLOUTPUTMODE_12BITDU (0x00008000)
#define CLOUTPUTMODE_14BITDU (0x00020000)
#define CLOUTPUTMODE_16BITDU (0x00080000)

// Image data structure
typedef struct _ClImageData
{
	unsigned short clipping_X_Pos;
	unsigned short clipping_Y_Pos;
	unsigned short clipping_Width;
	unsigned short clipping_Height;
	unsigned short width;
	unsigned short height;
	unsigned char colorDepth;
	unsigned char packed1bit : 1;
	unsigned char sony4bit : 1;
	unsigned char monoSensor : 1;
	unsigned char type1bit : 1;
	unsigned char es1 : 1;
	unsigned char reserved: 3;
	unsigned short step;
	unsigned char bitmapTop;
} ClImageData;

// Target tracking mode
#define CLTRACKINGMODE_SET_TO_FRAME_MEMORY      (0x00)
#define CLTRACKINGMODE_DIFF_BACKGROUND          (0x01)
#define CLTRACKINGMODE_DIFF_PREVIOUSFRAME       (0x02)
#define CLTRACKINGMODE_STORE_DIFF_PREVIOUSFRAME (0x03)
#define CLTRACKINGMODE_OUT_1BIT                 (0x04)
#define CLTRACKINGMODE_1BIT_AND_DIFF            (0x06)
#define CLTRACKINGMODE_SELFWINDOW               (0x07)

#define CLTRANSFERCHANNEL_SI    (0x00000001)
#define CLTRANSFERCHANNEL_IMAGE (0x00000002)


typedef struct _CUInfo
{
	ClVersionInfo firmwareVersion;		// Firmware version of CU
	ClVersionInfo fpgaVersion;			// FPGA version
	unsigned long platformVersion;		// Platform Identifier and Version
	unsigned short processorSpeed;		// Zynq frequency MHz (667 for V1)
	unsigned short numberOfCPUCore;		// Number of CPU core
	unsigned short liveMemorySize;		// SO-DIMM size in MB (4096 for V1)
	unsigned short longPlayMemorySize;	// SO-DIMM size in MB (4096 for V1)
	unsigned long fx3Revision;			// 0 if not existing
	ClVersionInfo fx3FirmwareVersion;	// 0 if not existing
	unsigned long sensorType;			// 0: bayer 1: mono
	unsigned short sensorBoardRevision;	// 
	unsigned short ifBoardRevision;		// 
	unsigned long eepromSize;			// EEPROM size in Byte
	unsigned char identifier[8];		// model number + individual number
	unsigned long networkSettingFlags;	// 
	unsigned long ipAddress;			// (stored in host byte order)
	unsigned long subnetMask;			// 
	unsigned long defaultGateway;		// 
	unsigned short tcpPortNumber;		// Port Number for TCP/IP communication (stored in host byte order)
	unsigned short udpPortNumber;		// Port Number for UDP/IP communication (stored in host byte order)
	char cuName[16];					// NULL-terminated string
	unsigned long region;				// region number
	unsigned long reserved1;
} CUInfo;


// Flags for ClSetNetworkFlags() and ClSetNetworkProperties()
#define CLNETWORKFLAG_FIX_ADDRESS	(0x00000001)
#define CLNETWORKFLAG_USE_DHCP		(0x00000002)

// Flags for ClLoadCUApplicationFrom*
#define CLCUAPPFLAG_LOADSTART		(0x00000001)
#define CLCUAPPFLAG_LOADEND			(0x00000002)
#define CLCUAPPFLAG_NOTMIDDLECORE	(0x00000004)
#define CLCUAPPFLAG_EXITAFTERINIT	(0x00000008)
#define CLCUAPPFLAG_CONNECTGDB		(0x00000010)
#define CLCUAPPFLAG_SETPRINTPORT	(0x00000020)
#define CLCUAPPPORT_UART			(0)
#define CLCUAPPPORT_NULL			(1)
#define CLCUAPPPORT_UDP				(2)
#define CLCUAPPPORT_TCP				(3)

typedef struct _CUApplicationPrintEtherParam
{
	unsigned long					IPAddress;
	unsigned short					Port;
	unsigned long					Reserved[2];
} CUApplicationPrintEtherParam;

typedef union _CUApplicationPrintPortParam
{
	CUApplicationPrintEtherParam	Ethernet;
} CUApplicationPrintPortParam;

typedef struct _CUApplicationPrintPort
{
	unsigned long					PortType;
	CUApplicationPrintPortParam		PortParam;
} CUApplicationPrintPort;

typedef struct _CUApplicationParam
{
	unsigned long					LoadAddress;
	unsigned long					Flags;
	CUApplicationPrintPort			PrintPort;
} CUApplicationParam;


typedef enum _CUApplicationType
{
	CLCUAPPTYPE_UNKNOWN = -1,	//!< error
	CLCUAPPTYPE_NONE = 0,		//!< unsupported
	CLCUAPPTYPE_V1,				//!< V1
	CLCUAPPTYPE_K2ES1,			//!< K2 ES1
	CLCUAPPTYPE_K2,				//!< K2
} CUApplicationType;


// Device state enumerator
typedef enum _ClDeviceState
{
	DEVICESTATE_UNKNOWN = -1,
	DEVICESTATE_DISCONNECTED,
	DEVICESTATE_INITIALIZING,
	DEVICESTATE_READY,
	DEVICESTATE_ACTIVE,
	DEVICESTATE_CLOSING,
} ClDeviceState;


typedef struct _ClStatistics
{
	// CL side
	unsigned long numberOfSentCUCommand;
	unsigned long numberOfSentSensorCommand;
	unsigned long numberOfReceivedCUResponse;
	unsigned long numberOfReceivedSensorCommand;
	unsigned long numberOfReceuvedCUNotification;
	unsigned long numberOfReceivedImage;
	unsigned long numberOfReceivedSI;
	unsigned long long numberOfSendBytes;
	unsigned long long numberOfReceivedBytes;
	unsigned long long numberOfReceivedMeaningfulBytes;

	// CU side
	unsigned long numberOfProceededCUCommand;
	unsigned long numberOfReceivedUserCommand;
	unsigned long numberOfDispatchedUserCommand;
	unsigned long numberOfProceededSensorCommand;
	unsigned long numberOfSentCUNotification;
	unsigned long numberOfDiscardedImage;
	unsigned long numberOfDiscardedSI;
	unsigned long numberOfMIPIInterrupt;
	unsigned long numberOfMIPIECCError;
	unsigned long numberOfMIPICRCError;
} ClStatistics;


// clLogLevel
typedef enum _ClLogType
{
	CLLOGTYPE_CRITICAL			= 0x0001,
	CLLOGTYPE_WARNING			= 0x0002,
	CLLOGTYPE_INFO				= 0x0004,
	CLLOGTYPE_ADDITIONALINFO	= 0x0008,
	CLLOGTYPE_FUNCTION			= 0x0010,
	CLLOGTYPE_PACKET			= 0x0020,
	CLLOGTYPE_RESERVE1			= 0x0040,
	CLLOGTYPE_RESERVE2			= 0x0080,
	CLLOGTYPE_RESERVE3			= 0x0100,
	CLLOGTYPE_RESERVE4			= 0x0200,
	CLLOGTYPE_RESERVE5			= 0x0400,
	CLLOGTYPE_RESERVE6			= 0x0800,
	CLLOGTYPE_CUSTOM1			= 0x1000,
	CLLOGTYPE_CUSTOM2			= 0x2000,
	CLLOGTYPE_CUSTOM3			= 0x4000,
	CLLOGTYPE_CUSTOM4			= 0x8000,
	CLLOGTYPE_ALL				= 0xFFFF,
} ClLogType;


typedef struct _CULog
{
	unsigned long type;
	unsigned long id;
	unsigned long long time;
	unsigned long data1;
	unsigned long data2;
	unsigned long data3;
	unsigned long data4;
} CULog;

typedef struct _CUNotification
{
	unsigned long notificationType;
	unsigned long id;
	unsigned long long time;
	unsigned long data1;
	unsigned long data2;
	unsigned long data3;
	unsigned long data4;
	unsigned long binaryLength;
	unsigned char binaryTop;
} CUNotification;

typedef struct _ClAvailableDevice_A
{
	ClPortType	portType;
	char		deviceIdentifier[DEVICE_IDENTIFIER_MAX_LENGTH];
	char		cuName[CUNAME_MAX_LENGTH + 1];
	char		cuIdentifier[32];
	int			platformVersion;
} ClAvailableDevice_A;

typedef struct _ClAvailableDevices_A
{
	int					numberOfAvailableDevices;
	ClAvailableDevice_A	availableDevices[1];
} ClAvailableDevices_A;

typedef struct _ClAvailableDevice_W
{
	ClPortType	portType;
	wchar_t		deviceIdentifier[DEVICE_IDENTIFIER_MAX_LENGTH];
	wchar_t		cuName[CUNAME_MAX_LENGTH + 1];
	wchar_t		cuIdentifier[32];
	int			platformVersion;
} ClAvailableDevice_W;

typedef struct _ClAvailableDevices_W
{
	int					numberOfAvailableDevices;
	ClAvailableDevice_W	availableDevices[1];
} ClAvailableDevices_W;

#if defined(UNICODE) || defined(_UNICODE)
	#define ClAvailableDevice				ClAvailableDevice_W
	#define ClAvailableDevices				ClAvailableDevices_W
	#define ClEnumerateDevices				ClEnumerateDevicesW
	#define ClOpenCameraByDeviceName		ClOpenCameraByDeviceNameW
	#define ClGetDeviceName					ClGetDeviceNameW
	#define ClSetCUName						ClSetCUNameW
	#define ClSaveCurrentImage				ClSaveCurrentImageW
	#define ClSaveCurrentSI					ClSaveCurrentSIW
	#define ClSaveTimeTracingInfAsCSV		ClSaveTimeTracingInfAsCSVW
	#define ClLoadSensorRegisterFromFile	ClLoadSensorRegisterFromFileW
	#define ClLoadCPALUProgramFromFile		ClLoadCPALUProgramFromFileW
	#define ClLoadCUApplicationFromFile		ClLoadCUApplicationFromFileW
	#define ClSetLogParam					ClSetLogParamW
#else
	#define ClAvailableDevice				ClAvailableDevice_A
	#define ClAvailableDevices				ClAvailableDevices_A
	#define ClEnumerateDevices				ClEnumerateDevicesA
	#define ClOpenCameraByDeviceName		ClOpenCameraByDeviceNameA
	#define ClGetDeviceName					ClGetDeviceNameA
	#define ClSetCUName						ClSetCUNameA
	#define ClSaveCurrentImage				ClSaveCurrentImageA
	#define ClSaveCurrentSI					ClSaveCurrentSIA
	#define ClSaveTimeTracingInfAsCSV		ClSaveTimeTracingInfAsCSVA
	#define ClLoadSensorRegisterFromFile	ClLoadSensorRegisterFromFileA
	#define ClLoadCPALUProgramFromFile		ClLoadCPALUProgramFromFileA
	#define ClLoadCUApplicationFromFile		ClLoadCUApplicationFromFileA
	#define ClSetLogParam					ClSetLogParamA
#endif // UNICODE

#define NamedPipeID int
typedef void* CameraHandle;

#ifdef _WIN32
#pragma warning(pop)
#endif

// callback functions
typedef void(*ClCallbackFunction_Image)(ClImageData* imageBuffer, unsigned long dataSize, unsigned long serialNumber, void* userData);
typedef void(*ClCallbackFunction_SI)(ClSensingInformation_MIPI* si, unsigned long dataSize, void* userData);
typedef void(*ClCallbackFunction_CUNotification)(CUNotification* notification, unsigned long dataSize, void* userData);

#ifdef __cplusplus
extern "C"
{
#endif

	// API prototypes

	CAMERALIBAPI unsigned long ClGetLibraryVersion(void);

	CAMERALIBAPI void ClFreeMemory(void* pointer);

	CAMERALIBAPI ClAvailableDevices_A* ClEnumerateDevicesA(void);
	CAMERALIBAPI ClAvailableDevices_W* ClEnumerateDevicesW(void);

	// Open and Close camera
	CAMERALIBAPI CameraHandle ClOpenCamera(void);
	CAMERALIBAPI CameraHandle ClOpenCameraByDeviceNameA(const char* deviceIdentifier);
	CAMERALIBAPI CameraHandle ClOpenCameraByDeviceNameW(const wchar_t* deviceIdentifier);
	CAMERALIBAPI CameraHandle ClOpenCameraByPortType(ClPortType portType);
	CAMERALIBAPI BOOL ClCloseCamera(CameraHandle cameraHandle);

	// Initialization and setup camera
	CAMERALIBAPI BOOL ClInitializeCU(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClResetCU(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClResetSensor(CameraHandle cameraHandle);
	CAMERALIBAPI ClPortType ClGetTransferPort(CameraHandle cameraHandle);
	CAMERALIBAPI char* ClGetDeviceNameA(CameraHandle cameraHandle);
	CAMERALIBAPI wchar_t* ClGetDeviceNameW(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClSetCameraParam(CameraHandle cameraHandle, ClCameraConfigKey configKey, unsigned long value);
	CAMERALIBAPI unsigned long ClGetCameraParam(CameraHandle cameraHandle, ClCameraConfigKey configKey);
	CAMERALIBAPI BOOL ClSetAllCameraParams(CameraHandle cameraHandle, ClCameraConfigAll* cameraConfig);
	CAMERALIBAPI ClCameraConfigAll* ClGetAllCameraParams(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClSetNetworkFlags(CameraHandle cameraHandle, unsigned long flags);
	CAMERALIBAPI BOOL ClSetNetworkProperties(CameraHandle cameraHandle, unsigned long flags, unsigned long ipAddress, unsigned long subnetMask, unsigned long defaultGateway);
	CAMERALIBAPI BOOL ClSetCUNameA(CameraHandle cameraHandle, const char* cuName);
	CAMERALIBAPI BOOL ClSetCUNameW(CameraHandle cameraHandle, const wchar_t* cuName);

	// CU Info and CU Statistics
	CAMERALIBAPI CUInfo* ClGetCUInfo(CameraHandle cameraHandle);
	CAMERALIBAPI ClStatistics* ClGetCUStatistics(CameraHandle cameraHandle);
	CAMERALIBAPI ClDeviceState ClGetDeviceState(CameraHandle cameraHandle);

	// callback method
	CAMERALIBAPI BOOL ClRegisterCallbackForImage(CameraHandle cameraHandle, ClCallbackFunction_Image function, void* userData);
	CAMERALIBAPI BOOL ClRegisterCallbackForSI(CameraHandle cameraHandle, ClCallbackFunction_SI function, void* userData);
	CAMERALIBAPI BOOL ClRegisterCallbackForNotification(CameraHandle cameraHandle, ClCallbackFunction_CUNotification function, void* userData);

	// Start stop transfer
	CAMERALIBAPI BOOL ClStartTransfer(CameraHandle cameraHandle, unsigned long channel);
	CAMERALIBAPI BOOL ClStopTransfer(CameraHandle cameraHandle, unsigned long channel);

	// Image and SI transfer
	CAMERALIBAPI ClImageData* ClGetCurrentImage(CameraHandle cameraHandle, unsigned long* dataSize, unsigned long* serialNumber);
	CAMERALIBAPI ClSensingInformation_MIPI* ClGetCurrentSI(CameraHandle cameraHandle, unsigned long* dataSize);
	CAMERALIBAPI BOOL ClIsNewImageReady(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClSaveCurrentImageA(CameraHandle cameraHandle, const char* fileName);
	CAMERALIBAPI BOOL ClSaveCurrentImageW(CameraHandle cameraHandle, const wchar_t* fileName);
	CAMERALIBAPI BOOL ClSaveCurrentSIA(CameraHandle cameraHandle, const char* fileName);
	CAMERALIBAPI BOOL ClSaveCurrentSIW(CameraHandle cameraHandle, const wchar_t* fileName);

	// Stored Image and SI transfer
	CAMERALIBAPI BOOL ClTransferStoredFrames(CameraHandle cameraHandle, unsigned long startFrame, unsigned long endFrame, unsigned long dataTypeBitField);
	CAMERALIBAPI BOOL ClTransferStoredFramesWithSpecifiedTimeRange(CameraHandle cameraHandle, long startTime, long endTime, unsigned long dataTypeBitField);
	CAMERALIBAPI BOOL ClRegisterCallbackForStoredImage(CameraHandle cameraHandle, ClCallbackFunction_Image function, void* userData);
	CAMERALIBAPI BOOL ClRegisterCallbackForStoredSI(CameraHandle cameraHandle, ClCallbackFunction_SI function, void* userData);
	CAMERALIBAPI BOOL ClSuspendTransferStoredFrames(CameraHandle cameraHandle, unsigned long dataTypeBitField);
	CAMERALIBAPI BOOL ClResumeTransferStoredFrames(CameraHandle cameraHandle, unsigned long dataTypeBitField);
	CAMERALIBAPI BOOL ClCancelTransferStoredFrames(CameraHandle cameraHandle, unsigned long dataTypeBitField);
	CAMERALIBAPI ClImageData* ClGetStoredImage(CameraHandle cameraHandle, unsigned long* dataSize, unsigned long* serialNumber);
	CAMERALIBAPI ClSensingInformation_MIPI* ClGetStoredSI(CameraHandle cameraHandle, unsigned long* dataSize);

	// CU logging related APIs
	CAMERALIBAPI BOOL ClStartCULogging(CameraHandle cameraHandle, unsigned long logtype, unsigned long maxLogCount, BOOL requireNotification);		// detail information logged using CU API
	CAMERALIBAPI BOOL ClStopCULogging(CameraHandle cameraHandle, BOOL freeMemory);
	CAMERALIBAPI BOOL ClTransferCULog(CameraHandle cameraHandle, unsigned long* numLogs, CULog** cuLogs);

	// Time tracing related APIs
	CAMERALIBAPI BOOL ClEnableTimeTracing(CameraHandle cameraHandle, unsigned long bufferSize);
	CAMERALIBAPI BOOL ClDisableTimeTracing(CameraHandle cameraHandle);
	CAMERALIBAPI unsigned char* ClGetTimeTracingInfo(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClSaveTimeTracingInfAsCSVA(CameraHandle cameraHandle, const char* fileName);
	CAMERALIBAPI BOOL ClSaveTimeTracingInfAsCSVW(CameraHandle cameraHandle, const wchar_t* fileName);
	CAMERALIBAPI BOOL ClFreeTimeTracingInfoBuffer(CameraHandle cameraHandle);

	// register related APIs
	CAMERALIBAPI BOOL ClWriteSensorRegister(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, int length, const void* value, BOOL responseRequired);
	CAMERALIBAPI BOOL ClWriteSensorRegisterByte(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned char value, BOOL responseRequired);
	CAMERALIBAPI BOOL ClWriteSensorRegisterWord(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned short value, BOOL responseRequired);
	CAMERALIBAPI BOOL ClWriteSensorRegisterDWord(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned long value, BOOL responseRequired);
	CAMERALIBAPI BOOL ClReadSensorRegister(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, int length, void* value);
	CAMERALIBAPI BOOL ClReadSensorRegisterByte(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned char *value);
	CAMERALIBAPI BOOL ClReadSensorRegisterWord(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned short *value);
	CAMERALIBAPI BOOL ClReadSensorRegisterDWord(CameraHandle cameraHandle, ClSensorRegisterCategory category, unsigned short offset, unsigned long *value);
	CAMERALIBAPI BOOL ClLoadSensorRegisterFromMemory(CameraHandle cameraHandle, const void* data, int dataSize, BOOL checkOneByOne);
	CAMERALIBAPI BOOL ClLoadSensorRegisterFromFileA(CameraHandle cameraHandle, const char* fileName, BOOL checkOneByOne);
	CAMERALIBAPI BOOL ClLoadSensorRegisterFromFileW(CameraHandle cameraHandle, const wchar_t* fileName, BOOL checkOneByOne);
	CAMERALIBAPI BOOL ClFlushSensorCommandBuffer(CameraHandle cameraHandle);

	// Other sensor command related APIs
	CAMERALIBAPI BOOL ClLoadCPALUProgramFromMemory(CameraHandle cameraHandle, const void* program, int programSize, BOOL checkOneByOne);
	CAMERALIBAPI BOOL ClLoadCPALUProgramFromFileA(CameraHandle cameraHandle, const char* fileName, BOOL checkOneByOne);
	CAMERALIBAPI BOOL ClLoadCPALUProgramFromFileW(CameraHandle cameraHandle, const wchar_t* fileName, BOOL checkOneByOne);

	// load/unload CU application to CU
	CAMERALIBAPI CUApplicationType ClGetCUApplicationType(CameraHandle cameraHandle);
	CAMERALIBAPI BOOL ClLoadCUApplicationFromMemory(CameraHandle cameraHandle, const void* app, int appSize, const CUApplicationParam* param);
	CAMERALIBAPI BOOL ClLoadCUApplicationFromFileA(CameraHandle cameraHandle, const char* fileName, const CUApplicationParam* param);
	CAMERALIBAPI BOOL ClLoadCUApplicationFromFileW(CameraHandle cameraHandle, const wchar_t* fileName, const CUApplicationParam* param);
	CAMERALIBAPI BOOL ClUnloadCUApplication(CameraHandle cameraHandle);

	// user define command related APIs. To be added
	CAMERALIBAPI BOOL ClInvokeUserCommand(CameraHandle cameraHandle, unsigned long command, unsigned char* param, unsigned long paramSize);

	CAMERALIBAPI NamedPipeID ClOpenNamedPipe(CameraHandle cameraHandle, unsigned long packetTypes);
	CAMERALIBAPI BOOL ClCloseNamedPipe(CameraHandle cameraHandle, NamedPipeID pipeId);
	CAMERALIBAPI CameraHandle ClConnectCameraByNamedPipe(NamedPipeID pipeId);

	CAMERALIBAPI BOOL ClSetLogParamA(const char* fileName, int showInStd, int level);
	CAMERALIBAPI BOOL ClSetLogParamW(const wchar_t* fileName, int showInStd, int level);
	CAMERALIBAPI BOOL ClStartLog(void);
	CAMERALIBAPI BOOL ClStopLog(void);

#ifdef __cplusplus
}
#endif

#endif /* HSVCL_H_ */
