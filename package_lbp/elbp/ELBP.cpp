#include <iostream>
#include <assert.h>
#include <algorithm>

#include "ELBP.h"

namespace lbplibrary
{
  ELBP::ELBP() : radius(1), neighbors(8)
  {
    std::cout << "ELBP()" << std::endl;
  }

  ELBP::~ELBP()
  {
    std::cout << "~ELBP()" << std::endl;
  }

  template <typename _Tp>
  void ELBP::ELBP_(const cv::Mat& src, cv::Mat& dst)
  {
    neighbors = std::max(std::min(neighbors, 31), 1); // set bounds...
    // Note: alternatively you can switch to the new OpenCV Mat_
    // type system to define an unsigned int matrix... I am probably
    // mistaken here, but I didn't see an unsigned int representation
    // in OpenCV's classic typesystem...
    dst = cv::Mat::zeros(src.rows - 2 * radius, src.cols - 2 * radius, CV_32SC1);
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
          // we are dealing with floating point precision, so add some little tolerance
          dst.at<unsigned int>(i - radius, j - radius) += ((t > src.at<_Tp>(i, j)) && (abs(t - src.at<_Tp>(i, j)) > std::numeric_limits<float>::epsilon())) << n;
        }
      }
    }
  }

  void ELBP::run(const cv::Mat &img_input, cv::Mat &img_output)
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
      case CV_8SC1: ELBP_<char>(img_gray, img_output); break;
      case CV_8UC1: ELBP_<unsigned char>(img_gray, img_output); break;
      case CV_16SC1: ELBP_<short>(img_gray, img_output); break;
      case CV_16UC1: ELBP_<unsigned short>(img_gray, img_output); break;
      case CV_32SC1: ELBP_<int>(img_gray, img_output); break;
      case CV_32FC1: ELBP_<float>(img_gray, img_output); break;
      case CV_64FC1: ELBP_<double>(img_gray, img_output); break;
    }
  }
}
