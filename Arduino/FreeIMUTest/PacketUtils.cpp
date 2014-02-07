
#include "PacketUtils.h"



PacketStream::PacketStream(SerialPort* str):
		outStream(str),
		lenght(0),
		pointer(array)
	{};

void PacketStream::newMessage()
{
	lenght = 0;
	pointer = array;
}
void PacketStream::flush()
{
	//write begining symbols
	outStream->write(0xAA);
	outStream->write(0xAA);
	//write lenght of the packet
	outStream->write(lenght);
	unsigned char checksum = 0;
	for(unsigned char i = 0; i< lenght; i++)
	{
		outStream->write(array[i]);
		checksum += array[i];
	}
	outStream->write(checksum);

	lenght = 0;
	pointer = array;
}



Parser::Parser(SerialPort* str):
		stream(str),
		newMessageFlag(false),//if there is new unreaded message
		wrongDataFlag(false),	//if there was some unstructured data
		state(ParsingState::SEARCH),	//state of the parsing
	    checksum(0),	//checksum
        readed(0)
	{

	}


bool Parser::parse()
{
	
	//parse new chars until there is something to parse

	//On arduino it reads until there is something availible at the serialStream 
	//on PC it reads until time out hits and exception is thrown
	#ifndef PC_APP
	while(stream->available() > 0)
	#else
	try{
		while(1)
	#endif
	{
		
		switch(state)
		{
			//search for beggining chars
		case ParsingState::SEARCH: 
			{
			unsigned char ch = stream->read();
			//std::cout<<(int)ch<<std::endl;
			if(ch == 0xAA)
			{
				state = ParsingState::SEARCH2;		
			}
				//if there is wrong character on input buffer
			else
			{
				
				wrongDataFlag = true;
			}
			}
		break;
		case ParsingState::SEARCH2:
			if(stream->read() == 0xAA)
			{

				state = ParsingState::SIZE;		
				
			}
				//if there is wrong character on input buffer
			else
			{
				wrongDataFlag = true;
				state = ParsingState::SEARCH;
			}
		break;
		//read the size of message
		case ParsingState::SIZE:
		
			//read size of packet
			messageLenght = stream->read();
			if(messageLenght > MAX_PACKET_LENGHT)
			{
				state = ParsingState::SEARCH;
				return false;
			}
			checksum = 0;
			readed = 0;
			state = ParsingState::DATA;
		break;
		//read data
		case ParsingState::DATA:
			//reads charfrom stream
			message[readed]= stream->read();
			//compute new checksum
			checksum += message[readed];
			readed++ ;
			//if all character has been readed;
			if(readed >= messageLenght) state = ParsingState::CHECKSUM;
		break;
		//check check sum
		case ParsingState::CHECKSUM:
		
			//next state doesn't depends on  whether checksum agrees or not
			state = ParsingState::SEARCH;
			//check whether is right check sum present
			if (checksum == stream->read())
			{
				//if checksum is right stop parsing and return true
				this->newMessageFlag = true;
				//reset index to message
				messageIndex = 0;
				return true;
				
			}
			else
			{
				wrongDataFlag = true;
			}
		break;
		}
	}
			//only at PC catch timeout_exception and just return fals
	#ifdef PC_APP
	}
		catch(timeout_exception e)
		{}
	#endif
	return false;
}

bool Parser::getNewMessageFlag()
{
	return newMessageFlag;
}

bool Parser::getWrongDataFlag()
{
	if(wrongDataFlag == true)
	{
		newMessageFlag = false;
		return true;
	}
	return false;
}

unsigned char Parser::getMessageLenght()
{
	return messageLenght;
}

void Parser::getMessage(unsigned char* buffer)
{
	for(unsigned char i = 0; i < messageLenght; i++)
	{
		buffer[i] = message[i];
	}
	newMessageFlag = false;
}

	//class will read one token from buffer of given type

