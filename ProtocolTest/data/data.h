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

// Used to hold tx id
typedef union {
	uint8_t   ID_8[4];
	uint32_t  ID_32;
} fixed_ID;

// Struct to hold calibration data for analogs
typedef struct {
	int16_t middle;
	int16_t min;
	int16_t max;
} __attribute__((__packed__)) Calibration;

// Trainer settings
typedef struct {
	uint8_t channel;//:6; // 0-7 = ch1-8
	uint8_t mode;//:2;   // off,add-mode,subst-mode
	int8_t  weight;
} __attribute__((__packed__)) TrainerChannel;

// Trainer settings
typedef struct {
	int16_t        calib[4];
	TrainerChannel mix[4];
} __attribute__((__packed__)) TrainerInput;

// Transmitter settings
typedef struct {
	uint8_t			version;
	Calibration		calib[NUM_STICKS+NUM_POTS];
	uint16_t		checkSum;
	int8_t			currentModel;
	uint8_t			contrast;
	uint8_t			vBatWarning;
	int8_t			voltageCalibration;
	uint8_t			vBatMin;
	uint8_t			vBatMax;
	TrainerInput	trainer;
	uint8_t			mainView;            // index of view in main screen	
	int8_t			beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
	int8_t			beepLength:3;
	int8_t			hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
	int8_t			hapticLength;
	int8_t			hapticStrength:3;
	uint8_t			alarmsFlash:1;
	uint8_t			disableMemoryWarning:1;
	uint8_t			disableAlarmWarning:1;
	uint8_t			stickMode:1;	
	uint8_t			inactivityTimer;		
	uint8_t			templateSetup;   // TAER order for receiver channels
	int8_t			PPM_Multiplier;
	uint8_t			stickReverse;	
	uint8_t			unexpectedShutdown:1;
	
	// 32 bits or 4*8 bits fixed ID
	fixed_ID	fixedID;
} __attribute__((__packed__)) GeneralSettings;

// Model settings
typedef struct {
	char      name[LEN_MODEL_NAME]; // must be first for eeLoadModelName
	
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
	//uint8_t thrTraceSrc:5;
	//uint8_t thrSwitch:3;
	//swarnstate_t  switchWarningState;
	//swarnenable_t switchWarningEnable;
	//SwashRingData swashR;          // Heli data
	//uint8_t UnusedModel; // Use later .. todo	
} __attribute__((__packed__)) ModelSettings;

extern ModelSettings g_model;
extern GeneralSettings g_general;

#endif /* DATA_H_ */