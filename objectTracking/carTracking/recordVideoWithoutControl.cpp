#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iosfwd>
#include <chrono>

using namespace std;
using namespace cv;
using namespace std::chrono;

const int camID = 1;

// Input: 'filename' without extension
// example:
//   $ ./recordVideoWithoutControl  /home/lecopivo/video
// This creates files `video.avi` and `video.txt`

int main(int arg, char** argc) {

  if(arg!=2){
    cout << "Please enter name of output file without extension!" << endl;
    return 0;
  }

  Mat src;

  // fire up camera
  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  if (!cap.isOpened()){
    cout << "Could not open camera!"<< endl;
    return 0;
  }

  // open output file
  VideoWriter outputVideo;
  Size S = Size(1280,720);
  string name = argc[1];
  string videoFileName = name + ".avi";
  string timeFileName = name + ".txt";
   
  outputVideo.open( videoFileName, 0, 30, S, true);
  
  if(!outputVideo.isOpened()){
    cout << "Could not open output video file! Check that you give file name withou extension." << endl;
    return 0;
  }

  ofstream timeFile( timeFileName );

  if(!timeFile.is_open()){
    cout << "Could not open output time file! Could not open output time file!" << endl;
    return 0;
  }

  timeFile << "# Format: 0-based frame number;time before capture of the frame in ms;time after capture of the frame in ms;" << endl << endl;
    
  cap >> src;

  // set up clock
  auto totalStart = system_clock::now();
  
  time_point<system_clock> start, end;

  int N = 0;

  while (1) {

    start = system_clock::now();
    cap >> src;
    end = system_clock::now();

    outputVideo << src;
    
    imshow("customResolutionCapture", src);

    timeFile << N << ';' <<
      duration_cast<milliseconds>(start-totalStart).count() << ';' << 
      duration_cast<milliseconds>(end-totalStart).count()   << ';' << endl;

    N++;

    if (waitKey(1) >= 0)
      break;
  }

  return 0;
}
