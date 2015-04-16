#include "Common.h"

Mat trackVeinsCentres(Mat curvatures, cv::Size imageSize) {
    Mat centers(imageSize, curvatures.type());
    int wr = 0;
    DATA_TYPE scr = 0;

    for (auto x = 0; x < imageSize.height; x++) {
        for (auto y = 0; y < imageSize.width; y++) {
            if (curvatures.at<DATA_TYPE>(x, y, 0))
                wr++;

            if (wr > 0 &&
                    (y == imageSize.width - 1 || (curvatures.at<DATA_TYPE>(x, y, 0) != 0))) {
                int pos_end;
                if (y == imageSize.width - 1)
                    pos_end = y;
                else
                    pos_end = y - 1;

                int pos_start = pos_end - wr + 1;

                int index = 0;
                DATA_TYPE maxValue = std::numeric_limits<DATA_TYPE>::min();

                for (auto i = pos_start; i <= pos_end; i++) {
                    auto currentValue = curvatures.at<DATA_TYPE>(x, i, 0);
                    if (currentValue > maxValue) {
                        maxValue = currentValue;
                        index = i - pos_start;
                    }
                }

                int pos_max = pos_start + index - 1;
                scr = curvatures.at<DATA_TYPE>(x, pos_max, 0) * wr;
                centers.at<DATA_TYPE>(x, pos_max) += scr;
                wr = 0;
            }
        }
    }

    for (auto y = 0; y < imageSize.width; y++) {
        for (auto x = 0; x < imageSize.height; x++) {
            if (curvatures.at<DATA_TYPE>(x, y, 1))
                wr++;

            if (wr > 0 &&
                (x == imageSize.height - 1 || (curvatures.at<DATA_TYPE>(x, y, 1) != 0))) {
                int pos_end;
                if (x == imageSize.height - 1)
                    pos_end = x;
                else
                    pos_end = x - 1;

                int pos_start = pos_end - wr + 1;

                int index = 0;
                DATA_TYPE maxValue = std::numeric_limits<DATA_TYPE>::min();

                for (auto i = pos_start; i <= pos_end; i++) {
                    auto currentValue = curvatures.at<DATA_TYPE>(i, y, 1);
                    if (currentValue > maxValue) {
                        maxValue = currentValue;
                        index = i - pos_start;
                    }
                }

                int pos_max = pos_start + index - 1;
                scr = curvatures.at<DATA_TYPE>(pos_max, y, 1) * wr;
                centers.at<DATA_TYPE>(pos_max, y) += scr;
                wr = 0;
            }
        }
    }

    for (auto start = 0; start < imageSize.height + imageSize.width - 1; start++) {
        int x, y;

        if (start < imageSize.width) {
            x = start;
            y = 0;
        } else {
            x = 0;
            y = start - imageSize.width + 1;
        }

        while(true) {
            if (curvatures.at<DATA_TYPE>(y, x, 2) > 0)
                wr++;

            if (wr > 0 &&
                    (y == imageSize.height - 1 || x == imageSize.width - 1 ||
                            curvatures.at<DATA_TYPE>(y, x, 2) != 0)) {
                int pos_x_end, pos_y_end;
                if (y == imageSize.height - 1 || x == imageSize.width - 1) {
                    pos_x_end = x;
                    pos_y_end = y;
                } else {
                    pos_x_end = x - 1;
                    pos_y_end = x - 1;
                }
                int pos_x_start = pos_x_end - wr + 1;
                int pos_y_start = pos_y_end - wr + 1;

                int index = 0;
                DATA_TYPE maxValue = std::numeric_limits<DATA_TYPE>::min();

                for (int i = pos_y_start, j = pos_x_start;
                     i <= pos_y_end && j <= pos_x_end; i++, j++) {
                    auto currentValue = curvatures.at<DATA_TYPE>(i, j, 2);
                    if (currentValue > maxValue) {
                        maxValue = currentValue;
                        index = i - pos_y_start;
                    }
                }

                int pos_x_max = pos_x_start + index - 1;
                int pos_y_max = pos_y_start + index - 1;
                scr = curvatures.at<DATA_TYPE>(pos_y_max, pos_x_max, 2) * wr;
                centers.at<DATA_TYPE>(pos_y_max, pos_x_max) += scr;
                wr = 0;
            }


            if ((x == imageSize.width - 1) || (y == imageSize.height - 1)) {
                break;
            } else {
                x++;
                y++;
            }
        }

    }

    for (auto start = 0; start < imageSize.height + imageSize.width - 1; start++) {
        int x, y;

        if (start < imageSize.width) {
            x = start;
            y = imageSize.height;
        } else {
            x = 0;
            y = imageSize.width + imageSize.height - start;
        }

        while(true) {
            if (curvatures.at<DATA_TYPE>(y, x, 3) > 0)
                wr++;

            if (wr > 0 &&
                (y == 0 || (x == imageSize.width - 1) ||
                        (curvatures.at<DATA_TYPE>(y, x, 3) != 0))) {
                int pos_x_end, pos_y_end;
                if (y == 0 || x == imageSize.width - 1) {
                    pos_x_end = x;
                    pos_y_end = y;
                } else {
                    pos_x_end = x - 1;
                    pos_y_end = y + 1;
                }
                int pos_x_start = pos_x_end - wr + 1;
                int pos_y_start = pos_y_end + wr - 1;

                int index = 0;
                DATA_TYPE maxValue = std::numeric_limits<DATA_TYPE>::min();

                assert(pos_y_start >= pos_y_end);
                assert(pos_x_end >= pos_x_start);

                for (int i = pos_y_end, j = pos_x_start;
                     i <= pos_y_start && j <= pos_x_end; i++, j++) {
                    auto currentValue = curvatures.at<DATA_TYPE>(i, j, 3);
                    if (currentValue > maxValue) {
                        maxValue = currentValue;
                        index = i - pos_y_end;
                    }
                }

                int pos_x_max = pos_x_start + index - 1;
                int pos_y_max = pos_y_start - index + 1;
                scr = curvatures.at<DATA_TYPE>(pos_y_max, pos_x_max, 3) * wr;
                centers.at<DATA_TYPE>(pos_y_max, pos_x_max) += scr;
                wr = 0;
            }

            assert(x < imageSize.width);
            assert(y >= 0);

            if ((x == imageSize.width - 1) || (y == 0)) {
                break;
            } else {
                x++;
                y--;
            }
        }
    }

    return centers;
}

