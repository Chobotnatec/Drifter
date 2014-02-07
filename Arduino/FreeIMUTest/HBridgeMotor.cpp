/*
 * HBridgeMotor.cpp
 *
 *  Created on: 27.1.2014
 *      Author: Martin Skalský
 */

#include "HBridgeMotor.h"
#include "Arduino.h"

HBridgeMotor::HBridgeMotor():
	speed(0),
	IsReady(false)

	{};
HBridgeMotor::HBridgeMotor(unsigned char FWMotorPin,unsigned char BWMotorPin)
{
	init(FWMotorPin, BWMotorPin);
}

void HBridgeMotor::init(unsigned char FWMotorPin,unsigned char BWMotorPin )
	{
		this->FWMotorPin =FWMotorPin;
		this->BWMotorPin =BWMotorPin;
		speed =0 ;
		IsReady = true;
	}

void HBridgeMotor::setSpeed(int newSpeed)
	{
			//if is not ready just return
		if(IsReady == false)
		{
			return;
		}
			//go forward
		else if(newSpeed > 0 + DEAD_ZONE )
		{
			analogWrite( FWMotorPin, newSpeed);
			analogWrite( BWMotorPin, 0);
		}
			//go backward
		else if(newSpeed < 0 - DEAD_ZONE)
		{
			analogWrite(FWMotorPin,0);

			analogWrite( BWMotorPin, -newSpeed);
		}
			//brake
		else
		{
			analogWrite(FWMotorPin,0);
			analogWrite(BWMotorPin,0);
		}
		speed = newSpeed;
	}


