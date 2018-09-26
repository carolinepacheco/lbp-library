#include <iostream>
#include <assert.h>
#include <algorithm>

#include "SCSLBP.h"

namespace lbplibrary
{
  SCSLBP::SCSLBP() : radius(2), neighbors(4)
  {
    std::cout << "SCSLBP()" << std::endl;
  }

  SCSLBP::~SCSLBP()
  {
    std::cout << "~SCSLBP()" << std::endl;
  }

  // compute the scs-lbp code image
  void SCSLBP::lbpcompute(const cv::Mat &input1, cv::Mat &LBPImage)
  {
    int neighbors_ = 2 * neighbors;

    float y, x, yd, xd, v;
    int fy, fx, cy, cx, ry, rx;
    int fyd, fxd, cyd, cxd, ryd, rxd;
    float miny = 999, minx = 999;
    float maxy = 0, maxx = 0;
    //int mapping = 0;
    float spoints[2][2 * 4];

    // angle step
    float a = 2 * CV_PI / neighbors_;
    for (int i = 0; i < neighbors_; i++)
    {
      spoints[0][i] = -(float)radius * sin(i*a);
      spoints[1][i] = (float)radius * cos(i*a);

      if (spoints[0][i] < miny)
        miny = spoints[0][i];

      if (spoints[0][i] > maxy)
        maxy = spoints[0][i];

      if (spoints[1][i] < minx)
        minx = spoints[1][i];

      if (spoints[1][i] > maxx)
        maxx = spoints[1][i];

    }

    // block size, each LBP code is computed within a block of size bsizey*bsizex
    int bsizey = ceil(std::max(maxy, (float)0.0)) - floor(std::min(miny, (float)0.0)) + 1;
    int bsizex = ceil(std::max(maxx, (float)0.0)) - floor(std::min(minx, (float)0.0)) + 1;

    // coordinates of origin (0,0) in the block
    int origy = 1 - floor(std::min(miny, (float)0.0)) - 1;
    int origx = 1 - floor(std::min(minx, (float)0.0)) - 1;

    // minimum allowed size for the input image depends on the radius of the used LBP operator
    if (input1.cols < bsizex || input1.rows < bsizey)
      std::cout << "Too small input image. Should be at least " << (2 * radius + 1) * (2 * radius + 1) << std::endl;

    // calculate dx and dy
    int dx = input1.cols - bsizex + 1;
    int dy = input1.rows - bsizey + 1;

    cv::Mat comImage, comImageCen;
    cv::Rect roi = cv::Rect(origx, origy, dx, dy); // because of transalation of coord from matlab
    input1(roi).copyTo(comImage);

    comImageCen = comImage.clone();

    cv::Mat result(dy, dx, CV_8UC1, cv::Scalar(0));        // sign LBP computing image
    cv::Mat D(comImage.rows, comImage.cols, CV_8UC1);
    cv::Mat Diff(comImage.rows, comImage.cols, CV_8UC1);

    cv::Mat newImage(comImage.rows, comImage.cols, CV_32FC1);
    cv::Mat newImageD(comImage.rows, comImage.cols, CV_32FC1);
    cv::Mat inputFloat(input1.rows, input1.cols, CV_32FC1);
    input1.convertTo(inputFloat, CV_32F);

    float* newImageData = (float*)newImage.data;
    float* newImageDData = (float*)newImageD.data;

    for (int i = 0; i < neighbors_ / 2; i++)
    {
      y = spoints[0][i] + origy;
      x = spoints[1][i] + origx;

      // calculate floors, ceils and rounds for the x and y.
      fy = floor(y); cy = ceil(y); ry = roundLocal(y);
      fx = floor(x); cx = ceil(x); rx = roundLocal(x);

      yd = spoints[0][i + neighbors_ / 2] + origy;
      xd = spoints[1][i + neighbors_ / 2] + origx;

      fyd = floor(yd); cyd = ceil(yd); ryd = roundLocal(yd);
      fxd = floor(xd); cxd = ceil(xd); rxd = roundLocal(xd);

      //int a = abs(x - rx);
      //int b = abs(y - ry);

      // check if interpolation is needed
      if ((abs(x - rx) < 1 / 100000.0) && (abs(y - ry) < 1 / 100000.0))
      {
        // --- interpolation is not needed, use original datatypes ---
        inputFloat(cv::Rect(rx, ry, dx, dy)).copyTo(newImage);
        inputFloat(cv::Rect(rxd, ryd, dx, dy)).copyTo(newImageD);

        // compute difference between newImage and newImageD
        for (int iRow = 0; iRow < newImage.rows; iRow++)
          for (int iCol = 0; iCol < newImage.cols; iCol++)
          {
            if (newImageData[iCol + newImage.cols * iRow] - newImageDData[iCol + newImageD.cols * iRow] >= 0)
              D.data[iCol + D.cols * iRow] = 1;
            else
              D.data[iCol + D.cols * iRow] = 0;
          }
      }
      else
      {
        // --- interpolation needed, use double type images ---
        float ty = y - fy;
        float tx = x - fx;

        // calculate the interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 = tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 = tx  *      ty;

        // compute interpolated pixel values
        cv::Mat dImage1, dImage2, dImage3, dImage4;
        inputFloat(cv::Rect(fx, fy, dx, dy)).copyTo(dImage1);
        inputFloat(cv::Rect(cx, fy, dx, dy)).copyTo(dImage2);
        inputFloat(cv::Rect(fx, cy, dx, dy)).copyTo(dImage3);
        inputFloat(cv::Rect(cx, cy, dx, dy)).copyTo(dImage4);

        newImage = w1*dImage1 + w2*dImage2 + w3*dImage3 + w4*dImage4;

        /////////////////////////////////////////
        float tyd = yd - fyd;
        float txd = xd - fxd;

        float w1d = (1 - txd) * (1 - tyd);
        float w2d = txd  * (1 - tyd);
        float w3d = (1 - txd) *      tyd;
        float w4d = txd  *      tyd;

        inputFloat(cv::Rect(fxd, fyd, dx, dy)).copyTo(dImage1);
        inputFloat(cv::Rect(cxd, fyd, dx, dy)).copyTo(dImage2);
        inputFloat(cv::Rect(fxd, cyd, dx, dy)).copyTo(dImage3);
        inputFloat(cv::Rect(cxd, cyd, dx, dy)).copyTo(dImage4);

        newImageD = w1d*dImage1 + w2d*dImage2 + w3d*dImage3 + w4d*dImage4;

        // compute difference between newImage and newImageD
        for (int iRow = 0; iRow < newImage.rows; iRow++)
          for (int iCol = 0; iCol < newImage.cols; iCol++)
          {
            if (newImageData[iCol + newImage.cols * iRow] - newImageDData[iCol + newImageD.cols * iRow] >= 0)
              D.data[iCol + D.cols * iRow] = 1;
            else
              D.data[iCol + D.cols * iRow] = 0;
          }
      }

      // update the result matrix
      v = std::pow(2.0, i);
      result = result + (int)v*D;
    }

    // save results taking into account margins
    result.copyTo(LBPImage(cv::Rect(origx, origy, dx, dy)));
  }

  void SCSLBP::run(const cv::Mat &input, cv::Mat &SCSLBP)
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

    SCSLBP = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
    lbpcompute(gray, SCSLBP);
  }
}
