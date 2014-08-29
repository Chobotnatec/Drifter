#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iosfwd>

using namespace std;
using namespace cv;

const int camID = 1;

int main() {

  Mat src;

  VideoCapture cap(camID);
  if(camID!=0){
    // HERE YOU CAN EDIT RESOLUTION
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  }
  if (!cap.isOpened())
    return false;

  cap >> src;

  while (1) {
    cap >> src;
    
    //flip(src, src, 1);

    imshow("customResolutionCapture", src);

    if (waitKey(30) >= 0)
      break;
  }

  return 0;
}
