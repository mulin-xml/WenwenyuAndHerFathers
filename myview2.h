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
    void showImg(cv::Mat img);
    cv::Rect cutCenter();

   private:
    bool isHold = false;
    QPoint lastMousePos;
    QGraphicsPixmapItem *imgItem;
    QGraphicsSimpleTextItem *textItem;
};

#endif  // MYVIEW2_H
