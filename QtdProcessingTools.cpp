#include "QtdProcessingTools.h"
#include <QDebug>
#include <iostream>

QtdProcessingTools::QtdProcessingTools(QObject *parent) : QObject(parent)
{

}


QtdProcessingTools::~QtdProcessingTools()
{

}


cv::Mat QtdProcessingTools::fourPointTransform(cv::Mat &image, cv::Mat &points)
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


cv::Mat QtdProcessingTools::resize(cv::Mat &image, int width, int height, cv::InterpolationFlags inter)
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


QImage QtdProcessingTools::convert(cv::Mat image)
{
    /*!< TODO */
    return QImage();
}


cv::Mat QtdProcessingTools::convert(QImage image)
{
    /*!< TODO */
    return cv::Mat();
}


cv::Mat QtdProcessingTools::orderPoints(cv::Mat &points)
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

