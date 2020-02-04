/*
 * ProtocolTest.cpp
 *
 * Created: 1/31/2020 1:07:25 PM
 * Author : andrew.shkolik
 */ 


#include "main.h"
uint16_t *pulses1MHzWPtr;
uint16_t frames[300];
uint16_t *framesPtr;
uint16_t pulses1MHz[PULSES_WORD_SIZE];
int16_t channelOutputs[PPMCHMAX];

volatile uint16_t g_tmr10ms;
volatile uint16_t framesTransmitted;

void blinkLed()
{
	LED_PORT ^= LED_PIN;
	_delay_ms(10);
	LED_PORT ^= LED_PIN;
}

void togglePin()
{
	OUT_PORT ^= OUT_PIN;
}

//run every frame after the end of transmitting synchronization pulse 
void setupPulsesPPM(enum ppmtype proto)
{
	int16_t PPM_range = 512;	// range of 0.7...1.2...1.7msec
								// channels will have resolution of 1024 (-512...512)
	uint16_t q = 300;			// Channel sync pulse.
	
	//65535us aka 65.535ms more than enough for ppm frame even if we running 2MHz timer instead of 1MHz
	//so multiplier will be 2 and frame len set to 30ms, this is unreal
	uint16_t rest = 22500u*TIMER_MULTIPLIER;
	
	// pointer to first event in ppm frame
	pulses1MHzWPtr = &pulses1MHz[0];
	
	for (uint8_t i = 0; i < PPMCHMAX; i++)
	{		
		// Just do channels 1-8 -512=1500 = 988 512+1500=2012
		int16_t ch = channelOutputs[i] >> TIMER_DEMULTIPLIER;
		int16_t v = limit((int16_t)-PPM_range, ch, (int16_t)PPM_range) + PPM_CENTER*TIMER_MULTIPLIER;
		rest -= v;
		*pulses1MHzWPtr++ = q;
		*pulses1MHzWPtr++ = v - q; // Total pulse width without channel sync pulse.
	}

	*pulses1MHzWPtr++ = q;
	
	rest = limit((uint16_t)9000, rest, (uint16_t)653535); // Prevents overflows.
	
	*pulses1MHzWPtr++ = rest;
	*pulses1MHzWPtr = 0;			// End array with (uint16_t) 0;
	
	//reset pointer to the first element of frame
	pulses1MHzWPtr = &pulses1MHz[0];
}

//run every 10ms
//have TODO something with synchronization
void getADC()
{
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
		//save result
		channelOutputs[adc_input] = temp_ana - 1024;			
	}
}


