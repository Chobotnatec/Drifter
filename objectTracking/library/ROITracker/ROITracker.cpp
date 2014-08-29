/*
  Author: Martin Skalský
  email: skalsky@email.cz
  lastEdit: 23.7.2014
  version: 0.1.5

  File contains:
  -implementation of simple tracker which is able to track end extract ROI from image


*/



#include "ROITracker.h"
#include "ProjectUtilities.h"

ROITracker::ROITracker( cv::Size objectMinSize, double sizeScale, double minSizeScale, double recoveryScale, double recoveryMinSizeScale ) :
  absoluteMinSize( objectMinSize ),
  minSize( objectMinSize ),
  sizeScale( sizeScale ),
  minSizeScale( minSizeScale ),
  recoveryROIScale( recoveryScale ),
  recoveryMinSizeScale( recoveryMinSizeScale ),
  ROI_rect( 0, 0, 32000, 32000 ),	//beggining with ROI with can accomodate any picture
  objectPosition(0, 0, 10, 10)
{
};
cv::Rect ROITracker::getObjectPosition( )
{
  return objectPosition;
};

cv::Point2d ROITracker::getObjectCenter()
{
  return objectCenter;
};

cv::Rect ROITracker::getROI_rect( )
{
  return ROI_rect;
};

cv::Mat ROITracker::getROI( const cv::Mat& frame )
{

  ROI_rect = utils::fitInMat( ROI_rect, frame );
  return cv::Mat( frame, ROI_rect );
	
};

cv::Size ROITracker::getMinSize( )
{
  return minSize;
}

void ROITracker::setRelativeLocation( const cv::Rect& relativeLocation )
{
  //convert to global coordinates and call as if it was absolute position
  return setAbsoluteLocation( utils::convertToParent( relativeLocation, ROI_rect ) );
}

void ROITracker::setRelativeLocation( const cv::Point2d relativeLocation )
{
  //set center of the object
  objectCenter.x = relativeLocation.x + ROI_rect.x;
  objectCenter.y = relativeLocation.y + ROI_rect.y;
  //create rectangle as new position of object
  cv::Rect newPosition((int)objectCenter.x, (int)objectCenter.y, 1, 1 );
  utils::addPadding( newPosition, objectPosition, 50 );
  //update min height and widht next time
  minSize.height = utils::bigger( (int)(objectPosition.height * minSizeScale), absoluteMinSize.height );
  minSize.width = utils::bigger( (int)(objectPosition.width * minSizeScale), absoluteMinSize.width );
  //set ROI for next time
  utils::scale( objectPosition, ROI_rect, sizeScale );
}


void ROITracker::setAbsoluteLocation( const cv::Rect & absoluteLocation )
{		
  //set position of object
  objectPosition = absoluteLocation;
  //set position of center
  objectCenter = cv::Point2d( objectPosition.x + objectPosition.width / 2.0, objectPosition.y + objectPosition.height / 2.0 );
  //set absolute min size of the object for nex time
  minSize.height = utils::bigger( (int)(objectPosition.height * minSizeScale), absoluteMinSize.height );
  minSize.width = utils::bigger( (int)(objectPosition.width * minSizeScale), absoluteMinSize.width );
  //set max size of the object for next time
  utils::scale( objectPosition, ROI_rect, sizeScale );
}

void ROITracker::fail( )
{	
  //scale ROI_rect by 
  utils::scale( ROI_rect, ROI_rect, recoveryROIScale );
  minSize.height = utils::bigger( (int)(minSize.height * recoveryMinSizeScale), absoluteMinSize.height );
  minSize.width = utils::bigger( (int)(minSize.width * recoveryMinSizeScale), absoluteMinSize.width );
}


void ROITracker::reset()
{	
  minSize = absoluteMinSize;
  ROI_rect.x = 0;
  ROI_rect.y = 0;
  ROI_rect.width = 32000;
  ROI_rect.height = 32000;
}
