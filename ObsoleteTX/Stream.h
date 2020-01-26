/*
 * Stream.h
 *
 * Created: 1/25/2020 1:09:38 PM
 *  Author: Andrew
 */ 


#ifndef STREAM_H_
#define STREAM_H_

#include "Print.h"

class Stream : public Print
{
	public:
	virtual uint8_t available() = 0;
	virtual uint8_t read() = 0;
};

#endif /* STREAM_H_ */