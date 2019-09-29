#pragma once
///
/// @file		Camera.hpp
/// @brief		�����J�����p�̒��ۃN���X
/// @author		Satoshi Tabata
/// @date		2017/7/27 �쐬
/// @date		2019/3/4 �ŏI�X�V
/// @version	1.2.1
///
/// @par version 1.2.0 by Satoshi Tabata
/// @par �g���K���C��: �g���K�A�E�g�ǉ�
///
/// @par version 1.2.1 by Satoshi Tabata
/// @par �^�C���A�E�g���Ԃ�ݒ�\�ɕύX(Basler, Ximea�ŗL��)
/// @par captureFrame�Ńt���[���ԍ���Ԃ��悤�ɕύX(Basler�ŗL��)
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

/// ver 1.2.0 �ǉ�
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
	/// ������. �J�����䐔�m�F��
	Camera() = delete;
	Camera(int width, int height, int fps) : width(width), height(height), fps(fps), timeout(5000) {}
	int width;
	int height;
	float fps;
	float gain;
	int timeout; /// Basler, Ximea�ɂ�����captureFrame�̃^�C���A�E�g����

public:
	virtual ~Camera() {}

	Camera(const Camera &) = delete;
	Camera &operator=(const Camera &) = delete;

	/// �ʃJ�����ڑ�?
	virtual bool connect(int id) = 0;
	/// �J�����I������
	virtual void disconnect() = 0;

	/// �p�����^�ݒ�� �B���J�n or �B���ҋ@
	virtual void start() = 0;
	/// �B���I��(�����)
	virtual void stop() = 0;

	/// �p�����^�̐ݒ�
	//virtual void setParam(const paramType &pT, void* param) = 0;
	virtual void setParam(const paramTypeCamera::paramInt &pT, const int param) = 0;
	virtual void setParam(const paramTypeCamera::paramFloat &pT, const float param) = 0;
	
	/// �p�����^�̎擾
	//virtual void getParam(const paramType &pT, void* param) = 0;
	virtual int getParam(const paramTypeCamera::paramInt &pT) = 0;
	virtual float getParam(const paramTypeCamera::paramFloat &pT) = 0;

	/// �摜�̎擾(�P��)
	/// 2019/3/4 �\�Ȃ�摜�C���f�b�N�X��Ԃ��悤�ɕύX: ����: �C���f�b�N�X(>=0), ���s: -1, ������: -2
	virtual int captureFrame(void* data) = 0;
	//virtual void captureFrame(void* data) = 0;
	//virtual int captureFrameWithIndex(void* data) { std::cerr << "Not Implemented." << std::endl; return -1; };
	/// �摜�̎擾(�X�e���I) : Photron �p
	virtual void captureFrameStereo(void* dataL, void* dataR) { std::cerr << "Not Implemented." << std::endl; };

	/// idpExpress�p
	virtual void setParam(const paramTypeIdpExpress::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeIdpExpress::CameraHead &pT) {};

	/// Basler�p
	virtual void setParam(const paramTypeBasler::Param &pT, const float param) {};
	virtual void setParam(const paramTypeBasler::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeBasler::FastMode &pT) {};
	virtual void setParam(const paramTypeBasler::CaptureType &pT) {};
	virtual void setParam(const paramTypeBasler::GrabStrategy &pT) {};
	virtual float getParam(const paramTypeBasler::Param &pT){ return 0; };
	virtual void parameter_all_print(){};
	///  ver.1.2.0 �ǉ�
	virtual void setParam(const paramTypeBasler::TriggerLineSelector &pT, const paramTypeBasler::TriggerLineSource &pTs, const bool invert) {};
	virtual void setParam(const paramTypeBasler::AcquisitionMode &pT, const paramTypeBasler::TriggerLineSelector &pTLine) {};
	virtual void setParam(const paramTypeBasler::ImageReverse &pT) {};

	/// Ximea�p
	virtual void setParam(const paramTypeXimea::AcquisitionMode &pT) {};
	virtual void setParam(const paramTypeXimea::CaptureType &pT) {};
	virtual void setParam(const paramTypeXimea::Param &pT, const  int param) {};
	virtual int getParam(const paramTypeXimea::Param &pT) { return 0; };

	/// Eosens�p
	virtual void setParam(const paramTypeEosens::paramInt &pT, const  int param) {};
	virtual void setParam(const paramTypeEosens::paramFloat &pT, const  float param) {};
	virtual int getParam(const paramTypeEosens::paramInt &pT) { return 0; };
	virtual float getParam(const paramTypeEosens::paramFloat &pT) { return 0; };

};
