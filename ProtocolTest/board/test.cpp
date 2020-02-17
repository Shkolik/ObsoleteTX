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

#include "test.h"

static const pm_uchar crossTrim[] PROGMEM = {
	TRIMS_GPIO_PIN_LHL,
	TRIMS_GPIO_PIN_LHR,
	TRIMS_GPIO_PIN_LVD,
	TRIMS_GPIO_PIN_LVU,
	TRIMS_GPIO_PIN_RVD,
	TRIMS_GPIO_PIN_RVU,
	TRIMS_GPIO_PIN_RHL,
	TRIMS_GPIO_PIN_RHR
};

uint8_t trimDown(uint8_t idx)
{
	uint8_t in = 0;//~PING;
	return (in & pgm_read_byte_near(crossTrim+idx));
}

uint8_t switchState(EnumKeys enuk)
{
	uint8_t result = 0 ;

	//if (enuk < NUM_KEYS)
	//return keys[enuk].state() ? 1 : 0;
	//
	//switch(enuk) {
	//case SW_ELE:
	//result = !(PINL & INP_L_ElevDR);
	//break;
	//
	//case SW_AIL:
	//result = !(PIND & INP_D_AileDR);
	//break;
	//
	//case SW_RUD:
	//result = !(PING & INP_G_RuddDR);
	//break;
	//
	////       INP_C_ID1  INP_C_ID2
	//// ID0      0          1
	//// ID1      1          1
	//// ID2      1          0
	//case SW_ID0:
	//result = !(PINC & INP_C_ID1);
	//break;
	//
	//case SW_ID1:
	//result = (PINC & INP_C_ID1) && (PINC & INP_C_ID2);
	//break;
	//
	//case SW_ID2:
	//result = !(PINC & INP_C_ID2);
	//break;
	//
	//case SW_GEA:
	//result = !(PING & INP_G_Gear);
	//break;
	//
	//case SW_THR:
	//result = !(PING & INP_G_ThrCt);
	//break;
	//
	//case SW_TRN:
	//result = !(PINL & INP_L_Trainer);
	//break;
	//
	//default:
	//break;
	//}

	return result;
}

void boardInit()
{
	//////////////////////////////////////////////////////////////////////////
	/// setup registers
	//////////////////////////////////////////////////////////////////////////
	//Led				//TX0				//RX0
	DDRE |= (1 << 4);	DDRE |= (1 << 1);	DDRE  &= ~(1 << 0);
	PORTE|= (1 << 4);	PORTE|= (1 << 1);	PORTE &= ~(1 << 0);
	
	//PPM OUT			//RX1				//TX1
	DDRD |= (1 << 0);	DDRD  &= ~(1 << 2);	DDRD |= (1 << 3);
	PORTD|= (1 << 0);	PORTD &= ~(1 << 2);	PORTD|= (1 << 3);
	
	//Change proto		//BIND
	DDRG &= ~(1 << 4);	DDRG &= ~(1 << 3);
	PORTG|= (1 << 4);	PORTG |= (1 << 3);
	
	DDRF = 0b00000000;  PORTF = 0b11111111; // All analog inputs PullUp
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// setup ADC
	//////////////////////////////////////////////////////////////////////////
	ADMUX = ADC_VREF_TYPE;
	ADCSRA = (1 << ADEN)| (1 << ADPS2); // ADC enabled, prescaler division=16 (no interrupt, no auto-triggering)
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/// setup timers
	//////////////////////////////////////////////////////////////////////////
	//Timer generates event every 10ms. Starts here and never stops
	TIMER_10MS_COMPVAL	= 78;			// count to 10ms
	TCCR0	= (7 << CS00);				// Norm mode, clk/1024 (1 << CS00 | 1 << CS01 | 1 << CS02)
	TIMSK	= (1<<OCIE0) | (1<<TOIE0);	// COMP - 78 (100Hz), OVF - 255 (30.64Hz)
	//////////////////////////////////////////////////////////////////////////

	//Timer counts with frequency 1MHz
	TCCR1A	= 0;						// Clear timer registers
	TCCR1B	= 0;						// Normal mode
	TCCR1C	= 0;
	TCNT1	= 0;						// starts from 0
	TCCR1B	|= 1<<CS11;					// F_TIMER1 = 1MHz (F_CPU/8), 1 tick per 1 microsecond
	RF_TIMER_COMPA_REG	= 20000;		// Next frame starts in 20 mS
	//Interrupt will be enabled by protocol callback when started
	//////////////////////////////////////////////////////////////////////////
}

//run every 10ms
//For some reason 1 and 2 channels not correct
void getADC()
{
	#if defined(INV_STICK_RH) || defined(INV_STICK_LV) || defined(INV_STICK_RV) || defined(INV_STICK_LH)
	uint8_t invMask = 0
	#if defined(INV_STICK_RH)
	+ _BV(0)
	#endif
	#if defined(INV_STICK_LV)
	+ _BV(1)
	#endif
	#if defined(INV_STICK_RV)
	+ _BV(2)
	#endif
	#if defined(INV_STICK_LH)
	+ _BV(3)
	#endif
	;
	#endif
	
	for (uint8_t adc_input=0; adc_input<6; adc_input++)	
	{
		//need at least 2 conversions for more reliable result
		int16_t temp_ana;
		ADMUX = adc_input|ADC_VREF_TYPE;
		// Start the AD conversion
		ADCSRA |= 1 << ADSC;
		// Wait for the AD conversion to complete
		while bit_is_set(ADCSRA,ADSC);
		temp_ana = ADC;
		// Start the second AD conversion
		ADCSRA |= 1 << ADSC;
		// Wait for the AD conversion to complete
		while bit_is_set(ADCSRA,ADSC);
		temp_ana += ADC;
		
		#if defined(INV_STICK_RH) || defined(INV_STICK_LV) || defined(INV_STICK_RV) || defined(INV_STICK_LH)
		if (invMask & 0x1)
		{
			temp_ana = 0x7FE - temp_ana;
		}
		invMask >>= 1;
		#endif
		
		//save result
		channelOutputs[adc_input] = temp_ana - 1024;
	}
	//Hack to fix unused channels
	//remove when mixer ready
	for(uint8_t i = 4; i<CHMAX; i++)
		channelOutputs[i] = -1024;
}

