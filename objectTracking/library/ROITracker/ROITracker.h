/*
Author: Martin Skalský
email: skalsky@email.cz
lastEdit: 23.7.2014
version: 0.1.5

File contains:
-implementation of simple tracker which is able to track end extract ROI from image


*/

#ifndef ROITRACKER_H__
#define ROITRACKER_H__

#include "opencv2/opencv.hpp"

class ROITracker
{
private:
	//rect around next ROI
	cv::Rect ROI_rect;
	//position of the object in last iteration
	cv::Rect objectPosition;
	//center of the last detected object
	cv::Point2d objectCenter;
	//minimal Size of the object
	const cv::Size absoluteMinSize;

	cv::Size minSize;
	//relative difference between size of the object and next time ROI
	const double sizeScale;
	//relative difference in size of object and next min size
	const double minSizeScale;
	//how fast to scale ROI on fail
	const double recoveryROIScale;
	//how fast to scale min size on fail
	const double recoveryMinSizeScale;
public:
	/* Constructor
	* @param objectMinSize minum size in pixels which can object ever be
	* @param sizeScale scale between last detecte object size and next time ROI size
	* @param minSizeScale bound on minimal object size compared to last detected object size
	* @param recoveryScale how much should be ROI scaled on fail()
	* @param recoveryMinSizeScale how much should be minimal object size scaled on fail()
	*/
	ROITracker( cv::Size objectMinSize = cv::Size( 100, 100 ), 
		double sizeScale = 1.3, 
		double minSizeScale = 0.8, 
		double recoveryScale = 2.0, 
		double recoveryMinSizeScale = 0.9 );
	/** get rectangle for current region of interest(beware it can be outside you picture!!!)
	* @return Rect around current ROI
	*/
	cv::Rect getROI_rect( );
	
	/** method returns last logged position of object in absolute coordinates 
	* @return Rect around last logged position of object
	*/
	cv::Rect getObjectPosition( );
	/**method returns center of last finded object
	* @return point in the center of followed object
	*/
	cv::Point2d getObjectCenter( );

	/**return ROI for next iteration of algorithm
	*	@param frame at which shoul ROI be
	*	@return ROI
	*/
	cv::Mat getROI( const cv::Mat& frame );

	/**
	* function returns min size of the object for next iteration
	* @return minSize of the object for next iteration
	*/
	cv::Size getMinSize( );

	/**
	* Function logs new position of the object relatively to last ROI
	* @param position of picture relative to current ROI
	*/
	void setRelativeLocation( const cv::Rect& relativeLocation );
	/**
	* Function logs new position of the object relatively to last ROI
	* @param position of picture relative to current ROI
	*/
	void setRelativeLocation( const cv::Point2d relativeLocation );
	/**
	*	Function logs new position of object in absolute position
	*	@param position of picture in absolute coordinates(means relative to parent of current ROI)
	*/
	void setAbsoluteLocation( const cv::Rect & absoluteLocation );

	/** logs that your algorithm failed to find desired object it basicaly makes bigger ROI and smaller minsize
	*
	*/
	void fail( );
	/** reset tracker to initial state e.g set ROI to the biggest possible and minSize to absoluteMinumumSize
	*/
	void reset();

};


#endif
