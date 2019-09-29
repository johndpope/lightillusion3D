#pragma once
#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<vector>

using namespace std;

class springSimulate {
private:
	int pointnum;
	vector<cv::Point2f> Rpoints;
	vector<cv::Point2f> Ipoints;
	vector<cv::Point2f> vels;
	vector<float> k;
	float lambda;
	vector < cv::Point3f > Rpoints3d;
	vector < cv::Point3f > Ipoints3d;
	vector < cv::Point3f > vels3d;

public:
	void Init(vector<cv::Point2f> rpoints,vector<float> kt,float lam=30);
	void Init(float* rpoint, vector<float> kt, float lam = 30);

	void simulate(vector<cv::Point2f> rpoints,float time);
	void simulate(float* rpoint, float time);

	vector<cv::Point2f> getIpoints();
	void getIpoints(float* points);
};