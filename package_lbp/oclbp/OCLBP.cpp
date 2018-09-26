#include <iostream>
#include <assert.h>
#include <algorithm>

#include "OCLBP.h"

namespace lbplibrary
{
  OCLBP::OCLBP() : isEfficient(1), filtDimsX(3), filtDimsY(3)
  {
    std::cout << "OCLBP()" << std::endl;
  }

  OCLBP::~OCLBP()
  {
    std::cout << "~OCLBP()" << std::endl;
  }

  int product8(cv::Mat &diffVecPositive, int weightVec[8], int nNeighbours)
  {
    int retVal = 0;

    // convert 0xFF elements of diffVecPositive to 0x01
    for (int i = 0; i < nNeighbours; i++)
    {
      if (diffVecPositive.data[i] == 255)
        diffVecPositive.data[i] = 1;
    }

    // compute cumulative product between the two vectors
    for (int i = 0; i < nNeighbours; i++)
    {
      retVal += diffVecPositive.data[i] * weightVec[i];
    }

    return retVal;
  }

  void efficientLBP(const cv::Mat &image, cv::Mat &efficientLBPImage, const int filter[2], int isEfficient)
  {
    cv::Mat gray;
    if (image.channels() == 3)
      cv::cvtColor(image, gray, CV_BGR2GRAY);
    else
      gray = image;

    // verify filter dimensions are odd, so a middle element always exists
    assert(filter[0] == (filter[0] + 1 - filter[0] % 2));
    assert(filter[1] == (filter[1] + 1 - filter[1] % 2));

    int nNeigh = filter[0] * filter[1] - 1;

    cv::Mat iHelix(1, filter[0] * filter[1], CV_8UC1);
    iHelix.data[0] = 0; iHelix.data[1] = 1; iHelix.data[2] = 2;
    iHelix.data[3] = 5; iHelix.data[4] = 8; iHelix.data[5] = 7;
    iHelix.data[6] = 6; iHelix.data[7] = 3; iHelix.data[8] = 4;

    cv::Mat iNeight(1, nNeigh, CV_8UC1);
    for (int i = 0; i < nNeigh; i++)
      iNeight.data[i] = iHelix.data[i];

    int filtCenter = ceil((float)((nNeigh + 1) / 2.0)) - 1;

    cv::Mat filt(filter[0], filter[1], CV_32FC1, cv::Scalar(0));
    float* filtData = (float*)filt.data;

    // Better filtering/convolution based approach
    if (isEfficient)
    {
      filtData[filtCenter] = 1.0;
      filtData[iNeight.data[0]] = -1.0;

      unsigned char* sumLBPData = efficientLBPImage.data;
      memset(sumLBPData, 0x00, efficientLBPImage.step * efficientLBPImage.rows);

      for (int i = 0; i < nNeigh; i++)
      {
        cv::Mat currNieghDiff(gray.rows, gray.cols, CV_32FC1);
        float* currNieghDiffData = (float*)currNieghDiff.data;
        cv::filter2D(gray, currNieghDiff, currNieghDiff.depth(), filt);

        int cnt = 0;
        for (int j = 0; j < currNieghDiff.rows; j++)
        {
          for (int k = 0; k < currNieghDiff.cols; k++)
          {
            sumLBPData[cnt++] += (currNieghDiffData[k + j*currNieghDiff.cols] > 0) ? pow(2.0, i) : 0;
          }
        }
        // update filter
        if (i < nNeigh)
        {
          filtData[iNeight.data[i]] = 0;
          filtData[iNeight.data[i + 1]] = -1;
        }
      }
    }
  }

