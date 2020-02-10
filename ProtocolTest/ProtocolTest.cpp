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

uint16_t g_tmr10ms;
volatile uint8_t g_tmr32ms;

uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;

uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = 100;
uint16_t dt;

uint8_t heartbeat;
uint16_t bind_tmr10ms = 0;
uint8_t change_debounce_tmr10ms = 0;

const ModelData g_model = {
	/*uint8_t   modelId:6;*/9,
	/*uint8_t   rfProtocol:6;*/MM_RF_PROTO_MJXQ,
	/*uint8_t   rfSubType:4;*/4,//e010
	/*int8_t    rfOptionValue1;*/MM_RF_PROTO_MJXQ,//mjxq
	/*int8_t    rfOptionValue2;*/0,
	/*uint8_t   rfOptionValue3:5;*/0,
	/*uint8_t   rfOptionBool1:1;*/0,
	/*uint8_t   rfOptionBool2:1;*/0,
	/*uint8_t   rfOptionBool3:1;*/0,
};

uint16_t (*timer_callback)(void);
const void * (*PROTO_Cmds)(enum ProtoCmds);

bool rangeModeIsOn = false; // manage low power TX
uint8_t protoMode = NORMAL_MODE;

Proto_struct Protos[] = {
	{ PROTOCOL_PPM, PROTO_PPM_Cmds },
	//{ PROTOCOL_DSM_SERIAL, 0},//DSM_SERIAL_Cmds },
	{ PROTOCOL_MULTI, MULTI_Cmds }
};

FORCEINLINE uint8_t pulsesStarted()
{
	return (s_current_protocol != 255);
}

void PROTO_Start_Callback( uint16_t (*cb)())
{
	if(!cb)
	{
		debugln("Protocol callback not set");
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

void PROTO_SetBindState(uint16_t t10ms)
{
	if(t10ms)
	{
		protoMode = BIND_MODE;
		bind_tmr10ms = t10ms;
	}
	else 
	{		
		protoMode = NORMAL_MODE;
	}
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
		PROTO_Cmds(PROTOCMD_RESET);
	}
	//set protocol
	s_current_protocol = protocol;
	debugln("Current protocol:");
	debugln(s_current_protocol);
	//get callbacks
	PROTO_Cmds = *Protos[s_current_protocol].Cmds;	
	
	//run command
	PROTO_Cmds(PROTOCMD_INIT);
}

void nextProtocol()
{
	if(s_current_protocol < PROTOCOL_COUNT - 1)
		startPulses((Protocols)(s_current_protocol+1));
	else
		startPulses(PROTOCOL_PPM);
}

//TODO: Can be replaced with macros
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

//TODO: Can be replaced with macros
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
	
	//F0 for some reason connected with F1 on my board... 
	//TODO: find out why...
	//for (uint8_t adc_input=0; adc_input<6; adc_input++)
	for (uint8_t adc_input=1; adc_input<5; adc_input++)
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
		channelOutputs[adc_input-1] = temp_ana - 1024;
		//see above
		//channelOutputs[adc_input] = temp_ana - 1024;
	}
	for(uint8_t i = 5; i<PPMCHMAX; i++)
		channelOutputs[i] = -1024;
}

uint8_t switchState(EnumKeys key)
{
	uint8_t result = 0 ;

	//if (key < NUM_KEYS)
	//return keys[enuk].state() ? 1 : 0;
	//
	switch(key) 
	{
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
	
		case SW_THR:
		result = IS_PIN_LOW(PING, 4);
		break;
	
		case SW_TRN:
		result = IS_PIN_LOW(PING, 3);
		break;
	
		default:break;
	}

	return result;
}

void readKeysAndTrims()
{
	//uint8_t index = KEY_MENU;

	// Test buttons ...
	uint8_t in = ~PING;
	
	keys[1].input(in & (1<<3));
	keys[2].input(in & (1<<4));
	
	
	//// User buttons ...
	//uint8_t in = ~PINB;
	//for (int i=1; i<7; i++) {
		//keys[index].input(in & (1<<i));
		//++index;
	//}
//
	//// Trims ...
	//in = ~PIND;
	//for (int i=0; i<8; i++) {
		//// INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
		//keys[index].input(trimHelper(in, i));
		//++index;
	//}
//
	//#if defined(ROTARY_ENCODER_NAVIGATION)
	//keys[index].input(ROTENC_DOWN()); // Rotary Enc. Switch
	//#endif
//
	//#if defined(NAVIGATION_STICKS)
	//if (~PINB & 0x7E) {
		//StickScrollTimer = STICK_SCROLL_TIMEOUT;
	//}
	//#endif
}

void per10ms()
{
	readKeysAndTrims();
	
	uint8_t evt = getEvent();
	
	switch(evt)
	{
		case EVT_KEY_FIRST(KEY_BIND):
			PORTE &= ~( 1<< 4);
			protoMode = BIND_MODE;
			debugln("Set bind");
			break;
		case EVT_KEY_BREAK(KEY_BIND):
			protoMode = NORMAL_MODE;
			PORTE |= ( 1<< 4);
			debugln("Set normal");
			break;
		case EVT_KEY_FIRST(KEY_CHANGE):
			nextProtocol();
			break;		
	}
	
	//if(BIND_PRESSED && protoMode == NORMAL_MODE)
	//{
		//PORTE &= ~( 1<< 4);
		//PROTO_SetBindState(300);
	//}
	//
	//if(CHANGE_PRESSED)
	//{
		//if(change_debounce_tmr10ms > 0)
			//change_debounce_tmr10ms--;
		//else
			//nextProtocol();
	//}
	if (bind_tmr10ms > 0)
	{
		//check if bind button was released
		if(!BIND_PRESSED)
		{
			bind_tmr10ms--;
			
			if (0 == bind_tmr10ms)
			{
				protoMode = NORMAL_MODE;
				PORTE |= ( 1<< 4);
				debugln("Set normal");
			}
		}		
	}
}

int main(void)
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

	//////////////////////////////////////////////////////////////////////////
	//setup UART for debug
	//////////////////////////////////////////////////////////////////////////
	init_debug(115200);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	sei();	// Enable global interrupts

	//Start protocol here!
	startPulses(PROTOCOL_MULTI);
	//startPulses(PROTOCOL_PPM);
	
	/* Replace with your application code */
	while (1)
	{
		if (heartbeat == HEART_WDT_CHECK) {
			wdt_reset();
			heartbeat = 0;
		}
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
	per10ms();
	getADC();
	heartbeat |= HEART_TIMER_10MS;
}

//continuous timer, call every 32.64ms (8MHz/1024)
ISR(TIMER0_OVF_vect, ISR_NOBLOCK )
{
	g_tmr32ms++;
}

// 1MHz clock, normal node, OCR1A for compare
ISR(RF_TIMER_COMPA_VECT, ISR_NOBLOCK)
{
	if(timer_callback)
	{
		uint16_t timer_counts = timer_callback(); // Protocol function pointer
		if(!timer_counts)
		{
			PROTO_Cmds(PROTOCMD_RESET);
			return;
		}
		RF_TIMER_COMPA_REG += timer_counts ;
	}
}