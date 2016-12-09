#ifndef TRACKER_H
#define TRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Tracker
{
public:


	std::vector<cv::Point> currentRegion;
    std::vector<cv::Point> centerPositions;

    cv::Rect currentBB;
    cv::Point predictedNextPosition;

    int consecutiveFramesWithoutMatch;
    double diagonalSize;
    double AspectRatio;
	int opticalFlow;

    bool matchFoundOrNewRegion;
    bool tracked;

	Tracker();
    Tracker(std::vector<cv::Point> _region);
    void predictNextPosition();

private:

};

#endif // TRACKER_H
