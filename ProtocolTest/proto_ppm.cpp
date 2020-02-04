/*
 * proto_ppm.cpp
 *
 * Created: 1/26/2020 12:11:14 AM
 *  Author: Andrew
 */ 
#include "main.h"

uint16_t *RptrA; // For OCR1A

//void SetOut(bool state)
//{
	//if(state)
		//OUT_PORT &= ~OUT_PIN;
	//else
		//OUT_PORT |= OUT_PIN;
//}

//TODO: Rewrite to use PD0 and Timer1 in normal mode
/*
 * PROTO_PPM uses PD0 OC1B
 * 16 Bit Timer running @ 8MHz has a resolution of 1us.
 * This should give a PPM resolution of 1024.
*/
static uint16_t PROTO_PPM_cb2()
{
	uint16_t full_us = 0;
	
	if(*RptrA == 0) 
	{ 	
		// End of timing events.
		RptrA = &pulses1MHzWPtr[0];
		// Set the PPM idle level.
		//SetOut(PULSEPOL);

		//SCHEDULE_MIXER_END_IN_US(22500);		
		setupPulsesPPM(PPM);
	
		dt = TCNT1 - OCR1A; // Show how long to setup pulses and ISR jitter.
		full_us = PULSES_SETUP_TIME_US;
	}
	else if (*(RptrA + 1) == 0) 
	{ 
		// Look ahead one timing event.
		// Need to prevent next toggle.
		// Read pin and store before disconnecting switching output.
		//if(OUT_PORT & OUT_PIN) 
			//OUT_PORT |= OUT_PIN;
		//else 
			//OUT_PORT &= ~OUT_PIN;		
	}
	full_us = *RptrA++;
	
	//if(OUT_PORT & OUT_PIN)
	//OUT_PORT |= OUT_PIN;
	//else
	//OUT_PORT &= ~OUT_PIN;
	
	if(!full_us)
	{
		PROTO_PPM_Cmds(PROTOCMD_RESET);
		return 0;
	}

	//  dt = TCNT1 - OCR1B; // Calculate latency and jitter.
	if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
	if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

	//RF_TIMER_COMPA_REG = full_us;
	return full_us;
}


static void PROTO_PPM_reset()
{		
	
	// Make pin idle state before disconnecting switching output.
	//SetOut(PULSEPOL);
	TIMSK &= ~(1<<OCIE1A); // Disable Output Compare A interrupt.	
}

static void PROTO_PPM_initialize()
{	
	RptrA = &pulses1MHzWPtr[0];
	PROTO_Start_Callback(PROTO_PPM_cb2);
}

const void *PROTO_PPM_Cmds(enum ProtoCmds cmd)
{
	switch(cmd) 
	{
		case PROTOCMD_INIT: 			
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