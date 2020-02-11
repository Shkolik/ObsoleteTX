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
#include "pgmtypes.h"


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


#include "usart_driver.h"
#include "keys.h"

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

enum Protocols {
	PROTOCOL_PPM,
	#ifdef DSM
	PROTOCOL_DSM,
	#endif
	#ifdef MULTI
	PROTOCOL_MULTI,
	#endif
	PROTOCOL_COUNT
};

#include "protocol/ppm.h"

#ifdef MULTI
#include "protocol/multi.h"
#endif
#ifdef DSM
#include "protocol/dsm.h"
#endif


//////////////////////////////////////////////////////////////////////////

typedef struct {
	//Rf data
	uint8_t   modelId:6;			//64 max
	uint8_t   rfProtocol:6;			//64 max
	uint8_t   rfSubType:4;			//16 max
	int8_t    rfOptionValue1;
	int8_t    rfOptionValue2;
	uint8_t   rfOptionValue3:5;		//32 max
	uint8_t   rfOptionBool1:1;
	uint8_t   rfOptionBool2:1;
	uint8_t   rfOptionBool3:1;
	//end of RF data	
	
	//TimerData timers[MAX_TIMERS];
	//uint8_t   thrTrim:1;            // Enable Throttle Trim
	//int8_t    trimInc:3;            // Trim Increments
	//uint8_t   disableThrottleWarning:1;
	uint8_t   extendedLimits:1;
	//uint8_t   extendedTrims:1;	
	//BeepANACenter beepANACenter;
	//MixData   mixData[MAX_MIXERS];
	//LimitData limitData[NUM_CHNOUT];
	//ExpoData  expoData[MAX_EXPOS];
	//CURVDATA  curves[MAX_CURVES];
	//int8_t    points[NUM_POINTS];
	//LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
	//CustomFunctionData customFn[NUM_CFN];
	//FlightModeData flightModeData[MAX_FLIGHT_MODES];
	//MODEL_GVARS_NAME // gvars name
	//uint8_t thrTraceSrc:5;
	//uint8_t thrSwitch:3;
	//swarnstate_t  switchWarningState;
	//swarnenable_t switchWarningEnable;
	//SwashRingData swashR;          // Heli data
	//uint8_t UnusedModel; // Use later .. todo
	//FrSkyData telemetry;
	
} __attribute__((__packed__)) ModelData;

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))

template<class t> FORCEINLINE t min(t a, t b){ return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b){ return a>b?a:b; }
template<class t> FORCEINLINE t limit(t mi, t x, t ma){	return min(max(mi,x),ma); }

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum Protocols Protocol;
	CMDS Cmds; // Cmds
} __attribute__((__packed__)) Proto_struct;



extern uint8_t heartbeat;

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

extern /*const*/  ModelData g_model;

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)

//  Dimension of Arrays
#define DIM(array) ((sizeof array) / (sizeof *array))
#define memclear(p, s) memset(p, 0, s)


#endif /* MAIN_H_ */