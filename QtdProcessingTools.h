#ifndef QTDPROCESSINGTOOLS_H
#define QTDPROCESSINGTOOLS_H

#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


/*!
 * \brief Helping image processing tools primitives
 */
class QtdProcessingTools : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent QObject
     */
    explicit QtdProcessingTools(QObject *parent = 0);

    /*!
     * \brief Destructor
     */
    ~QtdProcessingTools();

    /*!
     * \brief Transform Four Points
     * \param image Original image matrix
     * \param points Points matrix of object to transform
     * \return Matrix with the transformed image
     */
    static cv::Mat fourPointTransform(cv::Mat image, cv::Mat points);

    /*!
     * \brief Resize an image matrix
     * \param image Original image matrix
     * \param width Width to resize
     * \param height Height to resize
     * \param inter Interpolation flag
     * \return Matrix with the resized image
     */
    static cv::Mat resize(cv::Mat image, int width = 0, int height = 0, cv::InterpolationFlags inter = cv::INTER_AREA);

private:
    /*!
     * \brief Ordering points of a contour object
     * \param points Points Matrix to order
     * \return Matrix with ordered points
     */
    static cv::Mat orderPoints(cv::Mat points);

};

#endif // QTDPROCESSINGTOOLS_H
