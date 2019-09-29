/*
HSV SDK Camera Library

SensingInformation.h
*/

#ifndef SENSINGINFORMATION_H_
#define SENSINGINFORMATION_H_


#pragma once

#define SI_LINE1_EFFCTIVE_SIZE (468)
#define SI_LINE2_EFFCTIVE_SIZE (295)
#define SI_SIZE_TOTAL (SI_LINE1_EFFCTIVE_SIZE + SI_LINE2_EFFCTIVE_SIZE + 4)

#pragma pack(1)

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4214)
typedef struct _SensingInformation_MIPI
{
	// 4 bytes
	unsigned long frameNumber;

	// 2 bytes
	unsigned char aluProgramID : 2;
	unsigned char : 6;
	unsigned char enableAll : 1;
	unsigned char enableTargetTracking : 1;
	unsigned char : 1;
	unsigned char enableTargetBrightnessDetection : 1;
	unsigned char : 4;

	// 12 bytes
	unsigned char threshold_MONO_Max : 4;
	unsigned char : 4;
	unsigned char threshold_MONO_Min : 4;
	unsigned char : 4;
	unsigned char threshold_R_Max : 4;
	unsigned char : 4;
	unsigned char threshold_R_Min : 4;
	unsigned char : 4;
	unsigned char threshold_G_Max : 4;
	unsigned char : 4;
	unsigned char threshold_G_Min : 4;
	unsigned char : 4;
	unsigned char threshold_B_Max : 4;
	unsigned char : 4;
	unsigned char threshold_B_Min : 4;
	unsigned char : 4;
	unsigned char threshold_Alpha : 5;
	unsigned char : 3;
	unsigned char threshold_Beta : 5;
	unsigned char : 3;
	unsigned char threshold_Gamma_Max : 4;
	unsigned char : 4;
	unsigned char threshold_Gamma_Min : 4;
	unsigned char : 4;

	// 21 bytes
	struct {
		unsigned short x;
		unsigned short y;
		unsigned char similalityLevel1;
		unsigned char similalityLevel2;
		unsigned char similalityLevel3;
	} Similality[3];

	// 1 byte
	unsigned char TemplateScale : 3;

	// 144bytes
	struct {
		unsigned char histogram1;
		unsigned char histogram2;
		unsigned char histogram3 : 5;
	} HistogramDefaultRed[16], HistogramDefaultGreenMono[16], HistogramDefaultBlue[16];

	// 288bytes
	struct {
		unsigned short REFLevel : 12;
		unsigned long blightnessIntegral;
		unsigned char pixelIntegral1;
		unsigned char pixelIntegral2;
		unsigned char pixelIntegral3 : 4;
	} BrightnessHistogram[32];

	union {
		struct {
			unsigned char detection_Result_Frame1 : 1;
			unsigned char detection_Result_Frame2 : 1;
			unsigned char detection_Result_Frame3 : 1;
			unsigned char detection_Result_Frame4 : 1;
			unsigned char detection_Result_Frame5 : 1;
			unsigned char detection_Result_Frame6 : 1;
		};
		unsigned char detection_Results;
	};

	struct {
		unsigned char moment_0D1;
		unsigned char moment_0D2;
		unsigned char moment_0D3 : 5;
		unsigned long moment_1D_X : 30;
		unsigned long moment_1D_Y : 30;
		unsigned short moment_CentroidX : 11;
		unsigned short moment_CentroidY : 10;
		unsigned short moment_AverageCentroidX : 11;
		unsigned short moment_AverageCentroidY : 10;
		unsigned short moment_VectorX : 12;
		unsigned short moment_VectorY : 11;
		unsigned short moment_VectorLength : 12;
		unsigned char moment_VectorDirection : 3;
	} Moments[6];

	struct {
		union {
			struct {
				unsigned char detection_Result_Frame1 : 1;
				unsigned char detection_Result_Frame2 : 1;
				unsigned char detection_Result_Frame3 : 1;
				unsigned char detection_Result_Frame4 : 1;
				unsigned char detection_Result_Frame5 : 1;
				unsigned char detection_Result_Frame6 : 1;
			};
			unsigned char detection_Results_OldFrame : 6;
		};
		struct {
			unsigned char moment_0D1;
			unsigned char moment_0D2;
			unsigned char moment_0D3 : 5;
			unsigned short moment_1D_X : 11;
			unsigned short moment_1D_Y : 10;
		} Moments[6];
	} OldFrame[3];

	struct {
		unsigned char positive : 1;
		unsigned short frequencyCount : 10;
	} ledFlicker[3];

} ClSensingInformation_MIPI;

