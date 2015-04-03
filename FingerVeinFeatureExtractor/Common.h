#ifndef _FINGERVEINFEATUREEXTRACTOR_COMMON_H_
#define _FINGERVEINFEATUREEXTRACTOR_COMMON_H_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;

static void divide(Mat &M, double scalar) {
    for (auto i = 0; i < M.rows; i++)
        for (auto j = 0; j < M.rows; j++)
            M.at<double>(i, j) /= scalar;
}

static void minus(Mat &M) {
    for (auto i = 0; i < M.rows; i++)
        for (auto j = 0; j < M.rows; j++)
            M.at<double>(i, j) = -M.at<double>(i, j);
}

static void exp(Mat &M) {
    for (auto i = 0; i < M.rows; i++)
        for (auto j = 0; j < M.rows; j++)
            M.at<double>(i, j) = exp(M.at<double>(i, j));
}

#endif //_FINGERVEINFEATUREEXTRACTOR_COMMON_H_
