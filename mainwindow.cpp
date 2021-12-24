#include "mainwindow.h"

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

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_5, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_6, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_7, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_8, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_9, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_10, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_11, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    connect(ui->horizontalSlider_12, SIGNAL(valueChanged(int)), this, SLOT(on_horizontalSlider_valueChanged(int)));
    ChannelInit();
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


