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

Scalar scalarHSV2BGR( Scalar  colorInHSV ){
  cv::Mat M(1,1,CV_8UC3, colorInHSV);
  cvtColor( M, M, CV_HSV2BGR );
  return Scalar(M.at<Vec3b>(0,0));
}

int main(int arg, char** argc){

  Mat src;
  vector<drift::MarkerType> markerTypes;
  vector<vector<Point2d> > markers;
  if(arg==1){
    markerTypes.push_back(drift::MarkerType((string)DRIFTER_DATA_DIR+"/markers/pinkMarker.txt"));
    markerTypes.push_back(drift::MarkerType((string)DRIFTER_DATA_DIR+"/markers/orangeMarker.txt"));
    markerTypes.push_back(drift::MarkerType((string)DRIFTER_DATA_DIR+"/markers/blueMarker.txt"));
  }else{
    if( argc[1][0] == '-' && argc[1][1] == 'h' ){
      cout << "This program shows detected markers. \nExample of usage: \n$ checkMarkers pinkMarker.txt orangeMarker.txt\n";
      return 0;
    }else{
      for(int i=1;i<arg;i++){
	markerTypes.push_back(drift::MarkerType( argc[i] ));
      }
    }
  }

  Camera cam = loadCameraFromFile((string)DRIFTER_DATA_DIR+"/cameraData/camera.txt");

  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,cam.getResWidth());
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,cam.getResHeight());

  if (!cap.isOpened())
    return false;

  int MN = markerTypes.size();

  cap >> src;

  while (1) {
    cap >> src;

    drift::trackMarkers(src, markerTypes, markers);

    for(int j=0;j<markers.size();j++){
      auto& marks = markers[j];
      Scalar HSVcolor = 0.5*(markerTypes[j].minHSV + markerTypes[j].maxHSV);
      HSVcolor[0] = (int)(HSVcolor[0] + 128) % 256;
      HSVcolor[1] = 255;
      HSVcolor[2] = 255;
      Scalar color = scalarHSV2BGR( HSVcolor );
      
      for (size_t i = 0; i < marks.size(); i++) {
	//	HSVcolor[1] = 0; HSVcolor[2] = 255;
	circle(src, marks[i], 1, color, 2);
	stringstream ss;
	ss << i;
	putText(src, ss.str(), marks[i], FONT_HERSHEY_PLAIN, 1.5, color);
      }
    }

    imshow("check Markers", src);

    if (waitKey(1) >= 0)
      break;
  }

  return 0;
}
