#include "Common.h"

Mat generateFilter0(Mat X, Mat Y, int sigma);

Mat generateFilter1(Mat X, Mat hMat, int sigma);

Mat generateFilter2(Mat X, Mat hMat, int sigma);

Mat generateFilter5(Mat X, Mat Y, Mat hMat, int sigma);

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

    X.convertTo(X, originalImage.type());
    Y.convertTo(Y, originalImage.type());

    auto hMat = generateFilter0(X, Y, sigma);

    auto hxMat = generateFilter1(X, hMat, sigma);

    auto hxxMat = generateFilter2(X, hMat, sigma);

    Mat hyMat, hyyMat;

    transpose(hxMat, hyMat);
    transpose(hxxMat, hyyMat);

    auto hxyMat = generateFilter5(X, Y, hMat, sigma);

    //Actual filtering

    Mat fx, fxx, fy, fyy, fxy;
    auto anchor = Point( -1, -1 );
    auto delta = 0;

    cv::filter2D(originalImage, fx, originalImage.type(), hxMat, anchor, delta, BORDER_REPLICATE);
    cv::filter2D(originalImage, fxx, originalImage.type(), hxxMat, anchor, delta, BORDER_REPLICATE);
    cv::filter2D(originalImage, fy, originalImage.type(), hyMat, anchor, delta, BORDER_REPLICATE);
    cv::filter2D(originalImage, fyy, originalImage.type(), hyyMat, anchor, delta, BORDER_REPLICATE);
    cv::filter2D(originalImage, fxy, originalImage.type(), hxyMat, anchor, delta, BORDER_REPLICATE);

    Mat f1 = 0.5 * sqrt(2) * (fx + fy);
    Mat f2 = 0.5 * sqrt(2) * (fx - fy);
    Mat f11 = 0.5 * fxx + fxy + 0.5 * fyy;
    Mat f22 = 0.5 * fxx - fxy + 0.5 * fyy;

    int sizes[] = {originalImage.rows, originalImage.cols, 4};

    Mat k(3, sizes, originalImage.type(), Scalar(DATA_TYPE(0)));

    Mat fxSqr, k1result;
    pow(fx, 2, fxSqr);
    fxSqr = fxSqr + 1;
    pow(fxSqr, DATA_TYPE(3/2), fxSqr);
    divide(fxx, fxSqr, k1result);
    multiply(k1result, fingerRegion, k1result);

    for(auto i = 0; i < originalImage.rows; i++) {
        for (auto j = 0; j < originalImage.cols; j++) {
            k.at<DATA_TYPE>(i, j, 0) = k1result.at<DATA_TYPE>(i, j);
        }
    }

    Mat fySqr, k2result;
    pow(fy, 2, fySqr);
    fySqr = fySqr + 1;
    pow(fySqr, DATA_TYPE(3/2), fySqr);
    divide(fyy, fySqr, k2result);
    multiply(k2result, fingerRegion, k2result);

    for(auto i = 0; i < originalImage.rows; i++) {
        for (auto j = 0; j < originalImage.cols; j++) {
            k.at<DATA_TYPE>(i, j, 1) = k2result.at<DATA_TYPE>(i, j);
        }
    }

    Mat f1Sqr, k3result;
    pow(f1, 2, f1Sqr);
    f1Sqr = f1Sqr + 1;
    pow(f1Sqr, DATA_TYPE(3/2), f1Sqr);
    divide(f11, f1Sqr, k3result);
    multiply(k3result, fingerRegion, k3result);

    for(auto i = 0; i < originalImage.rows; i++) {
        for (auto j = 0; j < originalImage.cols; j++) {
            k.at<DATA_TYPE>(i, j, 2) = k3result.at<DATA_TYPE>(i, j);
        }
    }

    Mat f2Sqr, k4result;
    pow(f2, 2, f2Sqr);
    f2Sqr = f2Sqr + 1;
    pow(f2Sqr, DATA_TYPE(3/2), f2Sqr);
    divide(f22, f2Sqr, k4result);
    multiply(k4result, fingerRegion, k4result);

    for(auto i = 0; i < originalImage.rows; i++) {
        for (auto j = 0; j < originalImage.cols; j++) {
            k.at<DATA_TYPE>(i, j, 3) = k4result.at<DATA_TYPE>(i, j);
        }
    }

    return k;
}

Mat generateFilter5(Mat X, Mat Y, Mat hMat, int sigma) {
    Mat result;
    multiply(X, Y, result);

    result = result / (sigma * sigma * sigma * sigma);

    multiply(result, hMat, result);
    return result;
}

Mat generateFilter2(Mat X, Mat hMat, int sigma) {
    Mat xSqr;
    pow(X, 2, xSqr);
    xSqr = xSqr - (sigma * sigma);
    xSqr = xSqr / (sigma * sigma * sigma * sigma);
    Mat result(xSqr.size(), xSqr.type());
    multiply(xSqr, hMat, result);
    return result;
}

Mat generateFilter1(Mat X, Mat hMat, int sigma) {
    Mat copy = X.clone();
    copy = -copy;
    copy = copy / (sigma * sigma);
    Mat result(copy.size(), copy.type());
    multiply(copy, hMat, result);
    return result;
}

Mat generateFilter0(Mat X, Mat Y, int sigma) {
    Mat xSqr, ySqr;
    pow(X, 2, xSqr);
    pow(Y, 2, ySqr);

    auto factor1 = 1 / (2 * M_PI * sigma * sigma);
    auto factor2 = 2 * sigma * sigma;

    Mat xy = xSqr + ySqr;
    Mat negated = -xy;
    Mat divided = negated / factor2;
    Mat exponent = divided;
    exp(divided, exponent);
    Mat result = exponent * factor1;

    return result;
}