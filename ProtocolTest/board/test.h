/*
 * Copyright (C) ObsoleteTX
 *
 * Based on code named
 *   th9x - https://github.com/thus1/th9x
 *   er9x - https://github.com/MikeBland/mbtx
 *   OpenTx - https://github.com/opentx/opentx
 *   OpenAVRc - https://github.com/Ingwie/OpenAVRc_Dev
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef TEST_H_
#define TEST_H_

#define F_CPU 8000000UL

#define MULTI
#define DSM

//invert throttle for test pp only
#define INV_STICK_RH
#define NUM_STICKS		4
#define NUM_POTS		0
#define LEN_MODEL_NAME	10
#define MAX_MODELS		15
#define DEFAULT_MODE	1
#include "../main.h"

#define UNEXPECTED_SHUTDOWN()   (mcusr & _BV(WDRF))

#define CALCULATE_LAT_JIT()  dt = TCNT1 - OCR1A		// Calculate latency and jitter.

#define TIMER_10MS_VECT		TIMER0_COMP_vect //10ms timer
#define TIMER_10MS_COMPVAL	OCR0

#define RF_TIMER_COMPA_REG  OCR1A
#define RF_TIMER_COMPA_VECT TIMER1_COMPA_vect

#define OUT_PORT	PORTD
#define OUT_PIN		PIN0_bm

#define IN_PORT		PORTF
#define IN_PIN		PIN1_bm

#define LED_PORT	PORTE
#define LED_PIN		PIN4_bm

#define BIND_PRESSED	IS_PIN_LOW(PING, 3)
#define CHANGE_PRESSED	IS_PIN_LOW(PING, 4)

#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

//USART driver
#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

extern void getADC();
extern void boardInit();

#endif /* TEST_H_ */