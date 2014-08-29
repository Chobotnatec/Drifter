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

const int camID = 1;


  
bool preprocessTest() {

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
    drift::preprocessImage(src, src_processed,
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

bool objectTrackTest() {

  int MIN_H(0), MAX_H(45), MIN_S(20), MAX_S(255), MIN_V(40), MAX_V(255),
    minArea(0), maxArea(10000), erodeAmount(2), dilateAmount(2);

  // create trackbars
  namedWindow("trackbars", 0);
  createTrackbar("MIN_H", "trackbars", &MIN_H, 255);
  createTrackbar("MAX_H", "trackbars", &MAX_H, 255);
  createTrackbar("MIN_S", "trackbars", &MIN_S, 255);
  createTrackbar("MAX_S", "trackbars", &MAX_S, 255);
  createTrackbar("MIN_V", "trackbars", &MIN_V, 255);
  createTrackbar("MAX_V", "trackbars", &MAX_V, 255);
  createTrackbar("minArea", "trackbars", &minArea, 10000);
  createTrackbar("maxArea", "trackbars", &maxArea, 10000);
  createTrackbar("erodeAmount", "trackbars", &erodeAmount, 10);
  createTrackbar("dilateAmount", "trackbars", &dilateAmount, 10);

  Mat src_distorted,src, src_processed;

  VideoCapture cap(camID);
  if(camID!=0){
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  }

  if (!cap.isOpened())
    return false;

  //cap >> src;
  //cap >> src_distorted;


  Mat camMatrix = (Mat_<double>(3,3) << 1.4339392765994578e+03, 0., 6.3950000000000000e+02, 0. ,
		   1.4339392765994578e+03 , 3.5950000000000000e+02 , 0. , 0. , 1.);

  Mat distorCoeff =  (Mat_<double>(5,1) << -2.6991554586924132e-02, 4.4901163926966403e-01, 0., 0.,-2.5965718697420312e+00 );

  while (1) {
    cap >> src;

    //		undistort(src_distorted,src,camMatrix,distorCoeff);

    flip(src, src, 1);

    drift::preprocessImage(src, src_processed,
			   cv::Scalar(MIN_H, MIN_S, MIN_V),
			   cv::Scalar(MAX_H, MAX_S, MAX_V), erodeAmount, dilateAmount);

    vector<Point2d> objects;
    vector<vector<Point> > contours;

    drift::trackObjects(src_processed, objects, minArea, maxArea, contours);

    drawContours(src, contours, -1, Scalar(0, 0, 255), 1);

    for (size_t i = 0; i < objects.size(); i++) {
      circle(src, objects[i], 1, Scalar(0, 255, 0), 2);
    }

    imshow("preprocess test", src);

    if (waitKey(30) >= 0)
      break;
  }

  return true;
}

bool markerTrackTest() {

  Mat src;
  vector<drift::MarkerType> markerTypes;
  vector<vector<Point2d> > markers;

  markerTypes.push_back(drift::MarkerType("pinkMarker.txt"));
  markerTypes.push_back(drift::MarkerType("orangeMarker.txt"));
  markerTypes.push_back(drift::MarkerType("blueMarker.txt"));

  drift::MarkerType A(markerTypes[0]);
  drift::MarkerType B(markerTypes[1]);
  drift::MarkerType C(markerTypes[2]);

  VideoCapture cap(camID);
  cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);


  if (!cap.isOpened())
    return false;

  cap >> src;

  while (1) {
    cap >> src;

    //flip(src, src, 1);

    drift::trackMarkers(src, markerTypes, markers);

    for (size_t i = 0; i < markers[0].size(); i++) {
      circle(src, markers[0][i], 1, Scalar(255, 0, 0), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[0][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(255, 0, 0));
    }

    for (size_t i = 0; i < markers[1].size(); i++) {
      circle(src, markers[1][i], 1, Scalar(0, 255, 0), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[1][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(0, 255, 0));

    }

    for (size_t i = 0; i < markers[2].size(); i++) {
      circle(src, markers[2][i], 1, Scalar(0, 0, 255), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[2][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(0, 0, 255));

    }

    imshow("preprocess test", src);

    if (waitKey(30) >= 0)
      break;
  }

  return true;
}

bool cameraClassTest() {

  Camera cam(Matrix33::Identity(), Matrix33::Identity(), Vector3(1, -2, 5));
  //Camera cam(1.324);

  vector<Vector3> points;

  Vector3 p = Vector3(0, 0, 1);
  Vector3 a = 10 * Vector3(1, 0, 0.02);
  Vector3 b = 10 * Vector3(0, 1, 0.01);
  points.push_back(Vector3(2 * a + 3.4 * b + p));
  points.push_back(Vector3(-1.5 * a + 0.1 * b + p));
  points.push_back(Vector3(-0.3 * a - 2 * b + p));
  points.push_back(Vector3(0.2 * a + 2.5 * b + p));

  for (size_t i = 0; i < points.size(); i++) {

    cout << points[i].transpose();
    Point cp = cam.projectPoint(points[i]);
    cout << endl << cp << endl;
    Vector3 p = cam.getDirection(cp);
    cout << p.transpose();
    cout << endl << cam.projectPoint(p + cam.getCameraPosition()) << endl;

  }

  return true;
}

bool findPointPositionTest() {

  Matrix33 A;
  A << 662, 0, 320, 0, 662, 240, 0, 0, 1;
  Matrix33 R = Matrix33::Identity();
  R(1, 1) = -1;
  Vector3 T = Vector3::Zero();
  Camera cam(A, R, T);
  vector<cv::Point2d> inPoints;
  vector<Vector3> outPoints;
  vector<Vector3> refPoints;

  Vector3 p = Vector3(0, 0, 600);
  Vector3 a =  Vector3(1, 0, 0.002);
  Vector3 b =  Vector3(0, 1, 0.001);
  refPoints.push_back(Vector3(100 * a + 300 * b + p));
  refPoints.push_back(Vector3(-150 * a + 10 * b + p));
  refPoints.push_back(Vector3(-30 * a - 20 * b + p));
  refPoints.push_back(Vector3(20 * a + 250 * b + p));

  MatrixDD d(refPoints.size(), refPoints.size());

  for (size_t i = 0; i < refPoints.size(); ++i) {
    inPoints.push_back(cam.projectPoint(refPoints[i]));
    for (size_t j = 0; j < refPoints.size(); ++j) {
      d(i, j) = (refPoints[i] - refPoints[j]).norm();
    }
  }

  //	d(1,0) += 10;
  //	d(0,1) += 10;

  cout << "Residuum, should be zero" << endl
       << findPointPositions(d, inPoints, cam, outPoints) << endl << endl;

  cout << "Reference points" << endl;
  for (size_t i = 0; i < refPoints.size(); ++i) {
    cout << refPoints[i].transpose() << " " << inPoints[i];
    cout << endl;
  }

  cout << endl << "Out points: " << endl;

  for (size_t i = 0; i < outPoints.size(); ++i) {
    cout << outPoints[i].transpose();

    cout << endl;
  }

  cout << endl << "Distance matrix of reference points" << endl << d << endl
       << endl;

  cout << endl << "Distance matrix of out points" << endl;

  for (size_t i = 0; i < refPoints.size(); ++i) {
    for (size_t j = 0; j < refPoints.size(); ++j) {
      d(i, j) = (outPoints[i] - outPoints[j]).norm();
    }
  }

  cout << d << endl;

  return true;
}

bool sesitNalepkyTest() {

  Mat src;
  vector<drift::MarkerType> markerTypes;
  vector<vector<Point2d> > markers;

  // marker number 0
  markerTypes.push_back(drift::MarkerType("pinkMarker.txt"));
  // marker number 1
  markerTypes.push_back(drift::MarkerType("yellowMarker.txt"));
  // marker number 2
  markerTypes.push_back(drift::MarkerType("orangeMarker.txt"));

  /*drift::MarkerType A(markerTypes[0]);
    drift::MarkerType B(markerTypes[1]);
    drift::MarkerType C(markerTypes[2]);*/

  Matrix33 A;
  //A << 662, 0, 320, 0, 662, 240, 0, 0, 1;
  A << 1447.7, 0, 1280/2, 0, 1447.7, 720/2, 0, 0, 1;
  Matrix33 R = Matrix33::Identity();
  R(1, 1) = -1;
  Vector3 T = Vector3::Zero();

  Camera cam(A, R, T);
  vector<cv::Point2d> inPoints;
  vector<Vector3> outPoints;
  MatrixDD d(3, 3);
  d(0, 0) = 0;
  d(0, 1) = 121;
  d(0, 2) = 196;
  d(1, 0) = 121;
  d(1, 1) = 0;
  d(1, 2) = 177;
  d(2, 0) = 196;
  d(2, 1) = 177;
  d(2, 2) = 0;

  VideoCapture cap(camID);
  if(camID!=0){
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  }

  if (!cap.isOpened())
    return false;

  ofstream outFile("dataOut2.txt");

  if (!outFile.is_open())
    return false;

  outFile.clear();

  cap >> src;

  while (1) {
    cap >> src;

    //flip(src,src,1);

    drift::trackMarkers(src, markerTypes, markers);

    for (size_t i = 0; i < markers[0].size(); i++) {
      circle(src, markers[0][i], 1, Scalar(255, 0, 0), 2);
    }

    for (size_t i = 0; i < markers[1].size(); i++) {
      circle(src, markers[1][i], 1, Scalar(0, 255, 0), 2);
    }

    for (size_t i = 0; i < markers[2].size(); i++) {
      circle(src, markers[2][i], 1, Scalar(0, 0, 255), 2);
    }

    inPoints.clear();

    if (markers[0].size() == 1 && markers[1].size() == 1
	&& markers[2].size() == 1) {

      cout << markers[0] << endl;
      cout << markers[1] << endl;
      cout << markers[2] << endl;

      inPoints.push_back(markers[0][0]);
      inPoints.push_back(markers[1][0]);
      inPoints.push_back(markers[2][0]);

      double residuum = findPointPositions(d, inPoints, cam, outPoints);

      cout << "Resuduum is: " << residuum << endl;

      if (residuum < 0.01) {
	for (int i = 0; i < 3; i++) {
	  for (int j = 0; j < 3; j++) {
	    outFile << outPoints[i][j] << " ";
	  }
	}
	outFile << endl;
      }
    } else {
      cout << "Failed to recognize markers!" << endl;
    }

    imshow("preprocess test", src);

    if (waitKey(30) >= 0) {
      outFile.close();
      break;
    }
  }

  return true;
}

void findPlaneCoordinateFrameTest() {

  Matrix33 A = Matrix33::Identity();
  Matrix33 R = Matrix33::Identity();
  R(1, 1) = -1;
  Vector3 T = Vector3(1, 0, -5);

  Camera cam(A, R, T);

  vector<Vector3> points;

  points.push_back(Vector3(0.1, 2, 1));
  points.push_back(Vector3(8, -4, 1));
  points.push_back(Vector3(-5, -2, 1));
  points.push_back(Vector3(10, 10, 1));

  Matrix33 B;
  Vector3 V;

  findPlaneCoordinateFrame(cam, points, B, V);

  cout << B << endl;
  cout << V << endl;
}

void reorganizePointsTest() {

  vector<Point2d> points;

  points.push_back(Point2d(3, 1));
  points.push_back(Point2d(1, 1));
  points.push_back(Point2d(1, 2));
  points.push_back(Point2d(2, 3));
  points.push_back(Point2d(3, 3));

  reorganizePoints(points);

  for (size_t i = 0; i < points.size(); ++i) {
    cout << points[i] << endl;
  }
}

bool driveGroundTest() {

  Mat src;
  vector<drift::MarkerType> markerTypes;
  vector<vector<Point2d> > markers;

  markerTypes.push_back(drift::MarkerType("pinkMarker.txt"));
  markerTypes.push_back(drift::MarkerType("orangeMarker.txt"));

  drift::MarkerType A(markerTypes[0]);

  VideoCapture cap(camID);
  if(camID!=0){
    cap.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);
  }


  if (!cap.isOpened())
    return false;

  ofstream outFile,ballPosFile;
  outFile.open("trackingPositions.txt");
  ballPosFile.open("trackingBall.txt");

  if (!outFile.is_open())
    return false;
  if(!ballPosFile.is_open())
    return false;

  cap >> src;

  bool initializeOutPoint = true;
  vector<Vector3> outPoints(4);
  vector<Vector3> filPoints(4);
  double weight = 1;
  for(int i=0;i<4;i++){
    outPoints[i] = Vector3(0,0,0);
    filPoints[i] = Vector3(0,0,0);
  }

  Matrix33 planeR = Matrix33::Identity();
  Vector3 planeT;


  while (1) {
    cap >> src;

    //flip(src,src,1);

    drift::trackMarkers(src, markerTypes, markers);

    reorganizePoints(markers[0]);

    for (size_t i = 0; i < markers[0].size(); i++) {
      circle(src, markers[0][i], 1, Scalar(255, 0, 0), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[0][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(255, 0, 0));
    }
    for(size_t i=0;i<markers[1].size();i++){
      circle(src, markers[1][i], 1, Scalar(0, 255, 0), 2);
      stringstream ss;
      ss << i;
      putText(src, ss.str(), markers[0][i], FONT_HERSHEY_PLAIN, 1.5,
	      Scalar(255, 0, 0));
    }

    Matrix33 A;
    A << 1.4339392765994578e+03, 0., 6.3950000000000000e+02, 0. , 1.4339392765994578e+03 , 3.5950000000000000e+02 , 0. , 0. , 1.;
    //A << 1.4339392765994578e+03, 000, 639.5, 000, 1.4339392765994578e+03, 359.5, 000, 000, 001;
    Matrix33 R = Matrix33::Identity();
    R(1, 1) = 1;
    Vector3 T = Vector3::Zero();

    Camera cam(A, R, T);


    if (markers[0].size() == 4) {


      MatrixDD d(4, 4);
      /* desky */
      d(0, 0) = 000;
      d(0, 1) = 218;
      d(0, 2) = 269;
      d(0, 3) = 155;
      d(1, 0) = 218;
      d(1, 1) = 000;
      d(1, 2) = 152;
      d(1, 3) = 264;
      d(2, 0) = 269;
      d(2, 1) = 152;
      d(2, 2) = 000;
      d(2, 3) = 219;
      d(3, 0) = 155;
      d(3, 1) = 264;
      d(3, 2) = 219;
      d(3, 3) = 000;


      cout << "Error: " <<  findPointPositions(d, markers[0], cam, outPoints) << endl;

      if(initializeOutPoint){
	filPoints = outPoints;
      }
      initializeOutPoint = false;

      for(int i=0;i<4;i++){
	filPoints[i] += (1/(weight+1))*( outPoints[i] - filPoints[i]);
      }
      weight++;

      for(int i=0;i<4;i++){
	outFile << filPoints[i][0] << " " << filPoints[i][1] << " " << filPoints[i][2] << " " ;
      }
      outFile << endl;


      findPlaneCoordinateFrame(cam, filPoints, planeR, planeT);

      cout << planeR << endl;
      cout << planeT << endl;
    }

    if(markers.size()>1)
      for(int i=0;i<markers[1].size();i++){
	Vector3 pos = projectPointOntoPlane(cam, markers[1][i], 0, planeR, planeT);
	ballPosFile << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
      }


    imshow("preprocess test", src);

    if (waitKey(30) >= 0)
      break;
  }

  return true;
}
int main() {

  preprocessTest();
  //objectTrackTest();
  // markerTrackTest();
  //cameraClassTest();
  //findPointPositionTest();
  //sesitNalepkyTest();
  //findPlaneCoordinateFrameTest();
  //reorganizePointsTest();
  //driveGroundTest();

  return 0;
}

