#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat extractFingerRegion(Mat mat, int, int);
Mat extractFingerVeinsMaxCurvature(Mat originalImage, Mat fingerRegion, int sigma);


int main( int argc, char** argv )
{
    Mat image;
    image = imread("finger.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
    resize(image, image, cv::Size(image.size().width/2, image.size().height/2));

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    auto maskHeight = 24;
    auto maskWidth = 32;

    auto fingerRegion = extractFingerRegion(image, maskHeight, maskWidth);
    auto sigma = 3;

    auto veins = extractFingerVeinsMaxCurvature(image, fingerRegion, sigma);

    namedWindow( "Display window", WINDOW_AUTOSIZE );    // Create a window for display.
    imshow( "Display window", veins );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

Mat extractFingerVeinsMaxCurvature(Mat originalImage, Mat fingerRegion, int sigma) {
    return originalImage;
}

Mat extractFingerRegion(Mat originalImage, int maskHeight, int maskWidth) {
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
//return filteredImage;
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
        auto currentMin = numeric_limits<int>::max();
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