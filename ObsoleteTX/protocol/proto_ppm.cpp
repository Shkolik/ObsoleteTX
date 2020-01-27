/*
 * proto_ppm.cpp
 *
 * Created: 1/26/2020 12:11:14 AM
 *  Author: Andrew
 */ 
#include "../ObsoleteTX.h"

uint16_t *RptrA; // For OCR1A
uint16_t *RptrB; // For OCR1B

//TODO: Rewrite to use PD0 and Timer1 in normal mode
/*
 * PROTO_PPM uses PD0 OC1B
 * 16 Bit Timer running @ 8MHz has a resolution of 1us.
 * This should give a PPM resolution of 1024.
*/
static uint16_t PROTO_PPM_cb2()
{
  if(*RptrB == 0) { // End of timing events.
    RptrB = &pulses2MHz.pword[0];
    // Set the PPM idle level.
    if (g_model.PULSEPOL) {
      TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Clear
    }
    else {
      TCCR1A |= (0b11<<COM1B0); // Set
    }
    TCCR1C = 1<<FOC1B; // Strobe FOC1.
    TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1x on next match.

    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(22500 + ((g_model.PPMFRAMELENGTH * 1000) / 2));
    setupPulsesPPM(PPM);
    heartbeat |= HEART_TIMER_PULSES;
    dt = TCNT1 - OCR1B; // Show how long to setup pulses and ISR jitter.
    return PULSES_SETUP_TIME_US;// *2;
  }
  else if (*(RptrB +1) == 0) { // Look ahead one timing event.
    // Need to prevent next toggle.
    // Read pin and store before disconnecting switching output.
    if(PIND & PIN0_bm) PORTD |= PIN0_bm;
    else PORTD &= ~PIN0_bm;
    TCCR1A &= ~(0b11<<COM1B0);
    return *RptrB++;
  }
  else // Toggle pin.
    return *RptrB++;
}


void PROTO_PPM_cb1()
{
	uint16_t full_us = PROTO_PPM_cb2();

	if(! full_us) {
		//PROTO_PPM_Cmds(PROTOCMD_RESET);
		return;
	}

	//  dt = TCNT1 - OCR1B; // Calculate latency and jitter.
	if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
	if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

	OCR1B += full_us;
}


static void PROTO_PPM_reset()
{
  // Make pin idle state before disconnecting switching output.
  if(g_model.PULSEPOL) PORTD &= ~PIN0_bm;
  else PORTD |= PIN0_bm;

  TCCR1A &= ~(0b11<<COM1B0);
  TIMSK &= ~(1<<OCIE1B); // Disable Output Compare B interrupt.
  TIFR |= 1<<OCF1B; // Reset Flag.
}

static void PROTO_PPM_initialize()
{
#if defined(FRSKY)
	telemetryPPMInit();
#endif

	RptrB = &pulses2MHz.pword[0];
	*RptrB = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ocr1b_function_ptr = PROTO_PPM_cb1; // Setup function pointer used in ISR.
		OCR1B = TCNT1 + 16000U;
		TIFR |= 1<<OCF1B; // Reset Flag.
		TIMSK |= 1<<OCIE1B; // Enable Output Compare interrupt.
	}
}


const void *PROTO_PPM_Cmds(enum ProtoCmds cmd)
{
	switch(cmd) {
		case PROTOCMD_INIT: 
			PROTO_PPM_initialize();
			return 0;
		case PROTOCMD_RESET:
			PROTO_PPM_reset();
			return 0;
		case PROTOCMD_GETOPTIONS:
			sendOptionsSettingsPpm();
		return 0;
        default: break;
	}
	return 0;
}