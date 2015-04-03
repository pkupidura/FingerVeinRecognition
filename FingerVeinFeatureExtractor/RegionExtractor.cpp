#include "Common.h"
using namespace cv;

Mat extractFingerRegion(Mat originalImage) {
    auto maskHeight = 24;
    auto maskWidth = 32;

    auto imgHeigth = originalImage.size().height;
    auto imgWidth = originalImage.size().width;

    auto halfImgHeight = 0;

    if (imgHeigth % 2 == 0)
        halfImgHeight = (imgHeigth / 2) + 1;
    else
        halfImgHeight = ceil(imgHeigth / 2);

    auto mask = Mat(maskHeight, maskWidth, CV_64F, double(0));

    for (auto i = 0; i < mask.cols; i++) {
        for (auto j = 0; j < mask.rows; j++) {
            if (j < maskHeight / 2)
                mask.at<double>(j, i) = -1;
            else
                mask.at<double>(j, i) = 1;
        }
    }

    Mat filteredImage;
    auto anchor = Point( -1, -1 );
    auto delta = 0;

    cv::filter2D(originalImage, filteredImage, CV_64F, mask, anchor, delta, BORDER_REPLICATE);

    Mat imgFiltUp(floor(imgHeigth/2), imgWidth, CV_64F, double(0));
    Mat imgFiltLow(halfImgHeight, imgWidth, CV_64F, double(0));

    auto yUp = vector<int>();

    for (auto i = 0; i < imgFiltUp.cols; i++) {
        auto currentMax = -1;
        auto currentMaxIndex = -1;
        for (auto j = 0; j < imgFiltUp.rows; j++) {
            auto val = filteredImage.at<double>(j, i);
            if (val > currentMax) {
                currentMax = val;
                currentMaxIndex = j;
            }
        }
        yUp.push_back(currentMaxIndex);
    }

    auto yLow = vector<int>();

    for (auto i = 0; i < imgFiltLow.cols; i++) {
        auto currentMin = std::numeric_limits<int>::max();
        auto currentMinIndex = -1;
        for (auto j = 0; j < imgFiltLow.rows; j++) {
            auto val = filteredImage.at<double>(j, i);
            if (val < currentMin) {
                currentMin = val;
                currentMinIndex = j;
            }
        }
        yLow.push_back(currentMinIndex);
    }

    Mat region(originalImage.size(), CV_64F, double(0));

    for (auto i = 0; i < region.cols; i++) {
        for (auto j = 0; j < region.rows; j++) {
            if (j >= yUp[i] && j <= (yLow[i] + imgFiltLow.size().height - 1))
                region.at<double>(j, i) = 1;
        }
    }

    return region;
}
