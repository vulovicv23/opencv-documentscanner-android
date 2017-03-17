#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <numeric>

using namespace cv;
using namespace std;


Mat GetSquareImage(const cv::Mat &img, int target_width = 500) {
    int width = img.cols,
            height = img.rows;

    cv::Mat square = cv::Mat::zeros(target_width, target_width, img.type());

    int max_dim = (width >= height) ? width : height;
    float scale = ((float) target_width) / max_dim;
    cv::Rect roi;
    if (width >= height) {
        roi.width = target_width;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = (target_width - roi.height) / 2;
    } else {
        roi.y = 0;
        roi.height = target_width;
        roi.width = width * scale;
        roi.x = (target_width - roi.width) / 2;
    }

    cv::resize(img, square(roi), roi.size(), 0, 0, CV_INTER_AREA);

    return square;
}

extern "C"
bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
    double i = fabs(contourArea(cv::Mat(contour1)));
    double j = fabs(contourArea(cv::Mat(contour2)));
    return (i > j);
}


extern "C"
JNIEXPORT void JNICALL
Java_prisca_ctest_ScanActivity_doWithMat(JNIEnv *env, jobject instance, jlong matNumber) {
    Mat &image = *(Mat *) matNumber;
    Rect bounding_rect;

    Mat thr(image.rows, image.cols, CV_8UC1);
    cvtColor(image, thr, CV_BGR2GRAY); //Convert to gray
    threshold(thr, thr, 150, 255, THRESH_BINARY + THRESH_OTSU); //Threshold the gray

    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    RotatedRect rect;
    findContours(thr, contours, hierarchy, CV_RETR_CCOMP,
                 CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    sort(contours.begin(), contours.end(),
         compareContourAreas);            //Store the index of largest contour
    bounding_rect = boundingRect(contours[0]);
    rect = minAreaRect(contours[0]);
    // matrices we'll use
    Mat rot_mat, rotated;
    // get angle and size from the bounding box
    float angle = rect.angle;
    Size rect_size = rect.size;

    if (rect.angle < -45.) {
        angle += 90.0;
        swap(rect_size.width, rect_size.height);
    }

    rot_mat = getRotationMatrix2D(rect.center, angle, 1);

    warpAffine(image, rotated, rot_mat, image.size(), INTER_CUBIC);

    image = rotated;

    cvtColor(image, thr, CV_BGR2GRAY); //Convert to gray
    threshold(thr, thr, 150, 255, THRESH_BINARY + THRESH_OTSU); //Threshold the gray

    findContours(thr, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourAreas);
    bounding_rect = boundingRect(contours[0]);

    image = Mat(image, bounding_rect);
}




extern "C"
JNIEXPORT void JNICALL
Java_prisca_ctest_OpenCvCamera_doWithMat(JNIEnv *env, jobject instance, jlong matAddrGr,
                                         jlong matAddrRgba) {
    try {
        Mat &image = *(Mat *) matAddrRgba;
        Rect bounding_rect;

        Mat thr(image.rows, image.cols, CV_8UC1);
        cvtColor(image, thr, CV_BGR2GRAY); //Convert to gray
        threshold(thr, thr, 150, 255, THRESH_BINARY + THRESH_OTSU); //Threshold the gray

        vector<vector<Point> > contours; // Vector for storing contour
        vector<Vec4i> hierarchy;
        findContours(thr, contours, hierarchy, CV_RETR_CCOMP,
                     CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
        sort(contours.begin(), contours.end(),
             compareContourAreas);            //Store the index of largest contour
        bounding_rect = boundingRect(contours[0]);

    rectangle(image, bounding_rect, Scalar(250, 250, 250) , 5);
    } catch (int a) {

    }
}



//drawContours(image, Mat(screenCnt), -1, color, 25);
//    if (shouldRun) {
////    drawContours(image, Mat(screenCnt), -1, color, 25);
//
//        // matrices we'll use
//        Mat rot_mat, rotated;
//        // get angle and size from the bounding box
//        float angle = rect.angle;
//        Size rect_size = rect.size;
//        // thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
//        if (rect.angle < -45.) {
//            angle += 90.0;
//            swap(rect_size.width, rect_size.height);
//        }
//
//        rot_mat = getRotationMatrix2D(rect.center, angle, 1);
//
//        warpAffine(image, rotated, rot_mat, image.size(), INTER_CUBIC);
//
//        image = rotated;
//
////    rotated = GetSquareImage(rotated);
//        cvtColor(rotated, gray, CV_BGR2GRAY);
//        GaussianBlur(gray, gray, Size(3, 3), 0);
//        Canny(gray, edged, lowThreshold, lowThreshold * ratio, 3);
////    image = edged;
//        vector<vector<Point> > contours1;
//        findContours(edged, contours1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//        // sort contours
//        sort(contours1.begin(), contours1.end(), compareContourAreas);
//        shouldRun = false;
//        largest_area = 0;
//        Rect boundRect;
//        for (int i = (int) (contours1.size() - 1); i > 0; i--) {
//            double peri = arcLength(Mat(contours1[i]), true);
//            vector<Point> approx;
//            approxPolyDP(Mat(contours1[i]), approx, 0.02 * peri, true);
//            if (approx.size() == 4) {
//                double a = contourArea(contours1[i], false);  //  Find the area of contour
//                if (a > largest_area && a > 10000) {
//                    largest_area = a;
//                    screenCnt = approx;
//                    boundRect = boundingRect(
//                            approx); // Find the bounding rectangle for biggest contour
//                    shouldRun = true;
//                    break;
//                }
//            }
//        }
//        if (shouldRun) {
////        drawContours(image, Mat(screenCnt), -1, color, 5);
//            image = Mat(image, boundRect).clone();
//        }
//    }