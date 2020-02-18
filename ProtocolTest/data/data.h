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

#ifndef DATA_H_
#define DATA_H_

#include "../main.h"

#define EEPROM_VER	1

#define MAX_TIMERS           2
#define NUM_CYC              3
#define NUM_CAL_PPM          4

enum ModuleType {
	MODULE_PPM,
	#ifdef DSM
	MODULE_DSM,
	#endif
	#ifdef MULTI
	MODULE_MULTI,
	#endif
	MODULETYPES_COUNT
};

enum MixSources {
	MIXSRC_NONE,
	MIXSRC_FIRST_STICK,
	MIXSRC_Rud = MIXSRC_FIRST_STICK,
	MIXSRC_Ele,
	MIXSRC_Thr,
	MIXSRC_Ail,

	MIXSRC_FIRST_POT,
	MIXSRC_P1 = MIXSRC_FIRST_POT,
	MIXSRC_P2,
	MIXSRC_P3,
	MIXSRC_LAST_POT = MIXSRC_P3,

	MIXSRC_REa,
	MIXSRC_REb,
	MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REb,

	MIXSRC_MAX,

	MIXSRC_FIRST_HELI,
	MIXSRC_CYC1 = MIXSRC_FIRST_HELI,
	MIXSRC_CYC2,
	MIXSRC_CYC3,

	MIXSRC_FIRST_TRIM,
	MIXSRC_TrimRud = MIXSRC_FIRST_TRIM,
	MIXSRC_TrimEle,
	MIXSRC_TrimThr,
	MIXSRC_TrimAil,
	MIXSRC_LAST_TRIM = MIXSRC_TrimAil,

	MIXSRC_FIRST_SWITCH,

	MIXSRC_3POS = MIXSRC_FIRST_SWITCH,
	MIXSRC_THR,
	MIXSRC_RUD,
	MIXSRC_ELE,
	MIXSRC_AIL,
	MIXSRC_GEA,
	MIXSRC_TRN,
	MIXSRC_LAST_SWITCH = MIXSRC_TRN,
	MIXSRC_FIRST_LOGICAL_SWITCH,
	MIXSRC_SW1 = MIXSRC_FIRST_LOGICAL_SWITCH,
	MIXSRC_SW9 = MIXSRC_SW1 + 8,
	MIXSRC_SWA,
	MIXSRC_SWB,
	MIXSRC_SWC,
	MIXSRC_LAST_LOGICAL_SWITCH = MIXSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

	MIXSRC_FIRST_TRAINER,
	MIXSRC_LAST_TRAINER = MIXSRC_FIRST_TRAINER+NUM_TRAINER-1,

	MIXSRC_FIRST_CH,
	MIXSRC_CH1 = MIXSRC_FIRST_CH,
	MIXSRC_CH2,
	MIXSRC_CH3,
	MIXSRC_CH4,
	MIXSRC_CH5,
	MIXSRC_CH6,
	MIXSRC_CH7,
	MIXSRC_CH8,
	MIXSRC_CH9,
	MIXSRC_CH10,
	MIXSRC_CH11,
	MIXSRC_CH12,
	MIXSRC_CH13,
	MIXSRC_CH14,
	MIXSRC_CH15,
	MIXSRC_CH16,
	MIXSRC_LAST_CH = MIXSRC_CH1+NUM_CHNOUT-1,

	MIXSRC_FIRST_GVAR,
	MIXSRC_GVAR1 = MIXSRC_FIRST_GVAR,
	MIXSRC_LAST_GVAR = MIXSRC_FIRST_GVAR+MAX_GVARS-1,

	//MIXSRC_FIRST_TELEM,
	//MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+NUM_TELEMETRY-1
};

#define MIXSRC_FIRST   (MIXSRC_NONE+1)
#define MIXSRC_LAST    MIXSRC_LAST_CH
#define INPUTSRC_FIRST MIXSRC_Rud
#define INPUTSRC_LAST  MIXSRC_LAST_TELEM

#define MIN_POINTS 3
#define MAX_POINTS 17




// Used to hold tx id
typedef union {
	uint8_t   ID_8[4];
	uint32_t  ID_32;
} fixed_ID;

// Struct to hold calibration data for analogs
typedef struct {
	int16_t middle;			//2bytes
	int16_t min;			//2bytes
	int16_t max;			//2bytes
} __attribute__((__packed__)) Calibration; //6 bytes total

// Trainer settings
typedef struct {
	uint8_t channel;		//1byte		//:6; // 0-7 = ch1-8
	uint8_t mode;			//1byte		//:2;   // off,add-mode,subst-mode
	int8_t  weight;			//1byte
} __attribute__((__packed__)) TrainerChannel; //3 bytes total

// Trainer settings
typedef struct {
	int16_t        calib[4];	//8 bytes	(2*4)
	TrainerChannel mix[4];		//12 bytes	(3*4)
} __attribute__((__packed__)) TrainerInput; //20 bytes total

