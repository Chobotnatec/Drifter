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

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace drift;

const int camID = 1;

int main() {

  Mat src;
  vector<drift::MarkerType> markerTypes;
  vector<vector<Point2d> > markers;

  markerTypes.push_back(drift::MarkerType((string)DRIFTER_DATA_DIR+"/markers/pinkMarker.txt"));

  Camera cam = loadCameraFromFile((string)DRIFTER_DATA_DIR+"/cameraData/camera.txt");

  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,cam.getResWidth());
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,cam.getResHeight());

  if (!cap.isOpened())
    return false;

  Matrix33 planeR = Matrix33::Identity();
  Vector3 planeT;

  while (1) {
    cap >> src;

    //flip(src,src,1);

    // Track orange markers
    drift::trackMarkers(src, markerTypes, markers);

    // sort markers in circular patern
    reorganizePoints(markers[0]);

    // print markers in picture
    for (size_t i = 0; i < markers[0].size(); i++) {
      circle(src, markers[0][i], 1, Scalar(255, 0, 0), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[0][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(255, 0, 0));
    }


    imshow("preprocess test", src);

    if (waitKey(1) >= 0)
      break;
  }

  return 0;
}


