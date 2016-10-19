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
#include <QtdScanDocument.h>



int main(int argc, char *argv[])
{

    cv::Mat image = cv::imread("paper.jpg");
    cv::Mat result;

    QtdScanDocument::scanDocument(image, result);

    return 0;


}
