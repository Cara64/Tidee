#include "QtdScanDocument.h"
#include "QtdProcessingTools.h"
#include <QDebug>
#include <iostream>

QtdScanDocument::QtdScanDocument(QObject *parent) : QObject(parent)
{

}

QtdScanDocument::~QtdScanDocument()
{

}

void QtdScanDocument::scanDocument(cv::Mat &image, cv::Mat &result, int newWidth, int newHeight)
{
    qDebug() << "Processing scan documents... \n";

    cv::Mat transformed;

    double ratio = image.rows / 500.0;
    cv::Mat orig = image.clone();

    image = QtdProcessingTools::resize(image, 0, 500);

    qDebug() << "Finding contours in edged image... \n";

    std::vector<std::vector<cv::Point>> contours;
    contours = detectEdges(image);

    std::cout << "Number of contours detected is " << contours.size() << std::endl;

    qDebug() << "Finding document contour... \n";

    std::vector<cv::Point> documentContours;
    documentContours = findDocumentContour(contours);

    for (int i = 0; i < documentContours.size(); i++) {
        std::cout << "Approx " << i << " = " << documentContours[i] << std::endl;
    }

#ifdef QT_DEBUG
    qDebug() << "[DEBUG] Displaying contours found of document \n";
    std::vector<std::vector<cv::Point>> contoursFound;
    contoursFound.push_back(documentContours);

    cv::drawContours(image, contoursFound, -1, cv::Scalar(0,255,0), 2);
    cv::imshow("Outline", image);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    qDebug() << "Applying transformation... \n";

    transformed = applyTransformation(orig, documentContours, ratio);

#ifdef QT_DEBUG
    qDebug() << "[DEBUG] Displaying transformed matrix \n";
    cv::imshow("Warped", QtdProcessingTools::resize(transformed, newWidth, newHeight));
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    qDebug() << "Applying paper effects... \n";

    result = givePaperEffect(transformed);

#ifdef QT_DEBUG
    qDebug() << "[DEBUG] Displaying result \n";
    cv::imshow("Scanned", QtdProcessingTools::resize(result, newWidth, newHeight));
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

}

std::vector<std::vector<cv::Point>> QtdScanDocument::detectEdges(cv::Mat &image, double treshold1, double treshold2)
{
    cv::Mat gray;
    cv::Mat edged;
    std::vector<std::vector<cv::Point>> contours;

    qDebug() << "Applying filter for Canny filter \n";
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5,5),0);
    cv::Canny(gray, edged, treshold1, treshold2);

#ifdef QT_DEBUG
    qDebug() << "[DEBUG] : Displaying results of filter \n";
    cv::imshow("Image", image);
    cv::imshow("Gray", gray);
    cv::imshow("Edged", edged);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    qDebug() << "Finding contours and sorting them \n";
    cv::findContours(edged, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compareContours);

    return contours;
}

std::vector<cv::Point> QtdScanDocument::findDocumentContour(std::vector<std::vector<cv::Point> > &contours)
{
    bool screenFound = false;
    int i = 0;
    double peri;
    std::vector<cv::Point> approx;

    qDebug() << "Beginning to loop through the " << contours.size() << "detected contours \n";
    while (i < contours.size() && !screenFound)
    {
        qDebug() << "Contours iteration # " << i << "\n";
        peri = cv::arcLength(contours[i], true);
        cv::approxPolyDP(contours[i], approx, 0.02 * peri, true);

        if (approx.size() == 4)
        {
            qDebug() << "Document contours found at iteration # " << i << "\n";
            screenFound = true;
        }

        i++;
    }

    return approx;
}

cv::Mat QtdScanDocument::applyTransformation(cv::Mat &image, std::vector<cv::Point> &documentContours, double ratio)
{
    qDebug() << "Transferring document contours vector values into Matrix \n";
    std::cout << "Ratio inside applyTransformation method is " << ratio << std::endl;
    cv::Mat contourPaper(4,2, CV_32F);
    for (int i = 0; i < documentContours.size(); i++)
    {
        contourPaper.at<float>(i,0) = documentContours[i].x;
        contourPaper.at<float>(i,1) = documentContours[i].y;
        std::cout << " X" << i <<" = " << contourPaper.at<float>(i,0) << std::endl;
        std::cout << " Y" << i <<" = " << contourPaper.at<float>(i,1) << std::endl;
    }

    contourPaper *= ratio;

    qDebug() << "Applying four points transforming and return Matrix \n";
    return QtdProcessingTools::fourPointTransform(image, contourPaper);
}

cv::Mat QtdScanDocument::givePaperEffect(cv::Mat &image)
{
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(image, image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 251, 10);
    image.convertTo(image, CV_8U);
    image *= 255;

    return image;
}


bool QtdScanDocument::compareContours(std::vector<cv::Point> &contour1, std::vector<cv::Point> &contour2)
{
    double i = fabs(cv::contourArea(cv::Mat(contour1)));
    double j = fabs(cv::contourArea(cv::Mat(contour2)));
    return (i > j);
}
