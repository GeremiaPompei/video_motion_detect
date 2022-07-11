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

void runAnalysis(string dpp, string spp, FramesShifter *framesShifter, string printMode)
{
  int differentFrames = framesShifter->run();

  if (printMode == "CSV")
  {
    cout << dpp + "-" + spp << endl
         << framesShifter->detector->timerHandler.toCSV();
  }
  else
  {
    cout << "--------------------------------" << endl
         << "Data parallel pattern implementation: [" << dpp << "]" << endl
         << "Pipeline implementation: [" << spp << "]" << endl
         << "Number of different frames: " << differentFrames << endl
         << framesShifter->detector->timerHandler.toString() << "--------------------------------" << endl;
  }
}

int main(int argc, char *argv[])
{
  string videoPath = argv[1];
  double k = atof(argv[2]);
  string dpp = string(argv[3]);
  string spp = argv[4];
  string nwLabel = "";
  int nw = 1;
  string printMode = "";
  if (argc > 5)
    nw = atoi(argv[5]);
  if (argc > 6)
    printMode = argv[6];

  Mat kernel = avgKernel();
  Detector *detector;
  FramesShifter *framesShifter;

  if (dpp == "SEQUENTIAL")
  {
    detector = new SeqDetector(kernel, k);
  }
  else
  {
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

  runAnalysis(dpp + nwLabel, spp, framesShifter, printMode);

  return (0);
}