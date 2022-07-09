#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "utils.cpp"
#include "seq_detector.cpp"
#include "par_detector.cpp"
#include "ff_detector.cpp"

using namespace std;
using namespace cv;

void runAnalysis(String label, Detector *detector, String videoPath, bool show = false)
{
  int differentFrames = analyzeFrames(detector, videoPath, show);

  cout << 
  "--------------------------------" << endl << 
  label << endl <<
  "Number of different frames: " << differentFrames << endl << 
  detector->timerHandler.toString() << 
  "--------------------------------" << endl;
}

int main(int argc, char * argv[]) 
{
  String videoPath = argv[1];
  double k = atof(argv[2]);

  Mat kernel = avgKernel();

  runAnalysis("SEQUENTIAL", new SeqDetector(kernel, k), videoPath);
  int nws[] = {1, 2, 4, 8, 16, 32};
  for(int nw : nws) {
    runAnalysis("PARALLEL_"+to_string(nw)+"_NW", new ParDetector(kernel, k, nw), videoPath);
  }
  for(int nw : nws) {
    runAnalysis("FASTFLOW_"+to_string(nw)+"_NW", new FFDetector(kernel, k, nw), videoPath);
  }

  return(0);
}