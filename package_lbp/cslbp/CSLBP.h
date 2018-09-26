#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class CSLBP : public LBP
  {
  private:
    int filterDim;
    int numberOfWeights;

  public:
    CSLBP();
    ~CSLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
