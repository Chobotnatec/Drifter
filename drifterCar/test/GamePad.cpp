// GamePad.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <SFML/Window/Joystick.hpp>

//time out Serial for writing and reading from serial line
#include "../src/TimeoutSerial.h"
//for parsing and packeting job
#include "../src/PacketUtils.h"
#include "../src/DrifterCar.h" 

using namespace std;
using namespace sf;
using namespace boost;


int main()
{
  cout << "START" << endl;
  SerialPort serial;
  DrifterCar drifter;
  bool succes = false;
	
  if (drifter.connect( "/dev/rfcomm0", 9600 )) {}
  else if (drifter.connect( "COM4", 115200 )) {}
  else if (drifter.connect( "COM13", 115200 )) {}
  else
    {
      cout << "NO SERIAL FOUND" << endl;
      while (1);
    }
	

  double power = 0.0;
  double angle = 0.0;
		
  while(1)
    {
      Joystick::update();
      power = Joystick::getAxisPosition( 0, Joystick::Z ) /100.0;
      angle = Joystick::getAxisPosition( 0, Joystick::X ) /100.0;
      // int r = Joystick::getAxisPosition( 0, Joystick::R );
			
      drifter.setControlValues( power, angle );
			
      while (drifter.update( ))	//update state values
	{	
				
	}
      //if succed then show new data
      cout << "p:" << power << endl
	   << "a:" << angle << endl <<endl;;

      cout << "tStamp:" << drifter.getLastMessageTime( )
	   << endl << "ax: " << drifter.getXAccel( ) //range is +-4g over 16bit int
	   << endl << "ay: " << drifter.getYAccel( )
	   << endl << "gz: " << drifter.getRotSpeed( )	//range is +- 1000° over 16bit int
	   << endl << "v: " << drifter.getSpeed( ) //speed in  mm/s
	   << endl << "s: " << drifter.getDistanceTraveled( ) << endl<<endl;

      this_thread::sleep_for( std::chrono::milliseconds( 30 ));
			
    }

  return 0;
}
	


