#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <functional>
#include <mutex>
#include "detector.hpp"

using namespace std;
using namespace cv;

class ParDetector : public Detector
{
private:
    int nw;

    void runParallel(function<void(int)> internalCallback, Mat frame)
    {
        vector<thread *> threads;
        for (int t = 0; t < this->nw; t++)
        {
            auto callback = [&](int n)
            {
                int bin = frame.rows / this->nw;
                int end = (n + 1) * bin;
                end = end < frame.rows ? end : frame.rows;
                for (int x = n * bin; x < end; x++)
                {
                    internalCallback(x);
                }
            };
            threads.push_back(new thread(callback, t));
        }
        for (int t = 0; t < this->nw; t++)
        {
            threads[t]->join();
        }
    }

public:
    ParDetector(Mat _kernel, double _k, int _nw) : Detector(_kernel, _k)
    {
        this->nw = _nw;
    }

    void gray(Mat frame) override
    {
        auto callback = [&](int x)
        {
            this->rowGray(frame, x);
        };
        this->runParallel(callback, frame);
    }

    void smooth(Mat frame) override
    {
        Mat origin = frame.clone();
        auto callback = [&](int x)
        {
            this->rowSmooth(frame, origin, x);
        };
        this->runParallel(callback, frame);
    }

    bool detectDifference(Mat frame) override
    {
        mutex m;
        int summedResult(0);
        int threshold = this->k * this->background.cols * this->background.rows;
        auto callback = [&](int x)
        {
            int res = this->rowDetectDifference(frame, x);
            m.lock();
            summedResult += res;
            m.unlock();
        };
        this->runParallel(callback, frame);
        return summedResult >= threshold;
    }
};
