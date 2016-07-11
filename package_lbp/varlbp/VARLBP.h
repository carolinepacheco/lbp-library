#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class VARLBP : public LBP
  {
  private:
    int radius;
    int neighbors;

    template <typename _Tp>
    void VARLBP_(const cv::Mat& src, cv::Mat& dst);

  public:
    VARLBP();
    ~VARLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
