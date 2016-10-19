#ifndef QTDSCANDOCUMENT_H
#define QTDSCANDOCUMENT_H

#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*!
 * \brief Scanning documents primitive
 */
class QtdScanDocument : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent QObject
     */
    explicit QtdScanDocument(QObject *parent = 0);

    /*!
     * \brief Destructor
     */
    ~QtdScanDocument();

    /*!
     * \brief Scanning document primitives
     * \param image Original image to scan
     * \param result Result matrix of scanned image
     * \param newWidth New width of the image to resize (by default 0)
     * \param newHeight New height of the image to resize (by default 650)
     */
    static void scanDocument(cv::Mat &image, cv::Mat &result, int newWidth = 0, int newHeight = 650);

private:
    /*!
     * \brief Edge detection primitive
     * \param image Original image to detect edges
     * \param treshold1 Treshold alpha (by default 75)
     * \param treshold2 Treshold beta (by default 200)
     * \return Vector or Vector of Point for each edges detected
     */
    static std::vector<std::vector<cv::Point>> detectEdges(cv::Mat &image, double treshold1 = 75, double treshold2 = 200);

    /*!
     * \brief Find document edges
     * \param contours All edges detected on the image
     * \return Vector of point of document edges
     */
    static std::vector<cv::Point> findDocumentContour(std::vector<std::vector<cv::Point>> &contours);

    /*!
     * \brief Applying the top down transformation
     * \param image Image to apply the top down transformation
     * \param documentContours Vectors of point of document contours
     * \param ratio Ratio for the resize
     * \return Matrix of image with applied transformation
     */
    static cv::Mat applyTransformation(cv::Mat &image, std::vector<cv::Point> &documentContours, double ratio);

    /*!
     * \brief Give paper effect on the image (black and white)
     * \param image Image to apply the effect
     * \return Matrix of image with applied effect
     */
    static cv::Mat givePaperEffect(cv::Mat &image);

    /*!
     * \brief Comparing absolute value of contour
     * \param contour1 Contour 1 to compare
     * \param contour2 Contour 2 to compare
     * \return true if contour1 > contour2, else false
     */
    static bool compareContours(std::vector<cv::Point> &contour1, std::vector<cv::Point> &contour2);
};

#endif // QTDSCANDOCUMENT_H
