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

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "main.h"

enum SwitchSources {
	SWSRC_NONE = 0,

	SWSRC_FIRST_SWITCH,

	SWSRC_ID0 = SWSRC_FIRST_SWITCH,
	SWSRC_ID1,
	SWSRC_ID2,
	SWSRC_THR,
	SWSRC_RUD,
	SWSRC_ELE,
	SWSRC_AIL,
	SWSRC_GEA,
	SWSRC_TRN,
	SWSRC_TRAINER = SWSRC_TRN,
	SWSRC_LAST_SWITCH = SWSRC_TRN,

	SWSRC_FIRST_TRIM,
	SWSRC_TrimRudLeft = SWSRC_FIRST_TRIM,
	SWSRC_TrimRudRight,
	SWSRC_TrimEleDown,
	SWSRC_TrimEleUp,
	SWSRC_TrimThrDown,
	SWSRC_TrimThrUp,
	SWSRC_TrimAilLeft,
	SWSRC_TrimAilRight,
	SWSRC_LAST_TRIM = SWSRC_TrimAilRight,

	SWSRC_REA,
	SWSRC_REB,
	SWSRC_REN,
	SWSRC_XD0,
	SWSRC_XD1,
	SWSRC_XD2,

	SWSRC_FIRST_LOGICAL_SWITCH,
	SWSRC_SW1 = SWSRC_FIRST_LOGICAL_SWITCH,
	SWSRC_SW2,
	SWSRC_SW3,
	SWSRC_SW4,
	SWSRC_SW5,
	SWSRC_SW6,
	SWSRC_SW7,
	SWSRC_SW8,
	SWSRC_SW9,
	SWSRC_SWA,
	SWSRC_SWB,
	SWSRC_SWC,
	SWSRC_SWD,
	SWSRC_SWE,
	SWSRC_SWF,
	SWSRC_LAST_LOGICAL_SWITCH = SWSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

	SWSRC_ON,
	SWSRC_ONE,

	SWSRC_COUNT,

	SWSRC_OFF = -SWSRC_ON,

	SWSRC_LAST = SWSRC_COUNT-1,
	SWSRC_FIRST = -SWSRC_LAST,

	SWSRC_LAST_IN_LOGICAL_SWITCHES = SWSRC_LAST_LOGICAL_SWITCH,
	SWSRC_LAST_IN_MIXES = SWSRC_LAST_LOGICAL_SWITCH,

	SWSRC_FIRST_IN_LOGICAL_SWITCHES = -SWSRC_LAST_IN_LOGICAL_SWITCHES,
	SWSRC_FIRST_IN_MIXES = -SWSRC_LAST_IN_MIXES,

};

enum LogicalSwitchesFunctions {
	LS_FUNC_NONE,
	LS_FUNC_VEQUAL, // v==offset
	LS_FUNC_VALMOSTEQUAL, // v~=offset
	LS_FUNC_VPOS,   // v>offset
	LS_FUNC_VNEG,   // v<offset
	LS_FUNC_APOS,   // |v|>offset
	LS_FUNC_ANEG,   // |v|<offset
	LS_FUNC_AND,
	LS_FUNC_OR,
	LS_FUNC_XOR,
	LS_FUNC_EQUAL,
	LS_FUNC_GREATER,
	LS_FUNC_LESS,
	LS_FUNC_DIFFEGREATER,
	LS_FUNC_ADIFFEGREATER,
	LS_FUNC_TIMER,
	LS_FUNC_STICKY,
	LS_FUNC_COUNT,
	LS_FUNC_MAX = LS_FUNC_COUNT-1
};

enum CswFunctionFamilies {
	LS_FAMILY_OFS,
	LS_FAMILY_BOOL,
	LS_FAMILY_COMP,
	LS_FAMILY_DIFF,
	LS_FAMILY_TIMER,
	LS_FAMILY_STICKY,
	LS_FAMILY_RANGE,
	LS_FAMILY_EDGE
};

extern uint8_t getSwitch(int8_t swtch);
extern void logicalSwitchesReset();
extern LogicalSwitchData * lswAddress(uint8_t idx);
extern uint8_t lswFamily(uint8_t func);
extern void logicalSwitchesTimerTick();
extern int16_t lswTimerValue(int8_t val);

extern volatile uint16_t s_last_switch_used;

typedef struct {
	uint8_t state;
	uint8_t last;
} __attribute__((__packed__)) ls_sticky_struct;

#endif /* SWITCHES_H_ */