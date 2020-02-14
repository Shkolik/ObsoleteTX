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
	
	//TimerData timers[MAX_TIMERS];				//18bytes (2 timers)
	//uint8_t   thrTrim:1;						//1byte		// Enable Throttle Trim
	//int8_t    trimInc:3;						//1byte		// Trim Increments
	//uint8_t   disableThrottleWarning:1;		//1byte
	uint8_t   extendedLimits:1;					//1byte
	//uint8_t   extendedTrims:1;				//1byte
	//BeepANACenter beepANACenter;				//2bytes
	//MixData   mixData[MAX_MIXERS];			//304bytes (19*16 mixes) 
	//LimitData limitData[NUM_CHNOUT];			//144bytes (9*16channels)
	//ExpoData  expoData[MAX_EXPOS];			//64bytes (8*8expos)
	//CURVDATA  curves[MAX_CURVES];				//8bytes
	//int8_t    points[NUM_POINTS];				//104bytes
	//LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
	//CustomFunctionData customFn[NUM_CFN];		//84 bytes (7*12 funcs)
	//FlightModeData flightModeData[MAX_FLIGHT_MODES];
	//uint8_t thrTraceSrc:5;					//1byte
	//uint8_t thrSwitch:3;						//1byte
	//swarnstate_t  switchWarningState;			//1byte
	//swarnenable_t switchWarningEnable;		//1byte
	//SwashRingData swashR;						//6bytes		// Heli data
	//uint8_t UnusedModel; // Use later .. todo	
} __attribute__((__packed__)) ModelSettings;   //753bytes so... 

extern ModelSettings g_model;
extern GeneralSettings g_general;

#endif /* DATA_H_ */