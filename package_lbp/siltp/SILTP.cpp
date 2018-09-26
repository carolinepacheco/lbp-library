#include <iostream>
#include <assert.h>
#include <algorithm>

#include "SILTP.h"

namespace lbplibrary
{
  SILTP::SILTP() : tau(0.03), r(1), numPoints(4), encoder(0)
  {
    std::cout << "SILTP()" << std::endl;
  }

  SILTP::~SILTP()
  {
    std::cout << "~SILTP()" << std::endl;
  }

  void processSILTP(cv::Mat &in, cv::Mat &out, float tau, int radius, const int numPoints, int encoder)
  {
    int heightIn = in.size().height;
    int widthIn = in.size().width;

    const int N = 255; // max value of a u8 pixel

    // put the image in a larger container and replicate border image pixels to the outer area
    cv::Mat image_0 = in;
    cv::copyMakeBorder(in, image_0, radius, radius, radius, radius, cv::BORDER_REPLICATE);

    // declare mat images
    cv::Mat image_1, image_3, image_5, image_7;
    cv::Mat image_2, image_4, image_6, image_8;
    //int height = image_0.size().height;
    //int width = image_0.size().width;

    // copy image in specified directions
    cv::Rect roi_1(2 * radius, radius, widthIn, heightIn);
    image_0(roi_1).copyTo(image_1);

    cv::Rect roi_3(radius, 0, widthIn, heightIn);
    image_0(roi_3).copyTo(image_3);

    cv::Rect roi_5(0, radius, widthIn, heightIn);
    image_0(roi_5).copyTo(image_5);

    cv::Rect roi_7(radius, 2 * radius, widthIn, heightIn);
    image_0(roi_7).copyTo(image_7);

    if (numPoints == 8)
    {
      cv::Rect roi_2(2 * radius, 0, widthIn, heightIn);
      image_0(roi_1).copyTo(image_1);

      cv::Rect roi_3(0, 0, widthIn, heightIn);
      image_0(roi_3).copyTo(image_3);

      cv::Rect roi_5(0, 2 * radius, widthIn, heightIn);
      image_0(roi_5).copyTo(image_5);

      cv::Rect roi_7(2 * radius, 2 * radius, widthIn, heightIn);
      image_0(roi_7).copyTo(image_7);
    }

    // compute the upper and lower range
    cv::Mat inF32(heightIn, widthIn, CV_32FC1);
    cv::Mat lowRangeF32(heightIn, widthIn, CV_32FC1);
    cv::Mat upperRangeF32(heightIn, widthIn, CV_32FC1);

    in.convertTo(inF32, CV_32F);
    lowRangeF32 = (1 - tau) * inF32;
    upperRangeF32 = (1 + tau) * inF32;

    cv::Mat L, U;
    lowRangeF32.convertTo(L, CV_8U);
    upperRangeF32.convertTo(U, CV_8U);

    // compute the scale invariant local ternary patterns
    if (encoder == 0)
      if (numPoints == 4)
      {
        out = (image_1 < L) / N + (image_1 > U) / N * 2 + ((image_3 < L) / N + (image_3 > U) / N * 2) * 3 + ((image_5 < L) / N + (image_5 > U) / N * 2) * 9 + ((image_7 < L) / N + (image_7 > U) / N * 2) * 27;
      }
      else
      {
        out = (image_1 < L) + (image_1 > U) * 2 + ((image_2 < L) + (image_2 > U) * 2) * 3 + ((image_3 < L) + (image_3 > U) * 2) * pow(3.0, 2) +
          ((image_4 < L) + (image_4 > U) * 2) * pow(3.0, 3) + ((image_5 < L) + (image_5 > U) * 2) * pow(3.0, 4) + ((image_6 < L) + (image_6 > U) * 2) * pow(3.0, 5) +
          ((image_7 < L) + (image_7> U) * 2) * pow(3.0, 6) + ((image_8 < L) + (image_8 > U) * 2) * pow(3.0, 7);
      }
    else
      if (numPoints == 4)
      {
        out = (image_1 > U) + (image_1 < L) * 2 + (image_3 > U) * pow(2.0, 2) + (image_3 < L) * pow(2.0, 3) +
          (image_5 > U) * pow(2.0, 4) + (image_5 < L) * pow(2.0, 5) + (image_7 > U) * pow(2.0, 6) + (image_7 < L) * pow(2.0, 7);
      }
      else
      {
        out = (image_1 > U) + (image_1 < L) * 2 + (image_2 > U) * pow(2.0, 2) + (image_2 < L) * pow(2.0, 3) + (image_3 > U) * pow(2.0, 4) + (image_3 < L) * pow(2.0, 5) +
          (image_4 > U) * pow(2.0, 6) + (image_4 < L) * pow(2.0, 7) + (image_5 > U) * pow(2.0, 8) + (image_5 < L) * pow(2.0, 9) + (image_6 > U) * pow(2.0, 10) +
          (image_6 < L) * pow(2.0, 11) + (image_7 > U) * pow(2.0, 12) + (image_7 < L) * pow(2.0, 13) + (image_8 > U) * pow(2.0, 14) + (image_8 < L) * pow(2.0, 15);
      }
  }

  void SILTP::run(const cv::Mat &input, cv::Mat &SILTP)
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

    // create output background model
    /*cv::Mat*/ SILTP = cv::Mat(height, width, CV_8UC1);
    memset(SILTP.data, 0x00, SILTP.cols * SILTP.rows);

    // check parameters
    assert(tau > 0);
    //assert(floor(r) = ~r | r < 1); TODO

    // check images size
    assert(width > (2 * r + 1));
    assert(height > (2 * r + 1));

    // compute SILTP 
    processSILTP(gray, SILTP, tau, r, numPoints, encoder);
  }
}
