#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QPoint>
#include <opencv2/core.hpp>

using namespace cv;

class MyView : public QGraphicsView {
   public:
    MyView(QWidget *parent = nullptr);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void showImg(cv::Mat img);

    void initImgInfo();

    Mat srcImg;
    Mat grayImg;
    Mat tmpImg;
    bool isDraw = false;
    std::vector<Point> contour;

   private:
    bool isHold = false;
    QPoint lastMousePos;
    QGraphicsPixmapItem *imgItem;
};

#endif  // MYVIEW_H
