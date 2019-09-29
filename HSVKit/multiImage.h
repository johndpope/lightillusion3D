#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
#include <chrono>

using namespace std;
class multiEye
{
public:
	int numOfdivide;
	int width;
	int height;
	int stride;
	vector<cv::Mat> images;

	cv::Mat outputImage;

	multiEye(int mnumOfdivide, int mwidth, int mheight, int mstride)
	{
		numOfdivide = mnumOfdivide;
		width = mwidth;
		height = mheight;
		stride = mstride;
		outputImage = cv::Mat(height, width, CV_8UC1, cv::Scalar(0.0f));
	}

	void Load(vector<string> textures)
	{
		for (int i = 0; i < numOfdivide; i++) {
			cv::Mat tmp = cv::imread(textures[i]);
			if (tmp.data == NULL) {
				std::cout << "NULL IMAGE" << std::endl;
			}
			images.push_back(tmp);
		}
	}

	void Generate()
	{
		cv::Mat roilocal;
		for (int i = 0; i < width; i += stride / numOfdivide)
		{
			int k = i * numOfdivide / stride;
			k %= numOfdivide;
			int j = i / stride;
			j = j * stride / numOfdivide;

			roilocal = images[k](cv::Rect(i, 0, stride / numOfdivide, height));

			roilocal.copyTo(outputImage(cv::Rect(i, 0, stride / numOfdivide, height)));
		}
	}
};
