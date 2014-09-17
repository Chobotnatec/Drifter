/*
 * objectTracking.h
 *
 *  Created on: Oct 27, 2013
 *      Author: lecopivo
 */

#ifndef OBJECTTRACKING_H_
#define OBJECTTRACKING_H_

#include <string>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <chrono>
#include <iostream>

namespace tracking{

  typedef double Real;
  typedef Eigen::Matrix<Real,3,3> Matrix33;
  typedef Eigen::Matrix<Real,3,1> Vector3;
  typedef Eigen::Matrix<Real,Eigen::Dynamic,Eigen::Dynamic> MatrixDD;

  /*
   * This class just stored info how to preprocess image so we can find particular markers
   */
  class MarkerType{
  public:
    MarkerType(){}
    /*
     * Format of input file:
     minHue maxHue
     minSaturation maxSaturation
     minValue maxValue
     erode dilate
     minArea maxArea
     * \throw std::ios_base::failure If file could not be opened.
    */
    MarkerType(std::string fileName);

  public:
    cv::Scalar minHSV;
    cv::Scalar maxHSV;
    int erode;
    int dilate;
    int minArea;
    int maxArea;
  };

  /*
   * Camera class
   *
   * for its parameters see http://en.wikipedia.org/wiki/Camera_resectioning
   */
  class Camera{
  public:
    Camera();
    Camera( Real focalLength );
    /*
     * Paramaters A,R,T are used for projection from world space to picture space.
     * This is done by formula p = A*R*(v-T). Point v in world space is projected onto point p in picture space in homogenous coordinates.
     * \param A intrinsic parameters of camera see http://en.wikipedia.org/wiki/Camera_resectioning . This matrix is used for transformation from camera local space(in mm) to the picture frame(in pixels)
     * \param R rotation matrix, from world space to the camera space(objective is pointing in z coordinate)
     *          (following might not be true or is missleading - Watch out, coordinate frame of camera is left-handed! so if your world space is right handed, than R has to have determinant -1)
     * \param T position of camera in space
     */
    Camera(	Matrix33 A,
		Matrix33 R,
		Vector3 T);

    Camera(	Matrix33 A,
		Matrix33 R,
		Vector3 T,
		int resW,
		int resH);


    cv::Point2d projectPoint( Vector3 point ) const;

    /*
     * Returns direction vector v from camera which coresponds to the give point in picture
     * So line t*v+cameraPosition gets projected to the same point in picture, for any t.
     * Output vector is granted to be of length one.
     */
    Vector3 getDirection( cv::Point2d point ) const;

    Vector3 getCameraPosition() const;

    Vector3 localToWorld( Vector3 const v ) const;

    Vector3 worldToLocal( Vector3 const v ) const;

    Matrix33 getR() const;

    Vector3 getT() const;

    int getResWidth() const;

    int getResHeight() const;

    void setR( const Matrix33 & _R );
    
    void setT( const Vector3 & _T );

  private:
    Matrix33 A;
    Matrix33 R;
    Vector3 T;
    int resWidth,resHeight;
  };

  /** 
   * Creates distance matrix from distance file.
   * Distance file has to contain distances between markers. We denote distance between marker i and marker j by (ij). 
   *
   * Format of this file is follows:
   *
   * (01) (02) (03) ... (0n)
   *      (12) (13) ... (1n)
   *   .                  .
   *   .                  .
   *                ((n-1)n)
   *
   * Where n is number of markers. You have to obey only the order not the spaces, so the file can look like this:
   * 
   * (01) (02) ... (0n) (12) (13) ... (1n) (23) ... ((n-1)n)
   *
   * \param distanceFile Location of distance file.
   * \return distance matrix
   * \throw std::runtime_error If file could not be opened.
   */
  MatrixDD loadDistanceMatrix( std::string distanceFile );

  /*
   * Load camera from file.
   * 
   * Camera file contains resolution of image, intristic parameters(3x3 matrix) of camera, distorsion coefficients(not currently used).
   * 
   * \param cameraFile Location of camera file
   * \return Returns instance of Camera with given parameters.
   * \throw std::runtime_error If file could not be opened.
   */
  Camera loadCameraFromFile( std::string cameraFile );

  //! Preprocess Image for locating color markers in it.
  /* ADD DESCRIPTION
   * \param inImage color image to preprocess
   * \param outImage black and white output image
   * \param minHue the lowest(channel wise) value of hsv we are interested in
   * \param maxHue highest(channel wise) value of hsv we are interested in
   * \param erodeAmount amount of erosion applied
   * \param dilateAmount amaout of dilatation applied
   * \return false if function failed
   */
  bool preprocessImage( cv::Mat const & inImage,
			cv::Mat & outImage,
			cv::Scalar minHSV,
			cv::Scalar maxHSV,
			int erodeAmount,
			int dilateAmount );

