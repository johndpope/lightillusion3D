/*
HSV382 SDK Camera Library

ImageHelper.h
*/

#pragma once

// These are simplified demosaic functions which can be used for testing purpose.
// If you need good quality images, please use OpenCV or other libraries.

typedef struct _ClGainValues
{
	unsigned long gainR;
	unsigned long gainG;
	unsigned long gainB;
} ClGainValues;

typedef enum _ClPaperType
{
	CLPAPER_NOPAPER,
	CLPAPER_WHITE,
	CLPAPER_18GRAY,
	CLPAPER_50GRAY,
} ClPaperType;

#define WHITEBALANCE_SAMPLINGRATE (13)

typedef enum _ClRGBFormat
{
	CLRGBFORMAT_BGRA,
	CLRGBFORMAT_RGBA,
	CLRGBFORMAT_BGR,
	CLRGBFORMAT_RGB,
} ClRGBFormat;


#ifdef __cplusplus
extern "C"
{
#endif

BOOL ClImageHelper_Demosaic12Bit(unsigned char* source, unsigned char* destination, int width, int height, ClRGBFormat format);
BOOL ClImageHelper_Demosaic10Bit(unsigned char* source, unsigned char* destination, int width, int height, ClRGBFormat format);
BOOL ClImageHelper_Demosaic8Bit(unsigned char* source, unsigned char* destination, int width, int height, ClRGBFormat format);
BOOL ClImageHelper_Demosaic4Bit(unsigned char* source, unsigned char* destination, int width, int height, ClRGBFormat format);
BOOL ClImageHelper_Convert12bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
BOOL ClImageHelper_Convert10bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
BOOL ClImageHelper_Convert8bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
BOOL ClImageHelper_Convert4bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
BOOL ClImageHelper_Convert4bitMonoTo8bit(unsigned char* source, unsigned char* destination, int width, int height);
BOOL ClImageHelper_Convert4bitPacked1bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
BOOL ClImageHelper_Convert1bitMonoTo32bit(unsigned char* source, unsigned char* destination, int w, int h, ClRGBFormat format);
unsigned char* ClImageHelper_Demosaic(ClImageData* source, unsigned char* destination, ClRGBFormat format);
unsigned char* ClImageHelper_Binarize4bitImage(ClImageData* source, unsigned char* destination, int rMin, int rMax, int gMin, int gMax, int bMin, int bMax);

ClGainValues* ClImageHelper_GetGainValues(CameraHandle cameraHandle, ClPaperType paperType);
BOOL ClImageHelper_ApplyGainValues(unsigned char* image, int w, int h, ClGainValues* gainValues, ClRGBFormat format);
unsigned char* ClImageHelper_ClipImage(unsigned char* source, unsigned char* destination, int originalWidth, int originalHeight, int x, int y, int destinationWidth, int destinationHeight, ClRGBFormat format);

BOOL ClImageHelper_SaveRAWImage(ClImageData* image, char* fileName);
BOOL ClImageHelper_SaveRGBImageAsBMP(unsigned char* image, int w, int h, char* fileName, ClRGBFormat format);
BOOL ClImageHelper_DemosaicAndSaveImageAsBMP(ClImageData* image, ClGainValues* gainValues, char* fileName);

unsigned long ClImageHelper_ImageOutputModeFromImageData(ClImageData* image);
#ifdef __cplusplus
}
#endif