enum TimerModes {
	TMRMODE_NONE,
	TMRMODE_ABS,
	TMRMODE_THR,
	TMRMODE_THR_REL,
	TMRMODE_THR_TRG,
	TMRMODE_COUNT
};

typedef struct {
	TimerModes  mode;              // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
	uint16_t	start;
	uint8_t		countdownBeep:2;
	uint8_t		minuteBeep:1;
	uint8_t		persistent:2;
	uint8_t		spare:3;
	uint16_t	value;
} __attribute__((__packed__)) TimerData;

#define IS_MANUAL_RESET_TIMER(idx) (g_model.timers[idx].persistent == 2)

#define LIMIT_EXT_PERCENT   125
#define LIMIT_EXT_MAX       LIMIT_EXT_PERCENT
#define PPM_CENTER_MAX      125
#define LIMIT_MAX(lim)      (lim->max+100)
#define LIMIT_MIN(lim)      (lim->min-100)
#define LIMIT_OFS(lim)      (lim->offset)
#define LIMIT_MAX_RESX(lim) calc100toRESX(LIMIT_MAX(lim))
#define LIMIT_MIN_RESX(lim) calc100toRESX(LIMIT_MIN(lim))
#define LIMIT_OFS_RESX(lim) calc1000toRESX(LIMIT_OFS(lim))

typedef struct {
	int8_t		min;
	int8_t		max;
	int8_t		ppmCenter;
	int16_t		offset:14;
	uint16_t	symetrical:1;
	uint16_t	revert:1;
} __attribute__((__packed__)) LimitData;

typedef struct {
	uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
	uint8_t chn:2;
	uint8_t curveMode:1;
	uint8_t spare:3;
	uint8_t flightModes;
	int8_t  swtch;
	uint8_t weight;
	int8_t  curveParam;
} __attribute__((__packed__)) ExpoData;

typedef struct {
	uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
	uint8_t curveMode:1;       // O=curve, 1=differential
	uint8_t noExpo:1;
	uint8_t weightMode:1;
	uint8_t offsetMode:1;
	uint8_t srcRaw;
	int8_t  weight;
	int8_t  swtch;
	uint8_t flightModes;
	uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
	int8_t  carryTrim:3;
	uint8_t mixWarn:2;         // mixer warning
	uint8_t spare:1;
	uint8_t delayUp:4;
	uint8_t delayDown:4;
	uint8_t speedUp:4;
	uint8_t speedDown:4;
	int8_t  curveParam;
	int8_t  offset;
} __attribute__((__packed__)) MixData;

// Logical Switches data
typedef struct {			
	int8_t		v1;				//input
	int16_t		v2:11;			//offset
	uint16_t	func:5;
	int8_t		andsw;			
} __attribute__((__packed__)) LogicalSwitchData;

typedef struct {
	int8_t		swtch;
	uint8_t		func;
	uint8_t		mode:2;
	uint8_t		param:4;
	uint8_t		active:1;
	uint8_t		spare:1;
	uint8_t		value;
} __attribute__((__packed__)) CustomFunctionData;

#define CFN_SWITCH(p)       ((p)->swtch)
#define CFN_FUNC(p)         ((p)->func)
#define CFN_ACTIVE(p)       ((p)->active)
#define CFN_CH_INDEX(p)     ((p)->param)
#define CFN_TIMER_INDEX(p)  ((p)->param)
#define CFN_GVAR_INDEX(p)   ((p)->param)
#define CFN_PLAY_REPEAT(p)  ((p)->param)
#define CFN_PLAY_REPEAT_MUL 10
#define CFN_GVAR_MODE(p)    ((p)->mode)
#define CFN_PARAM(p)        ((p)->value)
#define CFN_RESET(p)        ((p)->active = 0, CFN_PARAM(p) = 0)
#define CFN_GVAR_CST_MAX    125

typedef struct {
	int16_t		trim[4];            //TRIMS_ARRAY;
	int8_t		swtch;               // swtch of phase[0] is not used
	char		name[LEN_FLIGHT_MODE_NAME];
	uint8_t		fadeIn:4;
	uint8_t		fadeOut:4;
	int8_t		gvars[MAX_GVARS];    //PHASE_GVARS_DATA;
} __attribute__((__packed__)) FlightModeData;

enum SwashType {
	SWASH_TYPE_NONE,
	SWASH_TYPE_120,
	SWASH_TYPE_120X,
	SWASH_TYPE_140,
	SWASH_TYPE_90,
	SWASH_TYPE_MAX = SWASH_TYPE_90
};

typedef struct {
	uint8_t		invertELE:1;
	uint8_t		invertAIL:1;
	uint8_t		invertCOL:1;
	SwashType	type:5;
	uint8_t		collectiveSource;
	uint8_t		value;
} __attribute__((__packed__)) SwashRingData;

#define TRIM_EXTENDED_MAX 500
#define TRIM_EXTENDED_MIN (-TRIM_EXTENDED_MAX)
#define TRIM_MAX 125
#define TRIM_MIN (-TRIM_MAX)

