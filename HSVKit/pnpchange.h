#pragma once
#include"common.h"

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include<opencv2/calib3d/calib3d.hpp>
class MVmatrix {
public:
	cv::Mat intrinsics_matrix;
	cv::Mat distortion_coeffs;
	cv::Mat rvec, tvec;
public:
	MVmatrix(const string fileName) {
		cv::FileStorage fs(fileName, cv::FileStorage::READ);
		fs["camera_matrix"] >> intrinsics_matrix;
		fs["distortion_coefficients"] >> distortion_coeffs;

		fs.release();
		tvec = (cv::Mat_<double>(3, 1) << 0, 0, 1);
		rvec = (cv::Mat_<double>(3, 1) << 0, 3.14, 0);
	}

	void change3Dpoint(float* objPoints, float* imgPoints, glm::mat4 &dst, int pointnum) {
		vector < cv::Point3f > objp;
		vector<cv::Point2f> imgp;
		for (int i = 0; i < pointnum; i++) {
			cv::Point3d tmp3 = cv::Point3d(objPoints[3 * i + 0], objPoints[3 * i + 1], objPoints[3 * i + 2]);
			cv::Point2d tmp2 = cv::Point2d(imgPoints[3 * i + 0], imgPoints[3 * i + 1]);
			//cout << tmp3 << endl;
			//cout << tmp2 << endl;
			objp.push_back(tmp3);
			imgp.push_back(tmp2);
		}
		//cout << endl;
		/*
		cv::Mat r= (cv::Mat_<double>(3, 3) << 1,0,0,0,1,0,0,0,1);
		cv::Mat t = (cv::Mat_<double>(3,1) << 0, 0, 1);
		cv::Mat rr;
		cv::Rodrigues(r, rr);
		cout << rr << endl;
		vector<cv::Point2f> imgpp;
		cv::projectPoints(objp, rr, t, intrinsics_matrix, distortion_coeffs, imgpp);
		cout << imgpp << endl;
		cout << endl;
		*/
		//cout << endl;
		//cv::Mat distortion = (cv::Mat_<double>(4, 1) << 0, 0, 0, 0);
		cv::solvePnPRansac(objp, imgp, intrinsics_matrix, distortion_coeffs, rvec, tvec, true);
		//cout << rvec << tvec << endl;
		//rvec=(cv::Mat_<double>(3, 1) << 0, 0, 0);
		cv::Mat R;
		cv::Rodrigues(rvec, R);
		
		R = R.t();  // rotation of inverse
		//cv::Rodrigues(R,rvec);
		//cout << rvec << endl;
		//cout << R << endl;
		//
		cv::Mat tveci = -R * tvec;
		//cout << tveci << endl;
		//tvec.at<double>(2)*=1.0;
		//tvec.at<double>(1) *=-1.0;
		//cout <<  -R * tvec << endl;
		//computeAnglesFromMatrix(R);

		cv::Mat T=cv::Mat::eye(4, 4, R.type()); // T is 4x4
		//cv::Mat Tgl(4, 4, R.type()); // T is 4x4

		T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1; // copies R into T
		T(cv::Range(0, 3), cv::Range(3, 4)) = tveci * 1;
		

		//T = T.inv();
		
		
	
		fromCV2GLM(T, dst);
		//cout << glm::to_string( dst) << endl;
		//cout << to_string(dst) << endl;
		glm::mat4 RotX = glm::mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1);

		//dst = dst*RotX;
		//dst = glm::mat4(1.0f);
		//dst = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)tvec.at<double>(0,0), -(float)tvec.at<double>(1,0), -(float)tvec.at<double>(2,0)));
		//dst = glm::translate(dst, glm::vec3(0.0f,0.0f,-1.0f));
		//cout << to_string(dst) << endl;
		

	}

	void fromCV2GLM(const cv::Mat& cvmat, glm::mat4& glmmat) {

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				//cout << (float)cvmat.at<float>(j, i) << " ";
				glmmat[i][j] = (float)cvmat.at<double>(j, i);
			}
			//cout << endl;
			
		}


		//cout << endl;
		
		//memcpy(glm::value_ptr(glmmat), cvmat.data, 16 * sizeof(float));
		//glmmat = glm::transpose(glmmat);
	}

	void computeAnglesFromMatrix(
		cv::Mat R
		
	) {
		double PI = 3.14159265359;
		double threshold = 0.001;
		double angle_x, angle_y, angle_z;
		if (abs(R.at<double>(2, 1) - 1.0) < threshold) { // R(2,1) = sin(x) = 1‚ÌŽž
			angle_x = PI / 2;
			angle_y = 0;
			angle_z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
		}
		else if (abs(R.at<double>(2, 1) + 1.0) < threshold) { // R(2,1) = sin(x) = -1‚ÌŽž
			angle_x = -PI / 2;
			angle_y = 0;
			angle_z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
		}
		else {
			angle_x = asin(R.at<double>(2, 1));
			angle_y = atan2(-R.at<double>(2, 0), R.at<double>(2, 2));
			angle_z = atan2(-R.at<double>(0, 1), R.at<double>(1, 1));
		}
		cout << angle_x*(180.0)/PI << " " << angle_y * (180.0) / PI << " " << angle_z* (180.0) / PI << endl;
	}

};