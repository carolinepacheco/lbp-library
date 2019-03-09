#include <iostream>
#include <assert.h>

#include "CSSILTP.h"

namespace lbplibrary
{
  CSSILTP::CSSILTP() : filterDim(3), numberOfWeights(4), tau(0.03)
  {
    std::cout << "CSSILTP()" << std::endl;
  }

  CSSILTP::~CSSILTP()
  {
    std::cout << "~CSSILTP()" << std::endl;
  }

  void CSSILTP::run(const cv::Mat &input, cv::Mat &CSSILTP)
  {
    if (input.empty())
      return;

    //const int N = 255; // max value of u8 pixel
    const int weights = numberOfWeights;
    //int rows = input.size().height;
    //int cols = input.size().width;
    int channels = input.channels();

    // convert input image to grayscale
    cv::Mat gray;
    if (channels > 1)
      cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else
      gray = input.clone();

    // create filter
    //CvMat* filter = cvCreateMat(filterDim, filterDim, CV_8UC1);

    // verify filter dimensions are odd, so a middle element always exists
    assert(filterDim == (filterDim + 1 - filterDim % 2));

    //int nNeighbours = filterDim * filterDim - 1;
    //int nWeights = numberOfWeights;

    int filterRadius[2] = { static_cast<int>(floor(filterDim / 2.)), static_cast<int>(floor(filterDim / 2.)) };

    // padd image with zeroes to deal with the edges
    cv::Mat paddedImage = gray;
    cv::copyMakeBorder(gray, paddedImage, filterRadius[0], filterRadius[0], filterRadius[0], filterRadius[0], cv::BORDER_CONSTANT, 0x00);

    // compute weights
    unsigned char* P = new unsigned char[weights];
    unsigned char* S = new unsigned char[numberOfWeights];
    float* weightVec = new float[numberOfWeights];
    for (int i = 0; i < numberOfWeights; i++)
      weightVec[i] = pow(3., i);

    // create output image and set its elements on zero
    CSSILTP = cv::Mat(gray.rows, gray.cols, CV_8UC1, cv::Scalar(0));

    // compute the upper and lower range
    cv::Mat inF32(input.rows, input.cols, CV_32FC1);
    cv::Mat lowRangeF32(input.rows, input.cols, CV_32FC1);
    cv::Mat upperRangeF32(input.rows, input.cols, CV_32FC1);

    input.convertTo(inF32, CV_32F);
    lowRangeF32 = (1 - tau) * inF32;
    upperRangeF32 = (1 + tau) * inF32;

    cv::Mat L, U;
    lowRangeF32.convertTo(L, CV_8U);
    upperRangeF32.convertTo(U, CV_8U);

    cv::Mat subImg;
    cv::Rect roi;
    int* central = new int[numberOfWeights];
    int* neighborhood = new int[numberOfWeights];
    int* neighborhood1 = new int[numberOfWeights];
    int* diffVec = new int[numberOfWeights];
    int* lowerVal = new int[numberOfWeights];
    int* upperVal = new int[numberOfWeights];

    // process image
    for (int i = 0; i < (paddedImage.rows - filterRadius[0] - 1); i++)
    {
      for (int j = 0; j < (paddedImage.cols - filterRadius[0] - 1); j++)
      {
        roi = cv::Rect(j, i, filterDim, filterDim);
        paddedImage(roi).copyTo(subImg);

        // neighborhood N0 to N3
        P[0] = subImg.data[2 + subImg.cols * 1];
        P[1] = subImg.data[2 + subImg.cols * 2];
        P[2] = subImg.data[1 + subImg.cols * 2];
        P[3] = subImg.data[0 + subImg.cols * 2];

        // neighborhood N4 to N7
        S[0] = subImg.data[0 + subImg.cols * 1];
        S[1] = subImg.data[0 + subImg.cols * 0];
        S[2] = subImg.data[1 + subImg.cols * 0];
        S[3] = subImg.data[2 + subImg.cols * 0];

        // value of the center pixel
        central[0] = central[1] = central[2] = central[3] = subImg.data[1 + subImg.cols * 1];

        // lower and upper limits
        lowerVal[0] = lowerVal[1] = lowerVal[2] = lowerVal[3] = L.data[1 + subImg.cols * 1];
        upperVal[0] = upperVal[1] = upperVal[2] = upperVal[3] = U.data[1 + subImg.cols * 1];

        //neighborhood subtraction
        for (int k = 0; k < numberOfWeights; k++)
        {
          neighborhood[k] = P[k] - central[k];
          neighborhood1[k] = S[k] - central[k];
          diffVec[k] = neighborhood[k] * neighborhood1[k];
        }

        // save output value
        int value = 0;
        for (int k = 0; k < numberOfWeights; k++)
        {
          int v;
          if (diffVec[k] < lowerVal[k])
            v = 3; // code 10
          else
            if (diffVec[k] > upperVal[k])
              v = 1; // code 01
            else
              v = 0; // code 00

          value += weightVec[k] * v;
        }
        CSSILTP.data[j + CSSILTP.cols * i] = value;
      }
    }
  }
}
