#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
using namespace std;



enum CaptureMode {
	LIGHT = 0,
	NOPROJ = 1,
	ONPROJ = 2,
};

struct CheckerBoard {
	int w;
	int h;
	int size;
	cv::Point center;
};

void checkerMake(int board_w, int board_h, int size, cv::Mat& dst) {
	int w = board_w * size;
	int h = board_h * size;
	dst = cv::Mat::zeros(h, w, CV_8UC1);

	for (int i = 0; i < board_w; i++) {
		for (int j = 0; j < board_h; j++) {
			for (int ii = 0; ii < size; ii++) {
				for (int jj = 0; jj < size; jj++) {
					if ((i + j) % 2 == 0) {
						dst.at<uchar>(j * size + jj, i * size + ii) = 255;

					}

				}
			}
		}
	}
}

void calibrate(int n_boards = 0, float image_sf = 0.5f, float delay = 1.f, int board_h = 0, int board_w = 0,int cam_width=648,int cam_height=474,int mode=7) {
	int board_n = board_w * board_h;
	cv::Size board_sz = cv::Size(board_w, board_h);

#pragma region HSC Open
	HSVMC1 cam;
	

	cv::Mat img_display_cam = cv::Mat(cam_height, cam_width, CV_8UC3);

	cam.connect(mode);


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

			std::cout << "Collected our " << (int)image_points.size() << " of " << n_boards << " needed chessboard images " << std::endl;

		}
		cv::imshow("Calibration", image);
		cv::waitKey(1);



	}

	cam.stop();
	cam.disconnect();


	cv::destroyWindow("Calibration");

	std::cout << object_points[0].size() << std::endl;
	
	std::cout << "\n\n*** CALIBLATING CAMERA...\n" << std::endl;

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

	std::cout << "*** DONE! \n\nReprojection error is " << err <<
		" \nStoring Intrinsics.xml \n\n";
	if (mode == 7) {
		cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);
		fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
			intrinsic_matrix << "cam_distortion" << cam_distortion;
		fs.release();
	}
	else {
		cv::FileStorage fs("intrinsics_large_cam.xml", cv::FileStorage::WRITE);
		fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
			intrinsic_matrix << "cam_distortion" << cam_distortion;
		fs.release();
	}
	
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

	vector < vector < cv::Point3f>> objPoints;
	vector<vector<cv::Point2f>> imgPoints;


	int pic_count = 0;
	vector<cv::Point3d> boardPoints;
	int board_n;

	cv::Size board_sz;

	int proj_chess_w = 7;
	int proj_chess_h = 7;


	CheckerBoard checkerBoard;
	int proj_width;
	int proj_height;
	cv::Mat img_render;

	bool thread_capture_flag = true;

	bool thread_process_flag = true;

	bool thread_project_flag = true;
	
	std::FILE* fp;
	int tvalue = 150;
