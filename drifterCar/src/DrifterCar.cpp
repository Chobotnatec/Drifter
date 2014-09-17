#include "DrifterCar.h"

DrifterCar::DrifterCar( ):
  Serial( ),
  Parser( &Serial ),
  OStream( &Serial ),
  Power( 0.0 ),
  Steering( 0.0 ),
  LastMessageTime( 0 ),
  Speed( 0 ),
  Xaccel( 0 ),
  Yaccel( 0 ),
  RotSpeed( 0 )
{
};

bool DrifterCar::connect( const string name, unsigned int baudRate )
{
  //try to connect to car on given serial port
  try
    {
      if (Serial.isOpen()) Serial.close();
      Serial.open( name, baudRate );
      Serial.setTimeout( boost::posix_time::millisec( 1 ) );

      this_thread::sleep_for( chrono::milliseconds( 1500 ) );
		
      TransmiterThread = thread( &DrifterCar::transmitLoop,this);
		
    }
  catch (boost::system::system_error& e)
    {
      cout << "Error: " << e.what( ) << endl;
      return false;
    }
  return true;
}

bool DrifterCar::isConnected()
{
  return Serial.isOpen();
}

void DrifterCar::disConnect()
{
  Serial.close();
	
}
void DrifterCar::setPower( const double power )
{
  transmitLock.lock();
  Power = power;
  if (Power > 1.0) Power = 1.0;
  if (Power < -1.0) Power = -1.0;
  transmitLock.unlock( );
}
void DrifterCar::setSteering( double steering )
{
  transmitLock.lock( );
  Steering = steering;
  if (steering > 1.0) Steering = 1.0;
  if (Steering < -1.0) Steering = -1.0;
  transmitLock.unlock( );
}

void DrifterCar::setControlValues( const double power, const double steering )
{
  transmitLock.lock( );
	
  Power = power;
  if (Power > 1.0) Power = 1.0;
  if (Power < -1.0) Power = -1.0;
  Steering = steering;
  if (steering > 1.0) Steering = 1.0;
  if (Steering < -1.0) Steering = -1.0;
	
  transmitLock.unlock();
}

long int DrifterCar::getLastMessageTime( )
{
  return LastMessageTime;
}
double DrifterCar::getSpeed( )
{
  return Speed * ENCODER_RESOLUTION;
}
double DrifterCar::getDistanceTraveled()
{
  return Distance * ENCODER_RESOLUTION;
}
double DrifterCar::getXAccel( )
{
  return Xaccel / ACCEL_SENSITIVITY;  //range is +-4g over 16bit int
}
double DrifterCar::getYAccel( )
{
  return Yaccel / ACCEL_SENSITIVITY;  //range is +-4g over 16bit int
}
double DrifterCar::getRotSpeed( )
{
  return RotSpeed / GYRO_SENSITIVITY;	//range is +- 1000° over 16bit int;
}
//
bool DrifterCar::update( )
{
  serialLock.lock();
  if (Parser.parse( ))	//try to parse new packet
    {	//if succed then there is new packet availible
			
      // you must read in right order see ARDUINO software on the other side
      Parser.readToken( &LastMessageTime );	//time stamp
      Parser.readToken( &Xaccel );	//acceleration x
      Parser.readToken( &Yaccel );	//acceleration y
      Parser.readToken( &RotSpeed );	//rotation speed z
      Parser.readToken( &Distance );	//traveled ticks;
      Parser.readToken( &Speed );	//speed
      serialLock.unlock( );
		
      return true;
    }
  serialLock.unlock( );
  return false;
}

void DrifterCar::transmitLoop( )
{
	
  while (1)
    {

      if (Serial.isOpen())
	{
	  cout << "trasmit" << endl;
			
	  //lock so it cannot be rewriten when sending
	  transmitLock.lock();
	  //TODO: sent meessage if it is older than something in order to avoid congestion
	  //send new state
	  //send steering value as one byte signed value
	  OStream.newMessage();
	  OStream.write( (int8_t)(Steering * 127) );
	  //send power as two byte value in range <-255,255>
	  OStream.write( (int16_t)(Power * 255) );
	  transmitLock.unlock();

	  serialLock.lock();
	  OStream.flush();
	  serialLock.unlock();
			

	}
      else
	{

	  std::terminate();
	  cout << "not connected  transmitting terminated" << endl;
	}
      std::this_thread::sleep_for( std::chrono::milliseconds( 30 ) );
    }
}
