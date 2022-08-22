#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
#include <functional>
#include <thread>
#include "timer_handler.hpp"

using namespace std;
using namespace cv;

class ThreadPool
{
private:
    vector<thread *> threads;
    queue<Mat>* inputs = new queue<Mat>();
    mutex lock;
    function<void(Mat)> callback;
    bool EOS = false;

    void run()
    {
        queue<Mat> inputs = *this->inputs;
        while (!EOS || inputs.size() > 0)
        {
            lock.lock();
            if (inputs.size() > 0)
            {
                Mat data = inputs.front();
                inputs.pop();
                lock.unlock();
                callback(data);
            }
            else
            {
                lock.unlock();
            }
        }
    }

public:
    ThreadPool(int nw, function<void(Mat)> callback)
    {
        this->callback = callback;
        for (int i = 0; i < nw; i++)
        {
            threads.push_back(new thread([&]()
                                         { this->run(); }));
        }
    }

    void pushInput(Mat frame)
    {
        inputs->push(frame);
    }

    void wait()
    {
        EOS = true;
        for (thread *t : threads)
        {
            t->join();
        }
    }
};

class NativeThreads
{
private:
    TimerHandler timerHandler;

    void gray(Mat frame)
    {
        timerHandler.computeTime("1_GRAYSCALE", [&]()
                                 {
            for (int x = 0; x < frame.rows; x++)
            {
                for (int y = 0; y < frame.cols; y++)
                {
                    Vec3b &color = frame.at<Vec3b>(x, y);
                    int bwColor = (color[0] + color[1] + color[2]) / 3;
                    for (int c = 0; c < 3; c++)
                        frame.at<Vec3b>(x, y)[c] = bwColor;
                }
            } });
    }

    void smooth(Mat frame, Mat kernel)
    {
        timerHandler.computeTime("2_SMOOTHING", [&]()
                                 {
            Mat origin = frame.clone();
            for (int x = 0; x < frame.rows; x++)
            {
                for (int y = 0; y < frame.cols; y++)
                {
                    int sum = 0;
                    for (int kx = 0; kx < kernel.rows; kx++)
                    {
                        int px = x + kx - kernel.rows / 2;
                        for (int ky = 0; ky < kernel.cols; ky++)
                        {
                            int py = y + ky - kernel.cols / 2;
                            if (px >= 0 && px < frame.rows && py >= 0 && py < frame.cols)
                            {
                                sum += origin.at<Vec3b>(px, py)[0] * kernel.at<Vec3d>(kx)[ky];
                            }
                        }
                    }
                    for (int c = 0; c < 3; c++)
                        frame.at<Vec3b>(x, y)[c] = sum;
                }
            } });
    }

    bool detect(Mat frame, Mat background, int threshold)
    {
        int differentFrames = 0;
        timerHandler.computeTime("3_DETECT", [&]()
                                 {
            for (int x = 0; x < background.rows; x++)
            {
                for (int y = 0; y < background.cols; y++)
                {
                    if (background.at<Vec3b>(x, y) != frame.at<Vec3b>(x, y))
                    {
                        differentFrames ++;
                    }
                }
            } });
        return differentFrames >= threshold;
    }

public:
    void run(string videoPath, double k, Mat kernel, int nw, string printMode)
    {
        int differentFrames = 0;
        int* totalFrames = new int(0);
        VideoCapture cap = VideoCapture(videoPath);
        Mat background;
        cap >> background;
        int threshold = k * background.cols * background.rows;
        mutex lock;

        auto callback = [&](Mat frame)
        {
            gray(frame);
            smooth(frame, kernel);
            bool detected = detect(frame, background, threshold);
            if(detected) {
                lock.lock();
                differentFrames ++;
                lock.unlock();
            }
        };
        ThreadPool tp(nw, callback);

        timerHandler.computeTime("TOTAL_TIME", [&]()
                                 {
        gray(background);
        smooth(background, kernel);
            while (true)
            {
                Mat frame;
                cap >> frame;
                if (frame.empty())
                    break;
                (*totalFrames)++;
                tp.pushInput(frame);
            } 
            tp.wait();
        });

        cap.release();
        string title = "NATIVE_THREADS_" + to_string(nw) + "_nw: detection=" + to_string(differentFrames) + "/" + to_string(*totalFrames);
        if (printMode == string("CSV"))
        {
            cout << title << ";" << to_string(nw) << ";" << timerHandler.toCSV() << endl;
        }
        else
        {
            cout << title << endl << timerHandler.toString() << endl;
        }
    }
};