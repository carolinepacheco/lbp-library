#include "histogram.hpp"
#include <vector>

namespace lbplibrary
{
  template <typename _Tp>
  void histogram_(const cv::Mat& src, cv::Mat& hist, int numPatterns) {
    hist = cv::Mat::zeros(1, numPatterns, CV_32SC1);
    for (int i = 0; i < src.rows; i++) {
      for (int j = 0; j < src.cols; j++) {
        int bin = src.at<_Tp>(i, j);
        hist.at<int>(0, bin) += 1;
      }
    }
  }

  template <typename _Tp>
  double chi_square_(const cv::Mat& histogram0, const cv::Mat& histogram1) {
    if (histogram0.type() != histogram1.type())
      CV_Error(CV_StsBadArg, "Histograms must be of equal type.");
    if (histogram0.rows != 1 || histogram0.rows != histogram1.rows || histogram0.cols != histogram1.cols)
      CV_Error(CV_StsBadArg, "Histograms must be of equal dimension.");
    double result = 0.0;
    for (int i = 0; i < histogram0.cols; i++) {
      double a = histogram0.at<_Tp>(0, i) - histogram1.at<_Tp>(0, i);
      double b = histogram0.at<_Tp>(0, i) + histogram1.at<_Tp>(0, i);
      if (std::abs(b) > std::numeric_limits<double>::epsilon()) {
        result += (a*a) / b;
      }
    }
    return result;
  }

  void spatial_histogram(const cv::Mat& src, cv::Mat& hist, int numPatterns, const cv::Size& window, int overlap) {
    int width = src.cols;
    int height = src.rows;
    std::vector<cv::Mat> histograms;
    for (int x = 0; x < width - window.width; x += (window.width - overlap)) {
      for (int y = 0; y < height - window.height; y += (window.height - overlap)) {
        cv::Mat cell = cv::Mat(src, cv::Rect(x, y, window.width, window.height));
        histograms.push_back(histogram(cell, numPatterns));
      }
    }
    hist.create(1, histograms.size()*numPatterns, CV_32SC1);
    // i know this is a bit lame now... feel free to make this a bit more efficient...
    for (int histIdx = 0; histIdx < histograms.size(); histIdx++) {
      for (int valIdx = 0; valIdx < numPatterns; valIdx++) {
        int y = histIdx*numPatterns + valIdx;
        hist.at<int>(0, y) = histograms[histIdx].at<int>(valIdx);
      }
    }
  }

  // wrappers
  void histogram(const cv::Mat& src, cv::Mat& hist, int numPatterns) {
    switch (src.type()) {
    case CV_8SC1: histogram_<char>(src, hist, numPatterns); break;
    case CV_8UC1: histogram_<unsigned char>(src, hist, numPatterns); break;
    case CV_16SC1: histogram_<short>(src, hist, numPatterns); break;
    case CV_16UC1: histogram_<unsigned short>(src, hist, numPatterns); break;
    case CV_32SC1: histogram_<int>(src, hist, numPatterns); break;
    }
  }

  double chi_square(const cv::Mat& histogram0, const cv::Mat& histogram1) {
    switch (histogram0.type()) {
    case CV_8SC1: return chi_square_<char>(histogram0, histogram1); break;
    case CV_8UC1: return chi_square_<unsigned char>(histogram0, histogram1); break;
    case CV_16SC1: return chi_square_<short>(histogram0, histogram1); break;
    case CV_16UC1: return chi_square_<unsigned short>(histogram0, histogram1); break;
    case CV_32SC1: return chi_square_<int>(histogram0, histogram1); break;
    }
  }

  void spatial_histogram(const cv::Mat& src, cv::Mat& dst, int numPatterns, int gridx, int gridy, int overlap) {
    int width = static_cast<int>(floor((double)src.cols / gridx));
    int height = static_cast<int>(floor((double)src.rows / gridy));
    spatial_histogram(src, dst, numPatterns, cv::Size_<int>(width, height), overlap);
  }

