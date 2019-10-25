#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;

enum CaptureMode {
	LIGHT = 0,
	NOPROJ = 1,
	ONPROJ = 2,
};

void calibrate(int n_boards = 0, float image_sf = 0.5f, float delay = 1.f, int board_h = 0, int board_w = 0) {
	int board_n = board_w * board_h;
	cv::Size board_sz = cv::Size(board_w, board_h);

#pragma region HSC Open
	HSVMC1 cam;
	int cam_width = 648;
	int cam_height = 474;


	cv::Mat img_display_cam = cv::Mat(cam_height, cam_width, CV_8UC3);

	cam.connect(7);


	cam.start();
#pragma endregion



	vector<vector<cv::Point2f>> image_points;
	vector<vector<cv::Point3f>> object_points;

	double last_captured_timestamp = 0;
	cv::Size image_size=cv::Size(cam_width,cam_height);

	while (image_points.size() < (size_t)n_boards) {
		cv::Mat img_cam(cam_height, cam_width, CV_8UC1);
		cv::Mat image(cam_height, cam_width, CV_8UC3);

		cam.captureFrame(img_cam.data);

		cv::cvtColor(img_cam, img_cam, cv::COLOR_GRAY2RGB);
		if (img_cam.empty()) continue;
		//image_size = image.size();

		cv::resize(img_cam, image, cv::Size(), image_sf, image_sf, cv::INTER_LINEAR);

		vector<cv::Point2f> corners;
		bool found = cv::findChessboardCorners(image, board_sz, corners);

		cv::drawChessboardCorners(image, board_sz, corners, found);

		double timestamp = (double)clock() / CLOCKS_PER_SEC;

		if (found && timestamp - last_captured_timestamp > 1) {
			last_captured_timestamp = timestamp;
			image ^= cv::Scalar::all(255);

			cv::Mat mcorners(corners);
			mcorners *= 1. / image_sf;
			image_points.push_back(corners);
			object_points.push_back(vector<cv::Point3f>());
			vector<cv::Point3f>& opts = object_points.back();
			opts.resize(board_n);

			for (int j = 0; j < board_n; j++) {
				opts[j] = cv::Point3f((float)(j / board_w), (float)(j % board_w), 0.f);
			}

			cout << "Collected our " << (int)image_points.size() << " of " << n_boards << " needed chessboard images " << endl;

		}
		cv::imshow("Calibration", image);
		cv::waitKey(1);



	}

	cam.stop();
	cam.disconnect();


	cv::destroyWindow("Calibration");

	cout << object_points[0].size() << endl;
	
	cout << "\n\n*** CALIBLATING CAMERA...\n" << endl;

	cv::Mat intrinsic_matrix, cam_distortion;
	double err = cv::calibrateCamera(
		object_points,
		image_points,
		image_size,
		intrinsic_matrix,
		cam_distortion,
		cv::noArray(),
		cv::noArray(),
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

	cout << "*** DONE! \n\nReprojection error is " << err <<
		" \nStoring Intrinsics.xml \n\n";

	cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);

	fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
		intrinsic_matrix << "cam_distortion" << cam_distortion;
	fs.release();
}


class Calibration {

private:
	cv::Mat cam_matrix;
	cv::Mat cam_distortion;
	cv::Mat R;
	cv::Mat P;

	cv::Size image_size;

	cv::Mat map1;
	cv::Mat map2;

	cv::Mat cam_light;
	cv::Mat cam_noproj;
	cv::Mat cam_onproj;

	vector < vector < cv::Point3d>> objPoints;
	vector<vector<cv::Point2d>> imgPoints;


	int pic_count = 0;
	vector<cv::Point3d> boardPoints;
	int board_n;

	cv::Size board_sz;

	int proj_chess_w = 7;
	int proj_chess_h = 7;

	cv::Mat img_proj;

public:

	void Load_matrix(string fileName, int image_w, int image_h) {
		cv::FileStorage fs(fileName, cv::FileStorage::READ);
		fs["camera_matrix"] >> cam_matrix;
		fs["distortion_coefficients"] >> cam_distortion;

		fs.release();

		image_size = cv::Size(image_w, image_h);

		
		cv::initUndistortRectifyMap(
			cam_matrix,
			cam_distortion,
			cv::Mat(),
			cam_matrix,
			image_size,
			CV_16SC2,
			map1,
			map2);
		
	}

	void Undistort(cv::InputArray image_captured, cv::OutputArray image_calibrated) {
		if (!image_captured.empty()) {
			//cv::undistort(image_captured, image_calibrated, cam_matrix, cam_distortion);
			cv::remap(
				image_captured,
				image_calibrated,
				map1,
				map2,
				cv::INTER_LINEAR,
				cv::BORDER_CONSTANT,
				cv::Scalar()
			);

		}
	}


