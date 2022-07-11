#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.hpp"

using namespace std;
using namespace cv;

class SeqDetector : public Detector
{
public:
    SeqDetector(Mat _kernel, double _k) : Detector(_kernel, _k) {}

    void gray(Mat frame) override
    {
        for (int x = 0; x < frame.rows; x++)
        {
            this->rowGray(frame, x);
        }
    }

    void smooth(Mat frame) override
    {
        Mat origin = frame.clone();
        for (int x = 0; x < frame.rows; x++)
        {
            this->rowSmooth(frame, origin, x);
        }
    }

    bool detectDifference(Mat frame) override
    {
        int sum = 0;
        int threshold = this->k * this->background.cols * this->background.rows;
        for (int x = 0; x < this->background.rows; x++)
        {
            sum += this->rowDetectDifference(frame, x);
        }
        return sum >= threshold;
    }
};
