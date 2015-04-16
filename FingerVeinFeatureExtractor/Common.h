#ifndef _FINGERVEINFEATUREEXTRACTOR_COMMON_H_
#define _FINGERVEINFEATUREEXTRACTOR_COMMON_H_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <bits/unique_ptr.h>
using namespace cv;

//#define IMAGE_AS_DOUBLE

#ifndef IMAGE_AS_DOUBLE
#define IMAGE_TYPE CV_32F
#define DATA_TYPE float
#else
#define IMAGE_TYPE CV_64F
#define DATA_TYPE double
#endif

#endif //_FINGERVEINFEATUREEXTRACTOR_COMMON_H_
