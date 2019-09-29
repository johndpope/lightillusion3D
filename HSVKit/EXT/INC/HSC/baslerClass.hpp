#pragma once

/// @file		baslerClass.hpp
/// @brief		baslerClass baslerカメラ
/// @author		Hikaru Amano
/// @date		2017/8/10 作成
/// @date		2019/2/27 最終更新
/// @version	1.2.0
///
/// @par version 1.2.0 by Satoshi Tabata
/// @par トリガ回り修正: トリガアウト追加
///

#include <HSC/Camera.hpp>
#include <Basler/pylon/PylonIncludes.h>
#include <Basler/pylon/PylonGUI.h>
using namespace Pylon;
#include <Basler/pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
typedef Camera_t::GrabResultPtr_t GrabResultPtr_t;
#pragma comment(lib, "GCBase_MD_VC120_v3_0_Basler_pylon_v5_0")
#pragma comment(lib, "GenApi_MD_VC120_v3_0_Basler_pylon_v5_0")
#pragma comment(lib, "PylonBase_MD_VC120_v5_0")
#pragma comment(lib, "PylonC_MD_VC120")
#pragma comment(lib, "PylonGUI_MD_VC120_v5_0")
#pragma comment(lib, "PylonUtility_MD_VC120_v5_0")

/// Camera.hpp へ移動
/*
namespace paramTypeBasler
{
	enum class Param
	{
		ExposureTime = 0, 
		TriggerDelay = 1,
	};
	enum class FastMode
	{
		SensorReadoutModeFast = 0,
		SensorReadoutModeNormal = 1
	};
	enum class AcquisitionMode
	{
		EnableAcquisitionFrameRate = 0,
		TriggerMode = 1
	};
	enum class CaptureType
	{
		ColorGrab = 0,
		MonocroGrab = 1,
		BayerGrab = 2
	};
	enum class GrabStrategy
	{
		OneByOne = 0,
		LatestOnlyFrame = 1
	};
}
*/

class basler : public Camera
{

private:
	struct debug_class
	{
		float exposure_time;
		float trigger_delay;
		int fast_mode;
		int acquisition_mode;
		int cap_type;
		int grab_strategy;
		/// ver.1.2.0. 追加
		int trigger_in_line;
		int trigger_out_line;
		int trigger_out_lineSource;
		bool trigger_out_invert;
		int image_reverse;
	};
	static constexpr int CAM_WIDTH = 640;
	static constexpr int CAM_HEIGHT = 480;
	static constexpr float CAM_FPS = 500;

	static unsigned int camera_number;
	static unsigned int camera_count;
	static DeviceInfoList_t devices;
	EGrabStrategy grab_strategy;
	paramTypeBasler::CaptureType capture_type_flag;
	CTlFactory* tlFactory;
	CImageFormatConverter formatConverter;
	Camera_t *Cameras;
	CGrabResultPtr ptrGrabResult;
	CPylonImage pylonImage;
	
	debug_class debug_flag;
	std::size_t frameNumber;

	std::string deviceName;

public:

	basler();
	~basler();

	void parameter_all_print();

	bool connect(int id);

	void disconnect();

	void start();
	void stop();

	void setParam(const paramTypeCamera::paramInt &pT, const int param);

	void setParam(const paramTypeCamera::paramFloat &pT, const float param);
	void setParam(const paramTypeBasler::Param &pT, const float param);
	void setParam(const paramTypeBasler::AcquisitionMode &pT);
	void setParam(const paramTypeBasler::FastMode &pT);
	void setParam(const paramTypeBasler::CaptureType &pT);
	void setParam(const paramTypeBasler::GrabStrategy &pT);


	//virtual void getParam(const paramType &pT, void* param) = 0;
	int getParam(const paramTypeCamera::paramInt &pT);

	float getParam(const paramTypeCamera::paramFloat &pT);
	float getParam(const paramTypeBasler::Param &pT);

	/// 画像の取得(単眼)
	int captureFrame(void* data);
	//void captureFrame(void* data);
	//int captureFrameWithIndex(void* data);

	void baslerMessage(std::string str);

	/// ver.1.2.0 追加
	void setParam(const paramTypeBasler::TriggerLineSelector &pT, const paramTypeBasler::TriggerLineSource &pTs, const bool invert);
	void setParam(const paramTypeBasler::AcquisitionMode &pT, const paramTypeBasler::TriggerLineSelector &pTLine);
	void setParam(const paramTypeBasler::ImageReverse &pT);
};

