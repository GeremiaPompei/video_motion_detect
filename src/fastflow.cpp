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

struct Compute : ff_node_t<Mat, bool>
{
    TimerHandler *timerHandler;
    Mat kernel;
    Mat *background;
    int threshold;

    Compute(TimerHandler *timerHandler, Mat kernel, Mat *background, int threshold)
    {
        this->timerHandler = timerHandler;
        this->kernel = kernel;
        this->background = background;
        this->threshold = threshold;
        gray(*background);
        smooth(*background, kernel);
    }

    void gray(Mat frame)
    {
        timerHandler->computeTime("1_GRAYSCALE", [&]()
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
        timerHandler->computeTime("2_SMOOTHING", [&]()
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
        timerHandler->computeTime("3_DETECT", [&]()
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

    bool *svc(Mat *frame)
    {
        gray(*frame);
        smooth(*frame, kernel);
        bool detected = detect(*frame, *background, threshold);
        return new bool(detected);
    }
};

struct Collector : ff_minode_t<bool, void>
{
    int *differentFrames;

    Collector(int *differentFrames)
    {
        this->differentFrames = differentFrames;
    }

    void *svc(bool *detected)
    {
        if (*detected)
        {
            (*differentFrames)++;
        }
        return GO_ON;
    }
};

class Fastflow
{
private:
    TimerHandler *timerHandler = new TimerHandler();

public:
    void run(string videoPath, double k, Mat kernel, int nw, string printMode)
    {
        int *differentFrames = new int(0);
        int *totalFrames = new int(0);
        VideoCapture cap = VideoCapture(videoPath);
        Mat *background = new Mat();
        cap >> *background;
        int threshold = k * background->cols * background->rows;

        Emitter emitter(cap, totalFrames);
        Compute compute(timerHandler, kernel, background, threshold);
        Collector collector(differentFrames);
        vector<unique_ptr<ff_node>> W;
        for (int i = 0; i < nw; i++)
        {
            W.push_back(make_unique<Compute>(compute));
        }
        ff_Farm<> farm(move(W));
        farm.add_emitter(emitter);
        farm.add_collector(collector);

        timerHandler->computeTime("TOTAL_TIME", [&]()
                                  { farm.run_and_wait_end(); });

        cap.release();
        string title = "FASTFLOW_" + to_string(nw) + "_nw: detection=" + to_string(*differentFrames) + "/" + to_string(*totalFrames);
        if (printMode == string("CSV"))
        {
            cout << title << ";" << to_string(nw) << ";" << timerHandler->toCSV() << endl;
        }
        else
        {
            cout << title << endl << timerHandler->toString() << endl;
        }
    }
};