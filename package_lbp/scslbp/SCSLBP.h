#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class SCSLBP : public LBP
  {
  private:
    int radius;
    int neighbors;

    void lbpcompute(const cv::Mat &input, cv::Mat &LBPImage);

    inline float roundLocal(float d)
    {
      return floor(d + 0.5);
    }

  public:
    SCSLBP();
    ~SCSLBP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
