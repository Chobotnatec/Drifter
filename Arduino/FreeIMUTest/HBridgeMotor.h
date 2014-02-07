/*
 * HBridgeMotor.h
 *
 *  Created on: 11.11.2013
 *      Author: Martin Skalský
 */

#ifndef HBRIDGEMOTOR_H_
#define HBRIDGEMOTOR_H_
#define DEAD_ZONE 2
		//pins for signal connection
#define FW_MOTORPIN 5
#define BW_MOTORPIN 6

class HBridgeMotor
{
	int speed;
	unsigned char FWMotorPin;
	unsigned char BWMotorPin;
	unsigned char enablePin;
	bool IsReady;
		/*
		 * Just initializes some variables motor is still not ready to work!!!
		 */
public:
	HBridgeMotor();

		/*
		 * motor will be ready to work on given pins
		 */
	HBridgeMotor(unsigned char FWMotorPin,unsigned char BWMotorPin);
		/*
		 * will attach to given pins
		 * @FWMotorPin :positive voltage on this pin makes motor turn forward
		 * @BWMotorPin :positive voltage on this pin makes motor turn backward
		 */
	void init(unsigned char FWMotorPin,unsigned char BWMotorPin );
		/*
		 * method will set speed new speed of motor
		 * only if was already attached
		 * @newSpeed speed in range <-255,255>
		 */
	void setSpeed(int newSpeed);

		/*
		 * getter for speed variable
		 * @return current speed in range <-255,255>
		 */
	int getSpeed()
	{
		return speed;
	}
	bool isReady()
	{
		return IsReady;
	}

};



#endif /* HBRIDGEMOTOR_H_ */
