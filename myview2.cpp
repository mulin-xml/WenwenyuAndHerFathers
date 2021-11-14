#include "myview2.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMessageBox>
#include <QWheelEvent>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

MyView2::MyView2(QWidget *parent) : QGraphicsView(parent) {
    QRectF rect(-100, -100, 200, 200);
    auto scene = new QGraphicsScene(rect);
    setScene(scene);

    imgItem = new QGraphicsPixmapItem();
    imgItem->setFlags(QGraphicsItem::ItemIsMovable);
    scene->addItem(imgItem);

    auto rectItem = new QGraphicsRectItem(rect);
    auto pen = rectItem->pen();
    pen.setWidth(2);
    pen.setColor(Qt::red);
    rectItem->setPen(pen);
    scene->addItem(rectItem);

    textItem = new QGraphicsSimpleTextItem();
    textItem->setPos(100, 100);
    textItem->setText("um");
    textItem->font().setPixelSize(30);
    textItem->setBrush(QBrush(Qt::red));
    scene->addItem(textItem);
}

void MyView2::wheelEvent(QWheelEvent *event) {
    auto wheelDeltaValue = event->delta();

    if (wheelDeltaValue > 0) {
        imgItem->setScale(imgItem->scale() * 1.1);
    } else {
        imgItem->setScale(imgItem->scale() / 1.1);
    }
    textItem->setText(QString::number(cutCenter().width * 0.09766) + "um");
    QGraphicsView::wheelEvent(event);
}

void MyView2::mouseMoveEvent(QMouseEvent *event) {
    if (isHold) {
        auto delta = event->pos() - lastMousePos;
        imgItem->setPos(imgItem->x() + delta.x(), imgItem->y() + delta.y());
    }
    lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void MyView2::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isHold = true;
        lastMousePos = event->pos();
    }

    QGraphicsView::mousePressEvent(event);
}

void MyView2::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) isHold = false;

    QGraphicsView::mouseReleaseEvent(event);
}

void MyView2::showImg(cv::Mat img) {
    imgItem->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8)));
    imgItem->setPos(-img.cols / 2, -img.rows / 2);
    textItem->setText(QString::number(cutCenter().width * 0.09766) + "um");
}

cv::Rect MyView2::cutCenter() {
    auto a = imgItem->mapFromScene(QPoint(-100, -100));
    auto b = imgItem->mapFromScene(QPoint(100, 100));
    return cv::Rect(a.x(), a.y(), b.x() - a.x(), b.y() - a.y());
}
