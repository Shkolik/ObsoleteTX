/*
 * Blinky.cpp
 *
 * Created: 2/3/2020 5:55:16 PM
 * Author : andrew.shkolik
 */ 

#define F_CPU 8000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define IS_PIN_LOW(reg, pin) (reg & (1 << pin))
#define IS_PIN_HI(reg, pin) !IS_PIN_LOW(reg, pin)

int main(void)
{
	//////////////////////////////////////////////////////////////////////////
	/// setup registers
	//////////////////////////////////////////////////////////////////////////
	//Led				//TX0				//RX0
	DDRE |= (1 << 4);	DDRE |= (1 << 1);	DDRE  &= ~(1 << 0);
	PORTE|= (1 << 4);	PORTE|= (1 << 1);	PORTE &= ~(1 << 0);
	
	//PPM OUT			//BIND				//TX1				//RX1
	DDRD |= (1 << 0);	DDRD &= ~(1 << 1);	DDRD |= (1 << 3);	DDRD  &= ~(1 << 2);
	PORTD|= (1 << 0);	PORTD |= (1 << 1);	PORTD|= (1 << 3);	PORTD &= ~(1 << 2);
	
	DDRF = 0b00000000;  PORTF = 0b11111111; // All analog inputs PullUp
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// setup ADC
	//////////////////////////////////////////////////////////////////////////
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN)| (1 << ADPS2); // ADC enabled, prescaler division=16 (no interrupt, no auto-triggering)
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// setup timers
	//////////////////////////////////////////////////////////////////////////
	//Timer generates event every 10ms. Starts here and never stops
	OCR0	= 78;			// count to 10ms
	TCCR0	= (7 << CS00);				// Norm mode, clk/1024 (1 << CS00 | 1 << CS01 | 1 << CS02)
	TIMSK	= (1<<OCIE0) | (1<<TOIE0);	// COMP - 78 (100Hz), OVF - 255 (30.64Hz)
	//////////////////////////////////////////////////////////////////////////

	//Timer counts with frequency 1MHz
	TCCR1A	= 0;						// Clear timer registers
	TCCR1B	= 0;						// Normal mode
	TCCR1C	= 0;
	TCNT1	= 0;						// starts from 0
	TCCR1B	|= 1<<CS11;					// F_TIMER1 = 1MHz (F_CPU/8), 1 tick per 1 microsecond
	OCR1A	= 20000;		// Next frame starts in 20 mS
	//Interrupt will be enabled by protocol callback
	//When started
	//TIMSK	|= (1<<OCIE1A);				// Enable output compare
	//////////////////////////////////////////////////////////////////////////

	
	sei();	// Enable global interrupts
	
    /* Replace with your application code */
    while (1) 
    {
		if(IS_PIN_LOW(PIND, 1))
		{
			//digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
			PORTE |= ( 1<< 4);
		}
		else
		{
			//digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
			PORTE &= ~( 1<< 4);
		}
    }
}

