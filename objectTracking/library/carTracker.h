/*
 * objectTracking.h
 *
 *  Created on: 23 July, 2014
 *      Author: lecopivo
 */

#ifndef CARTRACKER_H_
#define CARTRACKER_H_

#include <chrono>

#include "objectTracking.h"
#include "ROITracker/ROITracker.h"

namespace drift{

  /*
   *   CarTracker class
   *
   * Class to provide simple interface to track car.
   */

  class CarTracker{
  public:    
    /* Constructor of car tracker.
     * Constructor just saves all the given parameters.
     * \param camId Id of camera. If you want to track from video file, than set to -1.
     * \param filename Name of video file to track from. If you want to track from camera, than set camId to non-negative value and set filename to anything.
     * \param cam   Instance of Camera class which contains intrinsic, extrinsic parameters of camera i.e. its position, orientation, focal length, resolution
     * \param groundMarker Type of markers on ground
     * \param markerDistances Distances between markers. Please use program `showGroundMarkers` to see which marker is which.
     * \param frontCarMarker Type of marker at the front of the car
     * \param frontCarMarkerHeight Height of front marker center above ground.
     * \param backCarMarker Type of marker at the back of the car
     * \param backCarMarkerHeight Height of back marker center above ground.
     * \throw std::invalid_argument 
     * \see %Camera
     */
    CarTracker( int camId,
		std::string const& filename,
		Camera const& cam,
		MarkerType const& groundMarker,
		MatrixDD   const& markerDistances,
		MarkerType const& frontCarMarker,
		Real              frontCarMarkerHeight,
		MarkerType const& backCarMarker,
		Real              backCarMarkerHeight );

    
    /*
     * Initialize CarTracker.
     * It goes throught few steps:
     *   1. initialize camera
     *   2. find markers
     *   3. find position of camera relative to the markers
     *
     * \param R Transformation matrix from local space to world space. So to transform point v from local space to world space do R*v+T
     * \param T Origin of plane coordinate frame in world space.
     * \numOfInitFrames Initialization will use given number of first frames in stream. If tracking from file, than the file is reinitialized so the tracking starts from first frame in video file.
     * \return Positive value which should be as close to zero as possible or negative one if something failed. Currently it returns ridicously high values.
     * \throw std::runtime_error if something failed
     * \todo Make return value more meaningfull!
     * \see %findPlaneCoordinatesFrame
     */
    double initialize( Matrix33 & R, Vector3 & T, int numOfInitFrames = 20 );

    double initialize( int numOfInitFrames = 20 );

    /*
     * Finds actual position of car.
     *
     * \param frontMarkerPos Position of front marker center in plane space.
     * \param frontMarkerScreenPos Position of front marker in screen space.
     * \param backMarkerPos Position of back marker center in plane space.
     * \param backMarkerScreenPos Position of back marker in screen space.
     * \param time Car was at time `time` at give position.
     */
    bool trackCar( Vector3 & frontMarkerPos,
		   cv::Point2d & frontMarkerScreenPos,
		   Vector3 & backMarkerPos,
		   cv::Point2d & backMarkerScreenPos, 
		   std::chrono::time_point<std::chrono::system_clock> & timeStart,
		   std::chrono::time_point<std::chrono::system_clock> & timeEnd );

    bool trackCar( Vector3 & frontMarkerPos,
		   cv::Point2d & frontMarkerScreenPos,
		   Vector3 & backMarkerPos,
		   cv::Point2d & backMarkerScreenPos );

    bool trackCar( Vector3 & frontMarkerPos,
		   Vector3 & backMarkerPos,
		   std::chrono::time_point<std::chrono::system_clock> & timeStart,
		   std::chrono::time_point<std::chrono::system_clock> & timeEnd );

    bool trackCar( Vector3 & frontMarkerPos,
		   Vector3 & backMarkerPos );


  private:
    bool trackMarker( cv::Mat const& img,
		      int markerId, // 0 - front marker, 1 - back marker
		      Vector3 & markerPos,
		      cv::Point2d & markerScreenPos );
  private:
    int camId;
    std::string filename;
    Camera cam;
    MatrixDD markerDistances;
    int markerNum;
    MarkerType groundMarker;
    MarkerType carMarker[2]; // 0-front marker, 1-back marker
    ROITracker roiTracker[2];
    Real carMarkerHeight[2];
    cv::VideoCapture cap;
    Matrix33 planeR;
    Vector3  planeT; 
    bool readyToTrack;
  };
};

#endif /* CARTRACKER_H_ */
