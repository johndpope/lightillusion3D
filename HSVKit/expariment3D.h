#pragma once

#include "common.h"
#include "GLFWkit.h"
#include "GLkit.h"
#include <thread>
#include<glm/glm.hpp>
//#include<iostream>


//todo 直行する画像
//位置合わせ



#define PROJECTOR
#define CAMERA
const int track_r = 10;
const int search_r = 64;

const float epsilon = 0.2;

float circumstance = 0.0f;


float velocityAccel = 5.585f;
float lowpassParam = 0.0f;


int proj_fps = 1000;
float maxV = 40.0f;
const string directory = "Expariment/Velocity_gausian_Accel_169_100e";

#define STATIC 0
#define MOVE 1

/*
const int search_cxy[4][2] = {
	{ 324 - 128, 237 + 120 },
	{ 324 + 128, 237 + 120 },
	{ 324 + 128, 237 - 50 },
	{ 324 - 128, 237 - 50 },
};
*/
const int search_cxy[4][2] = {
	{ 324 - 128, 237 + 100 },
	{ 324 + 128, 237 + 100 },
	{ 324 + 128, 237 - 130 },
	{ 324 - 128, 237 - 130 },
};

int threshold = 230;
const uint min_m00 = 10;
int mspf = 1;

float homography_proj[4][2] = {
	{ 512 - 200, 384 + 200 },
	{ 512 + 200, 384 + 200 },
	{ 512 + 200, 384 - 200 },
	{ 512 - 200, 384 - 200 },
};

int cnt = 0;

const string intrinstFile = "intrinsics.xml";

float homography_cam[4][2];

cv::Mat img_ximea0_cam;

cv::Mat H;

void getHomography(cv::Mat img, float* h, bool read = false) {
	cv::Mat img_calib;
	cv::Mat img_bin;
	cv::Mat img_masked;
	mouseParam mouse;
	H = cv::Mat(3, 3, CV_64FC1);

	if (read) {
		FILE* fp = fopen("Homography.txt", "r");
		for (int i = 0; i < 9; i++) {
			fscanf(fp, "%f\n", &h[i]);
			
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				H.at<double>(i, j) = (double)h[i * 3 + j];
				//cout << h[i * 3 + j] << endl;
			}
		}
		//cout << H << endl;
		fclose(fp);
	}
	else {
		img.copyTo(img_calib);
		cv::threshold(img_calib, img_bin, threshold, 255, cv::THRESH_BINARY);
		cv::bitwise_and(img_calib, img_bin, img_masked);

		int key = 0;

		cv::namedWindow("Homography");
		cv::setMouseCallback("Homography", mouseCallBack, &mouse);
		for (int i = 0; i < 4; i++) {
			mouse.event = NULL;
			while (mouse.event != cv::EVENT_LBUTTONDOWN) {
				cv::imshow("Homography", img_calib);
				key = cv::waitKeyEx(33);
				switch (key) {
				case 2424832://left
					homography_proj[i][0] += 1.0;
					break;
				case 2490368://up
					homography_proj[i][1] += 1.0;
					break;
				case 2555904://right
					homography_proj[i][0] -= 1.0;
					break;
				case 2621440://down
					homography_proj[i][1] -= 1.0;
					break;
				case 'a'://left
					homography_proj[i][0] += 10.0;
					break;
				case 'w'://up
					homography_proj[i][1] += 10.0;
					break;
				case 's'://right
					homography_proj[i][0] -= 10.0;
					break;
				case 'z'://down
					homography_proj[i][1] -= 10.0;
					break;
				case 'q':
					cv::destroyWindow("Homography");
					return;
				}
			}
			mouse.event = NULL;
			int cx = mouse.x;
			int cy = mouse.y;
			cv::Rect rect;
			cv::Mat img_roi;
			cv::Moments moments;
			rect.x = std::max(0, cx - track_r);
			rect.y = std::max(0, cy - track_r);
			rect.width = std::min(track_r + img_calib.cols - cx, std::min(track_r + cx + 1, track_r * 2 + 1));
			rect.height = std::min(track_r + img_calib.rows - cy, std::min(track_r + cy + 1, track_r * 2 + 1));
			img_roi = img_masked(rect);
			moments = cv::moments(img_roi, false);
			if (moments.m00 < min_m00) {
				i--;
				printf("NOT detected\n");
				continue;
			}
			else {
				homography_cam[i][0] = (moments.m10 / moments.m00) + rect.x;
				homography_cam[i][1] = (moments.m01 / moments.m00) + rect.y;
				cv::rectangle(img_calib, rect, cv::Scalar(0));
				cv::circle(img_calib, cv::Point2i((int)homography_cam[i][0], (int)homography_cam[i][1]), 2, cv::Scalar(0), -1);
				cv::circle(img_calib, cv::Point2i((int)homography_cam[i][0], (int)homography_cam[i][1]), 1, cv::Scalar(255), -1);
				cv::imshow("Homography", img_calib);
				cv::waitKey(33);
				printf("%d Detected (%f, %f)\n", i, homography_cam[i][0], homography_cam[i][1]);
			}
		}
		cv::destroyWindow("Homography");

		std::vector<cv::Point2f> src;
		std::vector<cv::Point2f> dst;
		for (int i = 0; i < 4; i++) {
			src.push_back(cv::Point2f(homography_cam[i][0], homography_cam[i][1]));
			dst.push_back(cv::Point2f(homography_proj[i][0], homography_proj[i][1]));
		}

		H = cv::findHomography(src, dst);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				h[i * 3 + j] = H.at<double>(i, j);
			}
		}

		std::cout << H << std::endl;
		for (int i = 0; i < 3; i++) {
			printf("%f %f %f\n", h[i * 3 + 0], h[i * 3 + 1], h[i * 3 + 2]);
		}

		FILE* fp = fopen("Homography.txt", "w");
		for (int i = 0; i < 9; i++) {
			fprintf(fp, "%f\n", h[i]);
		}

		fprintf(fp, "\nINPUT(camera)\n");
		for (int i = 0; i < 4; i++) {
			fprintf(fp, "i=%d (x, y)=(%f, %f)\n", i, homography_cam[i][0], homography_cam[i][1]);
		}

		fprintf(fp, "\nOUTPUT(projector)\n");
		for (int i = 0; i < 4; i++) {
			fprintf(fp, "i=%d (X, Y)=(%f, %f)\n", i, homography_proj[i][0], homography_proj[i][1]);
		}
		fclose(fp);
	}
}

