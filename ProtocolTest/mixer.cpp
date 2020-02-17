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

#include "mixer.h"

int16_t  rawAnalogs[NUM_INPUTS] = {0};
int16_t  analogs [NUM_INPUTS] = {0};
int16_t  trims[NUM_STICKS] = {0};
int32_t  channels[NUM_CHNOUT] = {0};
uint16_t beepOnCenter = 0;

MixVal   mixValues[MAX_MIXERS];

uint8_t mixWarning;

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t channelOutputs[NUM_CHNOUT] = {0};
int16_t ex_channels[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain;

uint8_t mixerCurrentFlightMode;

#if defined(HELI)
int16_t cyc_analogs[3] = {0};
#endif


int16_t getValue(uint8_t i)
{
	if (i==MIXSRC_NONE) 
		return 0;

	else if IS_IN_RANGE(i, MIXSRC_FIRST_STICK, MIXSRC_LAST_POT) return calibratedStick[i-MIXSRC_Rud];

	//TODO: do something to get it work!
	//else if (i<MIXSRC_LAST_ROTARY_ENCODER+1) return getRotaryEncoder(i-MIXSRC_REa);

	else if (i==MIXSRC_MAX) 
		return 1024;

	else if (i<MIXSRC_CYC3+1)
		#if defined(HELI)
		return cyc_analogs[i-MIXSRC_CYC1];
		#else
		return 0;
		#endif

	else if (i<MIXSRC_TrimAil+1) 
		return calc1000toRESX((int16_t)8 * getTrimValue(mixerCurrentFlightMode, i-MIXSRC_TrimRud));
	else if (i==MIXSRC_3POS)
		return (getSwitch(SW_ID0-SW_BASE+1) ? -1024 : (getSwitch(SW_ID1-SW_BASE+1) ? 0 : 1024));
	// don't use switchState directly to give getSwitch possibility to hack values if needed for switch warning
	else if (i<MIXSRC_SW1) 
		return getSwitch(SWSRC_THR+i-MIXSRC_THR) ? 1024 : -1024;
	else if (i<MIXSRC_LAST_LOGICAL_SWITCH+1) 
		return getSwitch(SWSRC_FIRST_LOGICAL_SWITCH+i-MIXSRC_FIRST_LOGICAL_SWITCH) ? 1024 : -1024;
	else if (i<MIXSRC_LAST_TRAINER+1) 
	{
		int16_t x = ppmInput[i-MIXSRC_FIRST_TRAINER];
		if (i<MIXSRC_FIRST_TRAINER+NUM_CAL_PPM) 
		{
			x-= g_general.trainer.calib[i-MIXSRC_FIRST_TRAINER];
		}
		return x*2;
	} 
	else if (i<MIXSRC_LAST_CH+1) 
		return ex_channels[i-MIXSRC_CH1];

	#if defined(GVARS)
	else if (i<MIXSRC_LAST_GVAR+1) 
		return GVAR_VALUE(i-MIXSRC_GVAR1, getGVarFlightPhase(mixerCurrentFlightMode, i-MIXSRC_GVAR1));
	#endif

	else 
		return 0;
}
