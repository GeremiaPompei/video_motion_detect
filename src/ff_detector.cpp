#include <iostream>
#include <opencv2/opencv.hpp>
#include <functional>
#include <mutex>
#include <ff/ff.hpp>
#include "detector.hpp"

using namespace ff;
using namespace std;
using namespace cv;

struct Task {
    int i;
};

struct Emitter : ff_monode_t<Task>
{
    int max;

    Emitter(int max)
    {
        this->max = max;
    }

    Task *svc(Task *)
    {
        for(int i = 0; i < this->max; i++)
        {
            ff_send_out(new Task{i});
        }
        return EOS;
    }
};

struct Compute : ff_node_t<Task>
{
    function<void(int)> internalCallback;

    Compute(function<void(int)> internalCallback)
    {
        this->internalCallback = internalCallback;
    }

    Task *svc(Task *task)
    {
        this->internalCallback(task->i);
        return GO_ON;
    }
};


class FFDetector : public Detector
{
private:
    int nw;

    void runParallel(function<void(int)> internalCallback, Mat frame)
    {
        Emitter emitter(frame.rows);
        Compute compute(internalCallback);
        vector<unique_ptr<ff_node>> W;
        for(int i = 0; i < this->nw; i++) {
            W.push_back(make_unique<Compute>(compute));
        }
        ff_Farm<> farm(move(W));
        farm.add_emitter(emitter);
        farm.run_and_wait_end();
    }

public:
    FFDetector(Mat _kernel, double _k, int _nw) : Detector(_kernel, _k)
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
