/*! Main.cpp - Tidee SCAN
 *  Proof of concept of TeeDee Scan from a photo
 *  Inspired from : http://www.pyimagesearch.com/2014/09/01/build-kick-ass-mobile-document-scanner-just-5-minutes/
 *  Developped by Guillaume CARAYON - September 2019
 *  http://gcarayon.me
 */

#include <QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <QDebug>
#include <QtdProcessingTools.h>

cv::Mat image;  /*!< Original image */
cv::Mat orig;   /*!< Copy of the image */
cv::Mat gray;   /*!< Grayscale and blurried image */
cv::Mat edged;  /*!< Edge detected image */

/*!
 * \brief Compare absolue value of contours
 * \param contour1 First contour to compare
 * \param contour2 Second contour to compre
 * \return Boolean (true if contour1 > contour2)
 */
bool compareContours(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
{
    double i = fabs(cv::contourArea(cv::Mat(contour1)));
    double j = fabs(cv::contourArea(cv::Mat(contour2)));
    return (i > j);
}


int main(int argc, char *argv[])
{
    // Loading image and compute the ratio of the hold height
    image = cv::imread("paper.jpg");   // Insert this file in the build (Debug or Release) directory...
    double ratio = image.rows / 500.0;
    std::cout << "Ratio = " << ratio << std::endl;
    orig = image.clone();
    image = QtdProcessingTools::resize(image, 0, 500);

    // Convert image to grayscale, blur it and find edges
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5,5), 0);
    cv::Canny(gray, edged, 75, 200);

    // Show original and edge detected image
    qDebug() << "STEP 1 : Edge detection" << endl;
    cv::imshow("Image", image);
    cv::imshow("Gray", gray);
    cv::imshow("Edged", edged);
    cv::waitKey(0);
    cv::destroyAllWindows();

    // Find the contours in the edged image
    std::vector<std::vector<cv::Point>> contours;   // Vector including vector of point for contours
    cv::Mat edgedCopy;  // Copy of the edged photo

    edged.copyTo(edgedCopy);
    cv::findContours(edgedCopy, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContours);

    bool screenFound = false;
    int iterator = 0;
    double peri;
    std::vector<cv::Point> approx;
    std::vector<std::vector<cv::Point>> contourFound;

    // Loop through the contours
    while (iterator < contours.size() && !screenFound) {

        // Approximate the contour
        peri = cv::arcLength(contours[iterator], true);
        cv::approxPolyDP(contours[iterator], approx, 0.02 * peri, true);

        // If the contour has four points : it's our screen
        if (approx.size() == 4) {
            qDebug() << "Contour found" << endl;
            // contourPaper = cv::Mat(approx);
            contourFound.push_back(approx);
            screenFound = true;
        }

        iterator++;
    }


    // Show the contour (outline) of the piece of paper
    qDebug() << "STEP 2 : Find contours of paper" << endl;
    cv::drawContours(image, contourFound, -1, cv::Scalar(0, 255, 0), 2);

    cv::imshow("Outline", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    // Apply four points transform to obtain a top-down view of the original image
    cv::Mat contourPaper(4,2,CV_32F);
    cv::Point contourPoint;
    for (int i = 0; i < contourFound[0].size(); i++) {
        contourPaper.at<float>(i,0) = contourFound[0][i].x;
        contourPaper.at<float>(i,1) = contourFound[0][i].y;
    }

    contourPaper *= ratio;
    cv::Mat warped = QtdProcessingTools::fourPointTransform(orig, contourPaper);

    // Convert the warped image to grayscale, treshold it to give it black and white paper effect
    cv::cvtColor(warped, warped, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(warped, warped, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 251, 10);
    warped.convertTo(warped, CV_8U);
    warped *= 255;

    // Show the original and scanned images
    qDebug() << "STEP 3 : Apply perspective transform \n";
    cv::imshow("Original", QtdProcessingTools::resize(orig,0,650));
    cv::imshow("Scanned", QtdProcessingTools::resize(warped,0,650));
    cv::waitKey(0);

    return 0;


}
