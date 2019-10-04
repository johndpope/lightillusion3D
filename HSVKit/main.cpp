
//#include"mixedRateEvaluation.h"
#include"common.h"

#include"GLFWkit.h"
//#include"EstimateOffset.h"
//#include"exparimentLR.h"
//#include"exparimentSpring.h"
//#include"exparimentPurin.h"
//#include"exparimentLena.h"
int main() {
	//mainloop();
	GLFWkit glfwkit("glfwkit", 1024, 768,"horse.obj");
	glfwkit.setup();
	float corner_gl[12] = { 0, };
	int key;
	float v;
	vector<float> varray = glfwkit.glkit.model.varray;
	/*
	for (int i = 0; i < varray.size() / 8; i++) {
		
			cout << varray[i * 8 + 5] << " ";
		
		cout << endl;
	}
	*/
	while (glfwkit.update(corner_gl)) {
		
	}
	
	//cv::waitKey(0);
	//cv::Size size = cv::Size(400, 400);

	/*
	//EstimateOffset("pic.jpg", size);


	cv::Mat F = cv::imread("offset.png", 0);
	cv::Mat src1 = cv::imread("purin.jpg", 0);
	cv::Mat dst;
	src1.copyTo(dst);
	cv::Point2f center;
	
	cv::Mat src2;
	int key = 0;
	int count = 0;
	while ((key = cv::waitKey(1)) != 'q') {
		count++;
		int diff = 5.0f * sin((double)count * 0.01);
		center.x = src1.rows / 2.0f +diff;
		center.y = src1.cols / 2.0f;
		cv::getRectSubPix(src1, src1.size(), center, src2);

		diffImage(src1, src2, dst, F, 0.0);

		cv::imshow("warped", src2);
		cv::imshow("diff", dst);
	}
	
	*/
	/*
	cv::Mat img = cv::Mat(400, 400, CV_8UC3, cv::Scalar::all(255));
	springSimulate simulate;
	vector<cv::Point2f> rpoint;
	vector<float> k;
	k.push_back(1.0f);
	rpoint.push_back(cv::Point2f(200.0f, 200.0f));

	simulate.Init(rpoint, k,29.5);
	int key = 0;
	mouseParam mouse;
	cv::namedWindow("img");
	cv::setMouseCallback("img", mouseCallBack, &mouse);
	while ((key = cv::waitKey(1)) != 'q') {
		img= cv::Mat(400, 400, CV_8UC3, cv::Scalar::all(255));
		if (mouse.event!=NULL) {
			rpoint[0] = cv::Point2f(mouse.x, mouse.y);
			cout << "n" << endl;

		}
		mouse.event = NULL;
		simulate.simulate(rpoint, 0.033);
		vector<cv::Point2f> ipoint = simulate.getIpoints();
		cv::circle(img, rpoint[0], 5, cv::Scalar(0, 200, 0), -1);
		cv::circle(img, ipoint[0], 5, cv::Scalar(200, 0, 0), -1);
		cv::imshow("img", img);
	}

	

	*/

	/*
	dataStorage ds("A.csv");

	vector<float> v;
	v.push_back(0.0f);
	v.push_back(0.0f);
	v.push_back(1.0f);
	
	ds.writeData(1000.0f, v);
	*/

	/*
	cv::Mat mat1, mat2;
	checkerMake(120, 120, 12,12 , mat1, mat2);
	cv::imshow("mat1", mat1);
	cv::imshow("mat2", mat2);

	cv::imwrite("check0_l.png", mat1);
	cv::imwrite("check1_l.png", mat2);
	


	*/
	/*
	multiEye e(2,120,120,2);

	cv::Mat mat1, mat2;
	mat1=cv::imread("dog_l.jpg", 0);
	mat2=cv::imread("cat_l.jpg", 0);

	e.images.push_back(mat2);
	e.images.push_back(mat1);
	e.Generate();

	cv::imshow("generate", e.outputImage);
	
	cv::imwrite("output_l.png", e.outputImage);


	cv::waitKey(0);
	*/


	return 0;
}