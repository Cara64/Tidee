#include <QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
    cv::Mat image;

    image = cv::imread("lena.jpg");   // Read the file
    if(!image.data)
    {
        qDebug() << "nenhuma imagem!";
    }
    else
    {
        qDebug() << "imagem carregada!";
    }
    cv::namedWindow("Teste imagem");
    cv::imshow("Teste imagem", image);
    cv::waitKey(0);
    return 0;
}
