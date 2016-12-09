#include "tracker.h"
Tracker::Tracker()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
Tracker::Tracker(std::vector<cv::Point> _region)
{
    currentRegion = _region;
    currentBB = cv::boundingRect(currentRegion);

    cv::Point center;

    center.x = (2*currentBB.x + currentBB.width) / 2;
    center.y = (2*currentBB.y + currentBB.height) / 2;

    centerPositions.push_back(center);

    diagonalSize = sqrt(std::pow((double)currentBB.width,2) + std::pow((double)currentBB.height,2));
    AspectRatio = (double)currentBB.width / (double)currentBB.height;

    tracked = true;
    matchFoundOrNewRegion = true;

    consecutiveFramesWithoutMatch = 0;
	opticalFlow = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracker::predictNextPosition()
{

    int nPos = (int)centerPositions.size();

    if (nPos == 1)
    {
        predictedNextPosition.x = centerPositions.back().x;
        predictedNextPosition.y = centerPositions.back().y;
    }
    else if (nPos == 2)
    {
        int dX = centerPositions[1].x - centerPositions[0].x;
        int dY = centerPositions[1].y - centerPositions[0].y;

        predictedNextPosition.x = centerPositions.back().x + dX;
        predictedNextPosition.y = centerPositions.back().y + dY;
    }
    else
    {
        int sumXchanges = 0;
        int sumYchanges = 0;

        for (int i = nPos -1; i > 0; i--)
        {
            sumXchanges += i*(centerPositions[i].x - centerPositions[i-1].x);
            sumYchanges += i*(centerPositions[i].y - centerPositions[i-1].y);
        }

        int dX = (int)std::floor(((float)sumXchanges/std::pow(1.5, nPos)) + 0.5);
        int dY = (int)std::floor(((float)sumYchanges/std::pow(1.5, nPos)) + 0.5);
        predictedNextPosition.x = centerPositions.back().x + dX;
        predictedNextPosition.y = centerPositions.back().y + dY;
    }
}
