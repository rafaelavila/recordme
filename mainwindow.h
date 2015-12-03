#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <QtSerialPort/QtSerialPort>

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonPlay_clicked();
    void on_pushButtonStop_clicked();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void read();

private:
    Ui::MainWindow *ui;
    double timerI, timerF;
    bool stopButton;
    bool cap;
    bool view;

    VideoCapture vcap;
    VideoCapture vcap2;
    Mat frame, frame2, src;
    std::vector<VideoWriter> videos;
    std::vector<cv::Mat> frames;
    std::vector<cv::Mat> frames2;

    QSerialPort serialPort;
    QString string;
    QString videoName;

    int cnt, cntVideos;
    double tmedio;
    int frame_width,frame_height;
    bool save;

    bool video();

    bool connectPort(QString portName);
    void addSong(QString videoName);
    void checkTime();
    void saveVideo();
};

#endif // MAINWINDOW_H
