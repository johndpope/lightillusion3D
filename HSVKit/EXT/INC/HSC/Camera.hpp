#pragma once
///
/// @file		Camera.hpp
/// @brief		高速カメラ用の抽象クラス
/// @author		Satoshi Tabata
/// @date		2017/7/27 作成
/// @date		2019/3/4 最終更新
/// @version	1.2.1
///
/// @par version 1.2.0 by Satoshi Tabata
/// @par トリガ回り修正: トリガアウト追加
///
/// @par version 1.2.1 by Satoshi Tabata
/// @par タイムアウト時間を設定可能に変更(Basler, Ximeaで有効)
/// @par captureFrameでフレーム番号を返すように変更(Baslerで有効)
///

#include <iostream>
#include <cassert>
#include <type_traits>
#include <iomanip>

namespace paramTypeCamera
{
	enum class paramInt
	{
		WIDTH = 0,
		HEIGHT = 1,
		TIMEOUT = 2,
		//FPS = 2,
		//GAIN = 3,
	};

	enum class paramFloat
	{
		//WIDTH = 0,
		//HEIGHT = 1,
		FPS = 2,
		GAIN = 3,
	};
}

namespace paramTypeIdpExpress
{
	enum class AcquisitionMode
	{
		EnableAcquisitionFrameRate = 0,
		TriggerMode = 1
	};

	enum class CameraHead
	{
		CameraHead1 = 0,
		CameraHead2 = 1,
	};
};


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
		TriggerMode = 1,
	};
	enum class CaptureType
	{
		ColorGrab = 0,
		MonocroGrab = 1,
		BayerGrab = 2,
		Monocro10Grab = 3,
		ColorBGRGrab = 4,

		BayerBGGrab = 5,
		BayerGBGrab = 6,
		BayerRGGrab = 7,
		BayerGRGrab = 8,
	};
	enum class GrabStrategy
	{
		OneByOne = 0,
		LatestOnlyFrame = 1
	};

/// ver 1.2.0 追加
	enum class TriggerLineSelector
	{
		Line3 = 0,
		Line4 = 1,
	};
	enum class TriggerLineSource
	{
		ExposureActive = 0,
		Timer1Active = 1,
	};
	enum class ImageReverse
	{
		None = 0,
		ReverseX = 1,
		ReverseY = 2,
		ReverseXandY = 3,
	};
}

namespace paramTypeXimea
{
	enum class Param
	{
		ExposureTime = 0,
	};
	enum class CaptureType
	{
		ColorGrab = 0,
		MonocroGrab = 1,
		BayerGrab = 2
	};
	enum class AcquisitionMode
	{
		EnableAcquisitionFrameRate = 0,
		TriggerMode = 1
	};
}

namespace paramTypeEosens
{
	enum class AcquisitionMode
	{
		EnableAcquisitionFrameRate = 0,
		TriggerMode = 1
	};

	enum class paramInt
	{
		EXPOSURE = 2,
		MODE = 3,

	};

	enum class paramFloat
	{
		GAMMA = 5
	};
};


class Camera
{
public:
protected:
	/// 初期化. カメラ台数確認等
	Camera() = delete;
	Camera(int width, int height, int fps) : width(width), height(height), fps(fps), timeout(5000) {}
	int width;
	int height;
	float fps;
	float gain;
	int timeout; /// Basler, XimeaにおけるcaptureFrameのタイムアウト時間

public:
	virtual ~Camera() {}

	Camera(const Camera &) = delete;
	Camera &operator=(const Camera &) = delete;

	/// 個別カメラ接続?
	virtual bool connect(int id) = 0;
	/// カメラ終了処理
	virtual void disconnect() = 0;

	/// パラメタ設定後 撮像開始 or 撮像待機
	virtual void start() = 0;
	/// 撮像終了(あれば)
	virtual void stop() = 0;

	/// パラメタの設定
	//virtual void setParam(const paramType &pT, void* param) = 0;
	virtual void setParam(const paramTypeCamera::paramInt &pT, const int param) = 0;
	virtual void setParam(const paramTypeCamera::paramFloat &pT, const float param) = 0;
	
	/// パラメタの取得
	//virtual void getParam(const paramType &pT, void* param) = 0;
	virtual int getParam(const paramTypeCamera::paramInt &pT) = 0;
	virtual float getParam(const paramTypeCamera::paramFloat &pT) = 0;

	/// 画像の取得(単眼)
	/// 2019/3/4 可能なら画像インデックスを返すように変更: 成功: インデックス(>=0), 失敗: -1, 未実装: -2
	virtual int captureFrame(void* data) = 0;
	//virtual void captureFrame(void* data) = 0;
	//virtual int captureFrameWithIndex(void* data) { std::cerr << "Not Implemented." << std::endl; return -1; };
	/// 画像の取得(ステレオ) : Photron 用
	virtual void captureFrameStereo(void* dataL, void* dataR) { std::cerr << "Not Implemented." << std::endl; };

	/// idpExpress用
	virtual void setParam(const paramTypeIdpExpress::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeIdpExpress::CameraHead &pT) {};

	/// Basler用
	virtual void setParam(const paramTypeBasler::Param &pT, const float param) {};
	virtual void setParam(const paramTypeBasler::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeBasler::FastMode &pT) {};
	virtual void setParam(const paramTypeBasler::CaptureType &pT) {};
	virtual void setParam(const paramTypeBasler::GrabStrategy &pT) {};
	virtual float getParam(const paramTypeBasler::Param &pT){ return 0; };
	virtual void parameter_all_print(){};
	///  ver.1.2.0 追加
	virtual void setParam(const paramTypeBasler::TriggerLineSelector &pT, const paramTypeBasler::TriggerLineSource &pTs, const bool invert) {};
	virtual void setParam(const paramTypeBasler::AcquisitionMode &pT, const paramTypeBasler::TriggerLineSelector &pTLine) {};
	virtual void setParam(const paramTypeBasler::ImageReverse &pT) {};

	/// Ximea用
	virtual void setParam(const paramTypeXimea::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeXimea::CaptureType &pT) {};
	virtual void setParam(const paramTypeXimea::Param &pT, const  int param) {};
	virtual int getParam(const paramTypeXimea::Param &pT) { return 0; };

	/// Eosens用
	virtual void setParam(const paramTypeEosens::paramInt &pT, const  int param) {};
	virtual void setParam(const paramTypeEosens::paramFloat &pT, const  float param) {};
	virtual int getParam(const paramTypeEosens::paramInt &pT) { return 0; };
	virtual float getParam(const paramTypeEosens::paramFloat &pT) { return 0; };

};
