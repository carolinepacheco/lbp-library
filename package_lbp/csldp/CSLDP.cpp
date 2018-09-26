#include <iostream>
#include <assert.h>
#include <algorithm>

#include "CSLDP.h"

namespace lbplibrary
{
  CSLDP::CSLDP() : bilinearInterpolation(0), fxRadius(1), fyRadius(1), borderLength(1)
  {
    std::cout << "CSLDP()" << std::endl;
  }

  CSLDP::~CSLDP()
  {
    std::cout << "~CSLBP()" << std::endl;
  }

  void processCSLDP(cv::Mat &gray, cv::Mat &out, int fxRadius, int fyRadius, const int neighborPoints[3], int borderLength, int bilinearInterpolation)
  {
    int height = gray.size().height;
    int width = gray.size().width;

    int xyNeighborPoints = neighborPoints[0];
    double nDim = pow(2.0, xyNeighborPoints / 2.0);
    cv::Mat histogram(1, nDim, CV_8UC1);

    int p1XY, pXY;

    if (bilinearInterpolation == 0)
    {
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

          // loop to calculate CS-LDP
          for (int kXY = 0; kXY < 4; kXY++)
          {
            if (kXY == 0)
            {
              p1XY = vXYNeighbor[0] - centerVal;
              pXY = vXYNeighbor[4] - centerVal;
            }

            if (kXY == 1)
            {
              p1XY = vXYNeighbor[7] - centerVal;
              pXY = vXYNeighbor[3] - centerVal;
            }

            if (kXY == 2)
            {
              p1XY = vXYNeighbor[6] - centerVal;
              pXY = vXYNeighbor[2] - centerVal;
            }

            if (kXY == 3)
            {
              p1XY = vXYNeighbor[5] - centerVal;
              pXY = vXYNeighbor[1] - centerVal;
            }

            int currentVal = (p1XY*pXY);

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
  }

  void CSLDP::run(const cv::Mat &input, cv::Mat &CSLDP)
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

    int neighborPoints[3] = { 8, 8, 8 };
    //int xyNeighborPoints = neighborPoints[0];

    // create output background model
    /*cv::Mat*/ CSLDP = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
    memset(CSLDP.data, 0x00, CSLDP.cols * CSLDP.rows);

    // compute CSLDP
    processCSLDP(gray, CSLDP, fxRadius, fyRadius, neighborPoints, borderLength, bilinearInterpolation);
  }
}
