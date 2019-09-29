#pragma once

#include "common.h"
#include "GLFWkit.h"
#include "GLkit.h"
#include <thread>
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


const int search_cxy[4][2] = {
	{ 324 - 128, 237 + 120 },
	{ 324 + 128, 237 + 120 },
	{ 324 + 128, 237 - 50 },
	{ 324 - 128, 237 - 50 },
};
/*
const int search_cxy[4][2] = {
	{ 324 - 128, (237 + 50)/2 },
	{ 324 + 128, (237 + 50)/2 },
	{ 324 + 128, 64 },
	{ 324 - 128, 64 },
};
*/

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



void getHomography(cv::Mat img, float* h, bool read = false) {
	cv::Mat img_calib;
	cv::Mat img_bin;
	cv::Mat img_masked;
	mouseParam mouse;

	if (read) {
		FILE* fp = fopen("Homography.txt", "r");
		for (int i = 0; i < 9; i++) {
			fscanf(fp, "%f\n", &h[i]);
		}
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

		cv::Mat H = cv::findHomography(src, dst);
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

void correctHomography(float* h) {

	vector<cv::Point> target;
	target.push_back(cv::Point(0, 0));
	target.push_back(cv::Point(299, 0));
	target.push_back(cv::Point(0, 299));
	target.push_back(cv::Point(299, 299));

	cv::Size imSize = cv::Size(300, 300);

	cv::Mat check1 = cv::imread("check0.png", 0);
	cv::Mat check2 = cv::imread("check1.png", 0);


	vector<cv::Point> source;
	mouseParam mouse;
	cv::namedWindow("Homography");
	cv::setMouseCallback("Homography", mouseCallBack, &mouse);

	cv::Mat displaysrc;
	cv::cvtColor(img_ximea0_cam, displaysrc, cv::COLOR_GRAY2RGB);
	cv::imshow("Homography", displaysrc);
	for (int i = 0; i < 4; i++) {
		mouse.event = NULL;
		while (mouse.event != cv::EVENT_LBUTTONDOWN) {
			cv::waitKey(1);
		}
		mouse.event = NULL;
		int cx = mouse.x;
		int cy = mouse.y;
		source.push_back(cv::Point(cx, cy));

		cv::circle(displaysrc, cv::Point(cx, cy), 10, cv::Scalar(200, 0, 0), 1);

		cv::imshow("Homography", displaysrc);
		cv::waitKey(33);
	}
	cv::destroyWindow("Homography");


	cv::Mat homography0 = cv::findHomography(source, target);
	cv::Mat lastHomography;
	double maxscore = 0.0;
	int windowSize = 2;
	for (int x0 = -windowSize; x0 < windowSize; x0++) {
		for (int y0 = -windowSize; y0 < windowSize; y0++) {
			for (int x1 = -windowSize; x1 < windowSize; x1++) {
				for (int y1 = -windowSize; y1 < windowSize; y1++) {
					for (int x2 = -windowSize; x2 < windowSize; x2++) {
						for (int y2 = -windowSize; y2 < windowSize; y2++) {
							for (int x3 = -windowSize; x3 < windowSize; x3++) {
								for (int y3 = -windowSize; y3 < windowSize; y3++) {
									homography_proj[0][0] += x0;
									homography_proj[0][1] += y0;
									homography_proj[1][0] += x1;
									homography_proj[1][1] += y1;
									homography_proj[2][0] += x2;
									homography_proj[2][1] += y2;
									homography_proj[3][0] += x3;
									homography_proj[3][1] += y3;
									std::vector<cv::Point2f> src;
									std::vector<cv::Point2f> dst;
									for (int i = 0; i < 4; i++) {
										src.push_back(cv::Point2f(homography_cam[i][0], homography_cam[i][1]));
										dst.push_back(cv::Point2f(homography_proj[i][0], homography_proj[i][1]));
									}

									cv::Mat H = cv::findHomography(src, dst);
									for (int i = 0; i < 3; i++) {
										for (int j = 0; j < 3; j++) {
											h[i * 3 + j] = H.at<double>(i, j);
										}
									}
									cv::Mat d;
									img_ximea0_cam.copyTo(d);
									cv::warpPerspective(d, d, homography0, imSize);
									cv::equalizeHist(d, d);
									double score = Mrate(d, check1, check2);
									if (score > maxscore) {
										cout << score << endl;
										maxscore = score;
										H.copyTo(lastHomography);
									}

									homography_proj[0][0] -= x0;
									homography_proj[0][1] -= y0;
									homography_proj[1][0] -= x1;
									homography_proj[1][1] -= y1;
									homography_proj[2][0] -= x2;
									homography_proj[2][1] -= y2;
									homography_proj[3][0] -= x3;
									homography_proj[3][1] -= y3;
									int key = cv::waitKey(1);
									if (key == 'e') {
										goto Finish;
									}

								}
							}
						}
					}
				}
			}
		}
	}

Finish:

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			h[i * 3 + j] = lastHomography.at<double>(i, j);
		}
	}


	FILE* fp = fopen("Homography.txt", "w");
	for (int i = 0; i < 9; i++) {
		fprintf(fp, "%f\n", h[i]);
	}

}

