// Do not remove the include below
#include "FreeIMUTest.h"


#include "MPU60X0.h"
#include "I2Cdev.h"
#include "Encoder.h"
#include "HBridgeMotor.h"
#include "PacketUtils.h"

//CREATION OF ALL ACTIVE PARTS*************************************
MPU60X0 accelgyro(false,0x68);
PacketStream packetStream(&Serial);	//creating packet output stream from serial0
Parser parser(&Serial);		//creating parser on serial 0
HBridgeMotor motor(3,4);	//creating
Encoder encoder(1,2);
//*****************************************************************

int16_t ax, ay, az;
int16_t gx, gy, gz;
long int ticks = 0;
int timeStamp = 0;

//The setup function is called once at startup of the sketch
void setup()
{
		//initialize communication
	Serial.begin(115200);
	Wire.begin();
		//wait just to be sure
	delay(10);
	Serial.println("Hello beauty");

		//INITCIALIZATION OF MPU6050
    accelgyro.initialize();
    	//set low pas filter to 44hz
    accelgyro.setDLPFMode(3);
    	//set accel scale to +-4g
    accelgyro.setFullScaleAccelRange(1); //+-4g
    accelgyro.setFullScaleGyroRange(2);	//+-1000°/s
    	//verify connection

    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");


    delay(100);
}



// The loop function is called in an endless loop
void loop()
{

			//check whether there is message
		while(parser.parse())
		{
			int8_t angle = 0;
			unsigned int speed = 0;

			parser.readToken(&angle);
			parser.readToken(&speed);

			//TODO: servo stuff with angle;
			motor.setSpeed(speed);	//setting speed of mottors
		}
			//read data from sensor
		accelgyro.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);


		packetStream.newMessage(); //that one is not nessesary if you know that there is nothing in current message
		packetStream.write((uint32_t)millis() );	//32 uint
		packetStream.write(ax);	//16 int
		packetStream.write(ay);	//16 int
		packetStream.write(gz);	//16 int
		packetStream.write(encoder.getTicks()); //32 int
		packetStream.write(encoder.getSpeed()); //16 int
		packetStream.flush();	//together 16 bytes		//ends and sends packet






		delay(50);	//TODO: what about precise timing hmm;)
}
