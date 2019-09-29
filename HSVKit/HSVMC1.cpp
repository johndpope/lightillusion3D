#include"HSVMC1.h"
#include<iostream>

using namespace std;


void HSVMC1::connect(int setMode) {
	BOOL result;
	cameraHandle = ClOpenCamera();
	if (cameraHandle == NULL) {
		cout << "Failed to open camera" << endl;
		return;
	}
	unsigned long nowImageMode;
	ClCameraConfigAll* cucamConfig = ClGetAllCameraParams(cameraHandle);
	if (cucamConfig != NULL)
	{
		nowImageMode = cucamConfig->imageMode;
		std::cout << "mode : " << cucamConfig->imageMode << std::endl;
		ClFreeMemory(cucamConfig);
	}

	result = ClSetCameraParam(cameraHandle, CLP_IMAGEMODE, setMode);
	if (result == FALSE)
	{
		printf_s("Failed to set image mode.\r\n");
		getchar();
	}

	if (nowImageMode != setMode) {
		printf_s("Please Restart program to apply CLP_IMAGEMODE \r\n");
		ClCloseCamera(cameraHandle);
	}

	result = ClSetCameraParam(cameraHandle, CLP_ANALOGGAIN, 50000);
	if (result == FALSE)
	{
		printf_s("Failed to set analog gain.\r\n");
		getchar();
		return;
	}

	/// カメラからの映像出力方式を設定
	/// HSVSDK Host Appプログラミングガイド.pdf の53ページ参照
	result = ClSetCameraParam(cameraHandle, CLP_IMAGEOUTPUTMODE, CLOUTPUTMODE_4BITDU);
	if (result == FALSE)
	{
		printf_s("Failed to set image output mode to 4bit DU.\r\n");
		getchar();
		return;
	}

}


void HSVMC1::start() {
	BOOL result = ClStartTransfer(cameraHandle, CLTRANSFERCHANNEL_IMAGE);
	if (result == FALSE)
	{
		printf_s("Failed to start transfer Image.\r\n");
		getchar();
		return;
	}
}


void HSVMC1::captureFrame(unsigned char* dst) {
	//BOOL result = ClIsNewImageReady(cameraHandle);
	unsigned long dataSize;
	unsigned long serialNumber;

	//if (result == TRUE) 
	{
		ClImageData* cuData = ClGetCurrentImage(cameraHandle, &dataSize, &serialNumber);
		if (cuData != NULL) {
			
			/*
			for (int i = 0; i < size; ++i) {
				unsigned short* buf = reinterpret_cast<unsigned short*>(dst);
				*buf = ((unsigned short(*(src) & (0xF0))) << 8) | (unsigned short((*(src)) & (0x0F)) << 4);
				dst += 2;
				src += 1;
			}
			*/
			
			BOOL tmpResult = ClImageHelper_Convert4bitMonoTo8bit(&(cuData->bitmapTop), dst, cuData->width, cuData->height);

			ClFreeMemory(cuData);

		}
	}
}

void HSVMC1::stop() {
	BOOL result = ClStopTransfer(cameraHandle, CLTRANSFERCHANNEL_IMAGE);
	if (result == FALSE)
	{
		printf_s("Failed to stop transfer Image.\r\n");
		getchar();
		return;
	}
}

void HSVMC1::disconnect() {

	/// カメラを閉じる
	ClCloseCamera(cameraHandle);
}