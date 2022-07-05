#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#ifndef HEADER_DETECTOR
#define HEADER_DETECTOR
class Detector {
    protected:
        Mat background;
        Mat kernel;
        double k;
    
    public:
        Detector(Mat _kernel, double _k) {
            this->kernel = _kernel;
            this->k = _k;
        }
        
        virtual void gray(Mat frame) = 0;
        virtual void smooth(Mat frame) = 0;
        virtual bool makeDifference(Mat frame) = 0;

        void rowGray(Mat frame, int x) {
            for(int y=0;y<frame.cols;y++) {
                Vec3b & color = frame.at<Vec3b>(x,y);
                int bwColor = (color[0] + color[1] + color[2]) / 3;
                for(int c = 0; c < 3; c ++)
                    frame.at<Vec3b>(x,y)[c] = bwColor;
            }
        }

        void rowSmooth(Mat frame, Mat origin, int x) {
            for(int y=0;y<frame.cols;y++) {
                int sum = 0;
                for(int kx=0;kx<this->kernel.rows;kx++) {
                    int px = x + kx - this->kernel.rows / 2;
                    for(int ky=0;ky<this->kernel.cols;ky++) {
                        int py = y + ky - this->kernel.cols / 2;
                        if(px >= 0 && px < frame.rows && py >= 0 && py < frame.cols) {
                            sum += origin.at<Vec3b>(px, py)[0] * this->kernel.at<Vec3d>(kx)[ky];
                        }
                    }
                }
                for(int c = 0; c < 3; c ++)
                    frame.at<Vec3b>(x,y)[c] = sum;
            }
        }

        int rowMakeDifference(Mat frame, int x) {
            int sum = 0;
            for(int y=0;y<this->background.cols;y++) {
                if(this->background.at<Vec3b>(x,y) != frame.at<Vec3b>(x,y)) {
                    sum += 1;
                }
            }
            return sum;
        }
        
        bool transformAndCompute(Mat frame) {
            this->gray(frame);
            this->smooth(frame);
            return this->makeDifference(frame);
        }

        void setAndComputeBackground(Mat background) {
            this->transformAndCompute(background);
            this->background = background;   
        }
};
#endif
 