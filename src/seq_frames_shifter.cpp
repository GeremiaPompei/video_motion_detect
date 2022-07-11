#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "frames_shifter.hpp"

using namespace std;
using namespace cv;

class SeqFramesShifter : public FramesShifter
{
public:
    SeqFramesShifter(Detector *detector, string videoPath) : FramesShifter(detector, videoPath) {}

    int run() override
    {
        int differentFrames = 0;
        Mat background;
        this->cap >> background;
        this->detector->timerHandler.computeTime("TOTAL_TIME", [&]()
                                                 { 
                this->detector->transformAndCompute(background);
                this->detector->set(background);
                while(true) {
                Mat frame; 
                this->cap >> frame;
                if(frame.empty()) break;
                bool differs = this->detector->transformAndCompute(frame);
                if(differs) {
                    this->detector->set(frame);
                    differentFrames ++; 
                }
                } });
        this->cap.release();
        return differentFrames;
    }
};