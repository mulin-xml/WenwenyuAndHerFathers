#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    //    bool eventFilter(QObject* watched, QEvent* event);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_9_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_radioButton_4_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_14_clicked();

    void on_pushButton_10_clicked();

private:
    Ui::MainWindow *ui;
    void ShowSelectedImg();
    void ChannelInit();
};

#endif  // MAINWINDOW_H
