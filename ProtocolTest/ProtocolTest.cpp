/*
 * ProtocolTest.cpp
 *
 * Created: 1/31/2020 1:07:25 PM
 * Author : andrew.shkolik
 */


#include "main.h"
uint16_t *pulses1MHzWPtr;
uint16_t pulses1MHz[PULSES_WORD_SIZE];
int16_t channelOutputs[PPMCHMAX];

volatile uint16_t g_tmr10ms;
volatile uint8_t g_tmr32ms;

uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;

uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = 100;
uint16_t dt;

Proto_struct Protos[] = {
	{ PROTOCOL_PPM, PROTO_PPM_Cmds }
	//{ PROTOCOL_DSM_SERIAL, DSM_SERIAL_Cmds },
	//{ PROTOCOL_MULTI, MULTI_Cmds }
};

FORCEINLINE uint8_t pulsesStarted()
{
	return (s_current_protocol != 255);
}

uint16_t (*timer_callback)(void);
const void * (*PROTO_Cmds)(enum ProtoCmds);


void PROTO_Start_Callback( uint16_t (*cb)())
{
	if(!cb)
	{
		blinkLed(2);
		Serial0.println("Callback not set!");
		return;
	}

	// Start protocol callback in 16ms
	//SCHEDULE_MIXER_END_IN_US(16000U);	// Schedule next Mixer calculations.
	timer_callback = cb;				// timer_callback = pointer to function.

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		RF_TIMER_COMPA_REG = 16000U;	// 1 count is 1us.
	}

	TIMSK |= 1<<OCIE1A; // Enable Output Compare A interrupt.
}

void PROTO_Stop_Callback()
{
	TIMSK	&= ~(1<<OCIE1A);	// Disable Output Compare A interrupt.
	timer_callback = NULL;
}

void sendStopPulses()
{
	PROTO_Cmds(PROTOCMD_RESET);
	PROTO_Stop_Callback();
}

void startPulses(enum Protocols protocol)
{
	//stop transmit
	PROTO_Stop_Callback();

	//reset module
	if (pulsesStarted())
	{
		Serial0.println("Stop pulses");
		PROTO_Cmds(PROTOCMD_RESET);
	}
	//set protocol
	s_current_protocol = protocol;
	//get callbacks
	PROTO_Cmds = *Protos[s_current_protocol].Cmds;
	Serial0.println("get callback");
	Serial0.println(Protos[s_current_protocol].Protocol);
	//run command
	PROTO_Cmds(PROTOCMD_INIT);
}

void blinkLed(uint8_t count)
{
	count++;
	while(count > 0)
	{
		count--;
		LED_PORT ^= LED_PIN;
		_delay_ms(10);
	}
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

void togglePin()
{
	OUT_PORT ^= OUT_PIN;
}

static uint16_t getTmr128uS()
{
	while(1){
		uint8_t hb  = g_tmr32ms;
		uint8_t lb  = TCNT0;
		if(hb-g_tmr32ms==0) return (hb<<8)|lb;
	}
}

//run every frame after the end of transmitting synchronization pulse
void setupPulsesPPM()
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
	//Interrupt will be enabled by protocol callback
	//When started
	//TIMSK	|= (1<<OCIE1A);				// Enable output compare
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//setup UARTS
	//////////////////////////////////////////////////////////////////////////
	Serial0.init(115200);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////

	sei();	// Enable global interrupts

	//Start protocol here!
	startPulses(PROTOCOL_PPM);

	/* Replace with your application code */
	while (1)
	{
		Serial0.println("Max ISR latency:");
		Serial0.println(g_tmr1Latency_max);
		Serial0.println("Min ISR latency:");
		Serial0.println(g_tmr1Latency_min);
		_delay_ms(100);
	}
}

//10ms timer
ISR(TIMER_10MS_VECT, ISR_NOBLOCK )
{
	// Clocks every 9.984ms & 10.112ms
	static uint8_t accuracy = 0;

	// Clock correction
	// 7 out of 8 times count to 9.984ms and 1 of 8 to 10.112ms
	TIMER_10MS_COMPVAL += (++accuracy & 0x07) ? 78 : 79;

	++g_tmr10ms;
	getADC();
}

//continuous timer, call every 32.64ms (8MHz/1024)
ISR(TIMER0_OVF_vect, ISR_NOBLOCK )
{
	g_tmr32ms++;
}

// 1MHz clock, normal node, OCR1A for compare
ISR(RF_TIMER_COMPA_VECT)
{
	if(timer_callback)
	{
		uint16_t timer_counts = timer_callback(); // Protocol function pointer

		if(!timer_counts)
		{
			blinkLed(5);
			PROTO_Cmds(PROTOCMD_RESET);
			return;
		}
		RF_TIMER_COMPA_REG += timer_counts ;
	}
}