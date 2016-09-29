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


/*!
 * \brief Order points in a Matrix
 * \param points Matrix of points (usually contour)
 * \return OpenCV Matrix where 1st entry is Top-Left point, 2nd is Top-Right point, 3rd is Bottom-Right point, 4th is Bottom-Left point
 */
cv::Mat orderPoints(cv::Mat points)
{
    qDebug() << "Ordering Points Processing \n";

    cv::Mat rect(4,2, CV_32F);                          // Coordinate with first entry as Top Left point, second entry as Top right, third as Bottom right, and last as Bottom left
    double minVal, maxVal;                              // Min and max value for sum and diff
    cv::Point minLoc, maxLoc;                           // Min and max location for sum and diff
    cv::Mat sumMat = cv::Mat::zeros(4,1, CV_32F);       // Vector of sum of each points
    cv::Mat diffMat = cv::Mat::zeros(4,1, CV_32F);      // Vector of diff of each points
    float pointX, pointY;                               // Storage for point for sum and diff calcul

    // Doing the sum of each point of points Matrix
    qDebug() << "Processing sum of each coordinates \n";
    for (int i = 0; i < sumMat.rows; i++) {
        pointX = points.at<float>(i,0);
        pointY = points.at<float>(i,1);
        sumMat.at<float>(i, 0) = pointX + pointY;
    }

    // Locating the min and max sum of points
    qDebug() << "Locating the min and max sum of points \n";
    cv::minMaxLoc(sumMat, &minVal, &maxVal, &minLoc, &maxLoc);
    rect.at<cv::Point>(0,0) = points.at<cv::Point>(minLoc);
    rect.at<cv::Point>(2,0) = points.at<cv::Point>(maxLoc);
    rect.at<float>(0,0) = points.at<float>(minLoc.x, minLoc.y);

    // Doing the diff of each point of points Matrix
    qDebug() << "Processing diff of each coordinates \n";
    for (int i = 0; i < diffMat.rows; i++) {
        pointX = points.at<float>(i,0);
        pointY = points.at<float>(i,1);
        diffMat.at<float>(i,0) = pointY - pointX;
    }

    // Locating the min and max diff of points
    qDebug() << "Locating the min and max diff of points \n";
    cv::minMaxLoc(diffMat, &minVal, &maxVal, &minLoc, &maxLoc);
    rect.at<cv::Point>(1,0) = points.at<cv::Point>(minLoc);
    rect.at<cv::Point>(3,0) = points.at<cv::Point>(maxLoc);

    std::cout << "Ordonned points matrix =    " << std::endl << cv::format(rect, cv::Formatter::FMT_NUMPY) << std::endl;

    return rect;
}


/*!
 * \brief Transform four points given to a top-down view
 * \param image Matrix of the original image
 * \param points Matrix of the contour points
 * \return OpenCV Matrix containing the warped photo
 */
cv::Mat fourPointTransform(cv::Mat image, cv::Mat points)
{
    qDebug() << "Four points transform processing \n";

    // Get points and order them
    qDebug() << "Getting points and ordering them \n";
    cv::Mat rect = orderPoints(points);
    rect.convertTo(rect, CV_32F);
    cv::Mat result;

    cv::Point topLeft(rect.at<float>(0,0), rect.at<float>(0,1));
    cv::Point topRight(rect.at<float>(1,0), rect.at<float>(1,1));
    cv::Point bottomRight(rect.at<float>(2,0), rect.at<float>(2,1));
    cv::Point bottomLeft(rect.at<float>(3,0), rect.at<float>(3,1));

    std::cout << "Top Left = " << topLeft << std::endl << "Top Right = " << topRight << std::endl << "Bottom right = " << bottomRight << std::endl << "Bottom left = " << bottomLeft << std::endl;

    // Determine width of the new image
    qDebug() << "Determining width of new image \n";
    double widthA = std::sqrt((std::pow((bottomRight.x - bottomLeft.x), 2) + (std::pow((bottomRight.y - bottomLeft.y), 2))));
    double widthB = std::sqrt((std::pow((topRight.x - topLeft.x), 2) + (std::pow((topRight.y - topLeft.y), 2))));
    double maxWidth = cv::max(int(widthA), int(widthB));

    std::cout << "Width A = " << widthA << std::endl << "Width B = " << widthB << std::endl << "Max width = " << maxWidth << std::endl;

    // Determine height of the new image
    qDebug() << "Determining height of the new image \n";
    double heightA = std::sqrt((std::pow((topRight.x - bottomRight.x), 2) + (std::pow((topRight.y - bottomRight.y), 2))));
    double heightB = std::sqrt((std::pow((topLeft.x - bottomLeft.x), 2) + (std::pow((topLeft.y - bottomLeft.y), 2))));
    double maxHeight = cv::max(int(heightA), int(heightB));

    std::cout << "Height A = " << heightA << std::endl << "Height B = " << heightB << std::endl << "Max height = " << maxHeight << std::endl;

    // Create the DST matrix
    qDebug() << "Creating the Dst Matrix \n";
    cv::Mat dst(4,2,CV_32F);
    dst.at<float>(0,0) = 0;
    dst.at<float>(0,1) = 0;
    dst.at<float>(1,0) = maxWidth-1;
    dst.at<float>(1,1) = 0;
    dst.at<float>(2,0) = maxWidth-1;
    dst.at<float>(2,1) = maxHeight-1;
    dst.at<float>(3,0) = 0;
    dst.at<float>(3,1) = maxHeight-1;

    std::cout << "Dst =   " << std::endl << cv::format(dst, cv::Formatter::FMT_NUMPY) << std::endl;

    // Do the transform
    qDebug() << "Doing the transform \n";
    cv::Mat M = cv::getPerspectiveTransform(rect, dst);
    cv::warpPerspective(image, result, M, cv::Size(maxWidth, maxHeight));

    return result;
}


/*!
 * \brief Resize an image
 * \param image Matrix of image
 * \param width Width of image
 * \param height Height of image
 * \param inter Interpolation for the resize
 * \return OpenCV Matrix resized
 */
cv::Mat resize(cv::Mat image, int width = 0, int height = 0, cv::InterpolationFlags inter = cv::INTER_AREA)
{
    double ratio;
    cv::Mat result;

    int curDim [2] = { image.size().width, image.size().height };

    if (width == 0 && height == 0) {
        image.copyTo(result);
    } else if (width == 0) {
        ratio = height / double(curDim[1]);
    } else {
        ratio = width / double(curDim[0]);
    }

    cv::resize(image, result, cv::Size(curDim[0]*ratio, curDim[1]*ratio), 0, 0, inter);

    return result;
}


int main(int argc, char *argv[])
{
    // Loading image and compute the ratio of the hold height
    image = cv::imread("paper.jpg");   // Insert this file in the build (Debug or Release) directory...
    double ratio = image.rows / 500.0;
    std::cout << "Ratio = " << ratio << std::endl;
    orig = image.clone();
    image = resize(image, 0, 500);

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
    cv::Mat warped = fourPointTransform(orig, contourPaper);

    // Convert the warped image to grayscale, treshold it to give it black and white paper effect
    cv::cvtColor(warped, warped, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(warped, warped, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 251, 10);
    warped.convertTo(warped, CV_8U);
    warped *= 255;

    // Show the original and scanned images
    qDebug() << "STEP 3 : Apply perspective transform \n";
    cv::imshow("Original", resize(orig,0,650));
    cv::imshow("Scanned", resize(warped,0,650));
    cv::waitKey(0);

    return 0;


}
