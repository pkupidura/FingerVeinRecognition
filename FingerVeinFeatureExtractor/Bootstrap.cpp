#include "Common.h"
using namespace cv;

Mat extractFingerRegion(Mat);
Mat extractFingerVeinsMaxCurvature(Mat, int);
Mat filterAndCalculateCurvatures(Mat, Mat, int);
Mat trackVeinsCentres(Mat, Size);
Mat extractVeinsFromCentres(Mat);

int main( int argc, char** argv )
{
    Mat image;
    image = imread("finger.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
    double scale = (double)1/255;
    equalizeHist(image, image);
    Mat temp;
    cv::GaussianBlur(image, temp, cv::Size(0, 0), 3);
    cv::addWeighted(image, 1.5, temp, -0.5, 0, temp);
    image = temp;
    image.convertTo(image, IMAGE_TYPE, scale);
    resize(image, image, cv::Size(image.size().width/2, image.size().height/2));

    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

	auto sigma = 3;

    auto veins = extractFingerVeinsMaxCurvature(image, sigma);

    for (auto i = 0; i < veins.rows; i++)
        for (auto j = 0; j < veins.cols; j++) {
            auto val = veins.at<DATA_TYPE>(i, j);
            if (val >= DATA_TYPE(0.5))
                veins.at<DATA_TYPE>(i, j) = DATA_TYPE(1);
            else if (val < DATA_TYPE(0))
                veins.at<DATA_TYPE>(i, j) = DATA_TYPE(0);
        }

    namedWindow( "1", WINDOW_AUTOSIZE );    // Create a window for display.
    imshow( "1", veins );                   // Show our image inside it.
    namedWindow("2", WINDOW_AUTOSIZE);
    imshow("2", image);

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

Mat extractFingerVeinsMaxCurvature(Mat originalImage, int sigma) {
	auto fingerRegion = extractFingerRegion(originalImage);

	auto curvatures = filterAndCalculateCurvatures(originalImage, fingerRegion, sigma);

    auto trackedVeins = trackVeinsCentres(curvatures, originalImage.size());

	auto veins = extractVeinsFromCentres(trackedVeins);

    return veins;
}
