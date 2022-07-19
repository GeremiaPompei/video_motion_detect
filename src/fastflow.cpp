#include <iostream>
#include <opencv2/opencv.hpp>
#include <ff/ff.hpp>
#include "timer_handler.hpp"

using namespace std;
using namespace cv;
using namespace ff;

struct Emitter : ff_monode_t<Mat>
{
    VideoCapture cap;
    int *totalFrames;

    Emitter(VideoCapture cap, int *totalFrames)
    {
        this->cap = cap;
        this->totalFrames = totalFrames;
    }

    Mat *svc(Mat *)
    {
        while (true)
        {
            Mat *frame = new Mat();
            cap >> *frame;
            if (frame->empty())
                break;
            (*totalFrames)++;
            ff_send_out(frame);
        }
        return EOS;
    }
};

struct Compute : ff_node_t<Mat>
{
    TimerHandler timerHandler;
    Mat kernel;

    Compute(TimerHandler timerHandler, Mat kernel)
    {
        this->timerHandler = timerHandler;
        this->kernel = kernel;
    }

    Mat *svc(Mat *frame)
    {
        timerHandler.computeTime("1_GRAYSCALE", [&]()
                                 {
            for (int x = 0; x < frame->rows; x++)
            {
                for (int y = 0; y < frame->cols; y++)
                {
                    Vec3b &color = frame->at<Vec3b>(x, y);
                    int bwColor = (color[0] + color[1] + color[2]) / 3;
                    for (int c = 0; c < 3; c++)
                        frame->at<Vec3b>(x, y)[c] = bwColor;
                }
            } });
        timerHandler.computeTime("2_SMOOTHING", [&]()
                                 {
            Mat origin = frame->clone();
            for (int x = 0; x < frame->rows; x++)
            {
                for (int y = 0; y < frame->cols; y++)
                {
                    int sum = 0;
                    for (int kx = 0; kx < kernel.rows; kx++)
                    {
                        int px = x + kx - kernel.rows / 2;
                        for (int ky = 0; ky < kernel.cols; ky++)
                        {
                            int py = y + ky - kernel.cols / 2;
                            if (px >= 0 && px < frame->rows && py >= 0 && py < frame->cols)
                            {
                                sum += origin.at<Vec3b>(px, py)[0] * kernel.at<Vec3d>(kx)[ky];
                            }
                        }
                    }
                    for (int c = 0; c < 3; c++)
                        frame->at<Vec3b>(x, y)[c] = sum;
                }
            } });
        return frame;
    }
};

struct Collector : ff_minode_t<Mat>
{
    int *differentFrames;
    TimerHandler timerHandler;
    Mat background;
    int threshold;
    mutex lock;

    Collector(TimerHandler timerHandler, int *differentFrames, Mat background, int threshold)
    {
        this->timerHandler = timerHandler;
        this->differentFrames = differentFrames;
        this->background = background;
        this->threshold = threshold;
    }

    Mat *svc(Mat *frame)
    {
        int sum = 0;
        timerHandler.computeTime("3_DETECT", [&]()
                                 {
            for (int x = 0; x < background.rows; x++)
            {
                for (int y = 0; y < background.cols; y++)
                {
                    if (background.at<Vec3b>(x, y) != frame->at<Vec3b>(x, y))
                    {
                        sum ++;
                    }
                }
            }
            if(sum >= threshold) {
                lock.lock();
                (*differentFrames)++; // TODO FIX
                lock.unlock();
            } 
        });
        return GO_ON;
    }
};

class Fastflow
{
private:
    TimerHandler timerHandler;

public:
    void run(string videoPath, double k, Mat kernel, int nw)
    {
        int *differentFrames = new int(0);
        int *totalFrames = new int(0);
        VideoCapture cap = VideoCapture(videoPath);
        Mat background;
        cap >> background;
        int threshold = k * background.cols * background.rows;

        Emitter emitter(cap, totalFrames);
        Compute compute(timerHandler, kernel);
        Collector collector(timerHandler, differentFrames, background, threshold);
        vector<unique_ptr<ff_node>> W;
        for (int i = 0; i < nw; i++)
        {
            W.push_back(make_unique<Compute>(compute));
        }
        ff_Farm<> farm(move(W));
        farm.add_emitter(emitter);
        farm.add_collector(collector);

        timerHandler.computeTime("TOTAL_TIME", [&]()
                                 { farm.run_and_wait_end(); });

        cap.release();
        cout << "FASTFLOW_" << to_string(nw) << "_nw: detection=" << *differentFrames << "/" << *totalFrames << endl
             << timerHandler.toString() << endl;
    }
};