  void processOCLBP(const cv::Mat &in, std::vector<cv::Mat> &out, const int filter[2], const int filterR[2], int nComb, const int chnsComb[6][2], int isEfficient)
  {
    int heightIn = in.size().height;
    int widthIn = in.size().width;
    int channels = in.channels();

    // create a vector of input images of u8 to store each channel
    std::vector<cv::Mat> inputImage(nComb);
    cv::Mat sumLBP(heightIn, widthIn, CV_8UC1, cv::Scalar(0));

    // create a vector of output padded images
    std::vector<cv::Mat> outputImage(nComb);

    // compute number of neibghbors based on filter's dimensions
    int nNeighbours = filter[0] * filter[0] - 1;
    int filtCenter = ceil((float)((nNeighbours + 1) / 2));

    // create neighbors mask
    cv::Mat neightMat(filter[0], filter[1], CV_8UC1, cv::Scalar(1));
    neightMat.data[filtCenter] = 0;

    // assign weights to each neighbor
    int weightVec[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // declare Mat objects needed
    cv::Mat subImgMasked(1, nNeighbours, CV_32FC1);
    cv::Mat diffVec(1, nNeighbours, CV_32FC1);
    cv::Mat diffVecPositive(1, nNeighbours, CV_8UC1);

    float* subImgMaskedData = (float*)subImgMasked.data;
    //float* diffVecData = (float*)diffVec.data;

    // process each channel
    for (int i = 0; i < nComb; i++)
    {
      int iCurrCenterClr = chnsComb[i][0] - 1;
      int iCurrNeighClr = chnsComb[i][1] - 1;

      inputImage[i] = cv::Mat(heightIn, widthIn, CV_8UC1);

      if (iCurrCenterClr == iCurrNeighClr && isEfficient)
      {
        for (int iRow = 0; iRow < inputImage[i].rows; iRow++)
        {
          for (int iCol = 0; iCol < inputImage[i].cols; iCol++)
          {
            // each channel will account for one image of u8
            inputImage[i].data[iCol + iRow * inputImage[i].step] = in.data[iCol * channels + iCurrCenterClr + iRow * in.step];
          }
        }
        // compute efficient LBP and save it in output vector
        efficientLBP(inputImage[i], sumLBP, filter, isEfficient);
        outputImage[i] = sumLBP;
        continue;
      }

      outputImage[i] = cv::Mat(heightIn, widthIn, CV_8UC1);
      for (int iRow = 0; iRow < inputImage[i].rows; iRow++)
      {
        for (int iCol = 0; iCol < inputImage[i].cols; iCol++)
        {
          inputImage[i].data[iCol + iRow * inputImage[i].step] = in.data[iCol * channels + iCurrNeighClr + iRow * in.step];
        }
      }

      for (int iRow = filterR[0]; iRow < (inputImage[i].rows - filterR[0]); iRow++)
      {
        for (int iCol = filterR[1]; iCol < (inputImage[i].cols - filterR[1]); iCol++)
        {
          // crop image according to filter's dimensions
          cv::Mat subImg(filter[1], filter[0], CV_8UC1);
          cv::Rect roi = cv::Rect(iCol - filterR[0], iRow - filterR[1], filter[0], filter[1]);
          inputImage[i](roi).copyTo(subImg);

          // find differences between current pixel, and it's neighours
          int element = in.data[iCol * channels + iCurrCenterClr + iRow * in.step];
          cv::Mat replicatedElement(1, nNeighbours, CV_32FC1, cv::Scalar(element));

          int cnt = 0;
          for (int k = 0; k < neightMat.rows * neightMat.cols; k++)
          {
            if (neightMat.data[k] == 1)
            {
              subImgMaskedData[cnt] = subImg.data[k];
              cnt++;
            }
          }

          diffVec = replicatedElement - subImgMasked;
          diffVecPositive = diffVec > 0;

          outputImage[i].data[iCol + iRow*outputImage[i].step] = product8(diffVecPositive, weightVec, nNeighbours);
        }
      }
    }

    // crop the margins resulting from zero padding
    for (int i = 0; i < nComb; i++)
      outputImage[i](cv::Rect(1, 1, out[i].cols, out[i].rows)).copyTo(out[i]);
  }

  void OCLBP::run(const cv::Mat &input, std::vector<cv::Mat> &OCLBP)
  {
    if (input.empty())
      return;

    int height = input.size().height;
    int width = input.size().width;
    int channels = input.channels();

    // convert OpenCV's BGR style to Matlab's RGB style
    cv::Mat imageRGBToAlignWithMatlab;
    cv::cvtColor(input, imageRGBToAlignWithMatlab, CV_BGR2RGB);

    // go on with Matlab style RGB to be able to compare results with Matlab
    imageRGBToAlignWithMatlab.copyTo(input);

    // verify filter dimensions are odd, so a middle element always exists
    assert(filtDimsX == (filtDimsX + 1 - filtDimsX % 2));
    assert(filtDimsY == (filtDimsY + 1 - filtDimsY % 2));

    // Primitive pixelwise solution
    int filtDims[2] = { filtDimsX, filtDimsY };
    int filtDimsR[2] = { static_cast<int>(floor(filtDimsX / 2.)), static_cast<int>(floor(filtDimsY / 2.)) };

    // padd image with zeroes to deal with the edges
    cv::Mat paddedImage = input;
    cv::copyMakeBorder(input, paddedImage, filtDimsR[0], filtDimsR[0], filtDimsR[0], filtDimsR[0], cv::BORDER_CONSTANT, 0x00);

    int nComb = 2 * channels;
    int chnsComb[6][2] = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 1, 2 }, { 1, 3 }, { 2, 3 } };

    // create output background model   
    //std::vector<cv::Mat> oclbp(nComb);
    OCLBP = std::vector<cv::Mat>(nComb);
    for (int i = 0; i < nComb; i++)
      OCLBP[i] = cv::Mat(height, width, CV_8UC1, cv::Scalar::all(0));

    // compute OCLBP 
    processOCLBP(paddedImage, OCLBP, filtDims, filtDimsR, nComb, chnsComb, isEfficient);
  }
}
