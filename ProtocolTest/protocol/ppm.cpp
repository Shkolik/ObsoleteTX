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

#include "ppm.h"

uint16_t pulses1MHz[PULSES_SIZE];
uint16_t *pulses1MHzPtr;

//run every frame after the end of transmitting synchronization pulse
void setupPulsesPPM()
{
	int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;		// range of 0.7...1.2...1.7msec
	// channels will have resolution of 1024 (-512...512)

	uint16_t q = (g_model.PPMDELAY*50+300)*TIMER_MULTIPLIER;						// Channel sync pulse.
	
	
	//65535us aka 65.535ms more than enough for ppm frame even if we running 2MHz timer instead of 1MHz
	//so multiplier will be 2 and frame len set to 30ms, this is unreal
	uint16_t rest = (22500u + g_model.PPMFRAMELENGTH*1000)*TIMER_MULTIPLIER;

	// pointer to first event in ppm frame
	pulses1MHzPtr = &pulses1MHz[0];

	for (uint8_t i = 0; i < g_model.PPMNCH; i++)
	{
		// Just do channels 1-8 -512=1500 = 988 512+1500=2012
		int16_t ch = channelOutputs[i] >> TIMER_DEMULTIPLIER;
		int16_t v = limit((int16_t)-PPM_range, ch, (int16_t)PPM_range) + PPM_CENTER*TIMER_MULTIPLIER;
		rest -= v;
		*pulses1MHzPtr++ = q;
		*pulses1MHzPtr++ = v - q; // Total pulse width without channel sync pulse.
	}

	*pulses1MHzPtr++ = q;

	rest = limit((uint16_t)9000, rest, (uint16_t)653535); // Prevents overflows.

	*pulses1MHzPtr++ = rest;
	*pulses1MHzPtr = 0;			// End array with (uint16_t) 0;

	//reset pointer to the first element of frame
	pulses1MHzPtr = &pulses1MHz[0];
}

/*
 * PROTO_PPM uses PD0 OC1B
 * 16 Bit Timer running @ 8MHz has a resolution of 1us.
 * This should give a PPM resolution of 1024.
*/
static uint16_t PROTO_PPM_cb()
{
	uint16_t pulse = *pulses1MHzPtr++;

	//good pulse
	if(pulse != 0)
	{
		TOGGLE(OUT_PORT, OUT_PIN);
	}
	else //should never happened
	{
		return 0;
	}

	//next pulse is termination, time to make a new frame
	if(*pulses1MHzPtr == 0)
	{
		SETPIN(OUT_PORT, OUT_PIN, g_model.PULSEPOL);
		setupPulsesPPM();
		//For now latency to big > 100us
		//TODO: Have to find solution how to fix it. 50-55us will be ok
		CALCULATE_LAT_JIT();
	}

	if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
	if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
	
	heartbeat |= HEART_TIMER_PULSES;
	
	return pulse;
}

static void PROTO_PPM_reset()
{
	// Make pin idle state before stop transmitting.
	SETPIN(OUT_PORT, OUT_PIN, g_model.PULSEPOL);
	TIMSK &= ~(1<<OCIE1A); // Disable Output Compare A interrupt.
}

static void PROTO_PPM_initialize()
{
	setupPulsesPPM();
	PROTO_Start_Callback(PROTO_PPM_cb);
}

const void *PPM_Cmds(enum ProtoCmds cmd)
{
	switch(cmd)
	{
		case PROTOCMD_INIT:		
			debugln("PROTO_PPM_initialize");	
			PROTO_PPM_initialize();
			return 0;
		case PROTOCMD_RESET:			
			PROTO_PPM_reset();
			return 0;
		/*case PROTOCMD_GETOPTIONS:
			sendOptionsSettingsPpm();
			return 0;*/
        default: break;
	}
	return 0;
}