	//点単位で歪み補正を行う関数
	//fpsにもそんなに影響はなかった
	void UndistortPerPoint(float* corner_xyz_cam, float* corner_xyz_undistorted, bool *flags) {

		std::vector<cv::Point2f> points;
		std::vector<cv::Point2f> dst;
		for (int i = 0; i < 4; i++) {
			cv::Point2f point(corner_xyz_cam[3 * i + 0], corner_xyz_cam[3 * i + 1]);
			points.push_back(point);
		}

		cv::undistortPoints(points, dst, cam_matrix, cam_distortion,P=cam_matrix);
		for (int i = 0; i < 4; i++) {
			//if (flags[i]) {
				corner_xyz_undistorted[3 * i + 0] = dst[i].x;
				corner_xyz_undistorted[3 * i + 1] = dst[i].y;
			/*}
			else {
				corner_xyz_undistorted[3 * i + 0] = corner_xyz_cam[3 * i + 0];
				corner_xyz_undistorted[3 * i + 1] = corner_xyz_cam[3 * i + 1];
			}
			*/
		}
	}



	void corner_detect() {
		vector < cv::Point2d> corners;
		if (!cv::findChessboardCorners(cam_light, board_sz, corners)) {
			printf("Failed to find chessboardcorners in LIGHT...\n");
			return;
		}

		//平面式
		cv::Mat rvec, tvec;
		cv::solvePnP(boardPoints, corners, cam_matrix, cam_distortion, rvec, tvec);
		cv::Mat n = (cv::Mat_<double>(3, 1) << 0.0, 0.0, 1.0);
		cv::Mat R;
		cv::Rodrigues(rvec, R);
		n = R * n;


		cv::Mat proj_chess;
		cv::absdiff(cam_onproj, cam_noproj, proj_chess);

		cv::Size chess_sz = cv::Size(proj_chess_w, proj_chess_h);
		if (!cv::findChessboardCorners(proj_chess, chess_sz, corners)) {
			printf("Failed to find chessboardcorners in PROJ...\n");
			return;
		}

		cv::Mat cinv = cam_matrix.inv();
		vector<cv::Point3d> v;
		//平面逆投影
		for (int i = 0; i < corners.size(); i++) {
			cv::Mat tmp = (cv::Mat_<double>(3,1)<< corners[i].x, corners[i].y, 1.0f);
			tmp = cinv * tmp;

			double t = (n.dot(tvec)) / (n.dot(tmp));
			tmp = t * tmp;
			cv::Point3d x = cv::Point3d(tmp.at<double>(0), tmp.at<double>(1), tmp.at<double>(2));
			v.push_back(x);
		}

		if (!cv::findChessboardCorners(img_proj, chess_sz, corners)) {
			printf("Failed to find chessboardcorners in PROJIMG...\n");
			return;
		}
		objPoints.push_back(v);
		imgPoints.push_back(corners);
		pic_count++;

		printf("Succeed in %d corner detect!\n", pic_count);

	}

