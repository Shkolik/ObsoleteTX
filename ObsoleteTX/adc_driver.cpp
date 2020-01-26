/*
 * adc_driver.cpp
 *
 * Created: 1/24/2020 10:25:25 PM
 *  Author: Andrew
 */ 


#include "ObsoleteTX.h"

//need test, maybe should be 0
#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

void adcInit()
{
	ADMUX = ADC_VREF_TYPE;
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0); // ADC enabled, prescaler division=32 (no interrupt, no auto-triggering)
}


void getADC() // 0.56 mS
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

	for (uint8_t adc_input=0; adc_input<8; adc_input++) 
	{
		//need at least 2 conversions for more reliable result
		
		uint16_t temp_ana;
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
		
		//stick inversion 
		#if defined(INV_STICK_RH) || defined(INV_STICK_LV) || defined(INV_STICK_RV) || defined(INV_STICK_LH)
		if (invMask & 0x1)
		{
			temp_ana = 0x7FE - temp_ana;
		}
		invMask >>= 1;
		#endif
		
		//save result
		s_anaFilt[adc_input] = temp_ana;
		
	}
}
