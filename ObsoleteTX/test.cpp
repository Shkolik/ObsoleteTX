/*
 * test.cpp
 *
 * Created: 1/26/2020 6:34:20 PM
 *  Author: Andrew
 */ 
#include "ObsoleteTX.h"

void doWork(uint16_t val)
{
	_delay_ms(val/2);
	PORTE |= PIN4_bm;
	_delay_ms(val/2);
	PORTE &= ~PIN4_bm;
}