#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;

#define WAITKEY_DELAY 30
#define CAMERA_DEVICE 0
#define GAUSSIAN_SIGMA 3.5
#define KERNEL_SIZE 5
#define HIGH_THRESH 110
#define LOW_THRESH 3

int main(void)
{
    VideoCapture captureDevice(CAMERA_DEVICE);
    if(!captureDevice.isOpened())
        return -1;

    Mat edges;
    namedWindow("edges", 1);
    for(;;)
    {
        Mat frame;
        captureDevice >> frame;
        cvtColor(frame, edges, COLOR_BGR2GRAY);
//        GaussianBlur(edges, edges, Size(KERNEL_SIZE,KERNEL_SIZE), 
//                        GAUSSIAN_SIGMA, GAUSSIAN_SIGMA);
  //      Canny(edges, edges, 0, HIGH_THRESH, LOW_THRESH);
        imshow("edges", edges);
        auto key = waitKey(WAITKEY_DELAY);
        if(key >= 0) 
            break;
    }
}