public:

	Calibration() {
		checkerBoard.w = proj_chess_w;
		checkerBoard.h = proj_chess_h;
		checkerBoard.size = 30;
		checkerBoard.center = cv::Point(1024 / 2, 768 / 2);
	}

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

	bool checkFileExistence(const std::string& str)
	{
		std::ifstream ifs(str);
		return ifs.is_open();
	}

	static void onTrackbarChanged(int num, void* userdata) {

		CheckerBoard c = static_cast<Calibration*>(userdata)->checkerBoard;
		if (c.w * c.size > static_cast<Calibration*>(userdata)->proj_width || c.h * c.size > static_cast<Calibration*>(userdata)->proj_height) {
			printf("error! size is larger than winsize!\n");
			return;
		}
		cv::Mat d;


		static_cast<Calibration*>(userdata)->img_render = cv::Mat(static_cast<Calibration*>(userdata)->proj_height, static_cast<Calibration*>(userdata)->proj_width, CV_8UC1,cv::Scalar::all(0));
		checkerMake(c.w, c.h, c.size, d);
		c.center.x = min(max(c.center.x,c.w * c.size / 2+1), static_cast<Calibration*>(userdata)->proj_width - c.w * c.size / 2);
		c.center.y = min(max(c.center.y, c.h * c.size / 2+1), static_cast<Calibration*>(userdata)->proj_height - c.h * c.size / 2);
		//std::cout << c.center.x << " " << c.center.y << std::endl;
		cv::setTrackbarPos("中心x座標", "checkerBoard", c.center.x);
		cv::setTrackbarPos("中心y座標", "checkerBoard", c.center.y);

		static_cast<Calibration*>(userdata)->thread_capture_flag = false;
		static_cast<Calibration*>(userdata)->thread_process_flag = false;
		static_cast<Calibration*>(userdata)->thread_project_flag = false;
		
		d.copyTo(static_cast<Calibration*>(userdata)->img_render(cv::Rect(c.center.x - c.w * c.size / 2-1, c.center.y - c.h * c.size / 2-1, c.w * c.size, c.h * c.size)));
		cv::rectangle(static_cast<Calibration*>(userdata)->img_render, cv::Rect(c.center.x - c.w * c.size / 2 -10, c.center.y - c.h * c.size / 2 -10, c.w * c.size+20, c.h * c.size+20), cv::Scalar(255, 255, 255), 20);
		static_cast<Calibration*>(userdata)->thread_capture_flag = true;
		static_cast<Calibration*>(userdata)->thread_process_flag = true;
		static_cast<Calibration*>(userdata)->thread_project_flag = true;

		

	}

	virtual void corner_detect() {

		cv::Mat cam_noprojd,cam_onprojd;
		vector < cv::Point2f> corners;
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
		cv::Mat Rt = R.t();

		cv::Mat proj_chess;
		Undistort(cam_onproj, cam_onprojd);
		Undistort(cam_noproj, cam_noprojd);
		cv::absdiff(cam_onprojd, cam_noprojd, proj_chess);
		//cv::equalizeHist(cam_onprojd, cam_onprojd);
		//cv::threshold(cam_onprojd, cam_onprojd, tvalue, 250, cv::THRESH_BINARY);
		//cv::bitwise_not(cam_onprojd, cam_onprojd);
		cv::imshow("chess", proj_chess);

		cv::waitKey(1);
		cv::Size chess_sz = cv::Size(checkerBoard.w-1, checkerBoard.h-1);
		cout << chess_sz << endl;
		corners.clear();
		if (!cv::findChessboardCorners(proj_chess, chess_sz, corners,cv::CALIB_CB_ADAPTIVE_THRESH+ cv::CALIB_CB_NORMALIZE_IMAGE+cv::CALIB_CB_FILTER_QUADS+cv::CALIB_CB_FAST_CHECK)) {
			printf("Failed to find chessboardcorners in PROJ...\n");
			return;
		}

		cv::Mat cinv = cam_matrix.inv();
		vector<cv::Point3f> v;
		//平面逆投影
		for (int i = 0; i < corners.size(); i++) {
			cv::Mat tmp = (cv::Mat_<double>(3,1)<< corners[i].x, corners[i].y, 1.0f);
			tmp = cinv * tmp;

			cout << tmp << endl;

			double t = (n.dot(tvec)) / (n.dot(tmp));
			tmp = t * tmp;
			tmp = tmp - tvec;
			tmp = Rt * tmp;
			cv::Point3f x = cv::Point3f((float)tmp.at<double>(0), (float)tmp.at<double>(1), (float)tmp.at<double>(2));
			v.push_back(x);
		}

		if (!cv::findChessboardCorners(img_render, chess_sz, corners)) {
			printf("Failed to find chessboardcorners in PROJIMG...\n");
			return;
		}
		objPoints.push_back(v);
		imgPoints.push_back(corners);

		fprintf(fp, "%d\n", checkerBoard.w-1);
		fprintf(fp, "%d\n", checkerBoard.h-1);
		cv::imwrite("Calibration/img_cam/" + to_string(pic_count) + ".png", cam_light);
		cv::imwrite("Calibration/img_proj/" + to_string(pic_count) + ".png", proj_chess);
		cv::imwrite("Calibration/img_render/" + to_string(pic_count) + ".png", img_render);
		pic_count++;

		printf("Succeed in %d th corner detect!\n", pic_count);
		cv::destroyWindow("chess");
		
	}

	void calibrate_projector_read() {
		vector < vector < cv::Point3f>> objPoints;
		vector<vector<cv::Point2f>> imgPoints;
		fp=fopen("Calibration/chessSize.txt", "r");
		while (1) {
			string cam = "Calibration/img_cam/" + to_string(pic_count) + ".png";
			string proj = "Calibration/img_proj/" + to_string(pic_count) + ".png";
			string render = "Calibration/img_render/" + to_string(pic_count) + ".png";
			if (!checkFileExistence(cam) || !checkFileExistence(proj)||!checkFileExistence(render)) {
				printf("File dosen't exist\n");
				break;
			}
			pic_count++;

			cv::Mat proj_chess;
			cam_light=cv::imread(cam);
			proj_chess = cv::imread(proj);
			img_render= cv::imread(render);
			Undistort(proj_chess, proj_chess);


			
			vector < cv::Point2f> corners;
			if (!cv::findChessboardCorners(cam_light, board_sz, corners)) {
				printf("Failed to find chessboardcorners in LIGHT...\n");
				continue;
			}
			
			//平面式
			cv::Mat rvec, tvec;
			cv::solvePnPRansac(boardPoints, corners, cam_matrix, cam_distortion, rvec, tvec);
			//cout << rvec << endl;
			cv::Mat n = (cv::Mat_<double>(3, 1) << 0.0, 0.0, 1.0);
			cv::Mat R;
			cv::Rodrigues(rvec, R);
			n = R * n;
			cv::Mat Rt = R.t();
			//cout << tvec << endl;
			fscanf(fp, "%d\n", &proj_chess_w);
			fscanf(fp, "%d\n", &proj_chess_h);
			cv::Size chess_sz = cv::Size(proj_chess_w, proj_chess_h);

			if (!cv::findChessboardCorners(proj_chess, chess_sz, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FILTER_QUADS + cv::CALIB_CB_FAST_CHECK)) {
				printf("Failed to find chessboardcorners in PROJ...\n");
				continue;
			}

			cv::drawChessboardCorners(proj_chess, chess_sz, corners, true);
			cv::imshow("lightt", proj_chess);
			cv::waitKey(1);
			cv::Mat cinv = cam_matrix.inv();
			vector<cv::Point3f> v;
			cv::Point3f o;
			//平面逆投影
			for (int i = 0; i < corners.size(); i++) {
				cv::Mat tmp = (cv::Mat_<double>(3, 1) << corners[i].x, corners[i].y, 1.0f);
				tmp = cinv * tmp;
				//cout << tmp << endl;
				double t = (n.dot(tvec)) / (n.dot(tmp));
				tmp = t * tmp;
				tmp = tmp - tvec;
				tmp = Rt * tmp;
				//cout << tmp << endl;
				//cv::waitKey(0);
				cv::Point3f x = cv::Point3f((float)tmp.at<double>(0), (float)tmp.at<double>(1), (float)tmp.at<double>(2));
				if (i == 0) {
					o = x;
				}
				v.push_back(x);
			}
			corners.clear();
			if (!cv::findChessboardCorners(img_render, chess_sz, corners)) {
				printf("Failed to find chessboardcorners in PROJIMG...\n");
				continue;
			}

			objPoints.push_back(v);
			imgPoints.push_back(corners);
		
		}
		

		if (pic_count > 0) {
			std::cout << "\n\n*** CALIBLATING CAMERA...\n" << std::endl;
			int proj_width = 1024;
			int proj_height = 768;
			//cout << objPoints[0] << endl;
			cv::Mat intrinsic_matrix, distortion;
			double err = cv::calibrateCamera(
				objPoints,
				imgPoints,
				cv::Size(proj_width, proj_height),
				intrinsic_matrix,
				distortion,
				cv::noArray(),
				cv::noArray(),
				cv::CALIB_FIX_PRINCIPAL_POINT);

			std::cout << "*** DONE! \n\nReprojection error is " << err <<
				" \nStoring Intrinsics.xml \n\n";

			cv::FileStorage fs("intrinsics_proj.xml", cv::FileStorage::WRITE);

			fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
				intrinsic_matrix << "cam_distortion" << distortion;
			fs.release();
		}
		fclose(fp);
	cv::waitKey(0);

	}

	/*
	1キーで姿勢推定用のキャプチャ
	2キーでoffblackのキャプチャ
	3キーでプロジェクターのキャプチャ
	プロジェクターとライトのオンオフに注意！
	board_sizeはチェスボードの間隔をメートル単位で指定
	*/
	void calibrate_projector(int board_w = 0, int board_h = 0,double board_size=0,bool read=false) {
		board_n = board_h * board_w;
		board_sz = cv::Size(board_w, board_h);
		for (int j = 0; j < board_h; j++) {
			for (int i = 0; i < board_w; i++) {
				cv::Point3d tmp = cv::Point3d((double)i * board_size, (double)j * board_size, 0.0);
				boardPoints.push_back(tmp);
			}
		}
		if (read) {
			calibrate_projector_read();
			return;
		}

		fp = fopen("Calibration/chessSize.txt", "w");

		
		

		#pragma region HSC Open

		#ifdef CAMERA
				HSVMC1 cam;
				cam.connect(2);

				cam.start();
		#endif
				int cam_width = 1296;
				int cam_height = 968;
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

				proj_width = 1024;
				proj_height = 768;
				int proj_fps = 1000;
				int proj_num_buffer = 1;


				cv::Mat  img_proj = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255));
				cv::Mat img_display_proj = cv::Mat(proj_height, proj_width, CV_8UC3);
				cv::Mat img_proj_offblack = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
				cv::Mat img_proj_white = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255));
				cv::Mat img_proj_homography = cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(0));
				bool proj_offblack_flag = false;
				bool proj_white_flag = false;
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

