/*
 * main.cpp
 *
 *  Created on: Oct 27, 2013
 *      Author: lecopivo
 */
#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../library/objectTracking.h"
#include <iosfwd>

#include <chrono>
#include <string>

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace drift;

const int camID = 0;


  
int main(int arg,char** argc) {

  if(arg!=2){
    cout << "Please specify name of the new marker!\nUsage:\n $ setUpMarker greenMarker.txt\n" << endl;
    return 0;
  }
  
  string filename((string)DRIFTER_DATA_DIR+"/markers/"+argc[1]);

  int MIN_H(0), MAX_H(255), MIN_S(0), MAX_S(255), MIN_V(0), MAX_V(255),
    erodeAmount(0), dilateAmount(0);

  try{
    MarkerType mt( filename );
    
    MIN_H = mt.minHSV[0];
    MAX_H = mt.maxHSV[0];
    MIN_S = mt.minHSV[1];
    MAX_S = mt.maxHSV[1];
    MIN_V = mt.minHSV[2];
    MAX_V = mt.maxHSV[2];
    
    erodeAmount = mt.erode;
    dilateAmount = mt.dilate;
  }catch( std::ios_base::failure ){
  }

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


  ofstream outFile(filename);

  if(!outFile.is_open()){
    cout << "Could not open file: " << filename << endl;
    return 0;
  }

  Mat src, src_processed;

  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  if (!cap.isOpened())
    return false;

  cap >> src;

  while (1) {

    cap >> src;

    drift::preprocessImage(src, src_processed,
			   cv::Scalar(MIN_H, MIN_S, MIN_V),
			   cv::Scalar(MAX_H, MAX_S, MAX_V), erodeAmount, dilateAmount);
    cv::Mat src2;
    src.copyTo(src2,src_processed);
    //    multiply( src, src, src );
    imshow("preprocess test", src2);

    if (waitKey(1) >= 0){
      outFile << MIN_H << ' ' << MAX_H << endl;
      outFile << MIN_S << ' ' << MAX_S << endl;
      outFile << MIN_V << ' ' << MAX_V << endl;
      outFile << erodeAmount << ' ' << dilateAmount << endl;
      outFile << 20 << ' ' << 1000000;
      outFile.close();
      break;
    }
  }

  return 0;
}
