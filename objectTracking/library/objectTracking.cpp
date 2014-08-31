/*
 * objectTracking.cpp
 *
 *  Created on: Oct 27, 2013
 *      Author: lecopivo
 */

#include <fstream>
#include <cmath>
#include <opencv2/imgproc/imgproc.hpp>

#include <nlopt.hpp>

#include "objectTracking.h"

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace cv;

namespace tracking{

  bool preprocessImage( cv::Mat const & inImage, cv::Mat & outImage, cv::Scalar minHSV, cv::Scalar maxHSV, int erodeAmount, int dilateAmount ){

    Mat image_hsv;

    // convert input image to HSV
    cvtColor( inImage, image_hsv, COLOR_BGR2HSV );

    // select only those pixels we are interested in and produce B&W image
    inRange(image_hsv,minHSV,maxHSV,outImage);

    // erode image
    if(erodeAmount > 0){
      Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));

      for(int i=0;i<erodeAmount;i++)
	erode(outImage,outImage,erodeElement);
    }

    // dilate image
    if(dilateAmount >0){
      Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

      for(int i=0;i<dilateAmount;i++)
	dilate(outImage,outImage,dilateElement);
    }

    return true;
  }

  bool trackObjects( cv::Mat const & inImage, std::vector<cv::Point2d>& objects, int minArea, int maxArea, std::vector< std::vector<cv::Point> > & outContours ){

    objects.clear();

    Mat temp;

    inImage.copyTo(temp);

    vector< vector<Point> > contours;

    findContours(temp,contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE	);

    outContours = contours;


    for(unsigned int i=0;i<contours.size();i++){

      Mat cont(contours[i]);
      Moments moment = moments(cont);
      double area = moment.m00;

      if(area>=minArea && area<=maxArea)
	objects.push_back(Point2d(moment.m10/area,moment.m01/area));

    }

    return true;
  }

  bool trackObjects( cv::Mat const & inImage, std::vector<cv::Point2d> & objects, int minArea, int maxArea){
    std::vector< std::vector<cv::Point> > cont;
    return trackObjects( inImage, objects, minArea, maxArea, cont);
  }


  bool trackMarker( cv::Mat const & inImage, MarkerType const & markerType, std::vector<cv::Point2d> & markerPoints ){
   
    cv::Mat preprocessedImage;

    preprocessImage( inImage, preprocessedImage, markerType.minHSV, markerType.maxHSV, markerType.erode, markerType.dilate );

    trackObjects( preprocessedImage, markerPoints, markerType.minArea, markerType.maxArea );

    return true;
  }


  bool trackMarkers( cv::Mat const & inImage, std::vector<MarkerType> const & markerTypes, std::vector<std::vector<cv::Point2d> > & markers ){

    markers.resize(markerTypes.size());

    cv::Mat preprocessedImage;

    for(size_t i=0;i<markerTypes.size();++i){

      MarkerType const & acMarker = markerTypes[i];

      preprocessImage( inImage, preprocessedImage, acMarker.minHSV, acMarker.maxHSV, acMarker.erode, acMarker.dilate );

      trackObjects( preprocessedImage, markers[i], acMarker.minArea, acMarker.maxArea );
    }

    return true;
  }

  struct ObjectiveFunctionData{
    MatrixDD d; // distance matrix, entries are SQUARES of distances
    vector<Vector3> dir; // direction vectors pointing from camera
  };

  double objectiveFunction(vector<double> const & x, vector<double> & grad, void * f_data){

    MatrixDD const & d = ((ObjectiveFunctionData*)f_data)->d;
    vector<Vector3> const & dir = ((ObjectiveFunctionData*)f_data)->dir;

    if(!grad.empty()){
      if(grad.size()!=x.size()){
	grad.resize(x.size());
      }
      for(size_t alpha=0;alpha<x.size();++alpha){
	grad[alpha] = 0;
	for(size_t j=0;j<x.size();++j){
	  Vector3 v = x[alpha]*dir[alpha] - x[j]*dir[j];
	  grad[alpha] += -4*dir[alpha].dot(v)*(pow((double)d(alpha,j),2)-v.squaredNorm());
	}
      }
    }
    double result = 0;
    for(size_t i=0;i<x.size();++i){
      for(size_t j=0;j<i;++j){
	Vector3 v = x[i]*dir[i] - x[j]*dir[j];
	result += pow( pow((double)d(i,j),2) - v.squaredNorm() , 2 );
      }
    }
    return result;
  }

  double objectiveFunctionV2(vector<double> const & x, vector<double> & grad, void * f_data){

    MatrixDD const & d = ((ObjectiveFunctionData*)f_data)->d;
    vector<Vector3> const & dir = ((ObjectiveFunctionData*)f_data)->dir;

    if(!grad.empty()){
      if(grad.size()!=x.size()){
	grad.resize(x.size());
      }
      for(size_t alpha=0;alpha<x.size();++alpha){
	grad[alpha] = 0;
	for(size_t j=0;j<x.size();++j){
	  Vector3 v = x[alpha]*dir[alpha] - x[j]*dir[j];
	  grad[alpha] += -4*dir[alpha].dot(v)*(pow((double)d(alpha,j),2)-v.squaredNorm());
	}
      }
    }
    double result = 0;
    for(size_t i=0;i<x.size();++i){
      for(size_t j=0;j<i;++j){
	Vector3 v = x[i]*dir[i] - x[j]*dir[j];
	result += abs( (double)d(i,j) - v.norm() );
      }
    }
    return result;
  }  

  Real findPointPositions( MatrixDD const & d,
			   std::vector<cv::Point2d> const & points,
			   Camera const & cam,
			   std::vector<Vector3> & outPoints ){


    nlopt::opt solver(nlopt::LN_SBPLX,points.size());

    solver.set_lower_bounds(0);
    solver.set_upper_bounds(DBL_MAX);

    ObjectiveFunctionData data;

    data.d = d;
    data.dir.resize(points.size());
    Vector3 camPos = cam.getCameraPosition();


    for(size_t i=0;i<points.size();++i){
      data.dir[i] = cam.getDirection(points[i]);
    }

    vector<double> x(points.size(),2000);
    double fmin;

    solver.set_min_objective(objectiveFunctionV2,(void*)&data);

    try{
      solver.optimize(x,fmin);
    }catch(std::runtime_error & err){
    }

    outPoints.resize(points.size());

    for(size_t i=0;i<points.size();++i){
      outPoints[i] = x[i]*data.dir[i] + camPos;
    }
  
    return fmin;
  }

  bool findPlaneCoordinateFrame( Camera const & cam, std::vector<Vector3> const & planePoints, Matrix33 & R, Vector3 & T ){
    int N = planePoints.size();
    if(N>=3){
      // find equation of plane ax+by+cz = 1
      MatrixDD A(N,4);
      for(int i=0;i<N;++i){
	A(i,0) = planePoints[i][0];
	A(i,1) = planePoints[i][1];
	A(i,2) = planePoints[i][2];
	A(i,3) = 1;
      }

      Eigen::JacobiSVD<MatrixDD> svd =  A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);

      Eigen::Matrix<Real,4,1> sol = svd.matrixV().col(3);

      Vector3 camPos = cam.getCameraPosition();
      Vector3 n = Vector3(sol[0],sol[1],sol[2]);
      double d = sol[3]/n.norm();
      n.normalize();

      double camDistFromPlane = n.dot(camPos)+d;

      if(camDistFromPlane<0){
	n *= -1;
	d *= -1;
      }

      // now we projected camera position onto the plane
      T = camPos - camDistFromPlane*n;

      // now we project x coordinate and y coordinate of camera local space onto the plane.
      // And reorthogonalize projected y against projected x
      Vector3 xp,yp;
      Vector3 xc = cam.getR().row(0).transpose();
      Vector3 yc = -cam.getR().row(1).transpose();

      xp = xc - (n.dot(xc))*n;
      xp.normalize();
      yp = yc - (n.dot(yc))*n;
      yp = yp - (xp.dot(yp))*xp;
      yp.normalize();

      R.col(0) = xp;
      R.col(1) = yp;
      R.col(2) = n;

      return true;
    }else{
      cerr<< "To find a plane you need to supplement at least 3 points";
      return false;
    }
  }

  bool reorganizePoints( std::vector<cv::Point2d> & points ){
    if(points.size()>2){
      Point2d center = Point2d(0,0);
      for(size_t i=0;i<points.size();++i){
	center += points[i];
      }
      center *= 1.0/points.size();

      vector<double> w(points.size());

      for(size_t i=0;i<points.size();++i){
	Point2d p = points[i]-center;
	w[i] = atan2(-p.y,p.x);
      }

      for(size_t i=0;i<points.size()-1;++i){
	for(size_t j=points.size()-1;j>i;--j){
	  if(w[j]>w[j-1]){
	    swap(points[j],points[j-1]);
	    swap(w[j],w[j-1]);
	  }
	}
      }
    }
    return true;
  }

  Vector3 projectPointOntoPlane( Camera const & cam,
				 cv::Point2d const point,
				 Real height,
				 Matrix33 const & planeR,
				 Vector3 const & planeT ){
//#warning FUNKCE projectPointOntoPlane NENI JESTE OTESTOVANA
    // recompute everything to the plane space
    Vector3 dir = planeR.transpose()*cam.getDirection(point);
    Vector3 camPos = planeR.transpose()*(cam.getCameraPosition()-planeT);

    Vector3 p = Vector3(camPos[0],camPos[1],0) + Vector3(dir[0],dir[1],0) * camPos[2]/abs((double)dir[2]);
    p = p - Vector3(dir[0],dir[1],0) * height/abs((double)dir[2]);

    return p;

  }


  MarkerType::MarkerType(std::string fileName){
    std::ifstream inFile(fileName.c_str());

    if(inFile.is_open()){

      inFile >> minHSV[0] >> maxHSV[0];
      inFile >> minHSV[1] >> maxHSV[1];
      inFile >> minHSV[2] >> maxHSV[2];

      erode = dilate = minArea = maxArea = 0; // this is only because eclipse told me that these variables are not initialized

      inFile >> erode >> dilate;
      inFile >> minArea >> maxArea;
    }else{
      throw std::ios_base::failure("File " + fileName + " could not be opened!");
    }
  }

  Camera::Camera(){
    Camera(1);
  }

  Camera::Camera( Real focalLength ){
    R = Eigen::Matrix<Real,3,3>::Identity();
    T = Eigen::Matrix<Real,3,1>(0,0,0);
    A << focalLength,0.0,0.0,0.0,focalLength,0.0,0.0,0.0,1.0;
    resWidth = 1280;
    resHeight = 720;
  }

  Camera::Camera(Eigen::Matrix<Real,3,3> A,
		 Eigen::Matrix<Real,3,3> R,
		 Eigen::Matrix<Real,3,1> T ) : A(A),R(R),T(T){
    resWidth = A(0,2)*2 + 1;
    resHeight = A(1,2)*2 + 1;
//#warning Camera class should be reviewed, because I added resolution parameter to it. It should get new constructor with explicitely specifying resolution.
  }

  cv::Point2d Camera::projectPoint( Eigen::Matrix<Real,3,1> point ) const{

    point = A*R*(point-T);

    return cv::Point2d(point[0]/point[2],point[1]/point[2]);
  }

  Vector3 Camera::getDirection( cv::Point2d point ) const{

    Vector3 b(point.x,point.y,1);

    Matrix33 M = A*R;

    Vector3 v = M.colPivHouseholderQr().solve(b);

    if((R*v)[2] < 0){
      v*=-1;
    }

    v.normalize();

    return v;
  }

  Vector3 Camera::getCameraPosition() const{
    return T;
  }

  Vector3 Camera::localToWorld( Vector3 const v ) const{
    return R.transpose()*v + T;
  }

  Vector3 Camera::worldToLocal( Vector3 const v ) const{
    return R*(v-T);
  }

  Matrix33 Camera::getR() const{
    return R;
  }

  Vector3 Camera::getT() const{
    return T;
  }
  
  int Camera::getResWidth() const{
    return resWidth;
  }

  int Camera::getResHeight() const{
    return resHeight;
  }


  MatrixDD loadDistanceMatrix( std::string distanceFile ){

    ifstream file(distanceFile);

    if(!file.is_open()){
      throw runtime_error("Could not open distance file: "+distanceFile);
    }

    int N = 0;
  
    double a;
    file >> a;
    while(!file.eof()){
      N++;
      file >> a;
    }

    file.clear();
    file.seekg(0,ios::beg);

    double n = 0.5*(1.0+sqrt(1.0+8.0*N));

    if(abs(n-trunc(n))>1e-3){
      throw runtime_error("Invalid number of numbers in input file "+distanceFile);
    }

    N = trunc(n);

    MatrixDD D(N,N);

    for(int i=0;i<N;i++){
      for(int j=i+1;j<N;j++){
	file >> a;
	D(i,j) = a;
	D(j,i) = a;
      }
    }

    return D;    
  }


  Camera loadCameraFromFile( std::string cameraFile, Matrix33 R, Vector3 T ){
    ifstream file(cameraFile);

    if(!file.is_open()){
      throw runtime_error("Could not open camera file: "+cameraFile);
    }

    Matrix33 A;

    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
	file >> A(i,j);
      }
    }

    return Camera(A,R,T);
  }

  Timer::Timer(){
    start = std::chrono::system_clock::now();
  }

  void Timer::stop( std::string const & msg ){
    end = std::chrono::system_clock::now();
    std::cout << msg << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << " ms" << std::endl;
  }

};






