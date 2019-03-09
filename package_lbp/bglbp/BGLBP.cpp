#include <iostream>
#include <assert.h>

#include "BGLBP.h"

namespace lbplibrary
{
  BGLBP::BGLBP() : beta(3), filterDim(3), neighbours(1)
  {
    std::cout << "BGLBP()" << std::endl;
  }

  BGLBP::~BGLBP()
  {
    std::cout << "~BGLBP()" << std::endl;
  }

  void BGLBP::run(const cv::Mat &input, cv::Mat &BGLBP)
  {
    if (input.empty())
      return;

    //int rows = input.size().height;
    //int cols = input.size().width;
    int channels = input.channels();

    // convert input image to grayscale
    cv::Mat gray;
    if (channels > 1)
      cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else
      gray = input.clone();

    // for test
    gray.data[0] = 9;
    gray.data[1] = 7;
    gray.data[2] = 3;
    gray.data[0 + gray.cols] = 2;
    gray.data[1 + gray.cols] = 3;
    gray.data[2 + gray.cols] = 2;
    gray.data[0 + 2*gray.cols] = 1;
    gray.data[1 + 2*gray.cols] = 6;
    gray.data[2 + 2*gray.cols] = 8;

    // padd image with zeroes to deal with the edges
    int padding = neighbours;
    int binaryDescriptor = 0;
    cv::Mat paddedImage = gray;
    cv::copyMakeBorder(gray, paddedImage, padding, padding, padding, padding, cv::BORDER_CONSTANT, 0x00);

    // create output image and set its elements on zero
    BGLBP = cv::Mat(gray.rows, gray.cols, CV_8UC1);
    memset(BGLBP.data, 0x00, BGLBP.cols * BGLBP.rows);

    int* g_i = new int[filterDim*filterDim];
    int* g_p_2 = new int[filterDim*filterDim];
    float* weightVec = new float[filterDim*filterDim];
    for (int i = 0; i < filterDim*filterDim; i++)
      weightVec[i] = pow(2., i);

    int diametralPositionY[9] = {1, 0, -1, 1, 0, -1, 1, 0, -1};
    int diametralPositionX[9] = {1, 1, 1, 0, 0, 0, -1, -1, -1};

    // process image
    for (int i = 1; i < (gray.rows - padding); i++)
    {
      for (int j = 1; j < (gray.cols - padding); j++)
      {
        // current central pixel
        unsigned char* grayData = &gray.data[j + i * gray.cols];
        //int centralPixel = gray.data[j + i * gray.cols];

        // 3x3 neighborhood
        float sum = 0;
        float m = 0;
        binaryDescriptor = 0;
        int loop_counter = 0;
        
        for (int rowLocal = -1; rowLocal <= padding; rowLocal++)
        {
          for (int colLocal = -1; colLocal <= padding; colLocal++)
          {
            sum += grayData[colLocal + rowLocal * gray.cols];

            g_i[loop_counter] = grayData[colLocal + rowLocal * gray.cols];
            g_p_2[loop_counter] = grayData[diametralPositionY[loop_counter] + (diametralPositionX[loop_counter]) * gray.cols];
            loop_counter++;
          }
        }
          m = sum / (filterDim*filterDim);

          // save output value
          for (int k = 0; k < filterDim*filterDim; k++)
          {
            int v;
            if ( ( ((g_i[k] >= m) && (m >= g_p_2[k])) || ((g_i[k]< m) && (m < g_p_2[k])) ) && ((abs(g_i[k]-m) + abs(g_p_2[k]-m)) >= beta ) )
              v = 1; // code 1
            else
              v = 0; // code 0

          binaryDescriptor += weightVec[k] * v;
        }
        BGLBP.data[j + BGLBP.cols * i] = binaryDescriptor;
     }
   }
 }
}
