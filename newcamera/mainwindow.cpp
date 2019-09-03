#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPixmap>
#include<opencv2/videoio.hpp>
#include<QFileDialog>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"

//Global Variables
int flag4=0;
int brightness = 0;
double contrast = 1;
int flag=0;
int flag2=0;
int ctr;
double speed_index = 1;
int fastforward_count=1;
int reverse_vid= 0;
using namespace cv;
using namespace std;
QString filename,filename2;
VideoWriter Fvideo("normal.avi",CV_FOURCC('M','J','P','G'),25,Size(1280,720));
//end of global variable declaraations

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&pixmap);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void getExt(string out_name) {
    VideoCapture cap(out_name);
    //int ctr=0;
    string extension;
    int i;
    for(i=0; i < out_name.size(); i++) {
        if(out_name[i] == '.') break;
    }
    extension = out_name.substr(i,out_name.size()-1);
    cout << "File Type : " << extension << endl;

    ifstream file(out_name , ios::binary | ios::ate);
    double size_file = file.tellg();
    cout << "File Size : " << (size_file/1000000) << " MB" << endl;


    //cout << "Current path is " << current_path() << '\n';

    cap.release();
}

string getFileCreationTime(string path1) {
    char path[path1.size()+1];
    copy(path1.begin(),path1.end(),path);
    path[path1.size()] = '\0';
    struct stat attr;
    stat(path, &attr);
    return (ctime(&attr.st_mtime));

}

void MainWindow::on_pushButton_2_clicked()
{

    // For FPS
    QString fn = ui->label_6->text();
    string in_name = fn.toUtf8().constData();
    VideoCapture cap(in_name);
    double fpersec = cap.get(CV_CAP_PROP_FPS);
    ui->fpslabel->setText(QString::number(fpersec));

    // For Duration
    ctr=0;
    VideoCapture t(in_name);
    while(1) {
        Mat frame;
        t >> frame;
        ctr++;
        if(frame.empty()) break;
    }
    double dur = ctr/t.get(CV_CAP_PROP_FPS);
    ui->durlabel->setText(QString::number(dur));

    // File Creation Time
    string tm;
    tm = getFileCreationTime(in_name);
    ui->timecreatedlabel->setText(QString::fromStdString(tm));

    // Getting Extension
    string extension;
    int i;
    for(i=0; i < in_name.size(); i++) {
        if(in_name[i] == '.') break;
    }
    extension = in_name.substr(i,in_name.size()-1);
    ui->typelabel->setText(QString::fromStdString(extension));

    // Size of Video
    ifstream file(in_name , ios::binary | ios::ate);
    double size_file = file.tellg();
    double temp = (size_file/1000000);
    ui->sizelabel->setText(QString::number(temp));

    if(video.isOpened())
    {
        ui->pushButton_3->setText("Start");
        video.release();
        return;
    }
    bool isCamera;
    int cameraIndex = ui->label_6->text().toInt(&isCamera);
    if(isCamera)
    {
        if(!video.open(cameraIndex))
        {
            QMessageBox::critical(this,
                                  "Camera Error",
                                  "Make sure you entered a correct camera index,"
                                  "<br>or that the camera is not being accessed by another program!");
            return;
        }
    }
    else
    {
        if(!video.open(ui->label_6->text().trimmed().toStdString()))
        {
            QMessageBox::critical(this,
                                  "Video Error",
                                  "Make sure you entered a correct and supported video file path,"
                                  );
            return;
        }
    }

    ui->pushButton_3->setText("Pause");
    Mat frame;
    //int minutes = dur/60;
    //int seconds = (int)dur%60;

    while(video.isOpened())
    {
        if(!flag)
        {
            video >> frame;
            ui->pushButton_3->setText("Pause");
        }
        else
        {
            ui->pushButton_3->setText("Resume");
        }
        if(!frame.empty())
        {

            int frameno = video.get(CV_CAP_PROP_POS_FRAMES);
            //qInfo()<<frameno;
            if(frameno%fastforward_count==0)
            {
                //qInfo()<<frameno;
                frame.convertTo(frame,-1,contrast,brightness);
                //copyMakeBorder(frame,frame,frame.rows/2,frame.rows/2,frame.cols/2,frame.cols/2,BORDER_REFLECT);
                if(reverse_vid)
                {
                    Fvideo.write(frame);
                }

                //converting from Mat object to PixMap
                QImage qimg(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
                pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()) );
                ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);

                //moving the slider
                ui->horizontalSlider->setValue((frameno*1.0)/ctr*100.0);

                //waiting for a specified speed_index
                waitKey(speed_index);
            }
         }
        qApp->processEvents();
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    flag = (flag+1)%2;
}

