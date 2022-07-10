#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"

using namespace std;
using namespace cv;

#ifndef HEADER_FRAMES_SHIFTER
#define HEADER_FRAMES_SHIFTER

class FramesShifter
{
protected:
    VideoCapture cap;

public:
    Detector *detector;

    FramesShifter(Detector *detector, string videoPath)
    {
        this->detector = detector;
        this->cap = VideoCapture(videoPath);
    }

    virtual int run() = 0;
};
#endif