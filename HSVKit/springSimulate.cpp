
#include"springSimulate.h"

void springSimulate::Init(vector<cv::Point2f> rpoints, vector<float> kt,float lam) {
	pointnum = rpoints.size();
	Rpoints = rpoints;
	Ipoints = rpoints;
	k = kt;
	for (int i = 0; i < pointnum; i++) {
		vels.push_back(cv::Point2f(0.0f,0.0f));
	}
	lambda = lam;
}

void springSimulate::simulate(vector<cv::Point2f> rpoints,float time) {
	Rpoints = rpoints;
	for (int i = 0; i < pointnum; i++) {
		vels[i] += -k[i] * time*(Ipoints[i] - Rpoints[i]);

		Ipoints[i] += vels[i] * time;
		vels[i] *= lambda*time;
	}
}

vector<cv::Point2f> springSimulate::getIpoints() {
	return Ipoints;
}

void springSimulate::Init(float* rpoint, vector<float> kt, float lam) {
	pointnum = kt.size();
	vels3d.resize(pointnum);
	k = kt;
	for (int i = 0; i < pointnum; i++) {
		cv::Point3f p = cv::Point3f(rpoint[3 * i + 0], rpoint[3 * i + 1], rpoint[3 * i + 2]);
		vels.push_back(cv::Point2f(0.0f, 0.0f));
		Rpoints3d.push_back(p);
		Ipoints3d.push_back(p);
	}
	lambda = lam;
}

void springSimulate::simulate(float* rpoint, float time) {
	
	for (int i = 0; i < pointnum; i++) {
		cv::Point3f p = cv::Point3f(rpoint[3 * i + 0], rpoint[3 * i + 1], rpoint[3 * i + 2]);
		Rpoints3d[i] = p;
		vels3d[i] += -k[i] * time * (Ipoints3d[i] - Rpoints3d[i]);

		Ipoints3d[i] += vels3d[i] * time;
		vels3d[i] *= lambda * time;
	}
}

void springSimulate::getIpoints(float* points) {
	for (int i = 0; i < pointnum; i++) {
		points[3 * i + 0] = Ipoints3d[i].x;
		points[3 * i + 1] = Ipoints3d[i].y;
		points[3 * i + 2] = Ipoints3d[i].z;
	}
}

