#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class XCSLBP : public LBP
  {
  private:
    int fxRadius;
    int fyRadius;
    int borderLength;
    
  public:
    XCSLBP();
    ~XCSLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
