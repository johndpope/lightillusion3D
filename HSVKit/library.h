#pragma once

#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<math.h>

float dist(float* a, float* b) {
	float d = 0.0f;
	d = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]);
	return sqrtf(d);
}

void diffImage(cv::Mat& src1, cv::Mat src2, cv::Mat& dst, cv::Mat& F,uchar offset) {

	cv::Size size = src1.size();

	for (int j = 0; j < src1.rows; j++) {
		for (int i = 0; i < src1.cols; i++) {
			
			double d= std::min(std::max(((double)src2.at<uchar>(i, j) - (double)src1.at<uchar>(i, j)) *255.0*(double) src1.at<uchar>(i, j) /200.0/255.0+ offset, 0.0), 255.0);
			dst.at<uchar>(i, j) = (uchar)d;
		}
	}
}

template<class T>
const T& clamp(const T& v, const T& low, const T& high) {
	if (v > high) {
		return high;
	}
	else if (v < low) {
		return low;
	}
	else {
		return v;
	}
}