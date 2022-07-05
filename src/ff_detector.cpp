#include <iostream>
#include <opencv2/opencv.hpp>
#include <functional>
#include "detector.hpp"
#include <ff/parallel_for.hpp>

using namespace ff;
using namespace std;
using namespace cv;

class FFDetector : public Detector {
    private:
        int nw;

        void runParallel(function<void(int)> internalCallback, Mat frame) {
            ParallelFor pf;
            pf.parallel_for(0, frame.rows, [&](const long x) {
                internalCallback(x);
            });
        }
        
    public:
        FFDetector(Mat _kernel, double _k, int _nw) : Detector(_kernel, _k) {
            this->nw = _nw;
        }

        void gray(Mat frame) override {
            auto callback = [&] (int x) {
                this->rowGray(frame, x);
            };
            this->runParallel(callback, frame);
        }

        void smooth(Mat frame) override {
            Mat origin = frame.clone();
            auto callback = [&] (int x) {
                this->rowSmooth(frame, origin, x);
            };
            this->runParallel(callback, frame);
        }

        bool makeDifference(Mat frame) override {
            int summedResult(0);
            int threshold = this->k * this->background.cols * this->background.rows;
            auto callback = [&] (int x) {
                int res = this->rowMakeDifference(frame, x);
                summedResult += res;
            };
            this->runParallel(callback, frame);
            return summedResult >= threshold;
        }
};
 