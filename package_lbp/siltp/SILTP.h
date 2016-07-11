#include <iostream>
#include <opencv2/opencv.hpp>

#include "../LBP.h"

namespace lbplibrary
{
  class SILTP : public LBP
  {
  private:
    float tau;    // the scale parameter (>0) of SILTP. The default value is 0.03. Recommended interval 0.004 to 0.15. 
    /* Smaller tau results in better discrimination but would be more sensitive to noise.
    Larger tau results in better robustness to noise but is less discriminative.
    Generally tau in [0.01,0.08] gives a good balance between discrimination and robustness to noise.
    */
    int r;        // the radius parameter of SILTP. It should be a positive integer (default is 1).
    /* R is the neighborhood size or the scale parameter like in LBP.
    Generally, smaller R (e.g. R=1) results in better discrimination of fine details of image.
    Larger R can capture larger scale of structure, but is less discriminative.
    */
    int numPoints;    // the number of neighboring pixels, which can be 4 or 8 (default is 4).
    /* numPoints=4 results in lower feature dimensions. numPoints=8 results in better description but with longer feature length. */
    int encoder;      // the way of encoding; default is 0
    /* 0: encoded as 0 ~ 3^numPoints-1, suitable for histogram calculation.
    1: encoded as 0 ~ 2^(2*numPoints), as the way in the reference paper, suitable for calculating hamming distance. */

  public:
    SILTP();
    ~SILTP();

    void run(const cv::Mat &img_input, cv::Mat &img_output);
  };
}
