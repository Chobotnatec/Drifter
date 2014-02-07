/*
 * Encoder.h
 *
 *  Created on: 21.11.2013
 *      Author: Martin Skalský
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "arduino.h"
#include "Callbacks.h"

extern int maxAcceleration;
class Encoder
{
private:
		//pins on which is encoder connected
	char Pin1;
	char Pin2;
		//callback to call on new speed and acceleration
	Callback* callback;

	 	 //how many ticks has been recorded so far
	 volatile signed long int Ticks;
	 	 //absolute times of last edges
	 unsigned long int LastEdgeTime[4];
	 	 //number of ticks at last time
	 unsigned int LastEdgeTicks[4];
	 static const float TickLength = 5.0;
	 int Speed;
	 long int TimeWindow;
	 int countTicks;
	 long int countTime;
	 unsigned char count;
	 bool newMeasure;
	 static Encoder* This;





public:
	Encoder(byte pin1, byte pin2);
	Encoder(byte pin1, byte pin2, Callback* callback);

	long int getTicks()
	{
		return Ticks;
	}
	long int resetTicks()
	{
		long int ret = Ticks;
		Ticks = 0;
		return ret;
	}

	float getLength()
	{
		return Ticks * TickLength;
	}
	#define MINIMAL_SPEED  20
		//return Speed if Speed is large then cca 10cm/s
		//speed in
	int getSpeed()
	{
		if(abs(Speed) < MINIMAL_SPEED || micros() - LastEdgeTime[Ticks%4] > (1000000 / MINIMAL_SPEED))
		{
			return 0;
		}
		return Speed;
	}

		//this function is to be performed on every tick
		//forward -1 if going back 1 if going forward;
	static void int1(void)
	{
		This->thisInt1();
	}

	void thisInt1()
	{
		//   ____ xx
		//__|xx  |__
		// ____   xx
		//|  xx|____|
		//
		if(Ticks%2 == 0)
		{
			Ticks++;
		}
		else
		{
			Ticks--;
		}
		onTick(Ticks%4);
	}

	static void int2(void)
	{
		This->thisInt2();
	}

	void thisInt2(void)
	{
		if(Ticks%2 == 0 )
		{
			Ticks--;

		}
		else{
			Ticks++;
		}
		//call speed and other computation
		onTick(This->Ticks%4);
	}
#define TIME_WINDOW 50000 //lenght of time window in micro-s
void onTick(unsigned char phase)
{
		//measure time since last phase in micros i expect to get at least one tick every hour
	long int measureTime = micros() - LastEdgeTime[phase];
		//if measure time is bigger TIME_WINDOW
	if( measureTime > TIME_WINDOW  )
	{

			//add how much ticks passed
		countTicks +=  LastEdgeTicks[phase] - (unsigned int)Ticks;	//there is no chance of getting more than cca 100ticks per TIME_WINDOW
			//add how much time passed
		countTime += measureTime;
			// increase added
		count|= 1<<phase;
		//reset time measure
		LastEdgeTime[phase]+= measureTime;
				//update last ticks value
		LastEdgeTicks[phase] = Ticks;
		if(count ==  0b1111)
		{
				//reset count
			count = 0;
				//compute (float) value of speed tics/s
			Speed = (1000000 * countTicks) / countTime;
				//compute (float value of acceleration)
			countTicks = 0;
			countTime = 0;
			newMeasure = true;


		}

	}

}
public:

	~Encoder()
	{
		detachInterrupt(Pin1);
		detachInterrupt(Pin2);
	}

};


Encoder::Encoder(byte pin1, byte pin2,Callback* callback):
			callback(callback)
{
	Encoder(pin1,pin2);
}
Encoder::Encoder(byte pin1, byte pin2):
			Pin1(pin1),
			Pin2(pin2),
			Ticks(0),
			LastEdgeTime(),
			LastEdgeTicks(),
			Speed(0),
			TimeWindow(500000),
			countTicks(0),
			countTime(0),

			count(0),
			newMeasure(false)
	{
		This = this;

	//this determines which of two interupts is expected in forward direction
	if (digitalRead(pin1) == digitalRead(pin2))
	{
		Ticks = 0;
	}
	else
	{
		Ticks = 1;
	}

	pinMode(pin1,INPUT);
	pinMode(pin2,INPUT);
		//decide on which pins to put interrupt
		switch(pin1)
		{
			case 2:
				pin1 = 0;
				break;
			case 3:
				pin1 = 1;
				break;
			case 21:
				pin1 = 2;
				break;
			case 20:
				pin1 = 3;
				break;
			case 19:
				pin1 = 4;
				break;
			case 18:
				pin1 = 5;
				break;
			default:
				pin1 = 0;
				break;
		}
		switch(pin1)
		{
			case 2:
				pin2 = 0;
				break;
			case 3:
				pin2 = 1;
				break;
			case 21:
				pin2 = 2;
				break;
			case 20:
				pin2 = 3;
				break;
			case 19:
				pin2 = 4;
				break;
			case 18:
				pin2 = 5;
				break;
			default:
				pin2 = 1;
				break;
		}
			//attach interupts to chosen pins
		attachInterrupt(pin1,int1,CHANGE);
		attachInterrupt(pin2,int2,CHANGE);

	}

Encoder* Encoder::This = NULL;
#endif /* ENCODER_H_ */
