#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iosfwd>
#include <chrono>

using namespace std;
using namespace cv;

const int camID = 1;

int main(int arg, char** argc) {

  Mat src;

  VideoCapture cap(camID);
  if(camID!=0){
    // HERE YOU CAN EDIT RESOLUTION
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  }
  if (!cap.isOpened())
    return false;

  VideoWriter outputVideo;
  Size S = Size(1280,720);
  //Acquire input size
  outputVideo.open("capture.avi" , 0, 30, S, true);

  if(!outputVideo.isOpened()){
    return false;
  }

  cap >> src;

  auto totalStart = std::chrono::system_clock::now();
  std::chrono::time_point<std::chrono::system_clock> start, end;

  cap.set(CV_CAP_PROP_POS_MSEC,0);
  cap.set(CV_CAP_PROP_POS_FRAMES,0);

  while (1) {


    start = std::chrono::system_clock::now();
    cap >> src;
    outputVideo << src;
    end = std::chrono::system_clock::now();

    imshow("customResolutionCapture", src);

    cout << "Video capture start: \t" <<  std::chrono::duration_cast<std::chrono::milliseconds>(start-totalStart).count() << endl;
    cout << "Video capture end:   \t" << std::chrono::duration_cast<std::chrono::milliseconds>(end-totalStart).count()   << endl;
    cout << "Video capture time taken: \t" << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << endl;  
    cout << endl;

    if (waitKey(1) >= 0){
      break;
    }
  }

  return 0;
}
