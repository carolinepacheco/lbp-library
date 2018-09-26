#include <iostream>
#include <assert.h>
#include <algorithm>

#include "OLBP.h"

namespace lbplibrary
{
  OLBP::OLBP()
  {
    std::cout << "OLBP()" << std::endl;
  }

  OLBP::~OLBP()
  {
    std::cout << "~OLBP()" << std::endl;
  }

  template <typename _Tp>
  void OLBP::OLBP_(const cv::Mat& src, cv::Mat& dst)
  {
    dst = cv::Mat::zeros(src.rows - 2, src.cols - 2, CV_8UC1);
    for (int i = 1; i < src.rows - 1; i++) {
      for (int j = 1; j < src.cols - 1; j++) {
        _Tp center = src.at<_Tp>(i, j);
        unsigned char code = 0;
        code |= (src.at<_Tp>(i - 1, j - 1) > center) << 7;
        code |= (src.at<_Tp>(i - 1, j) > center) << 6;
        code |= (src.at<_Tp>(i - 1, j + 1) > center) << 5;
        code |= (src.at<_Tp>(i, j + 1) > center) << 4;
        code |= (src.at<_Tp>(i + 1, j + 1) > center) << 3;
        code |= (src.at<_Tp>(i + 1, j) > center) << 2;
        code |= (src.at<_Tp>(i + 1, j - 1) > center) << 1;
        code |= (src.at<_Tp>(i, j - 1) > center) << 0;
        dst.at<unsigned char>(i - 1, j - 1) = code;
      }
    }
  }

  void OLBP::run(const cv::Mat &img_input, cv::Mat &img_output)
  {
    if (img_input.empty())
      return;

    //int height = img_input.size().height;
    //int width = img_input.size().width;
    int channels = img_input.channels();

    cv::Mat img_gray;
    if (channels > 1)
      cv::cvtColor(img_input, img_gray, CV_BGR2GRAY);
    else
      img_input.copyTo(img_gray);

    switch (img_gray.type())
    {
      case CV_8SC1: OLBP_<char>(img_gray, img_output); break;
      case CV_8UC1: OLBP_<unsigned char>(img_gray, img_output); break;
      case CV_16SC1: OLBP_<short>(img_gray, img_output); break;
      case CV_16UC1: OLBP_<unsigned short>(img_gray, img_output); break;
      case CV_32SC1: OLBP_<int>(img_gray, img_output); break;
      case CV_32FC1: OLBP_<float>(img_gray, img_output); break;
      case CV_64FC1: OLBP_<double>(img_gray, img_output); break;
    }
  }
}