inline void cvtHomography(float* input, float* output, float* h) {
	float p[3];
	for (int i = 0; i < 4; i++) {
		p[0] = h[0] * input[i * 3 + 0] + h[1] * input[i * 3 + 1] + h[2];
		p[1] = h[3] * input[i * 3 + 0] + h[4] * input[i * 3 + 1] + h[5];
		p[2] = h[6] * input[i * 3 + 0] + h[7] * input[i * 3 + 1] + h[8];

		//output[i * 3 + 0] = (p[0] / p[2] - 512.0f) * 0.001302083f;
		//output[i * 3 + 1] = (p[1] / p[2] - 384.0f) * 0.001302083f;

		output[i * 3 + 0] = (p[0] / p[2]);
		output[i * 3 + 1] = (p[1] / p[2]);

	}

	//output[2] = -0.1f;
	//output[11] = -0.1f;
}


float v = 0;

void getCorners(cv::Mat& img, cv::Mat& img_display, float* corners, bool* flags, float* center, float* precenter, float* pprecenter) {
	cv::Mat img_bin;
	cv::Mat img_masked;
	cv::Mat img_roi;
	cv::Mat img_display_tmp;
	cv::Rect rect;
	cv::Moments moments;
	//cv::GaussianBlur(img, img, cv::Size(3, 3), 0, 0);
	cv::cvtColor(img, img_display_tmp, cv::COLOR_GRAY2BGR);



	cv::threshold(img, img_bin, threshold, 255, cv::THRESH_BINARY);
	cv::bitwise_and(img, img_bin, img_masked);
	//cv::cvtColor(img_masked, img_display_tmp, cv::COLOR_GRAY2BGR);
	cv::Point centerp = cv::Point(0, 0);
	pprecenter[0] = precenter[0];
	pprecenter[1] = precenter[1];
	precenter[0] = center[0];
	precenter[1] = center[1];
	center[0] = 0.0f;
	center[1] = 0.0f;
	for (int i = 0; i < 4; i++) {
		if (flags[i]) {
			int cx = (int)corners[i * 3 + 0];
			int cy = (int)corners[i * 3 + 1];

			float rx = std::max(0.0f, corners[i * 3 + 0] - (float)track_r);
			float ry = std::max(0.0f, corners[i * 3 + 1] - (float)track_r);
			rect.x = std::max(0, cx - track_r);
			rect.y = std::max(0, cy - track_r);
			rect.width = std::min(track_r + img.cols - cx, std::min(track_r + cx + 1, track_r * 2 + 1));
			rect.height = std::min(track_r + img.rows - cy, std::min(track_r + cy + 1, track_r * 2 + 1));
			cv::rectangle(img_display_tmp, rect, cv::Scalar(0, 0, 255));

			img_roi = img_masked(rect);
			cv::GaussianBlur(img_roi, img_roi, cv::Size(3, 3), 0.0);
			//img_roi = img_bin(rect);
			/*
			if (i == 0) {
				cv::imshow("img", img_roi);
				cv::waitKey(1);
			}
			*/
			moments = cv::moments(img_roi, true);
			if (moments.m00 < min_m00) {
				corners[i * 3 + 0] = search_cxy[i][0];
				corners[i * 3 + 1] = search_cxy[i][1];
				flags[i] = false;
			}
			else {
				corners[i * 3 + 0] = moments.m10 / moments.m00 + rect.x;
				corners[i * 3 + 1] = moments.m01 / moments.m00 + rect.y;
				flags[i] = true;
			}
		}
		else {
			rect.x = search_cxy[i][0] - search_r;
			rect.y = search_cxy[i][1] - search_r;
			rect.width = search_r * 2;
			rect.height = search_r * 2;
			cv::rectangle(img_display_tmp, rect, cv::Scalar(255, 0, 0));

			img_roi = img_masked(rect);
			//img_roi = img_bin(rect);
			moments = cv::moments(img_roi, false);
			if (moments.m00 < min_m00) {
				corners[i * 3 + 0] = search_cxy[i][0];
				corners[i * 3 + 1] = search_cxy[i][1];
				flags[i] = false;
				//continue;
			}
			else {
				corners[i * 3 + 0] = moments.m10 / moments.m00 + rect.x;
				corners[i * 3 + 1] = moments.m01 / moments.m00 + rect.y;
				flags[i] = true;
			}
		}
		centerp.x += corners[i * 3 + 0];
		centerp.y += corners[i * 3 + 1];

		center[0] += corners[i * 3 + 0];
		center[1] += corners[i * 3 + 1];

	}
	centerp.x /= 4;
	centerp.y /= 4;
	center[0] /= 4.0f;
	center[1] /= 4.0f;
	cv::circle(img_display_tmp, centerp, 15, cv::Scalar(200, 0, 0), 1);
	circumstance += center[0] - precenter[0];
	//v = center[0] - precenter[0];


	img_display_tmp.copyTo(img_display);
}