#pragma warning(pop)
#else

typedef struct _SensingInformation_MIPI
{
	unsigned long frameNumber;

	unsigned char aluProgramID : 2;
	unsigned char : 6;
	unsigned char enableAll : 1;
	unsigned char enableTargetTracking : 1;
	unsigned char : 1;
	unsigned char enableTargetBrightnessDetection : 1;
	unsigned char : 4;

	unsigned char threshold_MONO_Max : 4;
	unsigned char : 4;
	unsigned char threshold_MONO_Min : 4;
	unsigned char : 4;
	unsigned char threshold_R_Max : 4;
	unsigned char : 4;
	unsigned char threshold_R_Min : 4;
	unsigned char : 4;
	unsigned char threshold_G_Max : 4;
	unsigned char : 4;
	unsigned char threshold_G_Min : 4;
	unsigned char : 4;
	unsigned char threshold_B_Max : 4;
	unsigned char : 4;
	unsigned char threshold_B_Min : 4;
	unsigned char : 4;
	unsigned char threshold_Alpha : 5;
	unsigned char : 3;
	unsigned char threshold_Beta : 5;
	unsigned char : 3;
	unsigned char threshold_Gamma_Max : 4;
	unsigned char : 4;
	unsigned char threshold_Gamma_Min : 4;
	unsigned char : 4;

	struct {
		unsigned short x;
		unsigned short y;
		unsigned char similalityLevel1;
		unsigned char similalityLevel2;
		unsigned char similalityLevel3;
	} Similality[3];

	unsigned char TemplateScale : 3;
	unsigned char : 5;

	struct {
		unsigned char histogram1;
		unsigned char histogram2;
		unsigned char histogram3 : 5;
	} HistogramDefaultRed[16], HistogramDefaultGreenMono[16], HistogramDefaultBlue[16];

	struct {
		unsigned short REFLevel : 12;
		unsigned char : 4;
		unsigned long blightnessIntegral;
		unsigned char pixelIntegral1;
		unsigned char pixelIntegral2;
		unsigned char pixelIntegral3 : 4;
		unsigned char : 4;
	} BrightnessHistogram[32];

	union {
		struct {
			unsigned char detection_Result_Frame1 : 1;
			unsigned char detection_Result_Frame2 : 1;
			unsigned char detection_Result_Frame3 : 1;
			unsigned char detection_Result_Frame4 : 1;
			unsigned char detection_Result_Frame5 : 1;
			unsigned char detection_Result_Frame6 : 1;
		};
		unsigned char detection_Results;
	};

	struct {
		unsigned char moment_0D1;
		unsigned char moment_0D2;
		unsigned char moment_0D3 : 5;
		unsigned char : 3;
		unsigned long moment_1D_X : 30;
		unsigned char : 2;
		unsigned long moment_1D_Y : 30;
		unsigned char : 2;
		unsigned short moment_CentroidX : 11;
		unsigned char : 5;
		unsigned short moment_CentroidY : 10;
		unsigned char : 6;
		unsigned short moment_AverageCentroidX : 11;
		unsigned char : 5;
		unsigned short moment_AverageCentroidY : 10;
		unsigned char : 6;
		unsigned short moment_VectorX : 12;
		unsigned char : 5;
		unsigned short moment_VectorY : 11;
		unsigned char : 6;
		unsigned short moment_VectorLength : 12;
		unsigned char : 5;
		unsigned char moment_VectorDirection : 3;
		unsigned char : 5;
	} Moments[6];

	struct {
		union {
			struct {
				unsigned char detection_Result_Frame1 : 1;
				unsigned char detection_Result_Frame2 : 1;
				unsigned char detection_Result_Frame3 : 1;
				unsigned char detection_Result_Frame4 : 1;
				unsigned char detection_Result_Frame5 : 1;
				unsigned char detection_Result_Frame6 : 1;
			};
			unsigned char detection_Results_OldFrame : 6;
		};
		struct {
			unsigned char moment_0D1;
			unsigned char moment_0D2;
			unsigned char moment_0D3 : 5;
			unsigned char : 3;
			unsigned short moment_1D_X : 11;
			unsigned char : 5;
			unsigned short moment_1D_Y : 10;
			unsigned char : 6;
		} Moments[6];
	} OldFrame[3];

	struct {
		unsigned char positive : 1;
		unsigned char : 7;
		unsigned short frequencyCount : 10;
		unsigned char : 6;
	} ledFlicker[3];

} ClSensingInformation_MIPI;


