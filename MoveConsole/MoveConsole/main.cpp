#include "tracker.h"
#include <stdio.h>
#include <vector>
#include <iostream>
// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0,   0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

void addRegion2AllRegions(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions, int idx);
void addNewRegion(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions);
void drawContours(cv::Size imageSize, std::vector<Tracker> regions);   
void showCarCount(int &carCount);
void trackingRegion(std::vector<Tracker> &existingRegions, std::vector<Tracker> &currentFrameRegion);
void frame2screen();
void setCountingLine();

double euclideanDistance(cv::Point point1, cv::Point point2);
double round(double number);

bool crossCountingLine(std::vector<Tracker> &regions, int posCountingline, int &carCount);

int linePosition;
cv::Point countingLine[2];
int carCount;
    
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


int main()
{
	
    

	cap.open("D:/GoogleDrive/dataset/manizales_batallon_8am.mp4");
	if (!cap.isOpened())
	{
		std::cout<<"No se cargo el video"<< std::endl;
		return 0;
	}



	cap.read(pastFrame);
    pastFrame.copyTo(pastFrameCopy);
	firstFrame = true;
	char EscKey = 0;
	while (cap.isOpened() && EscKey != 27)
	{
		cap.read(currentFrame);
		currentFrame.copyTo(currentFrameCopy);

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
    bool crossTheLine = crossCountingLine(selectedregions, linePosition, carCount);

    if (crossTheLine)
        cv::line(currentFrame, countingLine[0], countingLine[1], SCALAR_GREEN);
    else
        cv::line(currentFrame, countingLine[0], countingLine[1], SCALAR_RED);

    showCarCount(carCount);

    //cv::cvtColor(currentFrame, currentFrame, CV_BGR2RGB);
	cv::imshow("salida",currentFrame);
	EscKey = cv::waitKey(10);
    currentFrameCopy.copyTo(pastFrame);
    currentFrameCopy.copyTo(pastFrameCopy);
    selectedCurrentRegions.clear();

    firstFrame = false;

	}


}

void setCountingLine()
{
	linePosition = (int)round((double)currentFrame.rows*0.74);
    countingLine[0].x = 0;
    countingLine[0].y = linePosition;

    countingLine[1].x = currentFrame.cols -1;
    countingLine[1].y = linePosition;
}

void trackingRegion(std::vector<Tracker> &existingRegions, std::vector<Tracker> &currentFrameRegion)
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

void showCarCount(int &carCount)
{
	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (currentFrame.rows * currentFrame.cols) / 300000.0;
    int intFontThickness = (int)round(dblFontScale * 1.5);

    cv::Size textSize = cv::getTextSize(std::to_string(static_cast<long long>(carCount)), intFontFace, dblFontScale, intFontThickness, 0);

    cv::Point ptTextBottomLeftPosition;

	ptTextBottomLeftPosition.x = currentFrame.cols - 1 - (int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

	cv::putText(currentFrame, std::to_string(static_cast<long long>(carCount)), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
}

bool crossCountingLine(std::vector<Tracker> &regions, int posCountingline, int &carCount)
{
	bool crossTheLine = false;

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
                        regions[i].opticalFlow < 0)
                {
                    carCount++;
                    crossTheLine = true;
                }
				if (regions[i].centerPositions[lastIdx].y < posCountingline &&
                    regions[i].centerPositions[currIdx].y >= posCountingline &&
                        regions[i].opticalFlow > 0)
				{
					carCount++;
					crossTheLine = true;
				}
            }
        }
    }

    return crossTheLine;
}

double round(double number)
{
    return number < 0.0 ? std::ceil(number - 0.5) : std::floor(number + 0.5);
}

double euclideanDistance(cv::Point point1, cv::Point point2)
{
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(std::pow(double(intX), 2) + std::pow(double(intY), 2)));
}

void drawContours(cv::Size imageSize, std::vector<Tracker> regions)
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
    cv::imshow("hull regions",image);
    cv::waitKey(0);
}

void addNewRegion(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions)
{
    currentFrameRegion.matchFoundOrNewRegion = true;
    existingRegions.push_back(currentFrameRegion);
}


void addRegion2AllRegions(Tracker &currentFrameRegion, std::vector<Tracker> &existingRegions, int idx)
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