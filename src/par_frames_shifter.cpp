#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "frames_shifter.hpp"
#include "pipeline.cpp"

using namespace std;
using namespace cv;

class ParFramesShifter : public FramesShifter
{
public:
    ParFramesShifter(Detector *detector, string videoPath) : FramesShifter(detector, videoPath) {}

    int run() override
    {
        int differentFrames = 0;
        mutex lock;
        Mat background;
        this->cap >> background;

        auto finalCallback = [&](bool differs, Mat frame)
        {
            lock.lock();
            if (differs)
            {
                this->detector->set(frame);
                differentFrames++;
            }
            lock.unlock();
        };

        Pipeline<Mat, Mat> *pipeline = new Pipeline<Mat, Mat>(
            [&](Mat frame)
            {this->detector->timerHandler.computeTime("1_GRAYSCALE", [&]() { this->detector->gray(frame); }); return frame; },
            new Pipeline<Mat, Mat>(
                [&](Mat frame)
                {this->detector->timerHandler.computeTime("2_SMOOTHING", [&]() { this->detector->smooth(frame); }); return frame; },
                new Pipeline<Mat, Mat>(
                    [&](Mat frame)
                    {this->detector->timerHandler.computeTime("3_MAKE_DIFFERENCE", [&]() { finalCallback(this->detector->makeDifference(frame), frame); }); return frame; })));

        this->detector->timerHandler.computeTime("TOTAL_TIME", [&]()
                                                 { 
                this->detector->transformAndCompute(background);
                this->detector->set(background);
                while(true) {
                    Mat frame; 
                    this->cap >> frame;
                    if(frame.empty()) break;
                    pipeline->run(frame);
                }
                pipeline->eosAndWait(); });

        this->cap.release();
        return differentFrames;
    }
};