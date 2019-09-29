#pragma once
#include"common.h"



void mainloop() {
	int a;
	string directory = "Expariment/Velocity_gausian_Accel_169_100e";
	
	vector<cv::Mat> img_static0_c(10);
	vector<cv::Mat> img_static0_i(10);
	vector<cv::Mat> img_static1_c(10);
	vector<cv::Mat> img_static1_i(10);
	for (int i = 0; i < 10; i++) {
		cv::Mat t1 = cv::imread(directory + "/Static/cam0/correct/"+to_string(i)+".png", 0);
		cv::Mat t2 = cv::imread(directory + "/Static/cam1/correct/"+to_string(i)+".png", 0);
		t1.copyTo(img_static0_c[i]);
		t2.copyTo(img_static1_c[i]);

		cv::Mat t3 = cv::imread(directory + "/Static/cam0/inverse/" + to_string(i+10) + ".png", 0);
		cv::Mat t4 = cv::imread(directory + "/Static/cam1/inverse/" + to_string(i+10) + ".png", 0);
		t3.copyTo(img_static0_i[i]);
		t4.copyTo(img_static1_i[i]);


	}
	cv::Mat t1 = cv::imread(directory+"/Static/cam0/correct/0.png", 0);
	cv::Mat t2 = cv::imread(directory+"/Static/cam1/correct/0.png", 0);

	cv::Mat check1 = cv::imread("check1.png", 0);
	cv::Mat check2 = cv::imread("check0.png", 0);

	vector<cv::Point> target;
	target.push_back(cv::Point(0, 0));
	target.push_back(cv::Point(299, 0));
	target.push_back(cv::Point(0, 299));
	target.push_back(cv::Point(299, 299));

	cv::Size imSize = cv::Size(300, 300);

	
	vector<cv::Point> source;
	mouseParam mouse;
	cv::namedWindow("Homography");
	cv::setMouseCallback("Homography", mouseCallBack, &mouse);

	cv::Mat displaysrc;
	cv::cvtColor(t1, displaysrc, cv::COLOR_GRAY2RGB);
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
	
		cv::circle(displaysrc, cv::Point(cx,cy), 10, cv::Scalar(200,0,0), 1);
	
		cv::imshow("Homography", displaysrc);
		cv::waitKey(33);
	}
	

	cv::Mat homography0 = cv::findHomography(source, target);
	cout << homography0 << endl;

	source.clear();
	cv::cvtColor(t2, displaysrc, cv::COLOR_GRAY2RGB);
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

	cv::Mat homography1 = cv::findHomography(source, target);
	cout << homography1 << endl;
	
	
	cv::Mat dst_image0;
	cv::Mat dst_image1;



	for (int i = 0; i < img_static0_c.size(); i++) {

		cv::warpPerspective(img_static0_c[i], img_static0_c[i], homography0, imSize);
		cv::warpPerspective(img_static1_c[i], img_static1_c[i], homography1, imSize);
		cv::warpPerspective(img_static0_i[i], img_static0_i[i], homography0, imSize);
		cv::warpPerspective(img_static1_i[i], img_static1_i[i], homography1, imSize);

		
		cv::equalizeHist(img_static0_c[i], img_static0_c[i]);
		cv::equalizeHist(img_static1_c[i], img_static1_c[i]);
		cv::equalizeHist(img_static0_i[i], img_static0_i[i]);
		cv::equalizeHist(img_static1_i[i], img_static1_i[i]);

	
	}
	//cv::equalizeHist(dst_image, dst_image);
	//cv::equalizeHist(t1, t1);
	//cv::equalizeHist(t2, t2);
	//cv::imshow("warped", img_static0[0]);

	//double rate = Mrate(dst_image, t1, t2);

	
	for (int i = 0; i < 10; i++) {

		vector<double> rate;

		for (int j = 0; j < img_static0_c.size(); j++) {
			for (int k = 0; k < img_static0_c.size(); k++) {
				if (i != k && i != j) {
					rate.push_back(Mrate(img_static0_c[i], img_static0_c[j], img_static0_i[k]));
					rate.push_back(Mrate(img_static1_c[i], img_static1_c[j], img_static1_i[k]));
				}
			}

		}
		double r = std::accumulate(rate.begin(), rate.end(), 0.0) / (double)rate.size();
		cout << r << endl;
	}
	cout << endl;
	
	cv::Mat visual;

	//cv::bitwise_and(check1, img_static0_c[0], img_static0_c[0]);
	//cv::imshow("and", img_static0_c[0]);
	
	cv::Mat src0 = cv::imread(directory + "/Moving/cam0/0.png", 0);
	cv::warpPerspective(src0, dst_image0, homography0, imSize);
	cv::equalizeHist(dst_image0, dst_image0);
	Mrate_val(dst_image0, img_static0_c[0], img_static0_i[0],visual);
	cv::imshow("visual", visual);
	vector<double> mean;
	for (int i = 0; i < 100; i++) {
		cv::Mat src0 = cv::imread(directory+"/Moving/cam0/"+to_string(i)+".png", 0);
		cv::Mat src1 = cv::imread(directory+"/Moving/cam1/"+to_string(i)+".png", 0);

		cv::warpPerspective(src0, dst_image0, homography0, imSize);
		cv::warpPerspective(src1, dst_image1, homography1, imSize);
		cv::equalizeHist(dst_image0, dst_image0);
		cv::equalizeHist(dst_image1, dst_image1);
		//cv::imshow("win", dst_image0);

		vector<double> rate;
		
		for (int j = 0; j< img_static0_c.size(); j++) {
			for (int k = 0; k < img_static0_c.size(); k++) {

				rate.push_back(Mrate(dst_image0, img_static0_c[j], img_static0_i[k]));
				rate.push_back(Mrate(dst_image1, img_static1_c[j], img_static1_i[k]));
			}

		}
		double r = std::accumulate(rate.begin(), rate.end(),0.0) / (double)rate.size();
		mean.push_back(r);
		cout << r << endl;
	}
	cout << endl;

	cout<< std::accumulate(mean.begin(), mean.end(), 0.0) / (double)mean.size();
	
	//cout << rate << endl;
	
	cv::waitKey(0);
	
}