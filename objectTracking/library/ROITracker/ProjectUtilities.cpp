/*
Author: Martin Skalský
email: skalsky@email.cz
lastEdit: 4.5.2014
version:0.1.1
File contains:
-utility functions for working with cv::Rect and cv::Mat
-implementation of two simpler fpsCounters
-some filtering alhorithms
*/

#include "ProjectUtilities.h"	
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

 

		//indicate that you made new frame and get average frame rate 
	utils::FpsCounter::FpsCounter():
		fps(0.0),
		smoothFactor(2),
		count(0),
		since( time(NULL))
	{
	}
	
			/** Function to be called once per frame in order to registre frame
			* @returns actual framerate
			*/
	double utils::FpsCounter::newFrame()
	{
		
		count++;	 
		time(&end);
		
		double sec=difftime(end,since);
		if(sec >= smoothFactor)
		{
			fps = count / sec;
			
			since = end;
			count = 0;
		}
		return fps;
	}

		/** Get current frame rate
		*@returns current frame rate
		*/
	double utils::FpsCounter::getFps()
	{
		return fps;
	} ;

#ifdef VS2013

	utils::FpsCounter11::FpsCounter11():
		clocks(),
		lastTime(clocks.now())
	{

	}

	double utils::FpsCounter11::newFrame()
	{
			//increase frames count
		

		now = clocks.now();
		deltaT = now - lastTime;
		lastTime = now;

		fps = 1000 /(double) chrono::duration_cast<chrono::milliseconds>(deltaT).count();
		return fps;
	}

	double utils::FpsCounter11::getFps()
	{
		return fps;
	}
#endif


	/**conver to the same referecnce frame as is parent
	* @param converted Rect 
	* @param parent rectangle on which is given rectagnle placed
	* @return rectagnle with same reference frame as parent
	*/
	cv::Rect utils::convertToParent( const cv::Rect& toBeConverted, const cv::Rect& parent)
	{
		return Rect(toBeConverted.x + parent.x, toBeConverted.y + parent.y, toBeConverted.width, toBeConverted.height);	
	}


		/*Crops provaded rectangel so it fits into provided Mat size
		*	@param input rectangle which will be cropped 
		*	@param frame on which it will be cropped
		*/
	cv::Rect utils::fitInMat(const cv::Rect& input, const cv::Mat& frame)
	{
			 
			//check if rect dont'overflow from picture
		return Rect(
			input.x < frame.cols ? input.x : frame.cols,
			input.y < frame.rows ? input.y : frame.rows,
			input.width + input.x > frame.cols ? frame.cols - input.x : input.width,
			input.height + input.y > frame.rows ? frame.rows - input.y : input.height );
		
	}


	void utils::addPadding(const cv::Rect& input, cv::Rect& output, int padding)
	{	   
		if (input.x - padding < 0)
		{
			output.width = input.width + padding + input.x;
			output.x = 0 ;
		}else
		{
			output.x = input.x - padding;
			output.width = input.width + 2 * padding;
		}

		if (input.y - padding < 0)
		{
			output.height = input.height + padding + input.y;
			output.y = 0 ;
		}
		else
		{
		   output.y = input.y - padding;
		   output.height = input.height +  2 * padding ;
		}
		
	}



	void utils::scaleX(const cv::Rect& input, cv::Rect& output, const double &scale)
	{
		output = input;
		output.x = onlyPositive( (int) (output.x -(input.width * abs(scale) - input.width) / 2.0));
		output.width = (int)(input.width * abs(scale));
	}

	
	void utils::scaleY(const cv::Rect& input, cv::Rect& output, const double &scale)
	{	
		output = input;
		output.y =  utils::onlyPositive((int)(output.y - (input.height * abs(scale) - input.height) / 2.0));
		output.height = (int)(input.height * abs(scale));
		
		
	}

	void utils::move(cv::Rect& rect, int x, int y)
	{
		
		rect.x = onlyPositive(rect.x + x);
		rect.y = onlyPositive(rect.y + y);
	}

	cv::Rect utils::circleToRect( cv::Point2f centre, double radius )
	{
		return cv::Rect( (int)(centre.x - radius),(int) (centre.y - radius), (int)(2 * radius),(int)( 2 * radius) );
	}


	cv::Point2i utils::getCenter(const cv::Rect& rect)
	{
		return Point2i(rect.x + rect.width / 2, rect.y + rect.height / 2);
	}

	void utils::scale(const cv::Rect& input, cv::Rect& output, const double &scale)
	{
		utils::scaleX(input, output, scale);
		utils::scaleY(output, output, scale);
	}
	

	utils::exponentialSmoother::exponentialSmoother(double gain, double trendGain, double initialEstimate, double initialTrend) :
		lastEstimate(initialEstimate),
		lastTrend(initialTrend),
		trendGain(trendGain),
		estimateGain(gain)
	{

	}

	double utils::exponentialSmoother::smooth(double value)
	{
		double prevEstimate = lastEstimate;
		lastEstimate = estimateGain * value + (1 - estimateGain) * (lastEstimate + lastTrend);
		lastTrend = trendGain *  (prevEstimate - lastEstimate) + (1 - trendGain) * lastTrend;
		return lastEstimate;
	}

	void utils::exponentialSmoother::setEstimate(double newEstimate)
	{
		lastEstimate = newEstimate;
	}

	
	
		
		

