#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextCodec>
#include <QWheelEvent>

#include "mainwindow.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

namespace {
Mat storedImg, fusionImg;
int displayMinValue, displayMaxValue;
}  // namespace

Mat GetHist() {
    Mat dstHist;
    float range[] = {0, 255};
    const float* ranges[] = {range};
    int size = 256;

    calcHist(&storedImg, 1, 0, Mat(), dstHist, 1, &size, ranges);

    double minValue = 0;
    double maxValue = 0;
    minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);
    int hpt = saturate_cast<int>(0.9 * 256);

    Mat dstImage(256, 256, CV_8UC1, Scalar(0));
    for (int i = 0; i < 256; i++) {
        int realValue = saturate_cast<int>(dstHist.at<float>(i) * hpt / maxValue);
        rectangle(dstImage, Point(i, 255), Point(i, 256 - realValue), Scalar(128));
    }
    return dstImage;
}

void MainWindow::on_pushButton_14_clicked() {
    auto path = QFileDialog::getOpenFileName(nullptr, "打开图片", ".", "Image file(*.tif)");
    auto img = imread(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), IMREAD_UNCHANGED);
    if (img.empty()) {
        return;
    }
    ui->lineEdit_13->setText(path);

    double minv, maxv;
    minMaxIdx(img, &minv, &maxv);  //取得像素值最大值和最小值

    displayMinValue = 0;
    displayMaxValue = 255;

    storedImg = Mat::zeros(img.size(), CV_8UC1);
    fusionImg = Mat::zeros(img.size(), CV_8UC1);

    for (int i = 0; i < img.rows; i++) {
        uint16_t* p_img = img.ptr<uint16_t>(i);
        uint8_t* p_stored = storedImg.ptr<uint8_t>(i);
        uint8_t* p_fusion = fusionImg.ptr<uint8_t>(i);
        for (int j = 0; j < img.cols; ++j) {
            p_stored[j] = (p_img[j] - minv) / (maxv - minv) * 255;
            p_fusion[j] = (p_img[j] - minv) / (maxv - minv) * 255;
        }
    }

    ui->graphicsView_3->showImg(fusionImg, QImage::Format_Grayscale8);
    auto dstImage = GetHist();
    cv::resize(dstImage, dstImage, Size(ui->label_15->width(), ui->label_15->height()));
    ui->label_15->setPixmap(QPixmap::fromImage(QImage(dstImage.data, dstImage.cols, dstImage.rows, dstImage.step, QImage::Format_Grayscale8)));

    // 显示位置居中，尺度重置
    ui->graphicsView_3->imgItem->setScale(1.0);
    ui->graphicsView_3->imgItem->setPos(-storedImg.cols / 2, -storedImg.rows / 2);
    ui->graphicsView_3->textItem->setText(QString::number(ui->graphicsView_3->cutCenter().width * 0.09766) + "um");
}

void MainWindow::on_pushButton_10_clicked() {
    auto path = QFileDialog::getSaveFileName(this, "保存图片", ".", "Image file(*.png)");
    auto rect = ui->graphicsView_3->cutCenter();

    cv::imwrite(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), fusionImg(rect));
}
