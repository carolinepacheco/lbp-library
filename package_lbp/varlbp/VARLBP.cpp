#include <iostream>
#include <assert.h>
#include <algorithm>

#include "VARLBP.h"

namespace lbplibrary
{
  VARLBP::VARLBP() : radius(1), neighbors(8)
  {
    std::cout << "VARLBP()" << std::endl;
  }

  VARLBP::~VARLBP()
  {
    std::cout << "~VARLBP()" << std::endl;
  }

  template <typename _Tp>
  void VARLBP::VARLBP_(const cv::Mat& src, cv::Mat& dst)
  {
    std::max(std::min(neighbors, 31), 1); // set bounds
    dst = cv::Mat::zeros(src.rows - 2 * radius, src.cols - 2 * radius, CV_32FC1); //! result
    // allocate some memory for temporary on-line variance calculations
    cv::Mat _mean = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
    cv::Mat _delta = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
    cv::Mat _m2 = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
    for (int n = 0; n<neighbors; n++) {
      // sample points
      float x = static_cast<float>(radius)* cos(2.0*M_PI*n / static_cast<float>(neighbors));
      float y = static_cast<float>(radius)* -sin(2.0*M_PI*n / static_cast<float>(neighbors));
      // relative indices
      int fx = static_cast<int>(floor(x));
      int fy = static_cast<int>(floor(y));
      int cx = static_cast<int>(ceil(x));
      int cy = static_cast<int>(ceil(y));
      // fractional part
      float ty = y - fy;
      float tx = x - fx;
      // set interpolation weights
      float w1 = (1 - tx) * (1 - ty);
      float w2 = tx  * (1 - ty);
      float w3 = (1 - tx) *      ty;
      float w4 = tx  *      ty;
      // iterate through your data
      for (int i = radius; i < src.rows - radius; i++) {
        for (int j = radius; j < src.cols - radius; j++) {
          float t = w1*src.at<_Tp>(i + fy, j + fx) + w2*src.at<_Tp>(i + fy, j + cx) + w3*src.at<_Tp>(i + cy, j + fx) + w4*src.at<_Tp>(i + cy, j + cx);
          _delta.at<float>(i, j) = t - _mean.at<float>(i, j);
          _mean.at<float>(i, j) = (_mean.at<float>(i, j) + (_delta.at<float>(i, j) / (1.0*(n + 1)))); // i am a bit paranoid
          _m2.at<float>(i, j) = _m2.at<float>(i, j) + _delta.at<float>(i, j) * (t - _mean.at<float>(i, j));
        }
      }
    }
    // calculate result
    for (int i = radius; i < src.rows - radius; i++) {
      for (int j = radius; j < src.cols - radius; j++) {
        dst.at<float>(i - radius, j - radius) = _m2.at<float>(i, j) / (1.0*(neighbors - 1));
      }
    }
  }

  void VARLBP::run(const cv::Mat &img_input, cv::Mat &img_output)
  {
    if (img_input.empty())
      return;

    int height = img_input.size().height;
    int width = img_input.size().width;
    int channels = img_input.channels();

    cv::Mat img_gray;
    if (channels > 1)
      cv::cvtColor(img_input, img_gray, CV_BGR2GRAY);
    else
      img_input.copyTo(img_gray);

    switch (img_gray.type())
    {
      case CV_8SC1: VARLBP_<char>(img_gray, img_output); break;
      case CV_8UC1: VARLBP_<unsigned char>(img_gray, img_output); break;
      case CV_16SC1: VARLBP_<short>(img_gray, img_output); break;
      case CV_16UC1: VARLBP_<unsigned short>(img_gray, img_output); break;
      case CV_32SC1: VARLBP_<int>(img_gray, img_output); break;
      case CV_32FC1: VARLBP_<float>(img_gray, img_output); break;
      case CV_64FC1: VARLBP_<double>(img_gray, img_output); break;
    }
  }
}
