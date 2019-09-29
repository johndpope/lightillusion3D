///
/// @file		Ximea.cpp
/// @brief		Ximea �J����
/// @author		
/// @date		2017/8/21 �쐬
/// @date		2017/8/22 �ŏI�X�V
/// @version	1.1.2
///
///

#include <HSC/Ximea.hpp>

//#define XI_SET_TEST(input, message) {if (input != XI_OK) { XimeaMessage("error!! after xiSetParam: " message);}}

Ximea::Ximea() : Camera(CAM_WIDTH, CAM_HEIGHT, CAM_FPS), deviceName(""){
	memset(&xi_img, 0, sizeof(xi_img));
	xi_img.size = sizeof(XI_IMG);
	xiGetNumberDevices(&xi_camera_num);
	if (!xi_camera_num) { XimeaMessage("error!! after xiGetNumberDevices."); }
	XimeaMessage("Ximea Init. Cam: " + std::to_string(xi_camera_num));

	exposure_time = static_cast<int>(1000000.0 / fps) - 100;
};

bool Ximea::connect(int id) {

	if (id < xi_camera_num)xi_status = xiOpenDevice(id, &xi_handle);
	if (xi_status != XI_OK) {
		XimeaMessage("error!! after xiOpenDevice."); 
		return false;
	}
	char deviceNameTmp[256] = "";
	xi_status = xiGetDeviceInfoString(id,XI_PRM_DEVICE_NAME, deviceNameTmp, 256);
	if (xi_status != XI_OK) {
		XimeaMessage("error!! after xiGetDeviceInfoString.");
		return false;
	}
	deviceName = std::string(deviceNameTmp);
	return true;
}


void Ximea::disconnect() {
	xiCloseDevice(xi_handle);
}

void Ximea::start() {
	//---- �g���K�ݒ�
	if (trigger_mode)
	{
		//---- �g���K���[�h�ɐݒ�
		xi_status = xiSetParamInt(xi_handle, XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);
		if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_TRG_SOURCE);}
	}
	else
	{
		//---- �t���[�������[�h�ɐݒ�
		xi_status = xiSetParamInt(xi_handle, XI_PRM_TRG_SOURCE, XI_TRG_OFF);
		if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_TRG_SOURCE); }
		//---- �t���[�����[�g���w�肷��Ƃ���ŎB������悤�ɐݒ�B��������Ȃ���MAX�̎��g���ŎB������
		xi_status = xiSetParamInt(xi_handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);
		if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_ACQ_TIMING_MODE); }
	}

	//---- �摜�̎�ނ�ݒ�
	xi_status = xiSetParamInt(xi_handle, XI_PRM_IMAGE_DATA_FORMAT, img_type);
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_IMAGE_DATA_FORMAT); }

	//---- �𑜓x�ݒ�A�t���[�����[�g�ݒ�
	std::stringstream message;
	int width_max, height_max;
	float fps_max;
	xiGetParamInt(xi_handle, XI_PRM_WIDTH XI_PRM_INFO_MAX, &width_max);
	xiGetParamInt(xi_handle, XI_PRM_HEIGHT XI_PRM_INFO_MAX, &height_max);
	xiGetParamFloat(xi_handle, XI_PRM_FRAMERATE XI_PRM_INFO_MAX, &fps_max);

	int width_min, height_min;
	xiGetParamInt(xi_handle, XI_PRM_WIDTH XI_PRM_INFO_MIN, &width_min);
	xiGetParamInt(xi_handle, XI_PRM_HEIGHT XI_PRM_INFO_MIN, &height_min);
	//XimeaMessage("min: (  " + std::to_string(width_min) + ", " + std::to_string(height_min) + " ) ");
	//XimeaMessage("max: (  " + std::to_string(width_max) + ", " + std::to_string(height_max) + " ) ");
	message << deviceName << ". max : " << fps_max << " fps @ " << width_max << " x " << height_max;
	XimeaMessage(message.str());

	xi_status = xiSetParamInt(xi_handle, XI_PRM_WIDTH, std::min<int>(width_max, width));
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_WIDTH); }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_HEIGHT, std::min<int>(height_max, height));
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_HEIGHT); }

	//---- �I�����Ԑݒ�
	exposure_time = std::min<int>(static_cast<int>(1000000.0 / fps) - 100, exposure_time);
	//exposure_time = 10000;
	//xi_status = xiSetParamInt(xi_handle, XI_PRM_EXPOSURE, (int)(1000000.0 / fps) - 100);
	xi_status = xiSetParamInt(xi_handle, XI_PRM_EXPOSURE, exposure_time);
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamInt : " XI_PRM_EXPOSURE); }


	if (!trigger_mode)
	{
		xi_status = xiSetParamFloat(xi_handle, XI_PRM_FRAMERATE, std::min<float>(fps_max, fps));
		if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamFloat : " XI_PRM_FRAMERATE); }
	}

	//---- �e��p�����[�^��\��
	xiGetParamInt(xi_handle, XI_PRM_WIDTH, &width);
	xiGetParamInt(xi_handle, XI_PRM_HEIGHT, &height);
	xiGetParamFloat(xi_handle, XI_PRM_FRAMERATE, &fps);
	xiGetParamInt(xi_handle, XI_PRM_EXPOSURE, &exposure_time);
	int depth;
	xiGetParamInt(xi_handle, XI_PRM_OUTPUT_DATA_BIT_DEPTH, &depth);
	XimeaMessage(" (  " + std::to_string(width) + ", " + std::to_string(height) + " ) @ " + std::to_string(fps) + " fps " + std::to_string(depth) + " bit ");
	XimeaMessage(" Exposure time : " + std::to_string(exposure_time));

	//---- �Q�C���ݒ�
	float gain_tmp;
	xiSetParamFloat(xi_handle, XI_PRM_GAIN, gain);
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiSetParamFloat : " XI_PRM_GAIN); }
	xiGetParamFloat(xi_handle, XI_PRM_GAIN, &gain_tmp);
	XimeaMessage("gain: " + std::to_string(gain_tmp));

	//---- �ݒ芮����A�B���J�n
	xi_status = xiStartAcquisition(xi_handle);
	if (xi_status != XI_OK) { XimeaMessage("error!! after xiStartAcquisition."); }

	Sleep(1000);

}


