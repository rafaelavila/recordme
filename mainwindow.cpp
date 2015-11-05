#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/time.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <QtSerialPort/QtSerialPort>
#include "read_write.h"

using namespace std;
using namespace cv;


double time();
void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Filma Eu!");
    this->showMaximized();
    saveRectification("rafael", 3 , 5, 3, 9, 10);


    const std::string videoStreamAddress = "http://admin:password@192.168.0.101:80/videostream.cgi?.mjpg";

//    vcap.open(videoStreamAddress);
//    vcap2.open(0);

    src.create(480, 640, CV_8UC4);
    src = imread("robokLogo.png", -1);

//      if(!vcap.isOpened()){
//             cout << "Error opening video stream or file" << endl;
//             exit(1);
//      }

     save = true;
     view = false;
     cntVideos = 0;
     srand(time(NULL));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonPlay_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->timeEdit->setTime(QTime(ui->spinBoxHora->value(), ui->spinBoxMin->value()));
    ui->progressBar->setValue(0);

    QString pasta = "mkdir videos/" + ui->lineEditNome->text();
    system(pasta.toUtf8());


    if(connectPort("ttyUSB0"))
        QObject::connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read()));

    video();
}

void MainWindow::checkTime(){
    char c;
    timerF = time();

    if((timerF - timerI) >= 60){
        timerI = time();

        int horaIni = ui->spinBoxHora->value();
        int minIni = ui->spinBoxMin->value();
        int segTotal = horaIni * 60 + minIni;

        int min = ui->timeEdit->time().minute();
        int hora = ui->timeEdit->time().hour();

        min--;
        cout << min << endl;
        if(min < 0){
            if(hora > 0){
                hora--;
                min = 59;
            }
        }

        if(min == 0 && hora == 0) {
            ui->timeEdit->setTime(QTime(0, 0));
            saveRectification(ui->lineEditNome->text().toUtf8().data(), 3 , 5, horaIni, minIni, cntVideos);
            on_pushButtonStop_clicked();
        }

        int prog = 100 - ((hora*60 + min) * 100)/segTotal;
        ui->progressBar->setValue(prog);
        ui->timeEdit->setTime(QTime(hora, min));
    }
}

void MainWindow::video(){
    cnt = 0;
    cap = true;
    stopButton = false;
    char c;
    const std::string videoStreamAddress = "1" ;   //http://admin:password@192.168.0.101:80/videostream.cgi?.mjpg";
    vcap.open(1);

    frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    frame_height= vcap.get(CV_CAP_PROP_FRAME_HEIGHT);

    if(!vcap.isOpened()) cout << "erro" << endl;

    tmedio = 0;
    double total = 0;

    timerI = time();

    while(cap==true){

        double ti,tf; // ti = tempo inicial // tf = tempo final

        ti = time();
        vcap >> frame;
        overlayImage(frame, src, frame, cvPoint(0,0));
        frames.push_back(frame.clone());

        if(view)
            imshow("result", frames.back());

        c = (char) waitKey(3);

        if( c == 27 ) break;

        if(frames.size() > 150){
            frames.erase(frames.begin());
        }
        cnt++;

        tf = time();
        total += (tf-ti)*1000;

        if(cnt % 20 == 0){
            tmedio = total/20;
            total = 0;
            cout << "Tempo gasto em milissegundos " << tmedio << endl;
            std::cout << "size " << frames.size() << std::endl;
        }
        checkTime();
    }
    destroyAllWindows();
}

void MainWindow::on_pushButtonStop_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->labelVideos->setText("0");
    cap = false;
    stopButton = true;
    cntVideos = 0;
}

bool MainWindow::connectPort(QString portName){
    serialPort.setPortName(portName);
    serialPort.setBaudRate(9600);
    if(serialPort.open(QIODevice::ReadWrite)){
        std::cout << serialPort.portName().toStdString() << std::endl;

        return true;
    }
    else{
        std::cout << "can't connect" << std::endl;
        sleep(3);
        return false;
    }
}

