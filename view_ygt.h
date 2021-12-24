#ifndef MYVIEW2_H
#define MYVIEW2_H

#include <QGraphicsView>
#include <QObject>
#include <QPoint>
#include <opencv2/core.hpp>

using namespace cv;

class MyView2 : public QGraphicsView {
   public:
    MyView2(QWidget *parent = nullptr);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void showImg(cv::Mat img, QImage::Format format = QImage::Format_BGR888);
    cv::Rect cutCenter();
    QGraphicsPixmapItem *imgItem;
    QGraphicsSimpleTextItem *textItem;

   private:
    bool isHold = false;
    QPoint lastMousePos;
};

#endif  // MYVIEW2_H
