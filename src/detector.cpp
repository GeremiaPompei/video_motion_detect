#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Detector {
    protected:
        Mat background;
        Mat kernel;
        int threshold;
    
    public:
        Detector(Mat _background, Mat _kernel, double _k)
        {
            this->background = _background;
            this->kernel = _kernel;
            this->threshold = _k * _background.cols * _background.rows;
        }
        
        void gray(Mat frame);
        void smooth(Mat frame);
        bool makeDifference(Mat frame);
        bool transformAndCompute(Mat frame);
};
 