inline void mainloop() {

#pragma region Imread
	

#pragma endregion


#pragma region DATA

	int mode = STATIC;
	bool captureflag = false;
	vector<float> static_psnr;

	vector<float> move_psnr;

	dataStorage info(directory + "/data.csv");
	int texid = 0;
	bool movecapture = true;

#pragma endregion
#pragma region HSC Open

#ifdef CAMERA
	HSVMC1 cam;
	cam.connect(7);

	cam.start();
#endif
	int cam_width = 648;
	int cam_height = 474;
	//int cam_width = 648;
	//int cam_height = 226;
	//float cam_fps = 2000.0f;
	//float cam_gain = 12.0f;

	cv::Mat img_cam = cv::Mat(cam_height, cam_width, CV_8UC1, cv::Scalar::all(0));
	cv::Mat img_display_cam = cv::Mat(cam_height, cam_width, CV_8UC3);
	cv::Mat img_proj_cam = cv::Mat(cam_height,cam_width, CV_8UC1, cv::Scalar::all(255));
	cv::Mat img_display_proj_cam = cv::Mat(cam_height, cam_width, CV_8UC3);

#pragma endregion


#pragma region HSP Open

	int proj_width = 1024;
	int proj_height = 768;
	int proj_fps = 1000;
	int proj_num_buffer = 1;


	cv::Mat img_proj = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255));
	cv::Mat img_display_proj = cv::Mat(proj_height, proj_width, CV_8UC3);
	cv::Mat img_proj_offblack = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
	cv::Mat img_proj_homography = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
	bool proj_offblack_flag = false;
	bool proj_calibration_flag = false;
	float homography[9] = { 0, };
	getHomography(img_cam, homography, true);

	float center[2] = { 0, };
	float pre_center[2] = { 0, };
	float ppre_center[2] = { 0, };
	float static_center[2] = { 0, };

