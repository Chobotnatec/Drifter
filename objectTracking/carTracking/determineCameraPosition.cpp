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

  // set up marker types
  // ground markers first
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

    // Track markers
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

    if (markers[0].size() == 4 ) {

      MatrixDD d(4, 4);
      /* desky */
      d(0, 0) = 000;
      d(0, 1) = 938.5;
      d(0, 2) = 1387;
      d(0, 3) = 1063;
      d(1, 0) = d(0,1);
      d(1, 1) = 000;
      d(1, 2) = 1126;
      d(1, 3) = 1446;
      d(2, 0) = d(0,2);
      d(2, 1) = d(1,2);
      d(2, 2) = 000;
      d(2, 3) = 867;
      d(3, 0) = d(0,3);
      d(3, 1) = d(1,3);
      d(3, 2) = d(2,3);
      d(3, 3) = 000;

      vector<Vector3> outPoints;

      cout << "Error: " <<  findPointPositions(d, markers[0], cam, outPoints) << endl;

      findPlaneCoordinateFrame( cam, outPoints, planeR, planeT );

      cout << endl;
      cout << planeR << endl;
      cout << planeT << endl;
      cout << -planeR.transpose()*planeT << endl;
      cout << endl;
    }

    imshow("preprocess test", src);

    if (waitKey(1) >= 0)
      break;
  }

  return 0;
}