void MainWindow::on_pushButton_clicked()
{
    //opening the Webcam
    video.open(0);\

    Mat frame;
    while(video.isOpened())
    {
        if(!flag2)
        {

            video>>frame;
            ui->pushButton_5->setText("Pause");
        }
        else
        {
            ui->pushButton_5->setText("Resume");

        }
        if(!frame.empty())
        {
            frame.convertTo(frame,-1,contrast,brightness);
            QImage qimg(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
            pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()));
            ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
        }
        qApp->processEvents();
    }

}

void MainWindow::on_pushButton_4_clicked()
{
    if(video.isOpened())
        video.release();
}

void MainWindow::on_pushButton_5_clicked()
{
    flag2 = (flag2+1)%2;
}

void MainWindow::on_pushButton_6_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Choose"),"", tr("Videos (*.mp4 *.avi *.mp4"));
    ui->label_6->setText(filename);
}

void MainWindow::on_pushButton_7_clicked()
{
    video.release();
    close();
}

void MainWindow::on_pushButton_8_clicked()
{
    brightness = brightness + 10;
}

void MainWindow::on_pushButton_9_clicked()
{
    brightness = brightness - 10;
}

void MainWindow::on_pushButton_10_clicked()
{
    contrast = contrast + 0.25;
    //qInfo()<<contrast;
}

void MainWindow::on_pushButton_11_clicked()
{
    contrast = contrast - 0.25;
    //qInfo()<<contrast;

}

void MainWindow::on_pushButton_12_clicked()
{
    brightness = 0;

}

void MainWindow::on_pushButton_13_clicked()
{
    contrast = 1;

}

void MainWindow::on_pushButton_14_clicked()
{

    fastforward_count = fastforward_count + 3;
    //qInfo()<<fastforward_count;

    //setting the speed indicator label
    QString curr = ui->label_13->text();
    int val = curr[curr.size()-1].digitValue();
    QString new_c = "";
    new_c.append(curr[0]);
    if(curr[curr.size()-2]=='-')
    {
        val = val - 1;
        if(!val == 0)
        {
            new_c.append('-');
        }
    }
    else
    {
        val = val + 1;
    }
    new_c.append(QString::number(val));
    //setting the speed label

    ui->label_13->setText(new_c);
}

void MainWindow::on_pushButton_15_clicked()
{
    //set the speed_index
    speed_index = speed_index+ 5;

    //setting the speed label
    QString curr = ui->label_13->text();
    QString new_c = "";
    int val = curr[curr.size()-1].digitValue();
    new_c.append(curr[0]);
    if(curr[curr.size()-2] == '-')
    {
        val = val + 1;
        new_c.append('-');
    }
    else
    {
        val = val -1;
    }
    new_c.append(QString::number(val));
    ui->label_13->setText(new_c);
    //end
    }

void MainWindow::on_pushButton_16_clicked()
{
    QMessageBox MsgBox;
    //taking the reverse saved directory
    QString saved_dir = ui->label_14->text();

    if(saved_dir.size()==0)
    {
        MsgBox.setText("Please select a proper directory for saving the video");
        MsgBox.exec();

    }
    else
    {
        if(!reverse_vid)
        {
            MsgBox.setText("The Reverse Video Recording has started.");
            reverse_vid = 1;
            /*if(!flag4)
             {
                flag4=1;
                VideoWriter Fvideo(filename.toStdString(),CV_FOURCC('M','J','P','G'),25,Size(1280,720));
              }
            */
            MsgBox.exec();
            ui->pushButton_16->setText("Stop the Reverse Store");
         }
        else
        {
            reverse_vid = 0;
            ui->pushButton_16->setText("start the Reverse Store");
            Fvideo.release();

            //reversing the video
            VideoCapture cap1("normal.avi");
            int frame_width = cap1.get(CV_CAP_PROP_FRAME_WIDTH);
            int frame_height = cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
            cap1.set(CV_CAP_PROP_POS_AVI_RATIO,1);
            VideoWriter video2(filename.toStdString(),CV_FOURCC('M','J','P','G'),cap1.get(CV_CAP_PROP_FPS),Size(frame_width,frame_height));
            Mat frame2;
            int no_f = cap1.get(CV_CAP_PROP_FRAME_COUNT);
            while(no_f>0)
            {
                no_f --;
                cap1.set(CV_CAP_PROP_POS_FRAMES, no_f);
                cap1>>frame2;
                if(!frame2.empty())
                {
                    video2.write(frame2);
                }
            }
            video2.release();

            MsgBox.setText("The Reverse Video is available after you exit in the specified target directory.");
            MsgBox.exec();
        }
    }
}

void MainWindow::on_pushButton_17_clicked()
{
    speed_index = 1;
    fastforward_count = 1;
}

void MainWindow::on_pushButton_18_clicked()
{
    QString filename1 = QFileDialog::getExistingDirectory(this,tr("select directory"),"/home/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    filename = filename1 + "/reverse.avi";
    ui->label_14->setText(filename);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    int dragged_frame_count = (position*1.0)/100.0 * ctr;
    video.set(CV_CAP_PROP_POS_FRAMES,dragged_frame_count);
}