#ifdef PROJECTOR
	HighSpeedProjector proj;
	//proj.set_projection_mode(PM::FLIP);
	proj.set_projection_mode(PM::ILLUMINANCE_HIGH);
	proj.set_parameter_value(PP::FRAME_RATE, proj_fps);
	proj.set_parameter_value(PP::BUFFER_MAX_FRAME, proj_num_buffer);

	proj.connect_projector();




#pragma endregion 
#endif


#pragma region GLFW kit
	//cv::Mat img_render = (cv::Mat(proj_height, proj_width, CV_8UC3, cv::Scalar::all(255)));
	cv::Mat img_render = (cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255)));
#pragma endregion

#pragma region Calibration kit
	Calibration calib;
	calib.Load_matrix(intrinstFile, cam_width, cam_height);
#pragma endregion

#pragma region Thread Open
	HANDLE event_capture = CreateEvent(NULL, false, false, "Event_capture");
	int thread_capture_cnt = 0;
	bool thread_capture_flag = true;

	HANDLE event_capture_ximea = CreateEvent(NULL, false, false, "Event_capture_ximea");
	int thread_capture_ximea_cnt = 0;
	bool thread_capture_ximea_flag = true;

	HANDLE event_process = CreateEvent(NULL, false, false, "Event_process");
	int thread_process_cnt = 0;
	bool thread_process_flag = true;

	HANDLE event_project = CreateEvent(NULL, false, false, "Event_project");
	int thread_project_cnt = 0;
	bool thread_project_flag = true;

	bool circumstance_flag = true;

	bool thread_fps_flag = true;

	std::thread thread_capture([&] {
		while (thread_capture_flag) {
#ifdef CAMERA
			cam.captureFrame(img_cam.data);
#endif
			thread_capture_cnt++;
			SetEvent(event_capture);
		}
		});
	thread_capture.detach();






	std::thread thread_process([&] {
		cv::Mat KI = (cv::Mat_<double>(3, 3) << proj_width, 0, proj_width / 2.0, 0, proj_height, proj_height / 2.0, 0, 0, 1);

		cv::Mat K = (cv::Mat_<double>(3, 3) << cam_width, 0, cam_width / 2.0, 0, cam_height, cam_height / 2.0, 0, 0, 1);
		//GLFWkit glfwkit("glfwkit", proj_width, proj_height,KI.inv()*H*K,"horse.obj");
		GLFWkit glfwkit("glfwkit",cam_width, cam_height,KI.inv()*H*K,"horse.obj");
		glfwkit.setup();

		//GLkit glkit(proj_width, proj_height);
		//glkit.setup();

		//cv::Mat img_cam_undistorted;


		float corner_xyz_cam[12] = { 0, };
		float corner_xyz_cam_i[12] = { 0, };

		float corner_xyz_undistorted[12] = { 0, };
		float corner_xyz_proj[12] = { 0, };


		float corner_gl[12] = { 0, };
		bool corner_detected[4] = { false, };
		int cntn = 0;
		float pre_circumstance = 0.0f;
		float prex = 0.0;

		cv::Point2f centers;

		cv::Mat src2;
		int count = 0;

		cv::Mat dst = cv::Mat(1024, 1024, CV_8UC1, cv::Scalar(0.0f));


		while (thread_process_flag) {
			WaitForSingleObject(event_capture, INFINITE);
			//calib.Calibrate(img_cam, img_cam_undistorted);
			getCorners(img_cam, img_display_cam, corner_xyz_cam, corner_detected, center, pre_center, ppre_center);
			//lowpassFilter(corner_xyz_cam_filtered, pre_corner_xyz_cam_filtered, corner_xyz_cam);

			calib.UndistortPerPoint(corner_xyz_cam, corner_xyz_undistorted, corner_detected);
			//std::cout << corner_xyz_undistorted[1] << std::endl;
			cvtHomography(corner_xyz_undistorted, corner_xyz_proj, homography);

			//cv::imshow("render", dst);
			//cv::waitKey(1);
			//glfwkit.render(corner_xyz_cam, &img_render);
			glfwkit.render(corner_xyz_cam, &img_proj_cam);
			//cv::flip(img_proj_cam, img_proj_cam, 0);
			cv::warpPerspective(img_proj_cam, img_render, H, img_render.size());


			//High-speed Rendering without GLFW GUI

			//glkit.render(corner_xyz_proj, &img_render, texid);
			//Display-rate Rendering with GLFW GUI
			//glfwkit.update(corner_gl, &img_render);
			if (img_render.data != NULL) {
				img_render.copyTo(img_proj);
				//cv::cvtColor(img_render, img_proj, cv::COLOR_RGB2GRAY);

			}
			


			if (cntn % 60 == 0) {

				if (abs(circumstance) < 1.5f) {
					circumstance = 0.0f;
					pre_circumstance = 0.0f;
				}
				else if (abs(pre_circumstance - circumstance) < 0.7f) {
					pre_circumstance = 0.0f;
					circumstance = 0.0f;
				}
				else {
					pre_circumstance = circumstance;
				}
				cntn++;
				v = center[0] - prex;
				prex = center[0];
				movecapture = true;
			}
			else {
				circumstance += center[0] - pre_center[0];
				cntn++;
			}


			thread_process_cnt++;
			SetEvent(event_process);
		}



		});
	thread_process.detach();




	std::thread thread_project([&] {
		while (thread_project_flag) {
			WaitForSingleObject(event_process, INFINITE);
#ifdef PROJECTOR
			if (!proj_calibration_flag) {
				if (cnt == 0) {
					if (!proj_offblack_flag) {
						proj.send_image_8bit(img_proj.data);
					}
					else {
						proj.send_image_8bit(img_proj_offblack.data);
					}
				}
				cnt = ++cnt % mspf;
			}
			else {
				img_proj_homography = cv::Scalar(0);
				for (int i = 0; i < 4; i++) {
					cv::circle(img_proj_homography, cv::Point2f(homography_proj[i][0], homography_proj[i][1]), 3, cv::Scalar(255), -1);
				}
				proj.send_image_8bit(img_proj_homography.data);
			}
#endif
			thread_project_cnt++;
			SetEvent(event_project);
		}
		});
	thread_project.detach();



	std::thread thread_fps([&] {
		LARGE_INTEGER freq;
		LARGE_INTEGER start, end;
		double sec;

		QueryPerformanceFrequency(&freq);

		while (thread_fps_flag) {
			thread_capture_cnt = 0;
			thread_process_cnt = 0;
			thread_project_cnt = 0;
			thread_capture_ximea_cnt = 0;

			QueryPerformanceCounter(&start);
			Sleep(2000);
			QueryPerformanceCounter(&end);

			sec = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

			if (!proj_calibration_flag) {

				printf("thread_capture %3.1f[fps]\n", thread_capture_cnt / sec);
				printf("thread_process %3.1f[fps]\n", thread_process_cnt / sec);
				printf("thread_project %3.1f[fps]\n", thread_project_cnt / sec);
				printf("thread_capture_ximea %3.1f[fps]\n", thread_capture_ximea_cnt / sec);

				printf("\n");

			}
		}
		});
	thread_fps.detach();
