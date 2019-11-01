#pragma once
#include"calibration.h"


void circleMake(int board_w, int board_h, int size, cv::Mat& dst) {
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


	class circleCalibration :public Calibration {


	};