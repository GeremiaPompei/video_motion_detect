#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "utils.cpp"
#include "seq_detector.cpp"
#include "par_detector.cpp"
#include "ff_detector.cpp"

using namespace std;
using namespace cv;

void runAnalysis(string label, Detector *detector, string videoPath, bool pipeline = false)
{
  int differentFrames = pipeline ? analyzeFramesPipeline(detector, videoPath) : analyzeFramesSeq(detector, videoPath);

  cout << 
  "--------------------------------" << endl << 
  label << endl << (pipeline ? "PIPELINE_ON" : "PIPELINE_OFF") << endl <<
  "Number of different frames: " << differentFrames << endl << 
  detector->timerHandler.toString() << 
  "--------------------------------" << endl;
}

int main(int argc, char * argv[]) 
{
  string videoPath = argv[1];
  double k = atof(argv[2]);
  string type = argv[3];
  bool pipeline = string(argv[4]) == "PIPELINE_ON";

  Mat kernel = avgKernel();

  if(type == "SEQUENTIAL") {
    runAnalysis(type, new SeqDetector(kernel, k), videoPath, pipeline);
  } else {
    int nw = atoi(argv[5]);
    if(type == "PARALLEL") {
      runAnalysis("PARALLEL_"+to_string(nw)+"_NW", new ParDetector(kernel, k, nw), videoPath, pipeline);
    } else if(type == "FASTFLOW") {
      runAnalysis("FASTFLOW_"+to_string(nw)+"_NW", new FFDetector(kernel, k, nw), videoPath, pipeline);
    }
  }

  return(0);
}