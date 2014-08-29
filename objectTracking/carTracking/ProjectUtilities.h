/*
Author: Martin Skalský
email: skalsky@email.cz
lastEdit: 5.5.2014
version: 0.1.2

File contains:
-utility functions for working with cv::Rect and cv::Mat
-implementation of two simpler fpsCounters
-some filtering alhorithms
*/

#ifndef PROJECTUTILITIES_H__
#define PROJECTUTILITIES_H__



#include<time.h>
#include<math.h>
#include<chrono>
#include<queue>
#include "opencv2/opencv.hpp"

using namespace std;



namespace utils{

	/** Class is supposed to serve as approximate fps measurement tool
	*/
	class FpsCounter
	{
	private:
		double fps; //actual fps
		int smoothFactor;	//time window in sec for counting fps
		int count;	//count frames durring current time window

		time_t since, end;
	public:

		FpsCounter();
		/** Function to be called once per frame in order to registr frame
		* @return current framerate
		*/
		double newFrame();
		/** Get current frame rate
		*@return current frame rate
		*/
		double getFps();
	};


	//fps counter which computes fps based on c++11 clocks which works on sub milliscenod scale
	//you can't use that one without full c++11 support!!!!!!!!!!!!!

/*----------->>    */#define VS2013 //if you use vs2010 comment this line!!!    <<---------------
#ifdef VS2013
	class FpsCounter11
	{
	private:
		chrono::system_clock clocks;	//system clocks from which time is drawn
		double fps;	// current fps value 

		chrono::system_clock::time_point now; //current time point
		chrono::system_clock::time_point lastTime; //last time new frame arrived

		chrono::duration<double, chrono::system_clock::period> deltaT; //deration bettween two frames
	public:

		FpsCounter11();
		//registr new frame and return refres rate since last frame
		double newFrame();
		//get last readed fps
		double getFps();
	};
#endif
	/**
	* Function returns center o the finded rectangle
	* @param rect rectangle from which we want to get center
	* @return 2d point in the middle of the frame
	*/
	cv::Point2i getCenter( const cv::Rect& rect );

	/**
	*	moves rectangle by give amount of pixels
	*	@param rect rectangle to be moved
	*	@param x amount of pixels around x axis
	*	@param y amount of pixels around y axis
	*/
	void move( cv::Rect& rect, int x, int y );

	/**function return input when input > 0 otherwise 0
	* @param input number to be truncated to positive value
	* @return input if input > 0, 0 otherwise
	*/
	template<typename T> inline T onlyPositive( T input )
	{
		return (input > 0) ? input : 0;
	}

	/**function return bigger one from two inputs
	* @param first number to be compared
	* @param second number to be compared
	* @return bigger of the two numbers
	*/
	template<typename T> inline T bigger( T first, T second )
	{
		return (first >= second) ? first : second;
	}

	/**conver to parent referecnce frame
	*	@param toBeConverted Rect which will be converted
	*	@param parent rectangle on which is given rectagnle placed
	*	@param converted Rectangle
	*/
	cv::Rect  convertToParent( const cv::Rect& convert, const cv::Rect& parent );
	/**
	*	Crops provaded rectangel so it fits into provided Mat size
	*	@param input rectangle which will be cropped
	*	@param frame on which it will be cropped
	*	@return croped rectangle so it fits into matrix
	*/
	cv::Rect fitInMat( const cv::Rect& input, const cv::Mat& frame );

	/** extend given rectangle by given number of pixels
	*   @param input rectangl to be extended
	*   @param output extended rectangle
	*   @param padding number of pixels by wchich should be rectangle extended
	*/
	void addPadding( const cv::Rect& input, cv::Rect& output, int padding );

	/** function scales given rectangle around x axis
	*	@param input rectangle to be scaled
	*	@param output where to store scaled rectangle
	*	@param scale scaling factor
	*/
	void scaleX( const cv::Rect& input, cv::Rect& output, const double &scale );

	/** function scales given rectangle arounf y axis
	*	@param input rectangle to be scaled
	*	@param output where to store scaled rectangle
	*	@param scale scaling factor
	*/
	void scaleY( const cv::Rect& input, cv::Rect& output, const double &scale );

	/** function scales given rectangle aroun both axes
	*	@param input rectangle to be scaled
	*	@param output where to store scaled rectangle
	*	@param scale scaling factor
	*/
	void scale( const cv::Rect& input, cv::Rect& output, const double &scale );

	//TODO: comment
	cv::Rect circleToRect( cv::Point2f centre, double radius );

	class exponentialSmoother
	{
	private:
		double lastEstimate;
		double lastTrend;

		double trendGain;
		double estimateGain;

	public:
		//
		exponentialSmoother( double gain, double trendGain = 0, double initialEstimate = 0, double initialTrend = 0 );
		double smooth( double value );
		void setEstimate( double newEstimate );
	};

	


	class bandPassSmooth
	{
		std::queue<double> storedValues;
		int order;
		int lowSize;
		int highSize;
		bandPassSmooth* subSmoother;

		bandPassSmooth( int lowPassSize, int highPassSize, int order ) :
			order( order ),
			lowSize( lowPassSize ),
			highSize( highPassSize )
		{
			if (order > 1)
			{
				subSmoother = new bandPassSmooth( lowSize, highSize, order - 1 );
			}
		}
		~bandPassSmooth()
		{
			delete subSmoother;
		}

		double smooth( double input )
		{
			if (order > 1)
			{
				input = subSmoother->smooth( input );
			}
			storedValues.push( input );

			//TODO: unfinished
		}
	};

};



#endif