#include <iostream>
#include <assert.h>

#include "CSLBP.h"

namespace lbplibrary
{
  CSLBP::CSLBP() : filterDim(3), numberOfWeights(4)
  {
    std::cout << "CSLBP()" << std::endl;
  }

  CSLBP::~CSLBP()
  {
    std::cout << "~CSLBP()" << std::endl;
  }

  void CSLBP::run(const cv::Mat &input, cv::Mat &CSLBP)
  {
    if (input.empty())
      return;

    const int weights = numberOfWeights;
    //int rows = input.size().height;
    //int cols = input.size().width;
    int channels = input.channels();

    // convert input image to grayscale
    cv::Mat gray;
    if (channels > 1)
      cv::cvtColor(input, gray, CV_BGR2GRAY);
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
      weightVec[i] = pow(2., i);

    // create output image and set its elements on zero
    /*cv::Mat*/ CSLBP = cv::Mat(gray.rows, gray.cols, CV_8UC1);
    memset(CSLBP.data, 0x00, CSLBP.cols * CSLBP.rows);

    cv::Mat subImg;
    cv::Rect roi;
    int* central = new int[numberOfWeights];
    int* neighborhood = new int[numberOfWeights];
    int* neighborhood1 = new int[numberOfWeights];
    int* diffVec = new int[numberOfWeights];

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
          int v = (diffVec[k] > 0.01) ? 1 : 0;
          value += weightVec[k] * v;
        }
        CSLBP.data[j + CSLBP.cols * i] = value;
      }
    }
  }
}
