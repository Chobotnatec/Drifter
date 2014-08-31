#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../library/objectTracking.h"
#include "../library/configParser.h"
#include <iosfwd>

#include <chrono>
#include <string>

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace tracking;

const int camID = 0;

int main(int argc, char *argv[]){

  ConfigParser parser;

  auto stat = parser.parse( argc, argv );

  if( stat != ConfigParser::SUCCESS ){
    return 0;
  }

  cout << "Configuration file is: " << parser.vm["configFile"].as<string>() << endl;
  cout << "Data folder is: " << parser.vm["dataFolder"].as<string>() << endl;

  int MIN_H(0), MAX_H(255), MIN_S(0), MAX_S(255), MIN_V(0), MAX_V(255),
    erodeAmount(0), dilateAmount(0);

  // create trackbars
  namedWindow("trackbars", 0);
  createTrackbar("MIN_H", "trackbars", &MIN_H, 255);
  createTrackbar("MAX_H", "trackbars", &MAX_H, 255);
  createTrackbar("MIN_S", "trackbars", &MIN_S, 255);
  createTrackbar("MAX_S", "trackbars", &MAX_S, 255);
  createTrackbar("MIN_V", "trackbars", &MIN_V, 255);
  createTrackbar("MAX_V", "trackbars", &MAX_V, 255);
  createTrackbar("erodeAmount", "trackbars", &erodeAmount, 10);
  createTrackbar("dilateAmount", "trackbars", &dilateAmount, 10);

  Mat src, src_processed;

  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  if (!cap.isOpened())
    return false;

  cap >> src;

  while (1) {

    Timer timer;

    cap >> src;

    Timer preprocessTimer;
    tracking::preprocessImage(src, src_processed,
			   cv::Scalar(MIN_H, MIN_S, MIN_V),
			   cv::Scalar(MAX_H, MAX_S, MAX_V), erodeAmount, dilateAmount);
    timer.stop( "Only preprocess took: " );

    imshow("preprocess test", src_processed);

    timer.stop( "Preprocess time taken: " );

    if (waitKey(1) >= 0)
      break;
  }

  return true;
}
