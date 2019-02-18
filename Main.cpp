#include <iostream>
#include <opencv2/opencv.hpp>

#include "lbplibrary.hpp"
using namespace lbplibrary;

void test_image()
{
	cv::Mat img_output;
	cv::Mat img_input = cv::imread("frames/1.png");
	cv::imshow("Input", img_input);

	cv::cvtColor(img_input, img_input, cv::COLOR_BGR2GRAY);
	//cv::GaussianBlur(img_input, img_input, cv::Size(7, 7), 5, 3, cv::BORDER_CONSTANT);

	LBP *lbp;
	lbp = new OLBP;     // 0-255
	//lbp = new ELBP;     // 0-255
	//lbp = new VARLBP;   // 0-953.0
	//lbp = new CSLBP;    // 0-15
	//lbp = new CSLDP;    // 0-15
	//lbp = new XCSLBP;   // 0-15
	//lbp = new SILTP;    // 0-80
	//lbp = new CSSILTP;  // 33-120
	//lbp = new SCSLBP;   // 0-15
	//lbp = new BGLBP;    // 0-239
	lbp->run(img_input, img_output);

	double min, max; cv::minMaxLoc(img_output, &min, &max); std::cout << "min: " << min << ", max: " << max;
	cv::normalize(img_output, img_output, 0, 255, cv::NORM_MINMAX, CV_8UC1);

	cv::imshow("Output", img_output);
	cv::waitKey();
	delete lbp;
}

void test_OCLBP()
{
	cv::Mat img_output;
	cv::Mat img_input = cv::imread("frames/1.png");
	cv::imshow("Input", img_input);

	LBP *lbp;
	lbp = new OCLBP;

	std::vector<cv::Mat> vecMat;
	lbp->run(img_input, vecMat);
	for (int i = 0; i < 6; i++)
	{
		img_output = vecMat[i];
		std::stringstream sstm;
		sstm << "Output " << i;
		cv::imshow(sstm.str(), img_output);
	}

	cv::waitKey();
	delete lbp;
}

void test_webcam()
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
		return;

	LBP *lbp;
	lbp = new OLBP;
	//lbp = new ELBP;
	//lbp = new VARLBP;
	//lbp = new CSLBP;
	//lbp = new CSLDP;
	//lbp = new XCSLBP;
	//lbp = new SILTP;
	//lbp = new CSSILTP;
	//lbp = new SCSLBP;
	//lbp = new BGLBP;

	cv::Mat frame, img_lbp;
	while (1)
	{
		cap >> frame;
		cv::resize(frame, frame, cv::Size(320, 240));

		imshow("capture", frame);
		show_multi_histogram(frame);

		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
		//cv::GaussianBlur(frame, frame, cv::Size(7, 7), 5, 3, cv::BORDER_CONSTANT);

		imshow("gray", frame);
		show_histogram("gray_hist", frame);

		lbp->run(frame, img_lbp);
		cv::normalize(img_lbp, img_lbp, 0, 255, cv::NORM_MINMAX, CV_8UC1);

		cv::imshow("lbp", img_lbp);
		show_histogram("lbp_hist", img_lbp);

		if (cv::waitKey(10) >= 0)
			break;
	}

	delete lbp;
}

int main(int argc, const char **argv)
{
	//test_image();
	//test_OCLBP();
	test_webcam();

	return 0;
}