inline void cvtHomography(float* input, float* output, float* h) {
	float p[3];
	for (int i = 0; i < 4; i++) {
		p[0] = h[0] * input[i * 3 + 0] + h[1] * input[i * 3 + 1] + h[2];
		p[1] = h[3] * input[i * 3 + 0] + h[4] * input[i * 3 + 1] + h[5];
		p[2] = h[6] * input[i * 3 + 0] + h[7] * input[i * 3 + 1] + h[8];

		output[i * 3 + 0] = (p[0] / p[2] - 512.0f) * 0.001302083f;
		output[i * 3 + 1] = (p[1] / p[2] - 384.0f) * 0.001302083f;

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

void OnVelocityAccelChanged(int param, void*) {
	velocityAccel = (float)param / 100.0f;
}
void OnLowpassChanged(int param, void*) {
	lowpassParam = (float)param / 100.0f;
}
void lowpassFilter(float* nowY, float* preY, float* x) {
	for (int i = 0; i < 4; i++) {
		nowY[3 * i + 0] = lowpassParam * preY[3 * i + 0] + (1.0f - lowpassParam) * x[3 * i + 0];
		nowY[3 * i + 1] = lowpassParam * preY[3 * i + 1] + (1.0f - lowpassParam) * x[3 * i + 1];
	}
}


inline void mainloop() {

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


#pragma endregion


#pragma region Ximea Open
	Ximea ximeaCam0, ximeaCam1;
	int ximea_cam_width = 648;
	int ximea_cam_height = 488;

	ximeaCam0.connect(0);

	ximeaCam0.setParam(paramTypeCamera::paramInt::WIDTH, ximea_cam_width);
	ximeaCam0.setParam(paramTypeCamera::paramInt::HEIGHT, ximea_cam_height);
	ximeaCam0.setParam(paramTypeCamera::paramFloat::FPS, 100.0f);
	ximeaCam0.setParam(paramTypeCamera::paramFloat::GAIN, 12.0f);
#ifdef PROJECTOR
	ximeaCam0.setParam(paramTypeXimea::AcquisitionMode::TriggerMode);
#else
	ximeaCam0.setParam(paramTypeXimea::AcquisitionMode::EnableAcquisitionFrameRate);
#endif

	ximeaCam0.start();

	cv::Mat img_ximea0_cam = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255));
	cv::Mat img_ximea0_cam_display = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC3);


	ximeaCam1.connect(1);

	ximeaCam1.setParam(paramTypeCamera::paramInt::WIDTH, ximea_cam_width);
	ximeaCam1.setParam(paramTypeCamera::paramInt::HEIGHT, ximea_cam_height);
	ximeaCam1.setParam(paramTypeCamera::paramFloat::FPS, 100.0f);
	ximeaCam1.setParam(paramTypeCamera::paramFloat::GAIN, 12.0f);
#ifdef PROJECTOR
	ximeaCam1.setParam(paramTypeXimea::AcquisitionMode::TriggerMode);
#else
	ximeaCam1.setParam(paramTypeXimea::AcquisitionMode::EnableAcquisitionFrameRate);