void MainWindow::read(){
    int nbytes = serialPort.bytesAvailable();
    //std::cout << "nbytes " << nbytes << std::endl;
    if(nbytes > 3){
        char bytes[150];
        serialPort.readLine(bytes, nbytes+2);

        string = QString::fromUtf8(bytes, 5);
        std::cout << string.toStdString() << std::endl;
        if(!string.compare(QString().fromStdString("HIGH4"))){
            saveVideo();
        }
        serialPort.clear();
    }
}

void MainWindow::saveVideo(){
    save = false;
    int size = frames.size();
    int fps = 1000/tmedio;
    destroyAllWindows();
    videoName = "video_" + QString::number(cntVideos);
    videoName = videoName + ".avi";
    videoName = "/" + videoName;
    videoName = ui->lineEditNome->text() + videoName;
    videoName = "videos/" + videoName;
    std::cout << "**************" << videoName.toStdString() << " fps: " << fps << " *****************"<< std::endl;
    VideoWriter video(videoName.toStdString() ,CV_FOURCC('M','J','P','G'),fps, Size(frame_width,frame_height),true);
    for (int i = 0 ; i < size && i <= fps *10; ++i){
         video.write(frames.front());
//         frames2.push_back(frames.front());
         frames.erase(frames.begin());
    }
//    for (int i = 0 ; i < size; ++i){
//         cout << i << endl;
//         video.write(frames2.front());
//         video.write(frames2.front());
//         frames2.erase(frames2.begin());
//    }

    addSong(videoName);

    cout << "salvo" << endl;
    save = true;
    cntVideos++;
    ui->labelVideos->setText(QString().number(cntVideos));
}

void MainWindow::addSong(QString videoName){
    QString music = "avconv -i " + videoName;
    music = music + " -i songs/";
    music = music + QString().number(rand()%3+1);
    music = music + ".wma -c copy ";
    videoName = "video_" + QString::number(cntVideos + 100);
    videoName = videoName + ".mp4";
    videoName = "/" + videoName;
    videoName = ui->lineEditNome->text() + videoName;
    videoName = "videos/" + videoName;
    music = music + videoName;

    system(music.toUtf8());
}

double time()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec/1e6;
}

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
  cv::Mat &output, cv::Point2i location)
{
  background.copyTo(output);


  // start at the row indicated by location, or at row 0 if location.y is negative.
  for(int y = std::max(location.y , 0); y < background.rows; ++y)
  {
    int fY = y - location.y; // because of the translation

    // we are done of we have processed all rows of the foreground image.
    if(fY >= foreground.rows)
      break;

    // start at the column indicated by location,

    // or at column 0 if location.x is negative.
    for(int x = std::max(location.x, 0); x < background.cols; ++x)
    {
      int fX = x - location.x; // because of the translation.

      // we are done with this row if the column is outside of the foreground image.
      if(fX >= foreground.cols)
        break;

      // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
      double opacity =
        ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

        / 255.;


      // and now combine the background and foreground pixel, using the opacity,

      // but only if opacity > 0.
      for(int c = 0; opacity > 0 && c < output.channels(); ++c)
      {
        unsigned char foregroundPx =
          foreground.data[fY * foreground.step + fX * foreground.channels() + c];
        unsigned char backgroundPx =
          background.data[y * background.step + x * background.channels() + c];
        output.data[y*output.step + output.channels()*x + c] =
          backgroundPx * (1.-opacity) + foregroundPx * opacity;
      }
    }
  }
}



void MainWindow::on_pushButton_clicked()
{
    saveVideo();
}

void MainWindow::on_pushButton_2_clicked()
{
    if(view){
        destroyWindow("result");
        ui->pushButton_2->setText("view");
        view = false;
    }else{
        ui->pushButton_2->setText("hide");
        view = true;
    }
}
