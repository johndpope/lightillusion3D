/********************************************************************************
 * File Name　：DynaFlash.h
 * Description：DynaFlash API
 *
 *--------------------------------------------------------------------------------
 * 2016/12/07 Create						Watase.h
 * 2019/03/28 Fix:Compatible Windows 10		Kiguchi.t
 ********************************************************************************/

#ifndef _DYNAFLASH_H_
#define _DYNAFLASH_H_

#ifdef DYNAFLASH_EXPORTS
#define DYNAFLASH_API __declspec(dllexport) 
#else
#define DYNAFLASH_API __declspec(dllimport) 
#endif

/********************************************************************************
 * Define function return value
 ********************************************************************************/

#define STATUS_SUCCESSFUL		 		(0x0000)
#define	STATUS_INCOMPLETE				(0x0001)
#define	STATUS_INVALID_PARAM			(0x0002)
#define STATUS_INVALID_BOARDINDEX		(0x0004)
#define STATUS_ALLOC_FAILED				(0x0008)
#define STATUS_INVALID_DEVICEHANDLE		(0x0010)
#define STATUS_INVALID_BARNUM			(0x0020)
#define STATUS_LOCK_FAILED				(0x0040)
#define STATUS_UNLOCK_FAILED			(0x0080)
#define STATUS_FREE_FAILED				(0x0100)
#define STATUS_INVALID_CHINDEX			(0x0200)
#define STATUS_DMA_TIMEOUT				(0x0400)
#define STATUS_NO_TRIGIN				(0x0800)
#define STATUS_FRAMEBUFF_UNREGISTER		(0x1000)
#define STATUS_FRAMEBUFF_INSUFFICIENT	(0x2000)


/********************************************************************************
 * Various definitions
 ********************************************************************************/

#define MAXIMUM_NUMBER_OF_DYNAFLASH	(4)				/* Maximum number of connections for DynaFlash */

#define FRAME_BUF_SIZE_8BIT		(1024 * 768)		/* Image buffer size in Gray mode. unit in Byte */
#define FRAME_BUF_SIZE_BINARY	(1024 * 768 / 8)	/* Image buffer size in Binary mode. unit in Byte */
#define FRAME_BUF_SIZE_24BIT	(1024 * 768 * 3)	/* Image buffer size in Color mode. unit in Byte */

			/* Trigger skip mode */

typedef enum class PROJECTION_MODE {
	MIRROR = 0x00000001,/* 投影画像の左右反転 */
	FLIP = 0x00000002,/* 投影画像の上下反転 */
	COMP = 0x00000004,/* ピクセルデータのbit反転 */
	ONESHOT = 0x00000008,/* ワンショット投影モード */
	BINARY = 0x00000010,/* バイナリモード */
	EXT_TRIGGER = 0x00000020,/* 外部トリガーモード */
	TRIGGER_SKIP = 0x00000040,/* トリガースキップモード */
	PATTERN_EMBED = 0x00000080,/* 1bitパターン埋め込みモード */
	ILLUMINANCE_HIGH = 0x00000100,/* 高照度モード */
	SELF_SEQUENCE = 0x00000200,/* 自前シーケンス利用モード */
	COLOR = 0x00000400/* カラープロジェクタモード */
}PM;


/* Define illumination settings */
typedef enum {
	LOW_MODE = 0,									/* Low illuminance */
	HIGH_MODE										/* High illuminance */
} ILLUMINANCE_MODE;


/* Define trigger settings */
typedef enum {
	TRIG_NORMAL = 0,								/* Normal Mode*/
	TRIG_FLAG,										/* Flag Mode */
	TRIG_DISABLE									/* Trigger Output Disable */
} TRIGGER_MODE;

/********************************************************************************
 * Structure definition
 ********************************************************************************/

/* System parameters acquisition structure */
typedef struct _tagDynaFlashStatus
{
	unsigned long	Error;							/* DynaFlas of error information */
	unsigned long	InputFrames;					/* Number of frame transferred to DynaFlash */
	unsigned long	OutputFrames;					/* Number of projected frames */
} DYNAFLASH_STATUS, *PDYNAFLASH_STATUS;

/********************************************************************************
 * DynaFlash class definition
 ********************************************************************************/
class CDynaFlash
{
public:
	explicit CDynaFlash() {}
	virtual ~CDynaFlash() {}
	
	virtual int Connect(unsigned int nDynaFlashIndex) = 0;
	virtual int Disconnect(void) = 0;
	virtual int PowerOff(void) = 0;
	virtual unsigned int GetIndex(void) = 0;

	virtual int GetDriverVersion(char nVersion[40]) = 0;
	virtual int GetHWVersion(unsigned long *pVersion) = 0;
	virtual int GetDLLVersion(unsigned long *pVersion) = 0;
	virtual int GetDynaVersion(unsigned long *pVersion) = 0;

	virtual int Reset(void) = 0;
	virtual int Start(void) = 0;
	virtual int Stop(void) = 0;
	virtual int Float(unsigned int isPowerFloat) = 0;
	virtual int SetParam(unsigned long nFrameRate, unsigned long nBitDepth, unsigned long nProjectionMode) = 0;
	virtual int GetStatus(PDYNAFLASH_STATUS pDynaFlashStatus) = 0;

	virtual int SetIlluminance(ILLUMINANCE_MODE eIlluminanceMode) = 0;
	virtual int GetIlluminance(ILLUMINANCE_MODE *pIlluminanceMode) = 0;

	virtual int AllocFrameBuffer(unsigned long nFrameCnt) = 0;
	virtual int ReleaseFrameBuffer(void) = 0;

	virtual int GetFrameBuffer(char **ppBuffer, unsigned long *pFrameCnt) = 0;
	virtual int PostFrameBuffer(unsigned long nFrameCnt) = 0;

	/* Undocumented Reserved Function Difinitions. It may be abolished in the future */
	virtual int WriteRegister(unsigned int nBar, unsigned int nOffset, unsigned long nData) = 0;
	virtual int ReadRegister(unsigned int nBar, unsigned int nOffset, unsigned long *pData) = 0;

	virtual int WriteDACRegister(unsigned long nIndex, unsigned long nData) = 0;
	virtual int ReadDACRegister(unsigned long nIndex, unsigned long *pData) = 0;

	virtual int GetParameter(unsigned long* u32GRSTOffset, unsigned long *SWCount) = 0;

	virtual int GetFpgaInfo(float *pTemp, float *pVaux, float *pVint) = 0;
	virtual int GetFanInfo(unsigned long *pData) = 0;
	virtual int GetSysInfo(unsigned long *pData) = 0;
	virtual int GetLedTemp(unsigned long nLedIndex, float *nTemp) = 0;

	virtual int GetLedEnable(unsigned long *pLedEn) = 0;
	virtual int SetLedEnable(unsigned long nLedEn) = 0;


};

/********************************************************************************
 * Function to create an instance of DynaFlash
 ********************************************************************************/
DYNAFLASH_API CDynaFlash * _stdcall CreateDynaFlash(void);

/********************************************************************************
 * Function to create an destroy of DynaFlash
 ********************************************************************************/
DYNAFLASH_API bool _stdcall ReleaseDynaFlash(CDynaFlash **pDynaFlash);

#endif