	/*
	1キーで姿勢推定用のキャプチャ
	2キーでoffblackのキャプチャ
	3キーでプロジェクターのキャプチャ
	プロジェクターとライトのオンオフに注意！
	*/
	void calibrate_projector(int board_h = 0, int board_w = 0,float board_size=0) {

		for (int j = 0; j < board_h; j++) {
			for (int i = 0; i < board_w; i++) {
				cv::Point3d tmp = cv::Point3d(-(float)i * board_size, (float)j * board_size, 0.0f);
				boardPoints.push_back(tmp);
			}
		}
		board_n = board_h * board_w;
		board_sz = cv::Size(board_w, board_h);

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
				cv::Mat img_proj_cam = cv::Mat(cam_height, cam_width, CV_8UC1, cv::Scalar::all(255));


				cam_light= cv::Mat(cam_height, cam_width, CV_8UC1, cv::Scalar::all(0));
				cam_noproj= cv::Mat(cam_height, cam_width, CV_8UC1, cv::Scalar::all(0));
				cam_onproj= cv::Mat(cam_height, cam_width, CV_8UC1, cv::Scalar::all(0));

		#pragma endregion


		#pragma region HSP Open

				int proj_width = 1024;
				int proj_height = 768;
				int proj_fps = 1000;
				int proj_num_buffer = 1;


				img_proj = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255));
				cv::Mat img_display_proj = cv::Mat(proj_height, proj_width, CV_8UC3);
				cv::Mat img_proj_offblack = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
				cv::Mat img_proj_homography = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
				bool proj_offblack_flag = false;
				bool proj_calibration_flag = false;
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



		#pragma region Thread Open
				HANDLE event_capture = CreateEvent(NULL, false, false, "Event_capture");
				int thread_capture_cnt = 0;
				bool thread_capture_flag = true;

				HANDLE event_process = CreateEvent(NULL, false, false, "Event_process");
				int thread_process_cnt = 0;
				bool thread_process_flag = true;

				HANDLE event_project = CreateEvent(NULL, false, false, "Event_project");
				int thread_project_cnt = 0;
				bool thread_project_flag = true;


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
					
					while (thread_process_flag) {
						WaitForSingleObject(event_capture, INFINITE);


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
								if (!proj_offblack_flag) {
									proj.send_image_8bit(img_proj.data);
								}
								else {
									proj.send_image_8bit(img_proj_offblack.data);
								}
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


						QueryPerformanceCounter(&start);
						Sleep(2000);
						QueryPerformanceCounter(&end);

						sec = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

						if (!proj_calibration_flag) {
							/*
							printf("thread_capture %3.1f[fps]\n", thread_capture_cnt / sec);
							printf("thread_process %3.1f[fps]\n", thread_process_cnt / sec);
							printf("thread_project %3.1f[fps]\n", thread_project_cnt / sec);

							printf("\n");
							*/
						}
					}
					});
				thread_fps.detach();
		#pragma endregion 

				
		#pragma region Main thread
				
				CaptureMode capMode = CaptureMode::LIGHT;
				printf("Preparing finish...\n");
				int key = 0;
				while ((key = cv::waitKey(1)) != 'q') {
					if (key == 'h') {
						proj_calibration_flag = true;
		#ifdef CAMERA
						cam.captureFrame(img_cam.data);
						//ここほんとに必要？
						calib.Calibrate(img_cam, img_cam);
		#endif
						proj_calibration_flag = false;
					}
					else if (key == 'b') {
						proj_offblack_flag = !proj_offblack_flag;
					}
					else if (key == '1') {
						if (capMode != CaptureMode::LIGHT) {
							printf("capMode is not LIGHT. Now, ");
							cout << capMode << endl;
						}
						else if (!proj_offblack_flag) {
							printf("please projector black flag on!\n");
						}
						else {
							img_cam.copyTo(cam_light);
							//Undistort(cam_light, cam_light);
							capMode = CaptureMode::NOPROJ;
							proj_offblack_flag = true;

							printf("LIGHT is captured. Next NOPROJ...\n");
						}
					}
					else if (key == '2') {
						if (capMode != CaptureMode::NOPROJ) {
							printf("capMode is not NOPROJ. Now, ");
							cout << capMode << endl;
						}
						else if (!proj_offblack_flag) {
							printf("please projector black flag on!\n");
						}
						else {
							img_cam.copyTo(cam_noproj);
							Undistort(cam_noproj,cam_noproj);
							capMode = CaptureMode::ONPROJ;
							proj_offblack_flag = false;
							printf("NOPROJ is captured. Next ONPROJ...\n");
						}
					}
					else if (key == '3') {
						if (capMode != CaptureMode::ONPROJ) {
							printf("capMode is not ONPROJ. Now, ");
							cout << capMode << endl;
						}
						else if (proj_offblack_flag) {
							printf("please projector black flag off!\n");
						}
						else {
							img_cam.copyTo(cam_onproj);
							Undistort(cam_onproj, cam_onproj);
							capMode = CaptureMode::LIGHT;
							proj_offblack_flag = true;
							printf("ONPROJ is captured. Next LIGHT...\n");

							corner_detect();
						}
					}
					
					img_render.copyTo(img_display_proj);
					//cv::cvtColor(img_render, img_display_proj, cv::COLOR_GRAY2RGB);
					//cv::imshow("img_display_cam", img_display_cam);
					//cv::circle(img_display_proj, cv::Point(486, 439), 10, cv::Scalar(200, 0, 0));
					cv::imshow("img_display_cam", img_display_cam);
					cv::imshow("img_display_proj", img_display_proj);
					//cv::imshow("img_display_proj_cam", img_display_proj_cam);


				}
				cv::destroyAllWindows();
		#pragma endregion 

		#pragma region calibration

				if (pic_count > 0) {
					cout << "\n\n*** CALIBLATING CAMERA...\n" << endl;

					cv::Mat intrinsic_matrix, cam_distortion;
					double err = cv::calibrateCamera(
						objPoints,
						imgPoints,
						cv::Size(proj_width, proj_height),
						intrinsic_matrix,
						cam_distortion,
						cv::noArray(),
						cv::noArray(),
						cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

					cout << "*** DONE! \n\nReprojection error is " << err <<
						" \nStoring Intrinsics.xml \n\n";

					cv::FileStorage fs("intrinsics_proj.xml", cv::FileStorage::WRITE);

					fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
						intrinsic_matrix << "cam_distortion" << cam_distortion;
					fs.release();
				}
		#pragma endregion 

		#pragma region Thread Close
				thread_capture_flag = false;

				thread_process_flag = false;

				thread_project_flag = false;

				thread_fps_flag = false;

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

	}


};