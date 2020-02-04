/*
 * Steram.h
 *
 * Created: 2/1/2020 9:09:06 PM
 *  Author: andrew.shkolik
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