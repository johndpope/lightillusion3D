#pragma once

#pragma region flags
#define CAMERA
#define PROJECTOR
#pragma endregion

#ifdef _DEBUG
#define SUFFIX_EXT "d.lib"
#else
#define SUFFIX_EXT ".lib"
#endif

//HSC
#include <HSC/Camera.hpp>
#include <HSC/CameraUI.hpp>
/*
//Basler
#include <HSC/baslerClass.hpp>
#pragma comment(lib, "BaslerLib" SUFFIX_EXT)
*/
//Eosens
#include <HSC/Eosens.hpp>
#pragma comment(lib, "EosensLib" SUFFIX_EXT)

//IdpExpress
#include <HSC/idpExpressClass.hpp>
#pragma comment(lib, "idpExpressLib" SUFFIX_EXT)

//Ximea
#include <HSC/Ximea.hpp>
//#pragma comment(lib, "XimeaLib" SUFFIX_EXT)



//HSP
#include <HSP/HighSpeedProjector.h>
#pragma comment(lib, "DynaFlash120" SUFFIX_EXT)

//HSV
#include "HSVMC1.h"


//Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

//OpenCV
#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#pragma comment(lib, "opencv_world401" SUFFIX_EXT)

//OpenGL

#pragma comment(lib, "opengl32.lib")

//GLFW
#define GLFW_DLL
#include <GLFW/gg.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3dll.lib")

#include<algorithm>

#include"library.h"

//CALIBLATION
#include "calibration.h"

#include "multiImage.h"

//EVALUATION
//#include"imageEvaluation.h"
#include"dataStorage.h"

#include<numeric>
#include"checkerMake.h"

#include"mouse.h"

#include"springSimulate.h"

#include"VertexArray.h"

#include<cuda_runtime.h>
#include<Windows.h>
#include<cuda.h>
#include <device_launch_parameters.h>]
