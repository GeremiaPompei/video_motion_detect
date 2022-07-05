#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <barrier>
#include <functional>
#include "detector.hpp"

using namespace std;
using namespace cv;

class ParDetector : public Detector {
    private:
        int nw;

        void runParallel(function<void(int)> internalCallback, Mat frame) {
            barrier sync_point(this->nw);
            vector<thread *> threads;
            for(int t=0; t<this->nw; t++) {
                auto callback = [&] (int n) {
                    int bin = frame.rows / this->nw;
                    int end = (n+1) * bin;
                    end = end < frame.rows ? end : frame.rows;
                    for(int x = n * bin; x < end; x++) {
                        internalCallback(x);
                    }
                    sync_point.arrive_and_wait();
                };
                threads.push_back(new thread(callback, t));
            }
            for(int t=0; t<this->nw; t++) {
                threads[t]->join();
            }
        }
        
    public:
        ParDetector(Mat _kernel, double _k, int _nw) : Detector(_kernel, _k) {
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
            atomic<int> sum(0);
            int threshold = this->k * this->background.cols * this->background.rows;
            auto callback = [&] (int x) {
                int res = this->rowMakeDifference(frame, x);
                sum.fetch_add(res, std::memory_order_release);
            };
            this->runParallel(callback, frame);
            return sum >= threshold;
        }
};
 