#pragma region TrackBar
				cv::namedWindow("checkerBoard");
				cv::createTrackbar("横の個数", "checkerBoard", &checkerBoard.w,20,this->onTrackbarChanged,(void*)this);
				cv::createTrackbar("縦の個数", "checkerBoard", &checkerBoard.h,20, this->onTrackbarChanged, (void*)this);
				cv::createTrackbar("チェックのサイズ", "checkerBoard", &checkerBoard.size,100, this->onTrackbarChanged,(void*)this);
				cv::createTrackbar("中心x座標", "checkerBoard", &checkerBoard.center.x,1024, this->onTrackbarChanged, (void*)this);
				cv::createTrackbar("中心y座標", "checkerBoard", &checkerBoard.center.y,768, this->onTrackbarChanged, (void*)this);
				cv::createTrackbar("tvalue", "checkerBoard", &tvalue,255);
#pragma endregion


		#pragma region GLFW kit
				//cv::Mat img_render = (cv::Mat(proj_height, proj_width, CV_8UC3, cv::Scalar::all(255)));
				img_render = (cv::Mat(proj_height, proj_width, CV_8UC1, cv::Scalar::all(255)));
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

						//img_render.copyTo(img_proj);
						thread_process_cnt++;
						SetEvent(event_process);
					}



					});
				thread_process.detach();




				std::thread thread_project([&] {
					while (thread_project_flag) {
						WaitForSingleObject(event_process, INFINITE);
		#ifdef PROJECTOR

								if (!proj_offblack_flag) {
									proj.send_image_8bit(img_proj.data);
								}
								else {
									proj.send_image_8bit(img_proj_offblack.data);
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
					
					if (key == 'b') {
						proj_offblack_flag = !proj_offblack_flag;
					}
					else if (key == '1') {
						if (capMode != CaptureMode::LIGHT) {
							printf("capMode is not LIGHT. Now, ");
							std::cout << capMode << std::endl;
						}
						else if (!proj_offblack_flag) {
							printf("please projector black flag on!\n\n");
						}
						else {
							img_cam.copyTo(cam_light);
							//Undistort(cam_light, cam_light);
							capMode = CaptureMode::NOPROJ;
							proj_offblack_flag = true;

							printf("LIGHT is captured. Next NOPROJ...\n");
							printf("Please turn off LIGHT...\n\n");
						}
					}
					else if (key == '2') {
						if (capMode != CaptureMode::NOPROJ) {
							printf("capMode is not NOPROJ. Now, ");
							std::cout << capMode << std::endl;
						}
						else if (!proj_offblack_flag) {
							printf("please projector black flag on!\n");
						}
						else {
							img_cam.copyTo(cam_noproj);
							///Undistort(cam_noproj,cam_noproj);
							capMode = CaptureMode::ONPROJ;
							proj_offblack_flag = false;
							printf("NOPROJ is captured. Next ONPROJ...\n\n");
						}
					}
					else if (key == '3') {
						if (capMode != CaptureMode::ONPROJ) {
							printf("capMode is not ONPROJ. Now, ");
							std::cout << capMode << std::endl;
						}
						else if (proj_offblack_flag) {
							printf("please projector black flag off!\n");
						}
						else {
							img_cam.copyTo(cam_onproj);
							//Undistort(cam_onproj, cam_onproj);
							capMode = CaptureMode::LIGHT;
							proj_offblack_flag = true;
							printf("ONPROJ is captured. Next LIGHT...\n");
							printf("Please turn on LIGHT! \n\n");

							corner_detect();
						}
					}
					else  if (key == 't') {
						vector < cv::Point > corners;
						cv::Size chess_sz = cv::Size(checkerBoard.w - 1, checkerBoard.h - 1);
						if (!cv::findChessboardCorners(img_proj, chess_sz, corners)) {
							printf("Failed to find chessboardcorners in PROJ...\n");
	
						}
						else {
							printf("found");
						}
					}
					img_render.copyTo(img_proj);
					img_render.copyTo(img_display_proj);
					img_cam.copyTo(img_display_cam);
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

				
		#pragma endregion 

		#pragma region Thread Close
				thread_capture_flag = false;

				thread_process_flag = false;

				thread_project_flag = false;

				thread_fps_flag = false;


				fclose(fp);
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
				if (pic_count > 0) {
					std::cout << "\n\n*** CALIBLATING CAMERA...\n" << std::endl;

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

					std::cout << "*** DONE! \n\nReprojection error is " << err <<
						" \nStoring Intrinsics.xml \n\n";

					cv::FileStorage fs("intrinsics_proj.xml", cv::FileStorage::WRITE);

					fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
						intrinsic_matrix << "cam_distortion" << cam_distortion;
					fs.release();
				}

	}

	void stereo_calibrate(int board_w = 0, int board_h = 0,float board_size=0) {
		vector<vector<cv::Point2f>> projPoints;
		board_n = board_h * board_w;
		board_sz = cv::Size(board_w, board_h);
		for (int j = 0; j < board_h; j++) {
			for (int i = 0; i < board_w; i++) {
				cv::Point3f tmp = cv::Point3f((float)i * board_size, (float)j * board_size, 0.0);
				boardPoints.push_back(tmp);
			}
		}
		fp = fopen("Calibration/chessSize.txt", "r");

		while (1) {
			string cam = "Calibration/img_cam/" + to_string(pic_count) + ".png";
			string proj = "Calibration/img_proj/" + to_string(pic_count) + ".png";
			string render = "Calibration/img_render/" + to_string(pic_count) + ".png";
			if (!checkFileExistence(cam) || !checkFileExistence(proj) || !checkFileExistence(render)) {
				printf("File dosen't exist\n");
				break;
			}
			pic_count++;

			cv::Mat proj_chess;
			cam_light = cv::imread(cam);
			proj_chess = cv::imread(proj);
			img_render = cv::imread(render);
			//Undistort(proj_chess, proj_chess);

			vector <cv::Point2f> corners;
			if (!cv::findChessboardCorners(cam_light, board_sz, corners)) {
				printf("Failed to find chessboardcorners in LIGHT...\n");
				continue;
			}
			
			//平面式
			cv::Mat rvec, tvec;
			cv::solvePnP(boardPoints, corners, cam_matrix, cam_distortion, rvec, tvec);
			cv::Mat n = (cv::Mat_<double>(3, 1) << 0.0, 0.0, 1.0);
			cv::Mat R;
			cv::Rodrigues(rvec, R);
			n = R * n;
			cv::Mat Rt = R.t();
			fscanf(fp, "%d\n", &proj_chess_w);
			fscanf(fp, "%d\n", &proj_chess_h);
			cv::Size chess_sz = cv::Size(proj_chess_w, proj_chess_h);
			if (!cv::findChessboardCorners(proj_chess, chess_sz, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FILTER_QUADS + cv::CALIB_CB_FAST_CHECK)) {
				printf("Failed to find chessboardcorners in PROJ...\n");
				continue;
			}

			cv::Mat cinv = cam_matrix.inv();
			vector<cv::Point3f> v;
			//平面逆投影
			for (int i = 0; i < corners.size(); i++) {
				cv::Mat tmp = (cv::Mat_<double>(3, 1) << corners[i].x, corners[i].y, 1.0f);
				tmp = cinv * tmp;

				double t = (n.dot(tvec)) / (n.dot(tmp));
				tmp = t * tmp;
				tmp = tmp - tvec;
				tmp = Rt * tmp;
				cv::Point3f x = cv::Point3f((float)tmp.at<double>(0), (float)tmp.at<double>(1), (float)tmp.at<double>(2));
				v.push_back(x);
			}
			vector<cv::Point2f> corners_p;
			if (!cv::findChessboardCorners(img_render, chess_sz, corners_p)) {
				printf("Failed to find chessboardcorners in PROJIMG...\n");
				continue;
			}

			objPoints.push_back(v);
			imgPoints.push_back(corners);
			projPoints.push_back(corners_p);
		}

		if (pic_count > 0) {
			std::cout << "\n\n*** CALIBLATING CAMERA...\n" << std::endl;
			int proj_width = 1024;
			int proj_height = 768;
			cv::Mat proj_matrix, proj_distortion;
			
			cv::FileStorage fs("intrinsics_proj.xml", cv::FileStorage::READ);
			fs["camera_matrix"] >> proj_matrix;
			fs["distortion_coefficients"] >> proj_distortion;

			fs.release();
			cv::Mat R, t, E, F;
			double err = cv::stereoCalibrate(
				objPoints,
				imgPoints,
				projPoints,
				cam_matrix,
				cam_distortion,
				proj_matrix,
				proj_distortion,
				cv::Size(proj_width, proj_height),
				R,
				t,
				E,
				F,
				cv::CALIB_FIX_INTRINSIC,
				cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6)
			);
			

			std::cout << "*** DONE! \n\nReprojection error is " << err <<
				" \nStoring Intrinsics.xml \n\n";

			cv::FileStorage fss("extrinc_param.xml", cv::FileStorage::WRITE);

			fss << "R"<<R<<"t"<<t<<"E"<<E<<"F"<<F;
			fss.release();
		}
		cv::imshow("l", cam_light);
		cv::waitKey(0);
		fclose(fp);
	}


};