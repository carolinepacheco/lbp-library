#include <iostream>
#include <assert.h>
#include <algorithm>

#include "XCSLBP.h"

namespace lbplibrary
{
  XCSLBP::XCSLBP() : fxRadius(1), fyRadius(1), borderLength(1)
  {
    std::cout << "XCSLBP()" << std::endl;
  }

  XCSLBP::~XCSLBP()
  {
    std::cout << "~XCSLBP()" << std::endl;
  }

  void processXCSLBP(cv::Mat &gray, cv::Mat &out, int fxRadius, int fyRadius, const int neighborPoints[3], int borderLength)
  {
    int height = gray.size().height;
    int width = gray.size().width;
    int xyNeighborPoints = neighborPoints[0];
    double nDim = pow(2.0, xyNeighborPoints / 2.0); // dimensionality of basic XC-LBP

    cv::Mat histogram(1, nDim, CV_8UC1);

    int p1XY, pXY, pcXY;

    for (int yc = borderLength; yc < (height - borderLength); yc++)
    {
      for (int xc = borderLength; xc < (width - borderLength); xc++)
      {
        // obtain center value
        int centerVal = gray.data[xc + yc * gray.cols];

        // XY plane
        int basicLBP = 0;
        int featureBin = 0;
        int countXY = 0;
        int vXYNeighbor[8];

        // store the neighbors in a vector
        for (int k = 0; k < xyNeighborPoints; k++)
        {
          float x = floor((double)xc + (double)fxRadius * cos((2 * M_PI * k) / xyNeighborPoints) + 0.5);
          float y = floor((double)yc - (double)fyRadius * sin((2 * M_PI * k) / xyNeighborPoints) + 0.5);

          vXYNeighbor[k] = gray.data[(int)x + (int)y * gray.cols];
          countXY++;
        }

        // loop to calculate XCSLBP
        for (int kXY = 0; kXY < 4; kXY++)
        {
          if (kXY == 0)
          {
            p1XY = vXYNeighbor[0] - vXYNeighbor[4];
            pXY = vXYNeighbor[0] - centerVal;
            pcXY = vXYNeighbor[4] - centerVal;
          }

          if (kXY == 1)
          {
            p1XY = vXYNeighbor[7] - vXYNeighbor[3];
            pXY = vXYNeighbor[2] - centerVal;
            pcXY = vXYNeighbor[6] - centerVal;
          }

          if (kXY == 2)
          {
            p1XY = vXYNeighbor[6] - vXYNeighbor[2];
            pXY = vXYNeighbor[1] - centerVal;
            pcXY = vXYNeighbor[5] - centerVal;
          }

          if (kXY == 3)
          {
            p1XY = vXYNeighbor[5] - vXYNeighbor[1];
            pXY = vXYNeighbor[3] - centerVal;
            pcXY = vXYNeighbor[7] - centerVal;
          }

          int currentVal = (p1XY + centerVal) + (pXY*pcXY);

          if (currentVal <= 0)
            basicLBP += pow(2.0, featureBin);

          featureBin++;
        }
        // save pixel in output
        out.data[xc + yc * gray.cols] = basicLBP;
        histogram.data[basicLBP] += 1;
      }
    }
  }

  void XCSLBP::run(const cv::Mat &input, cv::Mat &XCSLBP)
  {
    if (input.empty())
      return;

    int height = input.size().height;
    int width = input.size().width;
    int channels = input.channels();

    // convert input image to grayscale
    cv::Mat gray;
    if (channels > 1)
      cv::cvtColor(input, gray, CV_BGR2GRAY);
    else
      gray = input.clone();

    const int neighborPoints[3] = { 8, 8, 8 };
    //int xyNeighborPoints = neighborPoints[0];

    // create output background model
    /*cv::Mat*/ XCSLBP = cv::Mat(height, width, CV_8UC1);
    memset(XCSLBP.data, 0x00, XCSLBP.cols * XCSLBP.rows);

    // compute XCSLBP 
    processXCSLBP(gray, XCSLBP, fxRadius, fyRadius, neighborPoints, borderLength);
  }
}