typedef struct _SensingInformation_MIPI_line1
{
	unsigned long frameNumber;

	unsigned char aluProgramID : 2;
	unsigned char : 6;
	unsigned char enableAll : 1;
	unsigned char enableTargetTracking : 1;
	unsigned char : 1;
	unsigned char enableTargetBrightnessDetection : 1;
	unsigned char : 4;

	unsigned char threshold_MONO_Max : 4;
	unsigned char : 4;
	unsigned char threshold_MONO_Min : 4;
	unsigned char : 4;
	unsigned char threshold_R_Max : 4;
	unsigned char : 4;
	unsigned char threshold_R_Min : 4;
	unsigned char : 4;
	unsigned char threshold_G_Max : 4;
	unsigned char : 4;
	unsigned char threshold_G_Min : 4;
	unsigned char : 4;
	unsigned char threshold_B_Max : 4;
	unsigned char : 4;
	unsigned char threshold_B_Min : 4;
	unsigned char : 4;
	unsigned char threshold_Alpha : 5;
	unsigned char : 3;
	unsigned char threshold_Beta : 5;
	unsigned char : 3;
	unsigned char threshold_Gamma_Max : 4;
	unsigned char : 4;
	unsigned char threshold_Gamma_Min : 4;
	unsigned char : 4;

	struct {
		unsigned short x;
		unsigned short y;
		unsigned char similalityLevel1;
		unsigned char similalityLevel2;
		unsigned char similalityLevel3;
	} Similality[3];

	unsigned char TemplateScale : 3;
	unsigned char : 5;

	struct {
		unsigned char histogram1;
		unsigned char histogram2;
		unsigned char histogram3 : 5;
	} HistogramDefaultRed[16], HistogramDefaultGreenMono[16], HistogramDefaultBlue[16];

	struct {
		unsigned short REFLevel : 12;
		unsigned char : 4;
		unsigned long blightnessIntegral;
		unsigned char pixelIntegral1;
		unsigned char pixelIntegral2;
		unsigned char pixelIntegral3 : 4;
		unsigned char : 4;
	} BrightnessHistogram[32];

} SensingInformation_MIPI_line1;

typedef struct _SensingInformation_MIPI_line2
{
	union {
		struct {
			unsigned char detection_Result_Frame1 : 1;
			unsigned char detection_Result_Frame2 : 1;
			unsigned char detection_Result_Frame3 : 1;
			unsigned char detection_Result_Frame4 : 1;
			unsigned char detection_Result_Frame5 : 1;
			unsigned char detection_Result_Frame6 : 1;
		};
		unsigned char detection_Results;
	};

	struct {
		unsigned char moment_0D1;
		unsigned char moment_0D2;
		unsigned char moment_0D3 : 5;
		unsigned char : 3;
		unsigned long moment_1D_X : 30;
		unsigned char : 2;
		unsigned long moment_1D_Y : 30;
		unsigned char : 2;
		unsigned short moment_CentroidX : 11;
		unsigned char : 5;
		unsigned short moment_CentroidY : 10;
		unsigned char : 6;
		unsigned short moment_AverageCentroidX : 11;
		unsigned char : 5;
		unsigned short moment_AverageCentroidY : 10;
		unsigned char : 6;
		unsigned short moment_VectorX : 12;
		unsigned char : 5;
		unsigned short moment_VectorY : 11;
		unsigned char : 6;
		unsigned short moment_VectorLength : 12;
		unsigned char : 5;
		unsigned char moment_VectorDirection : 3;
		unsigned char : 5;
	} Moments[6];

	struct {
		union {
			struct {
				unsigned char detection_Result_Frame1 : 1;
				unsigned char detection_Result_Frame2 : 1;
				unsigned char detection_Result_Frame3 : 1;
				unsigned char detection_Result_Frame4 : 1;
				unsigned char detection_Result_Frame5 : 1;
				unsigned char detection_Result_Frame6 : 1;
			};
			unsigned char detection_Results_OldFrame : 6;
		};
		struct {
			unsigned char moment_0D1;
			unsigned char moment_0D2;
			unsigned char moment_0D3 : 5;
			unsigned char : 3;
			unsigned short moment_1D_X : 11;
			unsigned char : 5;
			unsigned short moment_1D_Y : 10;
			unsigned char : 6;
		} Moments[6];
	} OldFrame[3];

	struct {
		unsigned char positive : 1;
		unsigned char : 7;
		unsigned short frequencyCount : 10;
		unsigned char : 6;
	} ledFlicker[3];

} SensingInformation_MIPI_line2;

#endif

#pragma pack ()


#endif /* SENSINGINFORMATION_H_ */
