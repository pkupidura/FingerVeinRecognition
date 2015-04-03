#include "Common.h"

Mat generateFilter0(Mat X, Mat Y, int sigma);

Mat generateFilter1(Mat X, Mat hMat, int sigma);

static void meshgrid(const cv::Mat &xgv, const cv::Mat &ygv,
        cv::Mat &X, cv::Mat &Y)
{
    cv::repeat(xgv.reshape(1,1), ygv.total(), 1, X);
    cv::repeat(ygv.reshape(1,1).t(), 1, xgv.total(), Y);
}

// helper function (maybe that goes somehow easier)
static void meshgridFromRange(const cv::Range &xgv, const cv::Range &ygv,
        cv::Mat &X, cv::Mat &Y)
{
    std::vector<int> t_x, t_y;
    for (int i = xgv.start; i <= xgv.end; i++) t_x.push_back(i);
    for (int i = ygv.start; i <= ygv.end; i++) t_y.push_back(i);
    meshgrid(cv::Mat(t_x), cv::Mat(t_y), X, Y);
}

Mat filterAndCalculateCurvatures(Mat originalImage, Mat fingerRegion, int sigma) {
    auto winsize = ceil(4 * sigma);
    Mat X, Y;
    meshgridFromRange(cv::Range(-winsize, winsize), cv::Range(-winsize, winsize),
                        X, Y);

    auto hMat = generateFilter0(X, Y, sigma);

    std::cout << X << std::endl;
    auto hxMat = generateFilter1(X, hMat, sigma);

    std::cout << X << std::endl;

    return Mat();
}

Mat generateFilter1(Mat X, Mat hMat, int sigma) {
    minus(X);
    divide(X, sigma * sigma);
    multiply(X, hMat, X);
    return X;
}

Mat generateFilter0(Mat X, Mat Y, int sigma) {
    Mat xSqr, ySqr;
    pow(X, 2, xSqr);
    pow(Y, 2, ySqr);

    auto factor1 = 1 / (2 * M_PI * sigma * sigma);
    auto factor2 = 2 * sigma * sigma;

    Mat xy = xSqr + ySqr;
    minus(xy);
    divide(xy, factor2);
    //std::cout << xy << std::endl;
    exp(xy);

    Mat result(X.cols, X.rows, X.type(), Scalar::all(factor1));

    multiply(result, xy, result);

    return result;
}