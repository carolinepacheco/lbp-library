#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class OLBP : public LBP
  {
  private:
    template <typename _Tp>
    void OLBP_(const cv::Mat& src, cv::Mat& dst);

  public:
    OLBP();
    ~OLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
