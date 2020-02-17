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

#ifndef MAIN_H_
#define MAIN_H_

#include "board/test.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/eeprom.h>
#include "pgmtypes.h"

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))



#define CHMAX 6

//in case we need debug output
#ifdef USART_DBG
#include "DBG/Serial.h"
#define debugln(ln) Serial0.println(ln)
#define debug(ln) Serial0.print(ln)
#define init_debug(baud) Serial0.init(baud)
#else
#define debugln(ln) 
#define debug(ln) 
#define init_debug(baud) 
#endif

#define TRACE(ln, arg) debug(ln); debugln(arg)

#include "usart_driver.h"
#include "keys.h"
#include "data/data.h"
#include "data/eeprom_common.h"
#include "data/eepromFS.h"


#ifndef PIN0_bm
#define PIN0_bm  0x01
#endif
#ifndef PIN1_bm
#define PIN1_bm  0x02
#endif
#ifndef PIN2_bm
#define PIN2_bm  0x04
#endif
#ifndef PIN3_bm
#define PIN3_bm  0x08
#endif
#ifndef PIN4_bm
#define PIN4_bm  0x10
#endif
#ifndef PIN5_bm
#define PIN5_bm  0x20
#endif
#ifndef PIN6_bm
#define PIN6_bm  0x40
#endif
#ifndef PIN7_bm
#define PIN7_bm  0x80
#endif

#define IS_PIN_HI(reg, pin) ((reg & (1 << pin)) > 0)
#define IS_PIN_LOW(reg, pin) !IS_PIN_HI(reg, pin)



#define MAX_MIXER_DELTA_US	(50000)	// 50ms max as an interval between 2 mixer calculations

#define PULSES_SIZE			19			// A 8 Channel PPM frame has 18 timing events + 1 int terminator
										// So, 19 should be enough, but will I use the same buffer for Multiprotocol module
										// and it has 26 bytes frame, that is 13 WORDs - still will work

#define PPM_CENTER			1500
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]

//for 16MHz we need multiply some timings by 2
#define TIMER_MULTIPLIER (F_CPU == 8000000L ? 1 : 2)
//for 8MHz we need divide some values by 2 (using bit-shift)
#define TIMER_DEMULTIPLIER (F_CPU == 8000000L ? 1 : 0)


//////////////////////////////////////////////////////////////////////////
//protocols
//////////////////////////////////////////////////////////////////////////


enum PROTO_MODE {
	NORMAL_MODE,
	BIND_MODE,
	RANGE_CHECK_MODE
};

enum ProtoCmds {
	PROTOCMD_INIT,
	PROTOCMD_BIND,
	PROTOCMD_RESET,
	PROTOCMD_GETOPTIONS
};



#include "protocol/ppm.h"

#ifdef MULTI
#include "protocol/multi.h"
#endif
#ifdef DSM
#include "protocol/dsm.h"
#endif


//////////////////////////////////////////////////////////////////////////





template<class t> FORCEINLINE t min(t a, t b){ return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b){ return a>b?a:b; }
template<class t> FORCEINLINE t limit(t mi, t x, t ma){	return min(max(mi,x),ma); }

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum ModuleType moduleType;
	CMDS Cmds; // Cmds
} __attribute__((__packed__)) Module_struct;

extern uint8_t heartbeat;
extern uint8_t stickMode;

extern uint8_t s_current_protocol;



extern int16_t channelOutputs[CHMAX];

extern uint16_t nextMixerEndTime;
extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t dt;
extern uint8_t protoMode;

extern uint16_t bind_tmr10ms;

extern uint16_t g_tmr10ms;

#define TOGGLE(port, pin) (port ^= pin)
#define SETPIN(port, pin, state) (state ? (port |= pin) : (port &= ~pin))

extern void PROTO_Stop_Callback();
extern void PROTO_Start_Callback( uint16_t (*cb)());
extern void PROTO_SetBindState(uint16_t t10ms);

extern const void * (*PROTO_Cmds)(enum ProtoCmds);  //protocol callback
extern uint16_t (*timer_callback)(void);			// Function pointer to add flexibility and simplicity to ISR.

FORCEINLINE uint8_t pulsesStarted()
{
	return (s_current_protocol != 255);
}

FORCEINLINE void sendStopPulses()
{
	PROTO_Cmds(PROTOCMD_RESET);
	PROTO_Stop_Callback();
}

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)

//  Dimension of Arrays
#define DIM(array) ((sizeof array) / (sizeof *array))
#define memclear(p, s) memset(p, 0, s)

extern void modelDefault(uint8_t id);
extern void generalDefault();

#endif /* MAIN_H_ */