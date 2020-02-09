/*
 * proto_ppm.cpp
 *
 * Created: 1/26/2020 12:11:14 AM
 *  Author: Andrew
 */
#include "../main.h"

/*
 * PROTO_PPM uses PD0 OC1B
 * 16 Bit Timer running @ 8MHz has a resolution of 1us.
 * This should give a PPM resolution of 1024.
*/
static uint16_t PROTO_PPM_cb()
{
	uint16_t pulse = *pulses1MHzWPtr++;

	//good pulse
	if(pulse != 0)
	{
		togglePin();
	}
	else //should never happened
	{
		return 0;
	}

	//next pulse is termination, time to make a new frame
	if(*pulses1MHzWPtr == 0)
	{
		setPinState(PULSEPOL);
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
	setPinState(PULSEPOL);
	TIMSK &= ~(1<<OCIE1A); // Disable Output Compare A interrupt.
}

static void PROTO_PPM_initialize()
{
	setupPulsesPPM();
	PROTO_Start_Callback(PROTO_PPM_cb);
}

const void *PROTO_PPM_Cmds(enum ProtoCmds cmd)
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
		case PROTOCMD_GETOPTIONS:
			//sendOptionsSettingsPpm();
			return 0;
        default: break;
	}
	return 0;
}