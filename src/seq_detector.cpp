#include <iostream>
#include <opencv2/opencv.hpp>
#include "detector.cpp"

using namespace std;
using namespace cv;

class SeqDetector : public Detector {
    public:
        SeqDetector(Mat _background, Mat _kernel, double _k) : Detector(_background, _kernel, _k) {}

        void gray(Mat frame)
        {
            for(int x=0;x<frame.cols;x++)
            {
                for(int y=0;y<frame.rows;y++)
                {
                    Vec3b & color = frame.at<Vec3b>(x,y);
                    int bwColor = (color[0] + color[1] + color[2]) / 3;
                    for(int c = 0; c < 3; c ++)
                        frame.at<Vec3b>(x,y)[c] = bwColor;
                }
            }
        }

        void smooth(Mat frame)
        {
            for(int x=0;x<frame.rows;x++)
            {
                for(int y=0;y<frame.cols;y++)
                {
                int sum = 0;
                for(int kx=0;kx<this->kernel.rows;kx++)
                {
                    int px = x + kx - this->kernel.rows / 2;
                    for(int ky=0;ky<this->kernel.cols;ky++)
                    {
                    int py = y + ky - this->kernel.cols / 2;
                    if(px >= 0 && px < frame.rows && py >= 0 && py < frame.cols)
                    {
                        sum += frame.at<Vec3b>(px, py)[0] * this->kernel.at<Vec3b>(kx, ky)[0];
                    }
                    }
                }
                for(int c = 0; c < 3; c ++)
                    frame.at<Vec3b>(x,y)[c] = sum;
                }
            }
        }

        bool makeDifference(Mat frame)
        {
            int sum = 0;
            for(int x=0;x<this->background.rows;x++)
            {
                for(int y=0;y<this->background.cols;y++)
                {
                if(this->background.at<Vec3b>(x,y) != frame.at<Vec3b>(x,y))
                {
                    sum += 1;
                    if(sum >= this->threshold)
                    return true;
                }
                }
            }
            return false;
        }
        
        bool transformAndCompute(Mat frame)
        {
            this->gray(frame);
            this->smooth(frame);
            return this->makeDifference(frame);
        }
};
 