#pragma endregion 



#pragma region Main thread
	//dataStorage PSNR(directory+"/PSNR.csv");


	int key = 0;
	while ((key = cv::waitKey(1)) != 'q') {
		if (key == 'h') {
			proj_calibration_flag = true;
#ifdef CAMERA
			cam.captureFrame(img_cam.data);
			//ここほんとに必要？
			calib.Calibrate(img_cam, img_cam);
#endif
			getHomography(img_cam, homography);
			proj_calibration_flag = false;
		}
		else if (key == 'b') {
			proj_offblack_flag = !proj_offblack_flag;
		}
		else if (key == 'n') {
			switch (mspf) {
			case 1:
				mspf = 100; break;
			case 33:
				mspf = 1; break;
			case 100:
				mspf = 33; break;
			}
		}
		img_render.copyTo(img_display_proj);
		//img_proj_cam.copyTo(img_display_proj_cam);
		//cv::imshow("img_display_cam", img_display_cam);
		cv::imshow("img_display_cam", img_display_cam);
		cv::imshow("img_display_proj", img_display_proj);
	//	cv::imshow("img_display_proj_cam", img_display_proj_cam);

		if (abs(circumstance) > 1.3f) {
			//cout << circumstance << endl;
		}
		//cout << velocityAccel<<endl;

	}
	cv::destroyAllWindows();
#pragma endregion 

#pragma region Thread Close
	thread_capture_flag = false;

	thread_process_flag = false;

	thread_project_flag = false;

	thread_fps_flag = false;

	thread_capture_ximea_flag = false;

	circumstance_flag = false;
#pragma endregion 

#pragma region HSC Close
#ifdef CAMERA
	cam.stop();
	cam.disconnect();


#endif
#pragma endregion 

#ifdef PROJECTOR
#pragma region HSP Close
	proj.destruct_process_light();
#pragma endregion
#endif


	info.save();
}

