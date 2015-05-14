#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <stdio.h>
#include <stdarg.h>

void cvShowManyImages(char *title, int nArgs, ...) {
    // img - Used for getting the arguments
    IplImage *img;

    IplImage *DispImage;

    int size;
    int i;
    int m, n;
    int x, y;

    // w - Maximum number of images in a row
    // h - Maximum number of images in a column
    int w, h;

    // scale - How much we have to resize the image
    float scale;
    int max;

    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying
    if (nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if (nArgs > 12) {
        printf("Number of arguments too large....\n");
        return;
    }
        // Determine the size of the image,
        // and the number of rows/cols
        // from number of arguments
    else if (nArgs == 1) {
        w = h = 1;
        size = 300;
    }
    else if (nArgs == 2) {
        w = 2;
        h = 1;
        size = 300;
    }
    else if (nArgs == 3 || nArgs == 4) {
        w = 2;
        h = 2;
        size = 300;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 3;
        h = 2;
        size = 200;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 4;
        h = 2;
        size = 200;
    }
    else {
        w = 4;
        h = 3;
        size = 150;
    }

    // Create a new 3 channel image
    DispImage = cvCreateImage(cvSize(100 + size * w, 60 + size * h), 8, 3);

    // Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {

        // Get the Pointer to the IplImage
        img = va_arg(args, IplImage*);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if (img == 0) {
            printf("Invalid arguments");
            cvReleaseImage(&DispImage);
            return;
        }

        // Find the width and height of the image
        x = img->width;
        y = img->height;

        // Find whether height or width is greater in order to resize the image
        max = (x > y) ? x : y;

        // Find the scaling factor to resize the image
        scale = (float) max / size;

        // Used to Align the images
        if (i % w == 0 && m != 20) {
            m = 20;
            n += 20 + size;
        }

        // Set the image ROI to display the current image
        cvSetImageROI(DispImage, cvRect(m, n, (int) (x / scale), (int) (y / scale)));

        // Resize the input image and copy the it to the Single Big Image
        cvResize(img, DispImage);

        // Reset the ROI in order to display the next image
        cvResetImageROI(DispImage);
    }

    // Create a new window, and show the Single Big Image
    cvNamedWindow(title, 1);
    cvShowImage(title, DispImage);

    cvWaitKey();
    cvDestroyWindow(title);

    // End the number of arguments
    va_end(args);

    // Release the Image Memory
    cvReleaseImage(&DispImage);
}