  // Mat return type functions
  cv::Mat histogram(const cv::Mat& src, int numPatterns) {
    cv::Mat hist;
    histogram(src, hist, numPatterns);
    return hist;
  }

  cv::Mat spatial_histogram(const cv::Mat& src, int numPatterns, const cv::Size& window, int overlap) {
    cv::Mat hist;
    spatial_histogram(src, hist, numPatterns, window, overlap);
    return hist;
  }

  cv::Mat spatial_histogram(const cv::Mat& src, int numPatterns, int gridx, int gridy, int overlap) {
    cv::Mat hist;
    spatial_histogram(src, hist, numPatterns, gridx, gridy);
    return hist;
  }

  void show_multi_histogram(cv::Mat &img)
  {
    const int maxnc = 6;
    int bins = 256;             // number of bins
    int nc = img.channels();    // number of channels

    std::vector<cv::Mat> hist(nc);       // histogram arrays

    // Initalize histogram arrays
    for (int i = 0; i < hist.size(); i++)
      hist[i] = cv::Mat::zeros(1, bins, CV_32SC1);

    // Calculate the histogram of the image
    for (int i = 0; i < img.rows; i++)
    {
      for (int j = 0; j < img.cols; j++)
      {
        for (int k = 0; k < nc; k++)
        {
          uchar val = (nc == 1 ? img.at<uchar>(i, j) : img.at<cv::Vec3b>(i, j)[k]);
          hist[k].at<int>(val) += 1;
        }
      }
    }

    // For each histogram arrays, obtain the maximum (peak) value
    // Needed to normalize the display later
    int hmax[maxnc] = { 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < nc; i++)
    {
      for (int j = 0; j < bins - 1; j++)
        hmax[i] = hist[i].at<int>(j) > hmax[i] ? hist[i].at<int>(j) : hmax[i];
    }

    const char* wname6[6] = { "C1", "C2", "C3", "C4", "C5", "C6" };
    const char* wname[3] = { "blue", "green", "red" };
    cv::Scalar colors[3] = { cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255) };

    std::vector<cv::Mat> canvas(nc);

    // Display each histogram in a canvas
    for (int i = 0; i < nc; i++)
    {
      canvas[i] = cv::Mat::ones(125, bins, CV_8UC3);

      for (int j = 0, rows = canvas[i].rows; j < bins - 1; j++)
      {
        line(
          canvas[i],
          cv::Point(j, rows),
          cv::Point(j, rows - (hist[i].at<int>(j) * rows / hmax[i])),
          nc == 3 ? colors[i] : cv::Scalar(200, 200, 200),
          1, 8, 0
          );
      }

      if (nc == 1)
        cv::imshow("value", canvas[i]);
      else
        cv::imshow(nc == 3 ? wname[i] : wname6[i], canvas[i]);

      //cv::imshow(nc == 3 ? wname[i] : "value", canvas[i]);
      //cv::imshow(nc == 3 ? wname[i] : wname6[i], canvas[i]);
    }
  }

  void show_histogram(std::string const& name, cv::Mat1b const& image)
  {
    // Set histogram bins count
    int bins = 256;
    int histSize[] = { bins };
    // Set ranges for histogram bins
    float lranges[] = { 0, 256 };
    const float* ranges[] = { lranges };
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = { 0 };

    // create matrix for histogram visualization
    int const hist_height = 256;
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);

    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    double max_val = 0;
    minMaxLoc(hist, 0, &max_val);

    // visualize each bin
    for (int b = 0; b < bins; b++) {
      float const binVal = hist.at<float>(b);
      int   const height = cvRound(binVal*hist_height / max_val);
      cv::line
        (hist_image
        , cv::Point(b, hist_height - height), cv::Point(b, hist_height)
        , cv::Scalar::all(255)
        );
    }
    cv::imshow(name, hist_image);
  }
}