#include <iostream>
#include <opencv2/opencv.hpp>
#include "seq_detector.cpp"

using namespace std;
using namespace cv;

Mat avgKernel()
{
  const int rows = 3;
  const int cols = 3;
  double m[rows][cols] = {
    {1, 1, 1},
    {1, 1, 1}, 
    {1, 1, 1}
  };
  int sum = 0;
  for(int kx=0;kx<rows;kx++)
    for(int ky=0;ky<cols;ky++)
      sum += m[kx][ky];
  for(int kx=0;kx<rows;kx++)
    for(int ky=0;ky<cols;ky++)
      m[kx][ky] /= sum;
  return Mat(3, 3, CV_64F, m);
}


int main(int argc, char * argv[]) 
{
  String videoPath = argv[1];
  double k = atof(argv[2]);
  int differentFrames = 0;
  VideoCapture cap(videoPath);
  Mat background; 
  cap >> background;
  Mat kernel = avgKernel();
  SeqDetector *detector = new SeqDetector(background, kernel, k);
  detector->transformAndCompute(background);
  while(true) {
    Mat frame; 
    cap >> frame;
    if(frame.empty())
      break;
    bool differs = detector->transformAndCompute(frame);
    if(differs)
      differentFrames ++; 
    //imshow("img", frame ); waitKey(1); // TODO: show video
  }
  cap.release();
  cout << "Number of different frames: " << differentFrames << endl;
  return(0);
}