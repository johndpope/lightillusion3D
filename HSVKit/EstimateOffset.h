#pragma once

#include"common.h"

struct mouseParam {
	int x, y, event, flags;
};

void mouseCallBack(int event, int x, int y, int flags, void* data) {
	mouseParam* ptr = (mouseParam*)data;
	ptr->x = x; ptr->y = y; ptr->event = event; ptr->flags = flags;
}

void EstimateOffset(const string fileName, cv::Size size) {
	cv::Mat img = cv::imread(fileName, 0);
	//cv::imshow("img", img);
	vector<cv::Point> target;
	target.push_back(cv::Point(0, 0));
	target.push_back(cv::Point(size.width - 1, 0));
	target.push_back(cv::Point(0, size.height - 1));
	target.push_back(cv::Point(size.width - 1, size.height - 1));

	cv::Size imSize = size;


	vector<cv::Point> source;
	mouseParam mouse;
	cv::namedWindow("Homography");
	cv::setMouseCallback("Homography", mouseCallBack, &mouse);

	cv::Mat displaysrc;
	cv::cvtColor(img, displaysrc, cv::COLOR_GRAY2RGB);
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


	cv::Mat homography0 = cv::findHomography(source, target);

	cv::warpPerspective(img, img, homography0, imSize);

	cv::Mat dst;
	img.copyTo(dst);

	cv::imwrite("offset.png", img);

}