#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_

#include <opencv2/opencv.hpp>
#include <limits>

namespace lbplibrary
{
  // templated functions
  template <typename _Tp>
  void histogram_(const cv::Mat& src, cv::Mat& hist, int numPatterns);

  template <typename _Tp>
  double chi_square_(const cv::Mat& histogram0, const cv::Mat& histogram1);

  // non-templated functions
  void spatial_histogram(const cv::Mat& src, cv::Mat& spatialhist, int numPatterns, const cv::Size& window, int overlap = 0);

  // wrapper functions
  void spatial_histogram(const cv::Mat& src, cv::Mat& spatialhist, int numPatterns, int gridx = 8, int gridy = 8, int overlap = 0);
  void histogram(const cv::Mat& src, cv::Mat& hist, int numPatterns);
  double chi_square(const cv::Mat& histogram0, const cv::Mat& histogram1);

  // Mat return type functions
  cv::Mat histogram(const cv::Mat& src, int numPatterns);
  cv::Mat spatial_histogram(const cv::Mat& src, int numPatterns, const cv::Size& window, int overlap = 0);
  cv::Mat spatial_histogram(const cv::Mat& src, int numPatterns, int gridx = 8, int gridy = 8, int overlap = 0);

  void show_multi_histogram(cv::Mat &img);
  void show_histogram(std::string const& name, cv::Mat1b const& image);
}
#endif
