#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class ELBP : public LBP
  {
  private:
    int radius;
    int neighbors;

    template <typename _Tp>
    void ELBP_(const cv::Mat& src, cv::Mat& dst);

  public:
    ELBP();
    ~ELBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