  /*
   * \param inImage preprocessed image from function 'preprocessImage'
   * \param objects centers of detected objects
   * \param minArea ignore objects with area smaller than minArea
   * \param maxArae ignore objects with area bigger that maxArea
   * \param outContours this outputs contours of objects in image, it is only for debug pruposes
   * \return for now it always returns true
   */
  bool trackObjects(cv::Mat const & inImage,
		    std::vector<cv::Point2d> & objects,
		    int minArea, int maxArea,
		    std::vector< std::vector<cv::Point> > & outContours);

  /*
   * see %trackObjects
   */
  bool trackObjects( cv::Mat const & inImage,
		     std::vector<cv::Point2d> & objects,
		     int minArea,
		     int maxArea);

  /* 
   * \param inImage color image in which we look for markers
   * \param markerTypes type of marker to look for
   * \param markers output array of found marker
   */
  bool trackMarker( cv::Mat const & inImage,
		    MarkerType const & markerType,
		    std::vector<cv::Point2d> & markerPoints );
 
  /*
   * \param inImage color image in which we look for markers
   * \param markerTypes type of markers to look for
   * \param markers output array of found markers, marker[i] is array of found markers of type markerTypes[i]
   */
  bool trackMarkers( cv::Mat const & inImage,
		     std::vector<MarkerType> const & markerTypes,
		     std::vector<std::vector<cv::Point2d> > & markers );

  /*
   * Assumes that points form some kind of 'circle'. Than this function sort them so they are in clock wise order.
   * First point is in second quadrant(i.e. the quadrant where (-1,1) is)
   * \param points points to be sorted
   */
  bool reorganizePoints( std::vector<cv::Point2d> & points );

  /*
   * 
   * \param d distance matrix, d_{ij} gives distance(in mm) in world space between points i and j
   * \param points input points in picture space
   * \param cam camera
   * \param outPoints world coordinates of points
   * \param x Marker distances from camera. Only rough guess.
   * \return residuum
   */
  Real findPointPositions( MatrixDD const & d,
			   std::vector<cv::Point2d> const & points,
			   Camera const & cam,
			   std::vector<Vector3> & outPoints,
			   std::vector<Real> & x);

  Real findPointPositions( MatrixDD const & d,
			   std::vector<cv::Point2d> const & points,
			   Camera const & cam,
			   std::vector<Vector3> & outPoints);


  /* This function finds plane defined by input points(planePoints) and defines plane's local coordinate frame.
   * When you project position of camera onto this plane you get origin of plane's coordinate frame.
   * X-coordinate of plane's coordinate frame is same as x-coordinate of camera space projected onto the plane.
   * Z-coordinate points into the half-space where camera lies.
   * Y-coordinate, plane's coordinate frame is right handed so Y-coordinate is given by X and Z coordinate by cross product.
   * WE ASSUME THAT THE PLANE DOES NOT GO THROUGH ORIGIN OF WORLD SPACE. IN OUR APPLICATION THE CAMERA LIES IN ORIGIN OF WORLD SPACE AND THE PLANE WILL NOT DEFINITELY GO THROUGH CAMERA POSITION
   * \param cam camera
   * \param planePoints Points that lies on a plane, they are in world space.
   * \param R Transformation matrix from local space to world space. So to transform point v from local space to world space do R*v+T
   * \param T Origin of plane coordinate frame in world space
   * \return True is success
   */
  bool findPlaneCoordinateFrame( Camera const & cam,
				 std::vector<Vector3> const & planePoints,
				 Matrix33 & R,
				 Vector3 & T );

  /*
   * Project picture point onto the plane
   * \param cam camera
   * \param point point to be projected, in picture space
   * \param height height of point above the surface, if you don't know what it means that set it to zero
   * \param planeR transformation matrix from local space of plane to world space
   * \param planeT origin of plane local space, in world space coordinates
   *	\return projected point position in local space of the plane
   */
  Vector3 projectPointOntoPlane( Camera const & cam,
				 cv::Point2d const point,
				 Real height,
				 Matrix33 const & planeR,
				 Vector3 const & planeT );


  /*
   * Simple timer class
   */
  class Timer{
  public:
    Timer();

    void stop( std::string const & msg );

    std::chrono::time_point<std::chrono::system_clock> start, end;
  };

}


#endif /* OBJECTTRACKING_H_ */
