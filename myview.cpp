#include "myview.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMessageBox>
#include <QWheelEvent>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

MyView::MyView(QWidget *parent) : QGraphicsView(parent) {
    auto scene = new QGraphicsScene;  //图像显示
    imgItem = new QGraphicsPixmapItem();
    scene->addItem(imgItem);
    setScene(scene);
    //    contours.resize(12);
}

void MyView::wheelEvent(QWheelEvent *event) {
    auto wheelDeltaValue = event->delta();

    if (wheelDeltaValue > 0) {
        this->scale(1.2, 1.2);
    } else {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }
    QGraphicsView::wheelEvent(event);
}

void MyView::mouseMoveEvent(QMouseEvent *event) {
    if (isHold) {
        auto delta = event->pos() - lastMousePos;
        QPoint newCenter(width() * 0.5 - delta.x(), height() * 0.5 - delta.y());
        centerOn(mapToScene(newCenter));
    }
    lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void MyView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isHold = true;
        lastMousePos = event->pos();
    } else if (event->button() == Qt::LeftButton) {
        if (isDraw) {
            auto pos = mapToScene(event->pos());
            contour.push_back(Point(pos.x(), pos.y()));
            cv::circle(tmpImg, Point(pos.x(), pos.y()), 1, Scalar(0, 0, 255), 1);
            cv::polylines(tmpImg, contour, false, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);  //第2个参数可以采用contour或者contours，均可
            showImg(tmpImg);
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void MyView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) isHold = false;

    QGraphicsView::mouseReleaseEvent(event);
}

void MyView::showImg(cv::Mat img) { imgItem->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_BGR888))); }

void MyView::initImgInfo() {}
