#include <iostream>
#include <opencv2/opencv.hpp>
#include "timer_handler.hpp"

using namespace std;
using namespace cv;

class Sequential
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
                    int smoothed = 0;
                    for (int kx = 0; kx < kernel.rows; kx++)
                    {
                        int px = x + kx - kernel.rows / 2;
                        for (int ky = 0; ky < kernel.cols; ky++)
                        {
                            int py = y + ky - kernel.cols / 2;
                            if (px >= 0 && px < frame.rows && py >= 0 && py < frame.cols)
                            {
                                smoothed += origin.at<Vec3b>(px, py)[0] * kernel.at<Vec3d>(kx)[ky];
                            }
                        }
                    }
                    for (int c = 0; c < 3; c++) 
                    {
                        frame.at<Vec3b>(x, y)[c] = smoothed;
                    }
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
    void run(string videoPath, double k, Mat kernel, string printMode)
    {
        int differentFrames = 0;
        int totalFrames = 0;
        VideoCapture cap = VideoCapture(videoPath);
        Mat background;
        cap >> background;
        int threshold = k * background.cols * background.rows;
        gray(background);
        smooth(background, kernel);
        timerHandler.computeTime("TOTAL_TIME", [&]()
                                 {
            while (true)
            {
                Mat frame;
                cap >> frame;
                if (frame.empty())
                    break;
                totalFrames++;
                gray(frame);
                smooth(frame, kernel);
                bool differs = detect(frame, background, threshold);
                if (differs)
                {
                    differentFrames++;
                }
            } });
        cap.release();
        string title = "SEQUENTIAL: detection=" + to_string(differentFrames) + "/" + to_string(totalFrames);
        if (printMode == string("CSV"))
        {
            cout << title << ";" << 1 << ";" << timerHandler.toCSV() << endl;
        }
        else
        {
            cout << title << endl << timerHandler.toString() << endl;
        }
    }
};