#endif

	ximeaCam1.start();

	cv::Mat img_ximea1_cam = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255));
	cv::Mat img_ximea1_cam_display = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC3);

	cv::Mat img_ximea0_static = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255));
	cv::Mat img_ximea1_static = cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255));

	vector<cv::Mat> static_pic0;
	vector<cv::Mat> static_pic1;

	for (int i = 0; i < 10; i++) {
		static_pic0.push_back(cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255)));
		static_pic1.push_back(cv::Mat(ximea_cam_height, ximea_cam_width, CV_8UC1, cv::Scalar::all(255)));
	}
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
	proj.set_projection_mode(PM::FLIP);
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

	std::thread thread_capture_ximea([&] {
		while (thread_capture_ximea_flag) {
#ifdef CAMERA
			ximeaCam0.captureFrame(img_ximea0_cam.data);

			img_ximea0_cam.copyTo(img_ximea0_cam_display);
			ximeaCam1.captureFrame(img_ximea1_cam.data);

			img_ximea1_cam.copyTo(img_ximea1_cam_display);
#endif

			float d = abs(center[0] - static_center[0]);
			float dl = abs(v);


			if (d <= epsilon) {
				//ut << d << endl;


				if (mode == STATIC && captureflag) {
					double p1 = getPSNR(img_ximea0_static, img_ximea0_cam);
					double p2 = getPSNR(img_ximea1_static, img_ximea1_cam);
					printf("PSNR: cam1:%3.1f cam2:%3.1f \n", p1, p2);
					string pass0, pass1;
					if (texid == 0) {
						pass0 = directory + "/Static/cam0/correct/" + to_string(static_psnr.size()) + ".png";
						pass1 = directory + "/Static/cam1/correct/" + to_string(static_psnr.size()) + ".png";
					}
					else {
						pass0 = directory + "/Static/cam0/inverse/" + to_string(static_psnr.size()) + ".png";
						pass1 = directory + "/Static/cam1/inverse/" + to_string(static_psnr.size()) + ".png";
					}


					cv::imwrite(pass0, img_ximea0_cam);
					cv::imwrite(pass1, img_ximea1_cam);

					static_psnr.push_back((p1 + p2) / 2.0);


					std::cout << "static count:" << static_psnr.size() << std::endl;
					if (static_psnr.size() <= 10) {
						img_ximea0_cam.copyTo(static_pic0[static_psnr.size() - 1]);
						img_ximea1_cam.copyTo(static_pic1[static_psnr.size() - 1]);
					}
					printf("captured\n");
					captureflag = false;


				}
				else if (mode == MOVE && movecapture) {
					if (dl >= 1.5f && dl <= maxV) {
						string pass0 = directory + "/Moving/cam0/" + to_string(move_psnr.size()) + ".png";
						string pass1 = directory + "/Moving/cam1/" + to_string(move_psnr.size()) + ".png";

						cv::imwrite(pass0, img_ximea0_cam);
						cv::imwrite(pass1, img_ximea1_cam);
						double p1 = 0.0;
						double p2 = 0.0;
						printf("PSNR: cam1:%3.1f cam2:%3.1f \n", p1, p2);
						move_psnr.push_back((p1 + p2) / 2.0);

						info.datafile << dl << "," << endl;

						std::cout << "velocity: " << dl << std::endl;
						printf("captured\n");
						printf("Epoch:%d\n\n", move_psnr.size());
						captureflag = false;
						movecapture = false;

					}
				}

			}

			thread_capture_ximea_cnt++;
			SetEvent(event_capture_ximea);
		}
		});
	thread_capture_ximea.detach();





	std::thread thread_process([&] {
		GLFWkit glfwkit("glfwkit", proj_width, proj_height);
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
		cv::Mat F = cv::imread("offset.png", 0);
		cv::Mat src1 = cv::imread("purin.jpg", 0);
		cv::flip(src1, src1, -1);
		cv::flip(F, F, -1);
		cv::Mat dst;
		src1.copyTo(dst);
		cv::Point2f centers;

		cv::Mat src2;
		int count = 0;

		springSimulate spring;
		vector<float> k;
		k.push_back(0.8f);
		k.push_back(1.5f);
		k.push_back(1.5f);
		k.push_back(0.8f);

		spring.Init(corner_xyz_cam, k, 30.0f);
		

		while (thread_process_flag) {
			WaitForSingleObject(event_capture, INFINITE);
			//calib.Calibrate(img_cam, img_cam_undistorted);
			getCorners(img_cam, img_display_cam, corner_xyz_cam, corner_detected, center, pre_center, ppre_center);
			//lowpassFilter(corner_xyz_cam_filtered, pre_corner_xyz_cam_filtered, corner_xyz_cam);

			spring.simulate(corner_xyz_cam, 0.033);
			spring.getIpoints(corner_xyz_cam_i);
			calib.UndistortPerPoint(corner_xyz_cam, corner_xyz_undistorted, corner_detected);
			//std::cout << corner_xyz_undistorted[1] << std::endl;
			cvtHomography(corner_xyz_undistorted, corner_xyz_proj, homography);

			vector<cv::Point2f> target;
		

			cv::Size imSize = cv::Size(400, 400);


			vector<cv::Point2f> source;
			for (int i = 0; i < 4; i++) {
				target.push_back(cv::Point2f(corner_xyz_cam[3 * i + 0]+ (corner_xyz_cam_i[3*i+0]-corner_xyz_cam[3*i+0]) / 8.0f, corner_xyz_cam[3 * i + 1]+ (corner_xyz_cam_i[3 * i + 1] - corner_xyz_cam[3 * i + 1]) / 8.0f));
				source.push_back(cv::Point2f(corner_xyz_cam[3 * i + 0], corner_xyz_cam[3 * i + 1]));
			}
			cv::Mat homography0 = cv::findHomography(source, target);
			//centers.x = src1.rows / 2.0f +(corner_xyz_cam[0]-corner_xyz_cam_i[0])/3.0f;
			//centers.y = src1.cols / 2.0f+(corner_xyz_cam[1]-corner_xyz_cam_i[1])/3.0f;
			//cv::getRectSubPix(src1, src1.size(), centers, src2);
			cv::warpPerspective(src1, src2, homography0, imSize);
			diffImage(src1, src2, dst, F, 100.0);


			//High-speed Rendering without GLFW GUI
			glfwkit.render(corner_xyz_proj, &img_render, dst);
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

#pragma region Parameter bar
	cv::namedWindow("parameters");
	int velocityValue = 60;
	cv::createTrackbar("velocityAccel", "parameters", &velocityValue, 300, OnVelocityAccelChanged);
	cv::setTrackbarPos("velocityAccel", "parameters", 60);
	int lowpassValue = 0;
	cv::createTrackbar("lowpass", "parameters", &lowpassValue, 100, OnLowpassChanged);
	cv::setTrackbarPos("lowpass", "parameters", 0);
	cv::createTrackbar("threshold", "parameters", &threshold, 255);
	cv::setTrackbarPos("threshold", "parameters", 230);
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
		else if (key == 's') {
			cv::imwrite("cam0_move.png", img_ximea0_cam);
			cv::imwrite("cam1_move.png", img_ximea1_cam);
			//PSNR.writeData((float)proj_fps, static_psnr, move_psnr);

			cout << "saved" << endl;
		}
		else if (key == 'c') {
			static_center[0] = center[0];
			static_center[1] = center[1];
			printf("static_center x:%3.1f y:%3.1f\n\n", static_center[0], static_center[1]);
			info.datafile << "static center pos," << static_center[0] << "," << static_center[1] << "," << endl;
			img_ximea0_cam.copyTo(img_ximea0_static);
			img_ximea1_cam.copyTo(img_ximea1_static);
			circumstance = 0.0f;
		}
		else if (key == 'p') {
			printf("prepared capture...\n");
			captureflag = true;

		}
		else if (key == 'f') {
			printf("canceled capture...\n");
			captureflag = false;
		}
		else if (key == 'm') {
			mode = MOVE;
			printf("mode:move\n");
			info.datafile << "velocity,psnr" << endl;
		}
		else if (key == 't') {
			mode = STATIC;
			printf("mode:static\n\n");
			if (texid == 0) {
				texid = 1;
			}
			else if (texid == 1) {
				texid = 0;
			}
			printf("texid:%d\n\n", texid);
		}
		img_render.copyTo(img_display_proj);
		//cv::imshow("img_display_cam", img_display_cam);
		cv::imshow("img_display_cam", img_display_cam);
		cv::imshow("img_display_proj", img_display_proj);
		cv::imshow("img_display_ximea0", img_ximea0_cam_display);
		cv::imshow("img_display_ximea1", img_ximea1_cam_display);
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

	ximeaCam0.stop();
	ximeaCam0.disconnect();
	ximeaCam1.stop();
	ximeaCam1.disconnect();
#endif
#pragma endregion 

#ifdef PROJECTOR
#pragma region HSP Close
	proj.destruct_process_light();
#pragma endregion
#endif


	info.save();
}

