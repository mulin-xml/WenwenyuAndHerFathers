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

Mat storedImg;
Mat fusionImg;
Size imgSize(0, 0);
bool isSettingSliderValue = false;

class ChannelInfo {
   public:
    ChannelInfo(int i) : offset(i) {}
    const int offset;
    QLabel* histLabel;
    int displayMinValue = 0;
    int displayMaxValue = 255;
    Mat storedHist;

    void Adjust() {
        histLabel = nullptr;
        for (int i = 0; i < fusionImg.rows; i++) {
            uint8_t* p_fusion = fusionImg.ptr<uint8_t>(i);  //获取每行首地址
            uint8_t* p_stored = storedImg.ptr<uint8_t>(i);  //获取每行首地址
            for (int j = offset; j < fusionImg.cols * fusionImg.channels(); j += fusionImg.channels()) {
                p_fusion[j] = saturate_cast<uint8_t>((double)(static_cast<int>(p_stored[j]) - displayMinValue) / (displayMaxValue - displayMinValue) * 255);
            }
        }
    }

    void GetHist() {
        vector<Mat> channels;
        cv::split(storedImg, channels);
        auto grayImg = channels.at(offset);
        Mat dstHist;
        float range[] = {0, 255};
        const float* ranges[] = {range};
        int size = 256;

        calcHist(&grayImg, 1, 0, Mat(), dstHist, 1, &size, ranges);

        double minValue = 0;
        double maxValue = 0;
        minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);
        int hpt = saturate_cast<int>(0.9 * 256);

        Mat dstImage(256, 256, CV_8UC1, Scalar(0));
        for (int i = 0; i < 256; i++) {
            int realValue = saturate_cast<int>(dstHist.at<float>(i) * hpt / maxValue);
            rectangle(dstImage, Point(i, 255), Point(i, 256 - realValue), Scalar(128));
        }
        storedHist = dstImage;
        UpdateHist();
    }

    void UpdateHist() {
        Mat dstImage = storedHist.clone();
        cv::resize(dstImage, dstImage, Size(histLabel->width(), histLabel->height()));
        histLabel->setPixmap(QPixmap::fromImage(QImage(dstImage.data, dstImage.cols, dstImage.rows, dstImage.step, QImage::Format_Grayscale8)));
    }
};

ChannelInfo blue{0};
ChannelInfo green{1};
ChannelInfo red{2};

auto selectedChannel = &green;

void MainWindow::ChannelInit() {
    blue.histLabel = ui->label_12;
    green.histLabel = ui->label_13;
    red.histLabel = ui->label_14;
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    blue.displayMinValue = ui->horizontalSlider->value();
    blue.displayMaxValue = ui->horizontalSlider_2->value();
    blue.Adjust();

    green.displayMinValue = ui->horizontalSlider_5->value();
    green.displayMaxValue = ui->horizontalSlider_6->value();
    green.Adjust();

    red.displayMinValue = ui->horizontalSlider_9->value();
    red.displayMaxValue = ui->horizontalSlider_10->value();
    red.Adjust();

    ShowSelectedImg();
}

// 打开图片
void MainWindow::on_pushButton_13_clicked() {
    auto path = QFileDialog::getOpenFileName(nullptr, "打开图片", ".", "Image file(*.tif)");
    auto img = imread(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), IMREAD_UNCHANGED);
    if (img.empty()) {
        return;
    }
    ui->lineEdit_13->setText(path);

    // 尺寸不一致，重置三通道
    if (img.size() != storedImg.size()) {
        storedImg = Mat::zeros(img.size(), CV_8UC3);
        fusionImg = Mat::zeros(img.size(), CV_8UC3);
    }

    double minv, maxv;
    minMaxIdx(img, &minv, &maxv);  //取得像素值最大值和最小值

    selectedChannel->displayMinValue = 0;
    selectedChannel->displayMaxValue = 255;
    for (int i = 0; i < img.rows; i++) {
        uint16_t* p_img = img.ptr<uint16_t>(i);
        uint8_t* p_stored = storedImg.ptr<uint8_t>(i) + selectedChannel->offset;
        uint8_t* p_fusion = fusionImg.ptr<uint8_t>(i) + selectedChannel->offset;
        for (int j = 0; j < img.cols; ++j) {
            p_stored[j * 3] = (p_img[j] - minv) / (maxv - minv) * 255;
            p_fusion[j * 3] = (p_img[j] - minv) / (maxv - minv) * 255;
        }
    }
    ShowSelectedImg();
    selectedChannel->GetHist();

    // 显示位置居中，尺度重置
    ui->graphicsView_2->imgItem->setScale(1.0);
    ui->graphicsView_2->imgItem->setPos(-storedImg.cols / 2, -storedImg.rows / 2);
    ui->graphicsView_2->textItem->setText(QString::number(ui->graphicsView_2->cutCenter().width * 0.09766) + "um");
}

Mat GetSelectedImg() {
    if (selectedChannel == nullptr or fusionImg.empty()) {
        return fusionImg;
    } else {
        vector<Mat> channels;
        cv::split(fusionImg, channels);
        for (int i = 0; i < 3; i++) {
            if (i != selectedChannel->offset) {
                channels.at(i) = Mat::zeros(fusionImg.size(), CV_8UC1);
            }
        }
        Mat dstImg;
        cv::merge(channels, dstImg);
        return dstImg;
    }
}

// 保存选区
void MainWindow::on_pushButton_9_clicked() {
    auto path = QFileDialog::getSaveFileName(this, "保存图片", ".", "Image file(*.png)");
    auto rect = ui->graphicsView_2->cutCenter();
    auto img = GetSelectedImg();
    if (!img.empty()) {
        cv::imwrite(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), img(rect));
    }
}

void MainWindow::ShowSelectedImg() {
    auto img = GetSelectedImg();
    if (!img.empty()) {
        ui->graphicsView_2->showImg(img);
    }
}

void MainWindow::on_radioButton_clicked() {
    ui->pushButton_13->setEnabled(true);
    selectedChannel = &blue;
    ShowSelectedImg();
}

void MainWindow::on_radioButton_2_clicked() {
    ui->pushButton_13->setEnabled(true);
    selectedChannel = &green;
    ShowSelectedImg();
}

void MainWindow::on_radioButton_3_clicked() {
    ui->pushButton_13->setEnabled(true);
    selectedChannel = &red;
    ShowSelectedImg();
}

void MainWindow::on_radioButton_4_clicked() {
    ui->pushButton_13->setEnabled(false);
    selectedChannel = nullptr;
    ShowSelectedImg();
}
