#pragma once
#include"common.h"

#include<glm/glm.hpp>
class MVmatrix {
private:
	cv::Mat intrinsics_matrix;
	cv::Mat distortion_coeffs;
	cv::Mat rvec, tvec;
public:
	MVmatrix(const string fileName) {
		cv::FileStorage fs(fileName, cv::FileStorage::READ);
		fs["camera_matrix"] >> intrinsics_matrix;
		fs["distortion_coefficients"] >> distortion_coeffs;

		fs.release();
	}

	void change3Dpoint(float* objPoints, float* imgPoints, glm::mat4 &dst, int pointnum) {
		vector < cv::Point3f > objp;
		vector<cv::Point2f> imgp;
		for (int i = 0; i < pointnum; i++) {
			cv::Point3f tmp3 = cv::Point3f(objPoints[3 * i + 0], objPoints[3 * i + 1], objPoints[3 * i + 2]);
			cv::Point2f tmp2 = cv::Point2f(imgPoints[3 * i + 0], imgPoints[3 * i + 1]);
			//cout << tmp2 << endl;
			objp.push_back(tmp3);
			imgp.push_back(tmp2);
		}
		
		cv::solvePnP(objp, imgp, intrinsics_matrix, distortion_coeffs, rvec, tvec);
		//cout << rvec << tvec << endl;
		
		cv::Mat R;
		cv::Rodrigues(rvec, R);
		
		R = R.t();  // rotation of inverse
		tvec = -R * tvec;
		
		cv::Mat T=cv::Mat::eye(4, 4, R.type()); // T is 4x4
		//cv::Mat Tgl(4, 4, R.type()); // T is 4x4

		T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1; // copies R into T
		T(cv::Range(0, 3), cv::Range(3, 4)) = tvec * 1;
		

		
	
		fromCV2GLM(T, dst);
		//cout << glm::to_string( dst) << endl;
		
		glm::mat4 RotX = glm::mat4(1, 0, 0, 0,
			0, -1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 0);

		dst = RotX *dst;
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

};