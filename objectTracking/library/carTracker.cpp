/*
 * objectTracking.h
 *
 *  Created on: 23 July, 2014
 *      Author: lecopivo
 */

#include "carTracker.h"
#include "ROITracker/ProjectUtilities.h"

#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
using namespace cv;

namespace drift{

  CarTracker::CarTracker( int camId,
			  std::string const& filename,
			  Camera const& cam,
			  MarkerType const& groundMarker,
			  MatrixDD   const& markerDistances,
			  MarkerType const& frontCarMarker,
			  Real              frontCarMarkerHeight,
			  MarkerType const& backCarMarker,
			  Real              backCarMarkerHeight ){

    this->camId = camId;
    this->filename = filename;
    this->cam = cam;
    this->groundMarker = groundMarker;
    this->markerDistances = markerDistances;
    carMarker[0] = frontCarMarker;
    carMarkerHeight[0] = frontCarMarkerHeight;
    carMarker[1] = backCarMarker;
    carMarkerHeight[1] = backCarMarkerHeight;

    readyToTrack = false;

    if( markerDistances.cols() == markerDistances.rows() ){
      markerNum = markerDistances.cols();
    }else{
      throw std::invalid_argument( "MarkerDistances has to be square matrix" );
    }
  }
   
  double CarTracker::initialize( Matrix33 & R, Vector3 & T, int numOfInitFrames ){

    readyToTrack = false;

    Real residuum = -1;

    //
    //   Initialize Camera 
    //
    {
      if( camId >=0 ){
	cap.open( camId );
	cap.set(CV_CAP_PROP_FRAME_WIDTH,cam.getResWidth());
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,cam.getResHeight());
      }else{
	cap.open( filename );
      }      

      if(!cap.isOpened())
	throw std::runtime_error("Unable to open video stream(camera or video file).");

      cout << "Video feed succesfully opened!" << endl;
    }

    // 
    //   Find Ground Markers
    // 
    vector<Point2d> markersAvg( markerNum );
    {
      cv::Mat img;
      vector<Point2d> markers;

      int N = 0;
      
      for(int i=0;i<numOfInitFrames;i++){

	cap >> img;

	drift::trackMarker(img, groundMarker, markers);

	if( markers.size() == markerNum ){
	  N++;
	  reorganizePoints(markers);
	  for(int j=0;j<markerNum;j++)
	    markersAvg[j] += markers[j];
	}else{
	  cout << "Found " << markers.size() << " ground markers. " << markerNum << " ground markers should have been found." << endl;
	}
      }

      if(N>0){
	cout << "Ground markers succesfully found in " << N << " frames out of " << numOfInitFrames << " used frames. " << endl;
      }else{
	throw std::runtime_error( "Unable to find ground markers!" );
      }

      for(int j=0;j<markerNum;j++){
	markersAvg[j] *= 1.0/N;
      }
      
    }
    //
    //   Find plane coordinate frame
    // 
    {
      vector<Vector3> groundPoints;

      residuum = findPointPositions( markerDistances, markersAvg, cam, groundPoints );
      findPlaneCoordinateFrame( cam, groundPoints, planeR, planeT );

      R = planeR;
      T = planeT;

      cout << "Position determined, camera should be approximately " << (-planeR.transpose()*planeT)[2] << "mm above ground." << endl;

    }
    //
    // Reinitialize video capture if read from file
    //
    {
      if( camId >=0 ){
      }else{
	cap.open( filename );
      }      

      if(!cap.isOpened())
	throw std::runtime_error("Unable to reopen video stream(video file).");
    }

    readyToTrack = true;

    return residuum;
  }


  double CarTracker::initialize( int numOfInitFrames ){
    Matrix33 R;
    Vector3 T;
    return initialize( R, T, numOfInitFrames );
  }

  bool CarTracker::trackCar( Vector3 & frontMarkerPos,
			     Point2d & frontMarkerScreenPos,
			     Vector3 & backMarkerPos,
			     Point2d & backMarkerScreenPos, 
			     std::chrono::time_point<std::chrono::system_clock> & timeStart,
			     std::chrono::time_point<std::chrono::system_clock> & timeEnd){

    cv::Mat img;

    timeStart = std::chrono::system_clock::now();
    if(cap.read(img)){
      timeEnd = std::chrono::system_clock::now();

      trackMarker( img, 0, frontMarkerPos, frontMarkerScreenPos );
      trackMarker( img, 1, backMarkerPos, backMarkerScreenPos );

      {
	circle(img, frontMarkerScreenPos, 1, Scalar(255, 0, 0), 2);
	stringstream ss;
	ss << 0;
	putText(img, ss.str(), frontMarkerScreenPos, FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 0, 0));
      }
      {
	circle(img, backMarkerScreenPos, 1, Scalar(0, 255, 0), 2);
	stringstream ss;
	ss << 1;
	putText(img, ss.str(), backMarkerScreenPos, FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 0, 0));
      }

      imshow("car tracking", img);
    }else{
      cout << "Could not get next video frame!" << endl;
      return false;
    }
	  
    return true;
  }

  bool CarTracker::trackCar( Vector3 & frontMarkerPos,
			     Point2d & frontMarkerScreenPos,
			     Vector3 & backMarkerPos,
			     Point2d & backMarkerScreenPos ){
    std::chrono::time_point<std::chrono::system_clock> time ;
    return trackCar( frontMarkerPos, frontMarkerScreenPos, backMarkerPos, backMarkerScreenPos, time, time );
  }


  bool CarTracker::trackCar( Vector3 & frontMarkerPos,
			     Vector3 & backMarkerPos,
			     std::chrono::time_point<std::chrono::system_clock> & timeStart,
			     std::chrono::time_point<std::chrono::system_clock> & timeEnd){
    Point2d frontMarkerScreenPos;
    Point2d backMarkerScreenPos;
    return trackCar( frontMarkerPos, frontMarkerScreenPos, backMarkerPos, backMarkerScreenPos, timeStart, timeEnd );
  }

  bool CarTracker::trackCar( Vector3 & frontMarkerPos,
			     Vector3 & backMarkerPos ){
    Point2d frontMarkerScreenPos;
    Point2d backMarkerScreenPos;
    std::chrono::time_point<std::chrono::system_clock> time;
    return trackCar( frontMarkerPos, frontMarkerScreenPos, backMarkerPos, backMarkerScreenPos, time, time );
  }
    

  bool CarTracker::trackMarker( cv::Mat const& img,
		    int markerId, // 0 - front marker, 1 - back marker
		    Vector3 & markerPos,
		    cv::Point2d & markerScreenPos ){
    cv::Mat region;
    vector<Point2d> markers;

    region = roiTracker[markerId].getROI( img );

    drift::trackMarker(region, carMarker[markerId], markers);

    if( markers.size()==1 ){

      roiTracker[markerId].setRelativeLocation( markers[0] );

      markerScreenPos = roiTracker[markerId].getObjectCenter();

      markerPos = projectPointOntoPlane( cam, markerScreenPos, carMarkerHeight[markerId], planeR, planeT );
    }else{
      roiTracker[markerId].reset();
      return false;
    }

    return true;
  }

};