#define LEN_GVAR_NAME 6
#define GVAR_MAX      121 // used for phase decrypt
#define GVAR_LIMIT    120
#define PHASE_GVARS_DATA int8_t gvars[MAX_GVARS]
#define GVAR_VALUE(x, p) g_model.flightModeData[p].gvars[x]

typedef struct {
	char    name[LEN_GVAR_NAME];
} __attribute__((__packed__)) GlobalVariable;

#define RESERVE_RANGE_FOR_GVARS MAX_GVARS
// even we do not spend space in EEPROM for GVARS, we reserve the space inside the range of values, like offset, weight, etc.

#define DELAY_STEP  2
#define SLOW_STEP   2
#define DELAY_MAX   15 /* 7.5 seconds */
#define SLOW_MAX    15 /* 7.5 seconds */


// Transmitter settings
typedef struct {
	uint8_t			version;					//1byte
	//RF data
	ModuleType		rfModuleType:2;				//1byte		// PPM = 0, DSM = 1, MULTI = 2 maybe more from enum Protocols		
	//end of RF data
	
	Calibration		calib[NUM_STICKS+NUM_POTS];	//24 bytes (6*4) or 36 bytes with 2 pots 
	uint16_t		checkSum;					//2bytes
	int8_t			currentModel;				//1byte
	uint8_t			contrast;					//1byte
	uint8_t			vBatWarning;				//1byte
	int8_t			voltageCalibration;			//1byte
	uint8_t			vBatMin;					//1byte
	uint8_t			vBatMax;					//1byte
	TrainerInput	trainer;					//20bytes
	uint8_t			mainView;					//1byte		// index of view in main screen	
	int8_t			beepMode:2;					//1byte		// -2=quiet, -1=only alarms, 0=no keys, 1=all
	int8_t			beepLength:3;				//1byte
	int8_t			hapticMode:2;				//1byte		// -2=quiet, -1=only alarms, 0=no keys, 1=all
	int8_t			hapticLength;				//1byte
	int8_t			hapticStrength:3;			//1byte
	uint8_t			alarmsFlash:1;				//1byte
	uint8_t			disableMemoryWarning:1;		//1byte
	uint8_t			disableAlarmWarning:1;		//1byte
	uint8_t			stickMode:1;				//1byte
	uint8_t			inactivityTimer;			//1byte
	uint8_t			templateSetup;				//1byte		// TAER order for receiver channels
	int8_t			PPM_Multiplier;				//1byte
	uint8_t			stickReverse;				//1byte
	
	// 32 bits or 4*8 bits fixed ID
	fixed_ID	fixedID;						//4bytes
} __attribute__((__packed__)) GeneralSettings;	//66 bytes total (at least) or 78 with 2 pots 

// Model settings
typedef struct {
	char      name[LEN_MODEL_NAME];				//10 bytes	// must be first for eeLoadModelName
	
	//Rf data
	uint8_t   modelId:6;						//1byte
	uint8_t   rfSubType:4;						//1byte
	int8_t    rfOptionValue1;					//1byte
	int8_t    rfOptionValue2;					//1byte
	uint8_t   rfOptionValue3:5;					//1byte
	uint8_t   rfOptionBool1:1;					//1byte
	uint8_t   rfOptionBool2:1;					//1byte
	uint8_t   rfOptionBool3:1;					//1byte
	//end of RF data
	
	TimerData timers[MAX_TIMERS];				//18bytes (2 timers)
	uint8_t   thrTrim:1;						//1byte		// Enable Throttle Trim
	int8_t    trimInc:3;						//1byte		// Trim Increments
	uint8_t   disableThrottleWarning:1;			//1byte
	uint8_t   extendedLimits:1;					//1byte
	uint8_t   extendedTrims:1;					//1byte
	int16_t	  beepOnCenter;						//2bytes
	MixData   mixData[MAX_MIXERS];				//304bytes (19*16 mixes) 
	LimitData limitData[NUM_CHNOUT];			//144bytes (9*16channels)?
	ExpoData  expoData[MAX_EXPOS];				//64bytes (8*8expos)?
	int8_t	  curves[MAX_CURVES];				//8bytes
	int8_t    points[NUM_POINTS];				//104bytes
	LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
	CustomFunctionData customFn[NUM_CFN];		//84 bytes (7*12 funcs)
	FlightModeData flightModeData[MAX_FLIGHT_MODES];
	GlobalVariable gvars[MAX_GVARS];
	uint8_t thrTraceSrc:5;						//1byte
	uint8_t thrSwitch:3;						//1byte
	uint8_t switchWarningState;					//1byte
	uint8_t switchWarningEnable;				//1byte
	SwashRingData swashRing;					//6bytes		// Heli data
	
} __attribute__((__packed__)) ModelSettings;   //753bytes so... 

extern ModelSettings g_model;
extern GeneralSettings g_general;

#endif /* DATA_H_ */