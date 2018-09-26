#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class CSLDP : public LBP
  {
  private:
    int bilinearInterpolation;
    int fxRadius;
    int fyRadius;
    int borderLength;

  public:
    CSLDP();
    ~CSLDP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
