/*
 * Blinky.cpp
 *
 * Created: 2/3/2020 5:55:16 PM
 * Author : andrew.shkolik
 */ 

#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>



int main(void)
{
	DDRE = 0b11111111; PORTE = 0b11111111;
    /* Replace with your application code */
    while (1) 
    {
		PORTE |= 0x10;
		_delay_ms(100);
		PORTE &= ~0x10;
		_delay_ms(100);
    }
}