//----------------------------------------------------------//
//-------------    �p�����[�^�ݒ�֐�      -----------------//
//----------------------------------------------------------//
void Ximea::setParam(const paramTypeCamera::paramInt &pT, const int param) {
	switch (pT) {
	case paramTypeCamera::paramInt::WIDTH:
		width = param;
		break;
	case paramTypeCamera::paramInt::HEIGHT:
		height = param;
		break;
	case paramTypeCamera::paramInt::TIMEOUT:
		timeout = param;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}

}
void Ximea::setParam(const paramTypeCamera::paramFloat &pT, const float param) {
	switch (pT) {
	case paramTypeCamera::paramFloat::FPS:
		fps = param;
		break;
	case paramTypeCamera::paramFloat::GAIN:
		gain = param;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}
}

void Ximea::setParam(const paramTypeXimea::Param &pT, const  int param) {
	switch (pT) {
	case paramTypeXimea::Param::ExposureTime:
		if (param > (1000000.0f / fps - 100) && fps < 1000.0f) std::runtime_error("�B�����[�g�ɑ΂��ĘI�����Ԃ��������܂�");
		exposure_time = param;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}
}


//---- �g���K�ݒ�
void Ximea::setParam(const paramTypeXimea::AcquisitionMode &pT)
{
	if (pT == paramTypeXimea::AcquisitionMode::TriggerMode)
	{
		trigger_mode = true;
	}
	else if (pT == paramTypeXimea::AcquisitionMode::EnableAcquisitionFrameRate)
	{
		trigger_mode = false;
	}
}
//---- �摜�^�C�v�ݒ�
void Ximea::setParam(const paramTypeXimea::CaptureType &pT)
{
	if (pT == paramTypeXimea::CaptureType::ColorGrab)
	{
		img_type = XI_RGB24;
	}
	else if (pT == paramTypeXimea::CaptureType::MonocroGrab)
	{
		img_type = XI_MONO8;
	}
	else if (pT == paramTypeXimea::CaptureType::BayerGrab)
	{
		img_type = XI_RAW8;
	}
}

//----------------------------------------------------------//
//-------------    �p�����[�^�擾�֐�      -----------------//
//----------------------------------------------------------//
int Ximea::getParam(const paramTypeCamera::paramInt &pT) {
	switch (pT) {
	case paramTypeCamera::paramInt::WIDTH:
		return width;
		break;
	case paramTypeCamera::paramInt::HEIGHT:
		return  height;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}
	return 0;
}

float Ximea::getParam(const paramTypeCamera::paramFloat &pT) {
	switch (pT) {
	case paramTypeCamera::paramFloat::FPS:
		return fps;
		break;
	case paramTypeCamera::paramFloat::GAIN:
		return gain;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}
	return 0;
}

int  Ximea::getParam(const paramTypeXimea::Param &pT) {
	switch (pT) {
	case paramTypeXimea::Param::ExposureTime:
		return exposure_time;
		break;
	default:
		XimeaMessage("There are no implementation");
		assert(false);
		break;
	}
	return 0;
}

//----------------------------------------------------------//
//------------------    �摜�擾�֐�     -------------------//
//----------------------------------------------------------//
int Ximea::captureFrame(void* data) {
	xiGetImage(xi_handle, timeout, &xi_img);
	if (img_type == XI_RGB24) {
		memcpy((unsigned char *)data, xi_img.bp, 3 * xi_img.width*xi_img.height);
	}
	else {
		memcpy((unsigned char *)data, xi_img.bp, xi_img.width*xi_img.height);
	}
	return -2;
}


void  Ximea::XimeaMessage(std::string str) {
	HANDLE hStdout;
	WORD wAttributes;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &csbi);
	wAttributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(hStdout, wAttributes);
	std::cout << "[Ximea] " << str << std::endl;
	SetConsoleTextAttribute(hStdout, csbi.wAttributes);
	return;
}