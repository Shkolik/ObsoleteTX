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
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/eeprom.h>
#include "pgmtypes.h"

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
// Fiddle to force compiler to use a pointer
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))

//  Dimension of Arrays
#define DIM(array) ((sizeof array) / (sizeof *array))
#define memclear(p, s) memset(p, 0, s)

#define IS_IN_RANGE(value,lowest,highest) (!(value<(lowest)) && (value<((highest)+1)))


#define CHMAX 6
#define MAX_ALERT_TIME   60

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
#include "mixer.h"
#include "maths.h"
#include "trainer.h"
#include "buzzer.h"
#include "data/data.h"
#include "data/eeprom_common.h"
#include "data/eepromFS.h"
#include "timers.h"
#include "switches.h"
#include "functions.h"
#include "curves.h"

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

// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l


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

#ifdef GUI
#define POPUP(x) displayPopup(x);
#define MENU_MODEL_SETUP menuHandlers[menuLevel] == menuModelSetup
#else
#define MENU_MODEL_SETUP 0
#define POPUP(...)
#define DISPLAY_WARNING(...)
#define POPUP_WARNING(...)
#define POPUP_CONFIRMATION(...)
#define POPUP_INPUT(...)
#define WARNING_INFO_FLAGS           0
#define SET_WARNING_INFO(...)
#endif


enum Analogs {
	STICK_RH,
	STICK_LV,
	STICK_RV,
	STICK_LH,
	POT1,
	POT2,
	POT3,
	POT_LAST = POT3,
	TX_VOLTAGE,
	NUMBER_ANALOG
};

enum Functions {
	// first the functions which need a checkbox
	FUNC_OVERRIDE_CHANNEL,
	FUNC_TRAINER,
	FUNC_INSTANT_TRIM,
	FUNC_RESET,
	FUNC_ADJUST_GVAR,
	// then the other functions
	FUNC_FIRST_WITHOUT_ENABLE,
	FUNC_PLAY_SOUND = FUNC_FIRST_WITHOUT_ENABLE,
	FUNC_PLAY_TRACK,
	FUNC_PLAY_BOTH,
	FUNC_PLAY_VALUE,
	FUNC_VARIO,
	FUNC_HAPTIC,
	FUNC_LOGS,
	FUNC_BACKLIGHT,
	FUNC_MAX
};
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

extern uint16_t nextMixerEndTime;
extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t dt;
extern uint8_t protoMode;

extern uint16_t bind_tmr10ms;

extern uint16_t g_tmr10ms;

extern bool s_mixer_first_run_done;

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

extern uint16_t g_vbat10mV;
#define GET_TXBATT_BARS() (limit<uint8_t>(2, 20 * ((uint8_t)(g_vbat10mV/10) - g_general.vBatMin) / (g_general.vBatMax - g_general.vBatMin), 20))
#define IS_TXBATT_WARNING() (g_vbat10mV < (g_general.vBatWarning*10))


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

extern void startPulses(enum ModuleType module);

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)



extern const pm_uint8_t bchout_ar[];
extern const pm_uint8_t modn12x3[];

extern uint8_t stickMode;
extern uint8_t thrSource;
extern bool enableThr;

//convert from mode 1 to mode stickMode
//NOTICE!  =>  0..3 -> 0..3
#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3

#define CONVERT_MODE(x)  (((x)<=AIL_STICK) ? pgm_read_byte_near(modn12x3 + 4*stickMode + (x)) : (x) )

#define TRIM_OFF    (1)
#define TRIM_ON     (0)
#define TRIM_RUD    (-1)
#define TRIM_ELE    (-2)
#define TRIM_THR    (-3)
#define TRIM_AIL    (-4)


#if defined(GVARS)
uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx);
int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase);
void setGVarValue(uint8_t x, int16_t value, int8_t phase);
#define GET_GVAR(x, min, max, p) getGVarValue(x, min, max, p)
#define SET_GVAR(idx, val, p) setGVarValue(idx, val, p)
#else
#define GET_GVAR(x, ...) (x)
#endif

// highest bit used for small values in mix 128 --> 8 bit is enough
#define GV1_SMALL  128
// highest bit used for large values in mix 256 --> 9 bits is used (8 bits + 1 extra bit from weightMode/offsetMode)
#define GV1_LARGE  256

#define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE )
#define GV_INDEX_CALCULATION(x,max)  ( (max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE) )
#define GV_IS_GV_VALUE(x,min,max)    ( (x>max) || (x<min) )

#define GV_INDEX_CALC_DELTA(x,delta) ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL      (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG  (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE      (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG  (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))
// for stock we just use as much as possible
#define GV_RANGELARGE_WEIGHT      GV_RANGELARGE
#define GV_RANGELARGE_WEIGHT_NEG  GV_RANGELARGE_NEG
#define GV_RANGELARGE_OFFSET      GV_RANGELARGE
#define GV_RANGELARGE_OFFSET_NEG  GV_RANGELARGE_NEG

extern uint8_t channel_order(uint8_t x);
extern uint16_t analogIn(uint8_t channel);
extern void modelDefault(uint8_t id);
extern void generalDefault();
extern void flightReset();
extern void setThrSource();
extern uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);
extern int16_t getTrimValue(uint8_t phase, uint8_t idx);
extern uint8_t getFlightMode();
extern void evalTrims();

extern ExpoData *expoAddress(uint8_t idx );
extern LimitData *limitAddress(uint8_t idx);
extern MixData *mixAddress(uint8_t idx);

enum PerOutMode {
	e_perout_mode_normal = 0,
	e_perout_mode_inactive_flight_mode = 1,
	e_perout_mode_notrainer = 2,
	e_perout_mode_notrims = 4,
	e_perout_mode_nosticks = 8,
	e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};

#define MODE_DIFFERENTIAL  0
#define MODE_EXPO          0
#define MODE_CURVE         1


#endif /* MAIN_H_ */