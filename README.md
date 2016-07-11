# LBP Library

Last Page Update: **11/07/2016**

Latest Library Version: **1.0.0** (see Release Notes for more info).


The LBP Library is a collection of **eleven** Local Binary Patterns (LBP) algorithms developed for **background subtraction** problem. The algorithms were implemented in C++ based on [OpenCV](http://www.opencv.org/). A CMake file is provided and the library is complatible with Windows, Linux and Mac OS X. The library was tested successfully with OpenCV 2.4.10.

*Tests with OpenCV 3 are being conducted, and new changes can be made soon.*

<p align="center"><img src="https://raw.githubusercontent.com/carolinepacheco/lbplibrary/master/docs/screen0.png" border="0" height="200"/></p>

List of the algorithms available in the LBP Library
---------------------------------------------------
* BGLBP
* CSLBP
* CSLDP
* CSSILTP
* ELBP
* OCLBP
* OLBP
* SCSLBP
* SILTP
* VARLBP
* XCSLBP

Citation
--------
If you use this library for your publications, please cite it as:
```
@inproceedings{lbplibrary,
author    = {Silva, Caroline and Bouwmans, Thierry and Frelicot, Carl},
title     = {An eXtended Center-Symmetric Local Binary Pattern for Background Modeling and Subtraction in Videos},
booktitle = {10th International Joint Conference on Computer Vision, Imaging and Computer Graphics Theory and Applications (VISAPP)},
address   = {Berlin, Germany},
year      = {2015},
url       = {https://github.com/carolinepacheco/lbplibrary}
}
```

List of collaborators
---------------------
[Andrews Sobral](https://github.com/andrewssobral) and [Cristina Lazar](https://plus.google.com/105844506452223448788).

Example code
------------
```C++
#include <iostream>
#include <opencv2/opencv.hpp>

#include "lbplibrary.hpp"
using namespace lbplibrary;

int main(int argc, char **argv)
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

		cv::cvtColor(frame, frame, CV_BGR2GRAY);
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
```

<p align="center"><img src="https://raw.githubusercontent.com/carolinepacheco/lbplibrary/master/docs/screen2.png" border="0"/></p>

Release Notes:
--------------
* Version 1.0.0:
First stable version. Added 11 LBP algorithms.