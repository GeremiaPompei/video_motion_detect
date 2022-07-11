#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat avgKernel() 
{
  const int rows = 3, cols = 3;
  Mat kernel = (Mat_<double>({
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
  })).reshape(3);
  Scalar s = cv::sum(kernel);
  for(int kx=0;kx<rows;kx++)
    for(int ky=0;ky<cols;ky++)
      kernel.at<double>(kx,ky) /= s[0];
  return kernel;
}