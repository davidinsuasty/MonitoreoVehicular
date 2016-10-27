#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>

using namespace std;
using namespace cv;

int main( int argc, char** argv ){
  // show help
  if(argc<2){
    cout<<
      " Usage: MTmain.exe <video_name>\n"
      << endl;
    return 0;
  }

  // set the default tracking algorithm
  //MIL - BOOSTING - MEDIANFLOW - TLD - KCF
  std::string trackingAlg = "MEDIANFLOW";


  // create a multitracker object 
  MultiTracker trackers(trackingAlg);

  // container of the tracked objects
  vector<Rect2d> objects;

  // set input video
  std::string video = argv[1];
  VideoCapture cap(video);
  if(!cap.isOpened())  // check if we succeeded
  {
	  cout<< "ERROR: video file "<< video << " is not found! "<< endl;
	  return -1;
  }

  Mat frame;

  // get bounding box
  cap >> frame;
  //! [selectmulti]
  selectROI("tracker",frame,objects);
  //! [selectmulti]

  //quit when the tracked object(s) is not provided
  if(objects.size()<1)
    return 0;

  // initialize the tracker
  //! [init]
  trackers.add(frame,objects);
  //! [init]

  // do the tracking
  printf("Start the tracking process, press ESC to quit.\n");
  for ( ;; ){
    // get frame from the video
    cap >> frame;

    // stop the program if no more images
    if(frame.rows==0 || frame.cols==0)
      break;

    //update the tracking result
    //! [update]
    trackers.update(frame);
    //! [update]

    //! [result]
    // draw the tracked object
    for(unsigned i=0;i<trackers.objects.size();i++)
      rectangle( frame, trackers.objects[i], Scalar( 255, 0, 0 ), 2, 1 );
    //! [result]

    // show image with the tracked object
    imshow("tracker",frame);
    //quit on ESC button
    if(waitKey(1)==27)break;
  }

}