int main(void)
{
	//////////////////////////////////////////////////////////////////////////
	/// setup registers
	//////////////////////////////////////////////////////////////////////////
	DDRB = 0b11111111;  PORTB = 0b11111111; // All Outputs PullUp
	DDRD = 0b11111101;  PORTD = 0b00000010; // PD1 - bind Input PullUp
	DDRE = 0b00010010;  PORTE = 0b11111110; // PE4 - led, PE1 - TX, PE0 - RX
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
	TCCR0	= (7 << CS00);				// Norm mode, clk/1024 (1 << CS00 | 1 << CS01 | 1 << CS02
	TIMSK	= (1<<OCIE0) | (1<<TOIE0);	// COMP - 78 (100Hz), OVF - 255 (30.64Hz)
	//////////////////////////////////////////////////////////////////////////
	
	//prepopulate ppm frame 
	setupPulsesPPM(PPM);
	
	//Timer counts with frequency 1MHz
	TCCR1A	= 0;							// Clear timer registers
	TCCR1B	= 0;							// Normal mode
	TCNT1	= 0;							// starts from 0
	// An interrupt can be generated at each time the counter value reaches the TOP value by either using the OCF1A or ICF1 Flag,
	// depending on the actual CTC mode. If the interrupt is enabled, the interrupt handler routine can be used for updating the TOP value.
	#if 0									// The counter value (TCNT1) increases until a compare match occurs with OCR1A, and then TCNT1 is cleared.	
	TCCR1B	|= 1<<WGM12;					// code 4 (CTC, compare with OCR1A, Immediate update of OCR1x, overflow on MAX)
	#elif 0								// The counter value (TCNT1) increases until a compare match occurs with ICR1, and then TCNT1 is cleared.	
	TCCR1B	|= 1<<WGM13 | 1<<WGM12;			// code 12 (CTC, compare with ICR1, Immediate update of OCR1x, overflow on MAX)	
	#endif
	TCCR1B	|= 1<<CS11;						// F_TIMER1 = 1MHz (F_CPU/8), 1 tick per 1 microsecond
	TCCR1C	= 0;
	RF_TIMER_COMPA_REG	= 20000;			// Next frame starts in 20 mS	
	TIMSK	|= (1<<OCIE1A) | (1 << TOIE1);	// Enable output compare and overflow	
	//////////////////////////////////////////////////////////////////////////
	  
	//////////////////////////////////////////////////////////////////////////
	//setup UARTS
	//////////////////////////////////////////////////////////////////////////
	
	Serial0.init(115200);
	
	//////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////
	
	Serial0.println("pulses1MHzWPtr initial value:");
	uint16_t val;
	do 
	{
		val = *pulses1MHzWPtr++;
		Serial0.println(val);
	} while (*pulses1MHzWPtr != 0);
 	
	val = *pulses1MHzWPtr++;
	Serial0.println(val);
 	Serial0.println("");
 	
 	
 	pulses1MHzWPtr = &pulses1MHz[0];
	 
	 Serial0.println(OCR1A);
	 Serial0.println(TCNT1);
	 
	//////////////////////////////////////////////////////////////////////////
	//framesPtr = &frames[0];
	
	sei();	// Enable global interrupts
		
	/* Replace with your application code */
	while (1)
	{
		//_delay_ms(1000);
		//int dt = isr1 - latency - 500;
		//latency = isr1;
		//cli();
		//TIMSK	&= ~(1<<OCIE1A);
		//for(uint16_t i = 0; i < 300; i++)
		//{
			//Serial0.println(frames[i]);
		//}
		//Serial0.println(isr1);
		//if(t == 0)
		//{
			//for(uint8_t i = 0; i < 6; i++)
				//Serial0.println(channelOutputs[i] >> TIMER_DEMULTIPLIER);
				//Serial0.println("");
		//}
		//else
		//{
			//t++;
		//}
	}
}



ISR(TIMER_10MS_VECT) //10ms timer
{
	// Clocks every 9.984ms & 10.112ms
	static uint8_t accuracy = 0;

	// Clock correction
	// 7 out of 8 times count to 9.984ms and 1 of 8 to 10.112ms
	TIMER_10MS_COMPVAL += (++accuracy & 0x07) ? 78 : 79;
	
	++g_tmr10ms;
}

ISR(TIMER0_OVF_vect)	//continuous timer 32.64ms (6MHz/1024)
{
	// does not work! TODO: Check what is going on!
	//getADC();
}

void setPinState(uint8_t state)
{
	if(state)
	{
		OUT_PORT |= OUT_PIN;
	}
	else
	{
		OUT_PORT &= ~OUT_PIN;
	}
}

ISR(RF_TIMER_COMPA_VECT)
{
	uint16_t pulse = *pulses1MHzWPtr++;
	
	if(pulse != 0)
	{
		togglePin();
		RF_TIMER_COMPA_REG += pulse;
	}
	
	//next pulse is termination
	if(*pulses1MHzWPtr == 0)
	{
		setPinState(PULSEPOL);
		blinkLed();
		pulses1MHzWPtr = pulses1MHz;
	}
	
	
	/*
	if(timer_callback)
	{		
		timer_counts = timer_callback(); // Function pointer e.g. skyartec_cb().

		if(!timer_counts)
		{
			//bink(5);	
			PROTO_Cmds(PROTOCMD_RESET);
			return;
		}
		RF_TIMER_COMPA_REG += timer_counts ;
		
	}
	
	
	if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
	if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;	
	*/
}

ISR(TIMER1_OVF_vect)
{
	//blinkLed();
}