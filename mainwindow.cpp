#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextCodec>
#include <QWheelEvent>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setFixedSize(width(), height());
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
}

MainWindow::~MainWindow() { delete ui; }

namespace TXCF {
Mat srcImg, dstImg;
bool isOpen = false;
}  // namespace TXCF

void MainWindow::on_pushButton_clicked() {
    using namespace TXCF;
    auto path = QFileDialog::getOpenFileName(nullptr, "打开图片", ".", "Image file(*.png *.jpg *.bmp)");
    srcImg = imread(QTextCodec::codecForName("GB2312")->fromUnicode(path).data());
    if (srcImg.empty()) {
        return;
    }
    isOpen = true;
    ui->lineEdit->setText(path);
    ui->lineEdit_2->setText(QString::number(srcImg.cols));
    ui->lineEdit_3->setText(QString::number(srcImg.rows));
    ui->lineEdit_4->setText(QString::number((double)srcImg.rows * srcImg.cols * 3 / 1048576));
    Mat thumbImg;
    auto rate = (double)std::min(ui->label->size().width(), ui->label->size().height()) / std::max(srcImg.cols, srcImg.rows);
    cv::resize(srcImg, thumbImg, Size(srcImg.cols * rate, srcImg.rows * rate));
    QImage img = QImage(thumbImg.data, thumbImg.cols, thumbImg.rows, thumbImg.step, QImage::Format_BGR888);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_2_clicked() {
    using namespace TXCF;
    if (!isOpen) {
        return;
    }
    auto rate = ui->lineEdit_5->text().toDouble();

    if (rate <= 0 || rate > 5) {
        return;
    }

    cv::resize(srcImg, dstImg, Size(srcImg.cols * rate, srcImg.rows * rate));
    ui->lineEdit_6->setText(QString::number(dstImg.cols));
    ui->lineEdit_7->setText(QString::number(dstImg.rows));
    ui->lineEdit_8->setText(QString::number((double)dstImg.rows * dstImg.cols * 3 / 1048576));
}

void MainWindow::on_pushButton_3_clicked() {
    using namespace TXCF;
    auto path = QFileDialog::getSaveFileName();
    cv::imwrite(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), dstImg);
}

namespace JSHD {
int x, y;
}

void MainWindow::on_pushButton_4_clicked() {
    auto path = QFileDialog::getOpenFileName(nullptr, "打开图片", ".", "Image file(*.png *.jpg *.bmp *.tif)");

    auto& srcImg = ui->graphicsView->srcImg;
    auto& grayImg = ui->graphicsView->grayImg;
    srcImg = imread(QTextCodec::codecForName("GB2312")->fromUnicode(path).data());
    if (srcImg.empty()) {
        return;
    }

    ui->lineEdit_9->setText(path);
    ui->lineEdit_10->setText(QString::number(srcImg.cols));
    ui->lineEdit_11->setText(QString::number(srcImg.rows));

    cvtColor(srcImg, grayImg, COLOR_RGB2GRAY);
    //单通道图像的反色
    for (int row = 0; row < grayImg.rows; row++) {
        for (int col = 0; col < grayImg.cols; col++) {
            grayImg.at<uchar>(row, col) = 255 - grayImg.at<uchar>(row, col);
        }
    }

    ui->graphicsView->showImg(srcImg);
}

void MainWindow::on_pushButton_5_clicked() {}

// 开始框选
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column) {
    using namespace JSHD;
    JSHD::x = row;
    JSHD::y = column;
    ui->graphicsView->isDraw = true;
    ui->graphicsView->contour.clear();
    ui->graphicsView->tmpImg = ui->graphicsView->srcImg.clone();

    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
}

