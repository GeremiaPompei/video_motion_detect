#include <iostream>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <functional>
#include <ff/ff.hpp>
#include "detector.hpp"
#include "frames_shifter.hpp"

using namespace std;
using namespace cv;
using namespace ff;

struct Source : ff_node_t<Mat>
{
    Detector *detector;
    VideoCapture cap;

    Source(Detector *detector, VideoCapture cap)
    {
        this->detector = detector;
        this->cap = cap;
    }

    Mat *svc(Mat *)
    {
        mutex lock;
        Mat background;
        this->cap >> background;
        this->detector->transformAndCompute(background);
        this->detector->set(background);
        while (true)
        {
            Mat *frame = new Mat();
            cap >> *frame;
            if (frame->empty())
                break;
            ff_send_out(frame);
        }

        cap.release();
        return EOS;
    }
};

struct GrayScale : ff_node_t<Mat>
{
    Detector *detector;

    GrayScale(Detector *detector)
    {
        this->detector = detector;
    }

    Mat *svc(Mat *frame)
    {
        this->detector->timerHandler.computeTime("1_GRAYSCALE", [&]()
                                                 { this->detector->gray(*frame); });
        return frame;
    }
};

struct Smoothing : ff_node_t<Mat>
{
    Detector *detector;

    Smoothing(Detector *detector)
    {
        this->detector = detector;
    }

    Mat *svc(Mat *frame)
    {
        this->detector->timerHandler.computeTime("2_SMOOTHING", [&]()
                                                 { this->detector->smooth(*frame); });
        return frame;
    }
};

struct DetectDifference : ff_node_t<Mat>
{
    Detector *detector;
    function<void(bool, Mat)> finalCallback;

    DetectDifference(Detector *detector, function<void(bool, Mat)> finalCallback)
    {
        this->detector = detector;
        this->finalCallback = finalCallback;
    }

    Mat *svc(Mat *frame)
    {
        this->detector->timerHandler.computeTime("3_DETECT_DIFFERENCE", [&]()
                                                 { this->finalCallback(this->detector->detectDifference(*frame), *frame); });
        return GO_ON;
    }
};

class FFFramesShifter : public FramesShifter
{
public:
    FFFramesShifter(Detector *detector, string videoPath) : FramesShifter(detector, videoPath) {}

    int run() override
    {
        mutex lock;
        int differentFrames = 0;

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

        Source source(this->detector, this->cap);
        GrayScale grayScale(this->detector);
        Smoothing smoothing(this->detector);
        DetectDifference detectDifference(this->detector, finalCallback);
        ff_Pipe<> pipe(source, grayScale, smoothing, detectDifference);

        this->detector->timerHandler.computeTime("TOTAL_TIME", [&]()
                                                 { pipe.run_and_wait_end(); });
        return differentFrames;
    }
};