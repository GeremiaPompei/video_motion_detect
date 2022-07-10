#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "pipeline.cpp"

using namespace std;
using namespace cv;

Mat avgKernel() {
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

int analyzeFramesSeq(Detector *detector, string videoPath) {
  int differentFrames = 0;
  VideoCapture cap(videoPath);
  Mat background; 
  cap >> background;
  detector->timerHandler.computeTime("TOTAL_TIME", [&]() { 
    detector->transformAndCompute(background);
    detector->set(background);
    while(true) {
      Mat frame; 
      cap >> frame;
      if(frame.empty()) break;
      bool differs = detector->transformAndCompute(frame);
      if(differs) {
        detector->set(frame);
        differentFrames ++; 
      }
    }
  });
  cap.release();
  return differentFrames;
}

int analyzeFramesPipeline(Detector *detector, string videoPath) {
  int differentFrames = 0;
  mutex lock;
  VideoCapture cap(videoPath);
  Mat background; 
  cap >> background;

  auto finalCallback = [&](bool differs, Mat frame) {
    lock.lock();
    if(differs) {
      detector->set(frame);
      differentFrames ++;
    }
    lock.unlock();
  };

  Pipeline<Mat, Mat> *pipeline = new Pipeline<Mat, Mat>(
    [&](Mat frame) {detector->timerHandler.computeTime("1_GRAYSCALE", [&]() { detector->gray(frame); }); return frame;},
    new Pipeline<Mat, Mat>(
      [&](Mat frame) {detector->timerHandler.computeTime("2_SMOOTHING", [&]() { detector->smooth(frame); }); return frame;},
      new Pipeline<Mat, Mat>(
        [&](Mat frame) {detector->timerHandler.computeTime("3_MAKE_DIFFERENCE", [&]() { finalCallback(detector->makeDifference(frame), frame); }); return frame;}
      )
    )
  );

  detector->timerHandler.computeTime("TOTAL_TIME", [&]() { 
    detector->transformAndCompute(background);
    detector->set(background);
    while(true) {
      Mat frame; 
      cap >> frame;
      if(frame.empty()) break;
      pipeline->run(frame);
    }
    pipeline->eosAndWait();
  });

  cap.release();
  return differentFrames;
}