/*
 * PacketUtils.h
 *
 *  Created on: 26.1.2014
 *      Author: Martin Skalský
 */

	//THIS IS DEFINED ONLY IN PC IN ORDER TO WORK IT WITH ASIO AND TIMEOUTSERIAL
	//IT SHOULD BE COMMENTED for use with arduino
#define PC_APP

#ifndef PACKETUTILS_H_
#define PACKETUTILS_H_


#ifdef PC_APP
	#include "TimeoutSerial.h"
	typedef TimeoutSerial SerialPort;
#else
	#include "Arduino.h"
	typedef Stream SerialPort;
#endif





// class makes simple packets from provided dada
class PacketStream
{
#define MAX_PACKET_LENGHT 30

private:
	SerialPort* outStream;
	unsigned char array[MAX_PACKET_LENGHT];
	unsigned char lenght;
	unsigned char* pointer;
	static const unsigned char arraySize = MAX_PACKET_LENGHT;
	unsigned char checksum;


public:
		/*
		 *\construct Packet stream from provided stream
		 */
	PacketStream(SerialPort* str);

	/*
	 * resets message
	 */
	void newMessage();
	/*
	 * \ends and sends packet to stream
	 */
	void flush();
	/*
	 * \shity pity serialization for hopefully all simple integer types.
	 * It will not work for any other type!!!.
	 *
	 * @param character
	 * @return free space in packet in unsigned chars or 0 if last entry could not fit in
	 */
	template<class T> unsigned char write(T character);
};

namespace ParsingState{
enum ParsingState{SEARCH = 1,SEARCH2 = 2,SIZE = 3,DATA = 4,CHECKSUM = 5};
}

class Parser
{
private:
	SerialPort* stream;	//stream from which to parse;
	unsigned char message[30];	//newest message
	unsigned char messageLenght;	// lenght of newest message
	unsigned char messageIndex;
	bool newMessageFlag;	//if there is new unreaded message
	bool wrongDataFlag;	//if there was some unstructured data

	ParsingState::ParsingState state;	//state of the parsing
	unsigned char checksum;	//checksum
	unsigned char readed;

public:

	/*
	 * \crate parser on given stream
	 */
	Parser(SerialPort* str);

	/*
	 *\try to parse new message, Unvalidates old message!!!
	 *@return yes/on dependend on whether it founded message
	 */
	bool parse();	//return whether there was new packet

	bool getNewMessageFlag(); // reads and resets newMessageFlag
	bool getWrongDataFlag(); // reports whether there were unstructured data on line

	unsigned char getMessageLenght();
	void getMessage(unsigned char* buffer); // it will copy newest message into place which you provided
		
		/*
		 * shity deserialization for hopefull all integral types
		 */
	template <class T> void readToken(T* type);

};

template<class T> unsigned char PacketStream::write(T character)
	{
		if(sizeof character + lenght > arraySize ) return 0;
		else
		{
			for(char i = sizeof character; i > 0 ; i--)
			{

			//cast pointer to right type and place input
			*pointer = character >> 8 * (i - 1);
			//move pointer
			pointer++;
			//up date size


			}
			lenght += sizeof character;
			return arraySize - lenght;
		}
	}


template <class T> void Parser::readToken(T* type)
{
	*type = 0;
	if(messageIndex + sizeof(T) <= messageLenght)
	{
		for(int i = sizeof(T); i > 0; i--,messageIndex++)
		{


		*type += message[messageIndex] << (i-1) * 8;
		}

	}

}



#endif /* PACKETUTILS_H_ */
