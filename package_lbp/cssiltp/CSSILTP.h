#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class CSSILTP : public LBP
  {
  private:
    int filterDim;
    int numberOfWeights;
    float tau;

  public:
    CSSILTP();
    ~CSSILTP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
