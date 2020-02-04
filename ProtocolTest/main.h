/*
 * main.h
 *
 * Created: 1/31/2020 6:26:04 PM
 *  Author: andrew.shkolik
 */ 


#ifndef MAIN_H_
#define MAIN_H_
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include "Serial.h"

#define DBG_UART_USB

#define TIMER_10MS_VECT         TIMER0_COMP_vect //10ms timer
#define TIMER_10MS_COMPVAL      OCR0

#define RF_TIMER_COMPA_REG   OCR1A
#define RF_TIMER_COMPA_VECT  TIMER1_COMPA_vect

#define MAX_MIXER_DELTA_US   (50000)	// 50ms max as an interval between 2 mixer calculations
#define PULSES_SETUP_TIME_US (500)		// 500usec

#define PULSES_WORD_SIZE  35			// A 16 Channel PPM frame has 34 timing events + 1 int terminator
#define PULSES_BYTE_SIZE  (PULSES_WORD_SIZE * 2)

#define PPM_CENTER 1500

//for 16MHz we need multiply some timings by 2
#define TIMER_MULTIPLIER (F_CPU == 8000000L ? 1 : 2)
//for 8MHz we need divide some values by 2 (using bit-shift)
#define TIMER_DEMULTIPLIER (F_CPU == 8000000L ? 1 : 0)

#define OUT_PORT	PORTD
#define OUT_PIN		0x01

#define IN_PORT	PORTF
#define IN_PIN		0x02

#define LED_PORT	PORTE
#define LED_PIN		0x10

#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

#define FORCEINLINE inline __attribute__ ((always_inline))
template<class t> FORCEINLINE t min(t a, t b){ return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b){ return a>b?a:b; }
template<class t> FORCEINLINE t limit(t mi, t x, t ma){	return min(max(mi,x),ma); }

enum Protocols {
	PROTOCOL_PPM,
	PROTOCOL_DSM_SERIAL,
	PROTOCOL_MULTI,
	PROTOCOL_COUNT
};

enum ProtoCmds {
	PROTOCMD_INIT,
	PROTOCMD_BIND,
	PROTOCMD_RESET,
	PROTOCMD_GETOPTIONS
};

enum ppmtype{
	PPM,
	PPMSIM,
	PPM16LAST,
	PPM16FIRST,
};

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum Protocols Protocol;
	CMDS Cmds; // Cmds
} __attribute__((__packed__)) Proto_struct;


#define PULSEPOL 0
#define PPMCHMAX 6

extern uint8_t s_current_protocol;
extern uint16_t *RptrA; // For OCR1A

extern uint16_t *pulses1MHzWPtr;
extern uint16_t nextMixerEndTime;
extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t dt;

extern void bink(uint8_t count);
extern void PROTO_Start_Callback( uint16_t (*cb)());

extern void setupPulsesPPM(enum ppmtype proto);
extern const void * PROTO_PPM_Cmds(enum ProtoCmds);
extern const void * (*PROTO_Cmds)(enum ProtoCmds);  //protocol callback
extern uint16_t (*timer_callback)(void);			// Function pointer to add flexibility and simplicity to ISR.

#endif /* MAIN_H_ */