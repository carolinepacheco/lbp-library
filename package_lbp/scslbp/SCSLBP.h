#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class SCSLBP : public LBP
  {
  private:
    int rows;
    int cols;
    int classNum;
    float tpthresh;
    float tbthresh;
    int radius;
    int neighbors;
    int histRadius;
    float alpha;
    float alphatempral;
    int mappNb;
    int bins;
    int sigmaValues;

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
