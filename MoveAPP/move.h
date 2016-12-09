#ifndef MOVE_H
#define MOVE_H

#include <tracker/tracker.h>
#include <QMainWindow>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace Ui {
class move;
}

class move : public QMainWindow
{
    Q_OBJECT

public:
    explicit move(QWidget *parent = 0);
    ~move();

    void addRegion2AllRegions(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions, int idx);
    void addNewRegion(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions);
    void drawContours(cv::Size imageSize, std::vector<Tracker> regions);   
    void showCarCount(int &carCount, int &carCount2);
    void trackingRegion(std::vector<Tracker> &existingRegions, std::vector<Tracker> &currentFrameRegion);
    void frame2screen();
    void setCountingLine();

    double euclideanDistance(cv::Point point1, cv::Point point2);
    double round(double number);

    bool crossCountingLine(std::vector<Tracker> &regions, int posCountingline, int &carCount, int &carCount2);

private slots:
    void showTime();
    void initialize();
    void runApp();
    void on_btnPlaynPause_clicked();
    void on_btnStop_clicked();
    void on_selectSource_currentIndexChanged(const QString &arg1);
    void on_btnFilePath_clicked();

private:
    Ui::move *ui;
    QTimer* tmrTime;

    int pressPlay;
    int pressPause;
    int carCount;
    int carCount2;
    int linePosition;

    bool firstFrame;

    cv::VideoCapture cap;

    cv::Mat currentFrame;
    cv::Mat pastFrame;
    cv::Mat currentFrameResized;
    cv::Mat currentFrameCopy;
    cv::Mat pastFrameCopy;
    cv::Mat diffFrames;
    cv::Mat imThresh;

    std::vector<Tracker> selectedregions;

    cv::Point countingLine[2];

    QImage qOutput;
};

#endif // MOVE_H
