#pragma once

#include<opencv2/opencv.hpp>
#include<iostream>


using namespace std;

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

	cv::Mat intrinsic_matrix, distortion_coeffs;
	double err = cv::calibrateCamera(
		object_points,
		image_points,
		image_size,
		intrinsic_matrix,
		distortion_coeffs,
		cv::noArray(),
		cv::noArray(),
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

	cout << "*** DONE! \n\nReprojection error is " << err <<
		" \nStoring Intrinsics.xml \n\n";

	cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);

	fs << "image_width" << image_size.width << "image_height" << image_size.height << "camera_matrix" <<
		intrinsic_matrix << "distortion_coeffs" << distortion_coeffs;
	fs.release();
}

class Calibration {

private:
	cv::Mat intrinsics_matrix;
	cv::Mat distortion_coeffs;
	cv::Mat R;
	cv::Mat P;

	cv::Size image_size;

	cv::Mat map1;
	cv::Mat map2;

public:

	void Load_matrix(string fileName, int image_w, int image_h) {
		cv::FileStorage fs(fileName, cv::FileStorage::READ);
		fs["camera_matrix"] >> intrinsics_matrix;
		fs["distortion_coefficients"] >> distortion_coeffs;

		fs.release();

		image_size = cv::Size(image_w, image_h);

		
		cv::initUndistortRectifyMap(
			intrinsics_matrix,
			distortion_coeffs,
			cv::Mat(),
			intrinsics_matrix,
			image_size,
			CV_16SC2,
			map1,
			map2);
		
	}

	void Calibrate(cv::InputArray image_captured, cv::OutputArray image_calibrated) {
		if (!image_captured.empty()) {
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


	//ì_íPà Ç≈òcÇ›ï‚ê≥ÇçsÇ§ä÷êî
	//fpsÇ…Ç‡ÇªÇÒÇ»Ç…âeãøÇÕÇ»Ç©Ç¡ÇΩ
	void UndistortPerPoint(float* corner_xyz_cam, float* corner_xyz_undistorted, bool *flags) {

		std::vector<cv::Point2f> points;
		std::vector<cv::Point2f> dst;
		for (int i = 0; i < 4; i++) {
			cv::Point2f point(corner_xyz_cam[3 * i + 0], corner_xyz_cam[3 * i + 1]);
			points.push_back(point);
		}

		cv::undistortPoints(points, dst, intrinsics_matrix, distortion_coeffs,P=intrinsics_matrix);
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


};