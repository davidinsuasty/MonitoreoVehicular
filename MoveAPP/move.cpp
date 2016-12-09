#include <move.h>
#include <tracker/tracker.h>
#include "ui_move.h"
#include <QtCore>
#include <QFileDialog>

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0,   0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);


//////////////////////////////////////////////////////////////////////////////////////////////////
move::move(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::move)
{
    ui->setupUi(this);

    currentFrameResized.create(480, 640,CV_RGB2BGR);
    firstFrame = true;

    carCount = 0;
    carCount2 = 0;
    pressPlay = 0;
    pressPause = 0;
    initialize();

    tmrTime = new QTimer(this);
    connect(tmrTime,SIGNAL(timeout()),this,SLOT(runApp()));
    tmrTime->start(20);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::runApp()
{
    showTime();

    if (pressPlay==1)
    {
        cap.read(currentFrame);
        if (currentFrame.empty() == true)
        {        
            on_btnStop_clicked();
            return;
        }
        currentFrame.copyTo(currentFrameCopy);
        frame2screen();

        ui->txtMensajes->setText(QString("Procesando..."));

        pressPause = 1;
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::initialize()
{
    QPixmap pix(":/play.ico");
    QIcon play_icon(pix);
    ui->btnPlaynPause->setIcon(play_icon);
    ui->btnPlaynPause->setIconSize(QSize(70,50));

    pix.load(QString(":/stop.png"));
    QIcon stop_icon(pix);
    ui->btnStop->setIcon(stop_icon);
    ui->btnStop->setIconSize(QSize(70,50));

    pix.load(QString(":/report.png"));
    QIcon report_icon(pix);
    ui->btnReports->setIcon(report_icon);
    ui->btnReports->setIconSize(QSize(90,50));

    pix.load(QString(":/search.png"));
    QIcon search_icon(pix);
    ui->btnFilePath->setIcon(search_icon);
    ui->btnFilePath->setIconSize(QSize(20,20));

    ui->rbtn_param_SN_2->setChecked(true);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::showTime()
{
    QDate current_date = QDate::currentDate();
    QTime current_time = QTime::currentTime();
    QString text = current_date.toString("dd/MM/yyyy");
    ui->time_var->setText(QString("Fecha: ") + text);
    text = current_time.toString();
    ui->time_var_2->setText(QString(" Hora: ") + text);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::on_btnPlaynPause_clicked()
{
    pressPlay = 1;

    QString select = ui->selectSource->currentText();
    if(select == "Streaming")
    {
        int value = ui->selectPort->value();
        if (value == 0)
            cap.open(value);
        else if (value == 1)
            cap.open("http://200.6.188.43:80/mjpg/video.mjpg");
        else if (value == 2)
            cap.open(value);

        ui->selectPort->setEnabled(false);
        if (cap.isOpened()== false)
        {
            ui->txtMensajes->setText(QString("Error: No se conectó con ninguna cámara"));
            pressPlay = 0;
            ui->selectPort->setEnabled(true);
            return;
        }
        else
        {
            cap.read(pastFrame);
            pastFrame.copyTo(pastFrameCopy);
        }

    }else
    {
        if (ui->filePath->text().isEmpty())
        {
            ui->txtMensajes->setText(QString("No se seleccionó ningún archivo de video"));
            pressPlay =  0;
            return;
        } else
        {
            if (pressPause == 0)
            {
                cap.open(ui->filePath->text().toStdString());
                if (cap.isOpened()== false)
                {
                    ui->txtMensajes->setText(QString("Error: No se pudo cargar el video correctamente"));
                    pressPlay = 0;
                    ui->filePath->setEnabled(true);
                    ui->btnFilePath->setEnabled(true);
                    return;

                }
                else
                {
                    cap.read(pastFrame);
                    ui->filePath->setEnabled(false);
                    ui->btnFilePath->setEnabled(false);
                }
            }

        }

    }

    QPixmap pix(":/pause.png");
    QIcon play_icon(pix);
    ui->btnPlaynPause->setIcon(play_icon);
    ui->btnPlaynPause->setIconSize(QSize(70,50));

    if ((tmrTime->isActive()==true) & (pressPause==1))
    {
        tmrTime->stop();
        QPixmap pix(":/play.ico");
        QIcon play_icon(pix);
        ui->btnPlaynPause->setIcon(play_icon);
        ui->btnPlaynPause->setIconSize(QSize(70,50));
        ui->txtMensajes->setText(QString("Proceso pausado"));
        pressPlay = 0;
    }

    if ((tmrTime->isActive()==false) & (pressPlay==1))
        tmrTime->start(20);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::on_btnStop_clicked()
{
    cap.release();

    initialize();
    carCount = 0;
    pressPause = 0;
    pressPlay = 0;
    ui->txtMensajes->setText(QString(" "));
    ui->textCarCount->setText(QString("VEHICULOS:  0"));
    ui->rbtn_param_SN_2->setChecked(true);
    ui->VideoPlayer->clear();
    ui->VideoPlayer->setStyleSheet("background-color:gray");
    if (ui->selectSource->currentIndex() == 0)
    {
        ui->selectPort->setEnabled(true);
        ui->filePath->setEnabled(false);
        ui->btnFilePath->setEnabled(false);
    }
    else
    {
        ui->selectPort->setEnabled(false);
        ui->filePath->setEnabled(true);
        ui->btnFilePath->setEnabled(true);
    }


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::on_selectSource_currentIndexChanged(const QString &arg1)
{


    if (arg1 == "Streaming")
    {
        ui->filePath->setEnabled(false);
        ui->btnFilePath->setEnabled(false);
        ui->selectPort->setEnabled(true);
    }else
    {
        ui->filePath->setEnabled(true);
        ui->btnFilePath->setEnabled(true);
        ui->selectPort->setEnabled(false);

    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::on_btnFilePath_clicked()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Abrir archivo"),"/path/to/file/",tr("Archivo MP4(*.mp4);;Archivo AVI(*.avi)"));
    ui->filePath->setText(FileName);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::addRegion2AllRegions(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions, int idx)
{
    existingRegions[idx].currentRegion = currentFrameRegion.currentRegion;
    existingRegions[idx].currentBB = currentFrameRegion.currentBB;

    existingRegions[idx].centerPositions.push_back(currentFrameRegion.centerPositions.back());

    existingRegions[idx].diagonalSize = currentFrameRegion.diagonalSize;
    existingRegions[idx].AspectRatio = currentFrameRegion.AspectRatio;
    existingRegions[idx].consecutiveFramesWithoutMatch = currentFrameRegion.consecutiveFramesWithoutMatch;

    existingRegions[idx].tracked = true;
    existingRegions[idx].matchFoundOrNewRegion = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::addNewRegion(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions)
{
    currentFrameRegion.matchFoundOrNewRegion = true;
    existingRegions.push_back(currentFrameRegion);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::drawContours(cv::Size imageSize, std::vector<Tracker> regions)
{
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    std::vector<std::vector<cv::Point> > contours;

    for (unsigned int i = 0; i < regions.size(); i++)
    {
        if (regions[i].tracked == true)
        {
            contours.push_back(regions[i].currentRegion);
            cv::rectangle(image, regions[i].currentBB,SCALAR_GREEN, 2);
        }
    }

    cv::drawContours(image, contours, -1, SCALAR_RED, -1);
    cv::imshow("selected regions",image);
    cv::waitKey(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
double move::euclideanDistance(cv::Point point1, cv::Point point2)
{
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(std::pow(double(intX), 2) + std::pow(double(intY), 2)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
double move::round(double number)
{
    return number < 0.0 ? std::ceil(number - 0.5) : std::floor(number + 0.5);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool move::crossCountingLine(std::vector<Tracker> &regions, int posCountingline, int &carCount, int &carCount2)
{
    bool crossTheLine = false;

    ui->rbtn_param_SN_2->isChecked();

    for (unsigned int i = 0; i < regions.size(); i++)
    {
        if (regions[i].tracked && regions[i].centerPositions.size() >= 2)
        {
            int lastIdx = (int)regions[i].centerPositions.size() - 2;
            int currIdx = (int)regions[i].centerPositions.size() - 1;

            int diff = regions[i].centerPositions[currIdx].y - regions[i].centerPositions[lastIdx].y;

            regions[i].opticalFlow += diff < 0 ? -1:1;

            if (regions[i].centerPositions.size() > 5)
            {
                if (regions[i].centerPositions[lastIdx].y > posCountingline &&
                    regions[i].centerPositions[currIdx].y <= posCountingline &&
                        regions[i].opticalFlow < 0 &&
                        (ui->rbtn_param_SN->isChecked() ||ui->rbtn_param_SN_2->isChecked()))
                {
                    carCount++;
                    crossTheLine = true;
                }

                if (regions[i].centerPositions[lastIdx].y < posCountingline &&
                    regions[i].centerPositions[currIdx].y >= posCountingline &&
                        regions[i].opticalFlow > 0 &&
                        (ui->rbtn_param_NS->isChecked() ||ui->rbtn_param_SN_2->isChecked()))
                {
                    carCount2++;
                    crossTheLine = true;
                }
            }
        }
    }

    return crossTheLine;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::showCarCount(int &carCount, int &carCount2)
{
     ui->textCarCount->setText(QString("VEHICULOS || S-N: ")+QString::number(carCount)
                               +QString("  N-S: ")+QString::number(carCount2)
                               +QString("  Total: ")+QString::number(carCount + carCount2));

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::trackingRegion(std::vector<Tracker> &existingRegions, std::vector<Tracker> &currentFrameRegion)
{

    for (unsigned int i = 0; i < existingRegions.size(); i++)
    {
        existingRegions[i].matchFoundOrNewRegion = false;

        existingRegions[i].predictNextPosition();
    }

    for (unsigned int i = 0; i < currentFrameRegion.size(); i++)
    {
        int ndistances = 0;
        double leastDistance = 100000.0;

        for (unsigned int j =  0; j < existingRegions.size(); j++)
        {
            if (existingRegions[j].tracked)
            {
                double distance = euclideanDistance(currentFrameRegion[i].centerPositions.back(), existingRegions[j].predictedNextPosition);

                if (distance < leastDistance)
                {
                    leastDistance = distance;
                    ndistances = j;
                }
            }
        }

        if (leastDistance < currentFrameRegion[i].diagonalSize * 0.4)
        {
            addRegion2AllRegions(currentFrameRegion[i], existingRegions, ndistances);
        }
        else
        {
            addNewRegion(currentFrameRegion[i], existingRegions);
        }
    }

    for (unsigned int k = 0;  k < existingRegions.size(); k++)
    {
        if (!existingRegions[k].matchFoundOrNewRegion)
            existingRegions[k].consecutiveFramesWithoutMatch++;

        if (existingRegions[k].consecutiveFramesWithoutMatch >= 5)
        {
            existingRegions[k].tracked = false;
            existingRegions.erase(existingRegions.begin()+k);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::setCountingLine()
{

    if (ui->rbtn_param_SN->isChecked())
    {
        linePosition = (int)round((double)currentFrame.rows*0.74);
        countingLine[0].x = 0;
        countingLine[0].y = linePosition;

        countingLine[1].x = currentFrame.cols -1;
        countingLine[1].y = linePosition;

    }
    else if (ui->rbtn_param_NS->isChecked() || ui->rbtn_param_SN_2->isChecked())
    {
        linePosition = (int)round((double)currentFrame.rows*0.74);
        countingLine[0].x = 0;
        countingLine[0].y = linePosition;

        countingLine[1].x = currentFrame.cols -1;
        countingLine[1].y = linePosition;
    }
    else if (ui->rbtn_param_EO->isChecked())
    {
        linePosition = (int)round((double)currentFrame.cols*0.7);
        countingLine[0].x = linePosition;
        countingLine[0].y = 0;

        countingLine[1].x = linePosition;
        countingLine[1].y = currentFrame.rows -1;
    }
    else
    {
        linePosition = (int)round((double)currentFrame.cols*0.3);
        countingLine[0].x = linePosition;
        countingLine[0].y = 0;

        countingLine[1].x = linePosition;
        countingLine[1].y = currentFrame.rows -1;
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move::frame2screen()
{
    std::vector<Tracker> selectedCurrentRegions;

    cv::cvtColor(currentFrame, currentFrame, CV_BGR2GRAY);
    cv::cvtColor(pastFrame, pastFrame, CV_BGR2GRAY);

    cv::GaussianBlur(currentFrame, currentFrame, cv::Size(7,7),0);
    cv::GaussianBlur(pastFrame, pastFrame, cv::Size(7,7),0);

    cv::absdiff(currentFrame, pastFrame, diffFrames);
    cv::threshold(diffFrames, imThresh, 20, 255.0, CV_THRESH_BINARY);

    cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    for (unsigned int i = 0; i < 3; i++)
    {
        cv::dilate(imThresh, imThresh, structuringElement3x3);
        cv::dilate(imThresh, imThresh, structuringElement5x5);
        cv::erode(imThresh, imThresh, structuringElement5x5);
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imThresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point> > regions(contours.size());
    for (unsigned int i = 0; i < contours.size(); i++)
    {
        cv::convexHull(cv::Mat(contours[i]), regions[i], false);
    }


    for (unsigned int i = 0; i < regions.size(); i++)
    {
        Tracker possibleRegion(regions[i]);

        if(possibleRegion.currentBB.area() > 400 &&
           possibleRegion.currentBB.width > 30 &&
           possibleRegion.currentBB.height > 30 &&
           possibleRegion.AspectRatio > 0.2 &&
           possibleRegion.AspectRatio < 4.0 &&
           possibleRegion.diagonalSize > 60.0 &&
           (cv::contourArea(possibleRegion.currentRegion) / (double)possibleRegion.currentBB.area()) > 0.5)
        {
            selectedCurrentRegions.push_back(possibleRegion);
        }
    }



    if (firstFrame)
    {
        for (unsigned int i = 0; i < selectedCurrentRegions.size(); i++)
        {
            selectedregions.push_back(selectedCurrentRegions[i]);
        }
    }
    else
    {
        trackingRegion(selectedregions, selectedCurrentRegions);
    }
    
    currentFrame = currentFrameCopy.clone();
    setCountingLine();

    //drawContours(currentFrame.size(),selectedregions);
    bool crossTheLine = crossCountingLine(selectedregions, linePosition, carCount, carCount2);

    if (crossTheLine)
        cv::line(currentFrame, countingLine[0], countingLine[1], SCALAR_GREEN);
    else
        cv::line(currentFrame, countingLine[0], countingLine[1], SCALAR_RED);

    showCarCount(carCount, carCount2);

    cv::cvtColor(currentFrame, currentFrame, CV_BGR2RGB);
    cv::resize(currentFrame, currentFrameResized,cv::Size(640, 480));
    QImage qOutput((uchar*)currentFrameResized.data, currentFrameResized.cols, currentFrameResized.rows, currentFrameResized.step, QImage::Format_RGB888);
    ui->VideoPlayer->setPixmap(QPixmap::fromImage(qOutput));
    currentFrameCopy.copyTo(pastFrame);
    currentFrameCopy.copyTo(pastFrameCopy);
    selectedCurrentRegions.clear();

    firstFrame = false;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
move::~move()
{
    delete ui;
}
