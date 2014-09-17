//time out Serial for writing and reading from serial line
#include "TimeoutSerial.h"
//for parsing and packeting job
#include "PacketUtils.h"
#include <thread>
#include <mutex>
#include <chrono>


using namespace std;

class DrifterCar
{
private:
	//serial port where is car
	SerialPort Serial;
	//create input stream
	Parser Parser;
	//create out stream
	PacketStream OStream;
	//
	std::thread TransmiterThread;
	std::mutex transmitLock;
	std::mutex serialLock;


	double Power;
	double Steering;

	uint32_t LastMessageTime;	//last message time in ms
	int32_t Distance;	//Distance in ticks
	int16_t Speed;	//Distance in ticks/s
	int16_t Xaccel; 
	int16_t Yaccel;
	int16_t RotSpeed;

	const double ACCEL_SENSITIVITY = 8192.0; //LSB/g
	const double GYRO_SENSITIVITY = 32.768;  //LSB /(°/s)
	const double ENCODER_RESOLUTION = 5.0; //mm/tick

public:
	DrifterCar();
	bool connect( const string name, unsigned int baudRate );
	bool isConnected();
	void disConnect();
	
	void setPower( const double power );
	void setSteering( double steering );
	void setControlValues( const double power, const double steering );
	
	long int getLastMessageTime();
	double getSpeed();
	double getDistanceTraveled();
	double getXAccel();
	double getYAccel();
	double getRotSpeed();
		//
	bool update();
	void transmitLoop();

};
