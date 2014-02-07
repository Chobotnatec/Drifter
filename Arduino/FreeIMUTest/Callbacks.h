/*
 * Callbacks.h
 *
 *  Created on: 17.12.2013
 *      Author: Martin Skalský
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

class Object;

class Callback
{
public:
	virtual void onCall(Object*);
	virtual ~Callback() = 0;
};

class Caller
{
public:
		//returns pointer to old callback if there was any
	virtual Callback* attachCallback();
		//return pointer to old callback if there was any
	virtual Callback* disattachCallback();
		//
	virtual void call();
};

#endif /* CALLBACKS_H_ */
