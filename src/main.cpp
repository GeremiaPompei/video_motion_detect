#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "sequential.cpp"
#include "native_threads.cpp"
#include "fastflow.cpp"

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

int main(int argc, char *argv[])
{
    string videoPath = argv[1];
    double k = atof(argv[2]);
    string type = string(argv[3]);
    int nw = 1;
    string printMode = "";
    if (argc > 4)
        nw = atoi(argv[4]);
    if (argc > 5)
        printMode = argv[5];
    Mat kernel =  avgKernel();
    if (type == "SEQUENTIAL")
    {
        Sequential detector;
        detector.run(videoPath, k, kernel, printMode);
    }
    else
    {
        if (type == "NATIVE_THREADS")
        {
          NativeThreads detector;
          detector.run(videoPath, k, kernel, nw, printMode);
        }
        else if (type == "FASTFLOW")
        {
          Fastflow detector;
          detector.run(videoPath, k, kernel, nw, printMode);
        }
    }
    return (0);
}