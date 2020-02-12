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

