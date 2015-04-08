#include "Common.h"
using namespace cv;

Mat extractFingerRegion(Mat);
Mat extractFingerVeinsMaxCurvature(Mat, int);
Mat filterAndCalculateCurvatures(Mat, Mat, int);
Mat trackVeinsCentres(Mat mat);
Mat connectVeinCentres(Mat);
Mat extractVeinsFromCentres(Mat);

int main( int argc, char** argv )
{
    Mat image;
    image = imread("finger.png", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
    double scale = (double)1/255;
    image.convertTo(image, CV_64F, scale);
    resize(image, image, cv::Size(image.size().width/2, image.size().height/2));

    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

	auto sigma = 3;

    auto veins = extractFingerVeinsMaxCurvature(image, sigma);

    namedWindow( "Display window", WINDOW_AUTOSIZE );    // Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

Mat extractFingerVeinsMaxCurvature(Mat originalImage, int sigma) {
	auto fingerRegion = extractFingerRegion(originalImage);

	auto curvatures = filterAndCalculateCurvatures(originalImage, fingerRegion, sigma);

    auto trackedVeins = trackVeinsCentres(curvatures);

	auto veinsCentres = connectVeinCentres(trackedVeins);

	auto veins = extractVeinsFromCentres(veinsCentres);

    return veins;
}



Mat trackVeinsCentres(Mat curvatures) {
    return Mat();
}

Mat connectVeinCentres(Mat mat) {
    return Mat();
}

Mat extractVeinsFromCentres(Mat mat) {
    return Mat();
}
