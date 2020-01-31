/*
 * ProtocolTest.cpp
 *
 * Created: 1/31/2020 1:07:25 PM
 * Author : andrew.shkolik
 */ 

#include <avr/io.h>

volatile bool send = true;

#define F_CPU 8000000UL

int main(void)
{	
	// setup registers
	DDRD = 0b11111101;  PORTD = 0b00000010; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:PPM_IN, 3:BT_TX, 2:BT_RX, 1:BIND, 0:PPM_OUT
	DDRE = 0b00010010;  PORTE = 0b11111110; // 7:Flaps, 6:Rot_Left, 5:Rot_Right, 4:Rot_Enter, 3:Trainer_ON, 2:Power_ON, 1:TELEM_TX, 0:TELEM_RX
	//setup timers
	
	TCCR0  = (7 << CS00);								// Norm mode, clk/1024 (1 << CS00 | 1 << CS01 | 1 << CS02
	OCR0   = 78;										// count to 10ms
	TIMSK = (1<<OCIE0) | (1<<TOIE0);					// COMP - 78 (100Hz), OVF - 255 (30.64Hz)
	
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << WGM13);				// CTC
	TCCR1B |= (1 << CS11);								// 1MHz
	OCR1A = 20000u;										// Next frame starts in 20 mS
	//setup UARTS
	
	//start timers
	//TIMSK |= (1<<OCIE1A);
	
    /* Replace with your application code */
    while (1) 
    {
		if(send)
		{
			
		}
		else
		{
			
		}
    }
}

