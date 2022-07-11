#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "utils.cpp"
#include "seq_detector.cpp"
#include "par_detector.cpp"
#include "ff_detector.cpp"
#include "seq_frames_shifter.cpp"
#include "par_frames_shifter.cpp"
#include "ff_frames_shifter.cpp"

using namespace std;
using namespace cv;

void runAnalysis(string dpp, string spp, FramesShifter *framesShifter)
{
  int differentFrames = framesShifter->run();

  cout << "--------------------------------" << endl
       << "Data parallel pattern implementation: [" << dpp << "]" << endl
       << "Pipeline implementation: [" << spp << "]" << endl
       << "Number of different frames: " << differentFrames << endl
       << framesShifter->detector->timerHandler.toString() << "--------------------------------" << endl;
}

int main(int argc, char *argv[])
{
  string videoPath = argv[1];
  double k = atof(argv[2]);
  string dpp = string(argv[3]);
  string spp = argv[4];
  string nwLabel = "";

  Mat kernel = avgKernel();
  Detector *detector;
  FramesShifter *framesShifter;

  if (dpp == "SEQUENTIAL")
  {
    detector = new SeqDetector(kernel, k);
  }
  else
  {
    int nw = atoi(argv[5]);
    nwLabel = "_" + to_string(nw) + "_NW";
    if (dpp == "PARALLEL")
    {
      detector = new ParDetector(kernel, k, nw);
    }
    else if (dpp == "FASTFLOW")
    {
      detector = new FFDetector(kernel, k, nw);
    }
  }
  if (spp == "SEQUENTIAL")
  {
    framesShifter = new SeqFramesShifter(detector, videoPath);
  }
  else if (spp == "PARALLEL")
  {
    framesShifter = new ParFramesShifter(detector, videoPath);
  }
  else if (spp == "FASTFLOW")
  {
    framesShifter = new FFFramesShifter(detector, videoPath);
  }

  runAnalysis(dpp + nwLabel, spp, framesShifter);

  return (0);
}