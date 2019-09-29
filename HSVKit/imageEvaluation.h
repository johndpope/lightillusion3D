#pragma once

#include "common.h"

double getPSNR(const cv::Mat& I1, const cv::Mat& I2) {
	cv::Mat s1;
	cv::absdiff(I1, I2, s1);
	s1.convertTo(s1, CV_32F);
	s1 = s1.mul(s1);

	cv::Scalar s = sum(s1);
	
	double sse = s.val[0];

	if (sse <= 1e-10) {
		return 0;
	}
	else {
		double mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((225 * 225) / mse);
		return psnr;
	}
}

double meanPSNR(cv::Mat I1, vector < cv::Mat> i2) {
	double psnr = 0.0;
	int size = 10;

	for (int i = 0; i < size; i++) {;
		psnr += getPSNR(I1, i2[i]);

	}
	return psnr / (double)size;
}

double getMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	cv::Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	cv::Mat I2_2 = I2.mul(I2);        // I2^2
	cv::Mat I1_2 = I1.mul(I1);        // I1^2
	cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/*************************** END INITS **********************************/

	cv::Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

	cv::Mat mu1_2 = mu1.mul(mu1);
	cv::Mat mu2_2 = mu2.mul(mu2);
	cv::Mat mu1_mu2 = mu1.mul(mu2);

	cv::Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	cv::Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	cv::Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	cv::Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	double ssim = mssim.val[0];
	return ssim;
}

double meanSSIM(const cv::Mat& I1, const vector < cv::Mat>& i2) {
	double psnr = 0.0;
	int size = i2.size();

	for (int i = 0; i < size; i++) {
		psnr += getMSSIM(I1, i2[i]);
	}
	return psnr / (double)size;
}

double Mrate(const cv::Mat& src, const cv::Mat& t1, const cv::Mat& t2) {
	cv::Size size = src.size();
	float epsilon = 1e-4;
	double accumelate = 0.0f;
	double count = 0.0;
	for (int j = 0; j < src.rows; j++) {
		for (int i = 0; i < src.cols; i++) {
			double s = (double)src.at<uchar>(i, j);
			double t11 = (double)t1.at<uchar>(i, j);
			double t22 = (double)t2.at<uchar>(i, j);
			if (abs(t11 - t22) > 30) {
				double d = (s - t22) / (t11 - t22);

				if (d >= 1.0) {
					d = 1.0;
				}
				else if (d < 0.0) {
					d = 0.0;
				}
				accumelate += d;
				count++;
			}
			//cout << s - t22 << " " << t11 - t22 << endl;
		}
	}
	accumelate /= count;
	return accumelate;
}

double Mrate_val(const cv::Mat& src, const cv::Mat& t1, const cv::Mat& t2,cv::Mat& dst) {
	cv::Size size = src.size();
	src.copyTo(dst);
	float epsilon = 1e-4;
	double accumelate = 0.0f;
	double count = 0.0;
	for (int j = 0; j < src.rows; j++) {
		for (int i = 0; i < src.cols; i++) {
			double s = (double)src.at<uchar>(i, j);
			double t11 = (double)t1.at<uchar>(i, j);
			double t22 = (double)t2.at<uchar>(i, j);
			if (abs(t11 - t22) > 50) {
				double d = (s - t22) / (t11 - t22);
				//cout << d << " ";
				if (d >= 1.0) {
					d = 1.0;
				}
				else if (d < 0.0) {
					d = 0.0;
				}
				accumelate += d;
				count++;
				dst.at<uchar>(i, j) = (uchar)(d*255) ;
			}
			else {
				dst.at<uchar>(i, j) = 128;
			}
			//cout << s - t22 << " " << t11 - t22 << endl;
		}
		//cout << endl;
	}
	accumelate /= count;
	return accumelate;
}

void contrast(cv::Mat& image,double alpha=1.0,int beta=0) {


	//行・列・カラーチャネルをforで回し、各ピクセルに上式の変換を適用する
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			
				image.at<uchar>(y, x) =
					//saturate_cast<uchar>は値が整数でない場合や範囲外の場合に有効な値に直す
					alpha * image.at<uchar>(y, x) + beta;
			
		}
	}
}