// 确认区域
void MainWindow::on_pushButton_6_clicked() {
    ui->graphicsView->isDraw = false;
    //    ui->graphicsView->contours.push_back(ui->graphicsView->contour);
    std::vector<std::vector<Point>> contours;
    contours.push_back(ui->graphicsView->contour);

    Mat mask = Mat::zeros(ui->graphicsView->tmpImg.size(), CV_8UC1);
    cv::fillPoly(mask, contours,
                 cv::Scalar(255));  // fillPoly函数的第二个参数是二维数组！！
    //    cv::fillPoly(ui->graphicsView->srcImg, contours,
    //    cv::Scalar(255,255,0));//fillPoly函数的第二个参数是二维数组！！
    //    cv::polylines(ui->graphicsView->srcImg, contours, true,
    //    cv::Scalar(255, 255, 0), 1,
    //    cv::LINE_AA);//第2个参数可以采用contour或者contours，均可
    Mat img2;
    ui->graphicsView->grayImg.copyTo(img2, mask);

    ui->graphicsView->showImg(ui->graphicsView->srcImg);

    using namespace JSHD;

    ui->tableWidget->setItem(JSHD::x, JSHD::y, new QTableWidgetItem(QString::number(sum(img2)[0])));
    ui->tableWidget->setItem(JSHD::x, JSHD::y + 3, new QTableWidgetItem(QString::number(sum(img2)[0] - cv::countNonZero(mask) * 40)));

    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);

    if (ui->tableWidget->item(JSHD::x, 0) != nullptr && ui->tableWidget->item(JSHD::x, 1) != nullptr) {
        auto a = ui->tableWidget->item(JSHD::x, 0)->text().toInt();
        auto b = ui->tableWidget->item(JSHD::x, 1)->text().toInt();
        ui->tableWidget->setItem(JSHD::x, 2, new QTableWidgetItem(QString::number((double)b / (a + b))));
        auto c = ui->tableWidget->item(JSHD::x, 3)->text().toInt();
        auto d = ui->tableWidget->item(JSHD::x, 4)->text().toInt();
        ui->tableWidget->setItem(JSHD::x, 5, new QTableWidgetItem(QString::number((double)d / (c + d))));
    }
}

// 取消框选
void MainWindow::on_pushButton_7_clicked() {
    ui->graphicsView->isDraw = false;
    ui->graphicsView->showImg(ui->graphicsView->srcImg);

    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
}

void MainWindow::on_pushButton_8_clicked() {
    //    cv::imshow("grayImg",ui->graphicsView->grayImg);
}

namespace YGT {
Mat grayImg;
}

void MainWindow::on_pushButton_13_clicked() {
    auto path = QFileDialog::getOpenFileName(nullptr, "打开图片", ".", "Image file(*.tif)");
    auto srcImg = imread(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), IMREAD_UNCHANGED);
    if (srcImg.empty()) {
        return;
    }

    ui->lineEdit_13->setText(path);

    YGT::grayImg = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);  //先生成空的目标图片
    double minv = 0.0, maxv = 0.0;
    double* minp = &minv;
    double* maxp = &maxv;
    minMaxIdx(srcImg, minp, maxp);  //取得像素值最大值和最小值

    //用指针访问像素，速度更快
    ushort* p_img;
    uchar* p_dst;
    for (int i = 0; i < srcImg.rows; i++) {
        p_img = srcImg.ptr<ushort>(i);  //获取每行首地址
        p_dst = YGT::grayImg.ptr<uchar>(i);
        for (int j = 0; j < srcImg.cols; ++j) {
            p_dst[j] = (p_img[j] - minv) / (maxv - minv) * 255;
        }
    }
    ui->graphicsView_2->showImg(YGT::grayImg);
}

void MainWindow::on_pushButton_9_clicked() {
    auto path = QFileDialog::getSaveFileName(this, "保存图片", ".", "Image file(*.png)");
    auto rect = ui->graphicsView_2->cutCenter();
    ui->lineEdit_14->setText(QString::number(rect.width * 0.09766));
    cv::imwrite(QTextCodec::codecForName("GB2312")->fromUnicode(path).data(), YGT::grayImg(rect));
}
