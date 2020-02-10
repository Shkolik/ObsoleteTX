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

#define PULSES_WORD_SIZE	19			// A 8 Channel PPM frame has 18 timing events + 1 int terminator
										// So, 19 should be enough, but will I use the same buffer for Multiprotocol module
										// and it has 26 bytes frame, that is 13 WORDs - still will work

#define PPM_CENTER			1500
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]

//for 16MHz we need multiply some timings by 2
#define TIMER_MULTIPLIER (F_CPU == 8000000L ? 1 : 2)
//for 8MHz we need divide some values by 2 (using bit-shift)
#define TIMER_DEMULTIPLIER (F_CPU == 8000000L ? 1 : 0)



//////////////////////////////////////////////////////////////////////////
//MULTI
//////////////////////////////////////////////////////////////////////////
#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)

#define MULTI_CHANS							16
#define MULTI_CHAN_BITS						11

#define MULTIRFPROTOCOL rfOptionValue1
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3

#define NO_SUBTYPE							0
#define MM_RF_CUSTOM_SELECTED				0xff

#define MULTI_RF_PROTO_LAST 100  // Use a large value

enum PROTO_MODE {
	NORMAL_MODE,
	BIND_MODE,
};

enum MultiModuleRFProtocols {
	MM_RF_PROTO_CUSTOM = -1,
	MM_RF_PROTO_FIRST = MM_RF_PROTO_CUSTOM,
	MM_RF_PROTO_FLYSKY=0,
	MM_RF_PROTO_HUBSAN,
	MM_RF_PROTO_FRSKY,
	MM_RF_PROTO_HISKY,
	MM_RF_PROTO_V2X2,
	MM_RF_PROTO_DSM2,
	MM_RF_PROTO_DEVO,
	MM_RF_PROTO_YD717,
	MM_RF_PROTO_KN,
	MM_RF_PROTO_SYMAX,
	MM_RF_PROTO_SLT,
	MM_RF_PROTO_CX10,
	MM_RF_PROTO_CG023,
	MM_RF_PROTO_BAYANG,
	MM_RF_PROTO_ESky,
	MM_RF_PROTO_MT99XX,
	MM_RF_PROTO_MJXQ,
	MM_RF_PROTO_SHENQI,
	MM_RF_PROTO_FY326,
	MM_RF_PROTO_SFHSS,
	MM_RF_PROTO_J6PRO,
	MM_RF_PROTO_FQ777,
	MM_RF_PROTO_ASSAN,
	MM_RF_PROTO_HONTAI,
	MM_RF_PROTO_OLRS,
	MM_RF_PROTO_FS_AFHDS2A,
	MM_RF_PROTO_Q2X2,
	MM_RF_PROTO_WK_2X01,
	MM_RF_PROTO_Q303,
	MM_RF_PROTO_GW008,
	MM_RF_PROTO_DM002,
	MM_RF_PROTO_LAST= MM_RF_PROTO_DM002
};

enum MMDSM2Subtypes {
	MM_RF_DSM2_SUBTYPE_DSM2_22,
	MM_RF_DSM2_SUBTYPE_DSM2_11,
	MM_RF_DSM2_SUBTYPE_DSMX_22,
	MM_RF_DSM2_SUBTYPE_DSMX_11,
	MM_RF_DSM2_SUBTYPE_AUTO
};

enum MMRFrskySubtypes {
	MM_RF_FRSKY_SUBTYPE_D16,
	MM_RF_FRSKY_SUBTYPE_D8,
	MM_RF_FRSKY_SUBTYPE_D16_8CH,
	MM_RF_FRSKY_SUBTYPE_V8,
	MM_RF_FRSKY_SUBTYPE_D16_LBT,
	MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
};

//////////////////////////////////////////////////////////////////////////

typedef struct {
	//Rf data
	uint8_t   modelId:6;         //64 max
	uint8_t   rfProtocol:6;      //64 max
	uint8_t   rfSubType:4;       //16 max
	int8_t    rfOptionValue1;
	int8_t    rfOptionValue2;
	uint8_t   rfOptionValue3:5;  //32 max
	uint8_t   rfOptionBool1:1;
	uint8_t   rfOptionBool2:1;
	uint8_t   rfOptionBool3:1;
	//end of RF data	
} __attribute__((__packed__)) ModelData;

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
template<class t> FORCEINLINE t min(t a, t b){ return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b){ return a>b?a:b; }
template<class t> FORCEINLINE t limit(t mi, t x, t ma){	return min(max(mi,x),ma); }

enum Protocols {
	PROTOCOL_PPM,
	PROTOCOL_DSM,
	PROTOCOL_MULTI,
	PROTOCOL_COUNT
};

enum ProtoCmds {
	PROTOCMD_INIT,
	PROTOCMD_BIND,
	PROTOCMD_RESET,
	PROTOCMD_GETOPTIONS
};

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum Protocols Protocol;
	CMDS Cmds; // Cmds
} __attribute__((__packed__)) Proto_struct;


#define PULSEPOL 0
#define PPMCHMAX 6

extern uint8_t heartbeat;

extern uint8_t s_current_protocol;

extern int16_t channelOutputs[PPMCHMAX];
extern uint16_t pulses1MHz[PULSES_WORD_SIZE];
extern uint16_t *pulses1MHzWPtr;
extern uint16_t nextMixerEndTime;
extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t dt;
extern uint8_t protoMode;
extern bool rangeModeIsOn;

extern uint16_t bind_tmr10ms;

extern uint16_t g_tmr10ms;
//extern void blinkLed(uint8_t count);
extern void setPinState(uint8_t state);
extern void togglePin();
extern void PROTO_Stop_Callback();
extern void PROTO_Start_Callback( uint16_t (*cb)());
extern void PROTO_SetBindState(uint16_t t10ms);

extern void setupPulsesPPM();

extern const void *DSM_Cmds(enum ProtoCmds);
extern const void *PPM_Cmds(enum ProtoCmds);
extern const void *MULTI_Cmds(enum ProtoCmds cmd);
extern const void * (*PROTO_Cmds)(enum ProtoCmds);  //protocol callback
extern uint16_t (*timer_callback)(void);			// Function pointer to add flexibility and simplicity to ISR.

extern void getADC();

extern const ModelData g_model;

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2 // when multiple modules this is the first one
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)

//  Dimension of Arrays
#define DIM(array) ((sizeof array) / (sizeof *array))
#define memclear(p, s) memset(p, 0, s)


#endif /* MAIN_H_ */