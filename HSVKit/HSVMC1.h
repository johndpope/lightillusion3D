#pragma once

#ifdef _DEBUG
#define CAMERA_EXT "d.lib"
#else
#define CAMERA_EXT ".lib"
#endif
#define STR_EXP(__A) #__A
#define STR(__A) STR_EXP(__A)



#include <bitset>


#include "HSVCL.h"
#include "ImageHelper.h"

class HSVMC1 {
public:
	CameraHandle cameraHandle;


public:

	void connect(int setMode = 7);
	void start();

	void captureFrame(unsigned char *dst);


	void stop();
	void disconnect();
};