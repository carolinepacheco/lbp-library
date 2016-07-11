#pragma once

#include <opencv2/opencv.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

namespace lbplibrary
{
  class LBP
  {
  public:
    virtual void run(const cv::Mat &img_input, cv::Mat &img_output){};
    virtual void run(const cv::Mat &img_input, std::vector<cv::Mat> &vec_output){};
    virtual ~LBP(){}
  };
}
