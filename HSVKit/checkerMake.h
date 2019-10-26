#pragma once

#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>

void checkerMake(int width, int height,int wstride,int hstride,cv::Mat &mat1,cv::Mat &mat2) {
	mat1 = cv::Mat::zeros(width, height, CV_8UC3);
	mat2 = cv::Mat::zeros(width, height, CV_8UC3);

	int wnum = width / wstride;
	int hnum = height / hstride;

	for (int i = 0; i < wnum; i++) {
		for (int j = 0; j < hnum; j++) {
			for (int ii = 0; ii < wstride; ii++) {
				for (int jj = 0; jj < hstride; jj++) {
					if ((i + j) % 2 == 0) {
						mat1.at<cv::Vec3b>(j*hstride+jj,i*wstride+ii)[0]= 255;
						mat1.at<cv::Vec3b>(j*hstride+jj,i*wstride+ii)[1]= 255;
						mat1.at<cv::Vec3b>(j*hstride+jj,i*wstride+ii)[2]= 255;
					}
					else {
						mat2.at<cv::Vec3b>(j * hstride + jj, i * wstride + ii)[0] = 255;
						mat2.at<cv::Vec3b>(j * hstride + jj, i * wstride + ii)[1] = 255;
						mat2.at<cv::Vec3b>(j * hstride + jj, i * wstride + ii)[2] = 255;
					}
				}
			}
		}
	}
	cv::cvtColor(mat1, mat1, cv::COLOR_RGB2GRAY);
	cv::cvtColor(mat2, mat2, cv::COLOR_RGB2GRAY);
}

