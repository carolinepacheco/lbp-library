#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class BGLBP : public LBP
  {
  private:
    int cols;
    int rows;
    int beta;
    int filterDim;
    int neighbours;

  public:
    BGLBP();
    ~BGLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}