#include "myview2.h"
#include <QMessageBox>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

MyView2::MyView2(QWidget *parent):QGraphicsView(parent)
{
    auto scene = new QGraphicsScene;//图像显示
    imgItem = new QGraphicsPixmapItem();
    scene->addItem(imgItem);
    setScene(scene);
    //    contours.resize(12);
}

void MyView2::wheelEvent(QWheelEvent *event)
{
    auto wheelDeltaValue = event->delta();

    if (wheelDeltaValue > 0){
        this->scale(1.2, 1.2);
    } else {
        this->scale(1.0 / 1.2, 1.0 / 1.2);
    }
    QGraphicsView::wheelEvent(event);
}

void MyView2::mouseMoveEvent(QMouseEvent *event)
{
    if (isHold){
        auto delta = event->pos() - lastMousePos;
        QPoint newCenter(width()*0.5-delta.x(),height()*0.5-delta.y());
        centerOn(mapToScene(newCenter));
    }
    lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void MyView2::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        isHold = true;
        lastMousePos = event->pos();
    }

    QGraphicsView::mousePressEvent(event);
}

void MyView2::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        isHold = false;

    QGraphicsView::mouseReleaseEvent(event);
}

void MyView2::showImg(cv::Mat img)
{
    imgItem->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows,img.step, QImage::Format_Grayscale8)));
}

