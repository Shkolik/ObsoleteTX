/*
 * pulses_avr.h
 *
 * Created: 1/25/2020 9:06:47 PM
 *  Author: Andrew
 */ 


#ifndef PULSES_AVR_H_
#define PULSES_AVR_H_

#include "../ObsoleteTX.h"
#include "../protocol/misc.h"
//#include "../spi.h"


static volatile uint16_t timer_counts;


extern uint8_t s_current_protocol;
extern uint16_t *RptrA; // For OCR1A
extern uint16_t *RptrB; // For OCR1B

extern uint8_t *pulses2MHzWPtr;

extern uint16_t nextMixerEndTime;

#define MAX_MIXER_DELTA_US   (50000) /* 50ms max as an interval between 2 mixer calculations */

#define PULSES_SETUP_TIME_US (500) // 500usec

extern void (*ocr1b_function_ptr)(); // Function pointer to add flexibility and simplicity to ISR.

extern uint16_t dt;

enum ppmtype{
	PPM,
	PPMSIM,
	PPM16LAST,
	PPM16FIRST,
};

extern void setupPulses();
extern void setupPulsesPPM(enum ppmtype proto);
void DSM2_Init();
void DSM2_Done();

extern uint8_t pulsesStarted();
extern void sendStopPulses();

#endif /* PULSES_AVR_H_ */