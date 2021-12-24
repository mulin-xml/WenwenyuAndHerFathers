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
    auto selectionList = ui->tableWidget->selectedRanges();
    if (selectionList.count() == 0) return;

    int minLeft = selectionList.first().leftColumn();
    int minTop = selectionList.first().topRow();
    int maxRight = selectionList.first().rightColumn();
    int maxBottom = selectionList.first().bottomRow();

    for (const auto& range : selectionList) {
        minLeft = range.leftColumn() < minLeft ? range.leftColumn() : minLeft;
        minTop = range.topRow() < minTop ? range.topRow() : minTop;
        maxRight = range.rightColumn() > maxRight ? range.rightColumn() : maxRight;
        maxBottom = range.bottomRow() > maxBottom ? range.bottomRow() : maxBottom;
    }

    QString s;
    auto items = ui->tableWidget->selectedItems();

    for (int i = minTop; i <= maxBottom; i++) {
        for (int j = minLeft; j <= maxRight; j++) {
            const auto& it = ui->tableWidget->item(i, j);
            if (items.contains(it)) {
                s += it->text();
            } else {
                s += "";
            }
            if (j < maxRight) {
                s += "\t";
            }
        }
        if (i < maxBottom) {
            s += "\n";
        }
    }

    QApplication::clipboard()->setText(s);
}
