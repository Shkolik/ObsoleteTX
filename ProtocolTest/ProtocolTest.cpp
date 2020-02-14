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


#include "main.h"
int16_t channelOutputs[CHMAX];

uint16_t g_tmr10ms;
volatile uint8_t g_tmr32ms;

uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;

uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = 100;
uint16_t dt;

uint8_t heartbeat;
uint8_t stickMode;

uint16_t bind_tmr10ms = 0;
uint8_t change_debounce_tmr10ms = 0;

bool unexpectedShutdown = false;

ModelSettings g_model;
GeneralSettings g_general;

const pm_uint8_t bchout_ar[] PROGMEM = {
	0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
	0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
	0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
	0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4
};

uint8_t channel_order(uint8_t x)
{
	return ( ((pgm_read_byte_near(bchout_ar + g_general.templateSetup) >> (6-(x-1) * 2)) & 3 ) + 1 );
}

/*
mode1 rud ele thr ail
mode2 rud thr ele ail
mode3 ail ele thr rud
mode4 ail thr ele rud
*/
const pm_uint8_t modn12x3[] PROGMEM = {
	0, 1, 2, 3,
	0, 2, 1, 3,
	3, 1, 2, 0,
	3, 2, 1, 0
};

#if defined(TEMPLATES)
inline void applyDefaultTemplate()
{
	applyTemplate(TMPL_SIMPLE_4CH); // calls eeDirty internally
	g_model.PPMNCH = 2; // 8Ch
}
#else
void applyDefaultTemplate()
{
	eeDirty(EE_MODEL);
	/* TODO: ADD mixes to model
	for (uint8_t i=0; i<NUM_STICKS; ++i)
	{
		MixData *mix = mixAddress(i);
		mix->destCh = i;
		mix->weight = 100;
		mix->srcRaw = MIXSRC_Rud - 1 + channel_order(i+1);
	}
	g_model.PPMNCH = 2; // 8Ch
	*/
}
#endif

void modelDefault(uint8_t id)
{
	memset(&g_model, 0, sizeof(g_model));
	//applyDefaultTemplate();
	g_model.modelId = id+1;
}

void generalDefault()
{
	memclear(&g_general, sizeof(g_general));
	
	g_general.version  = EEPROM_VER;
	//g_general.rfModuleType = MODULE_MULTI;
	g_general.currentModel = 0;
	g_general.contrast = 33;
	g_general.vBatWarning = 60;	
	g_general.vBatMin = 50;
	g_general.vBatMax = 90;
	g_general.stickMode = DEFAULT_MODE - 1;
	g_general.inactivityTimer = 10;
	g_general.checkSum = 0xFFFF;
}


uint16_t (*timer_callback)(void);
const void * (*PROTO_Cmds)(enum ProtoCmds);

bool rangeModeIsOn = false; // manage low power TX
uint8_t protoMode = NORMAL_MODE;

Module_struct Protos[] = {
	{ MODULE_PPM, PPM_Cmds },
#ifdef DSM
	{ MODULE_DSM, DSM_Cmds },
#endif
#ifdef MULTI
	{ MODULE_MULTI, MULTI_Cmds }
#endif
};

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


void startPulses(enum ModuleType module)
{
	//stop transmit
	PROTO_Stop_Callback();

	//reset module
	if (pulsesStarted())
	{		
		PROTO_Cmds(PROTOCMD_RESET);
	}
	//set protocol
	s_current_protocol = module;
	debugln("Current protocol:");
	debugln(s_current_protocol);
	
	//////////////////////////////////////////////////////////////////////////
	//For test only
	//TODO:Remove when UI ready
	switch(s_current_protocol)
	{
		case MODULE_PPM:
		g_general.rfModuleType = MODULE_PPM;
		g_model.PPMNCH = 6;
		g_model.PPMDELAY = 0;
		g_model.PPMFRAMELENGTH = 0;
		g_model.PULSEPOL = 0;
		break;
		case MODULE_DSM:
		g_general.rfModuleType = MODULE_DSM;
		g_model.DSM_TYPE = Sub_DSM2;
		break;
		
		case MODULE_MULTI:
		g_general.rfModuleType = MODULE_MULTI;
		g_model.MULTIRFPROTOCOL = MM_RF_PROTO_MJXQ;
		g_model.SUB_TYPE = 4;
		break;
	}
	//////////////////////////////////////////////////////////////////////////
	
	//get callbacks
	PROTO_Cmds = *Protos[s_current_protocol].Cmds;	
	
	//run command
	PROTO_Cmds(PROTOCMD_INIT);
}

void nextProtocol()
{
	if(s_current_protocol < MODULETYPES_COUNT - 1)
		startPulses((ModuleType)(s_current_protocol+1));
	else
		startPulses(MODULE_PPM);
}

static uint16_t getTmr128uS()
{
	while(1){
		uint8_t hb  = g_tmr32ms;
		uint8_t lb  = TCNT0;
		if(hb-g_tmr32ms==0) return (hb<<8)|lb;
	}
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

void doMixerCalculations()
{
}

void doSplash()
{
	debugln("Welcome to ObsoleteTX V1.0");
	
	//debug("GeneralSettings size: ");
	//debugln(sizeof(GeneralSettings)); //67
	//
	//debug("ModelSettings size: ");
	//debugln(sizeof(ModelSettings)); //16
}

void Start() // Run only if it is not a WDT reboot
{
	doSplash();
	/*
	#if defined(GUI)
	checkAlarm();
	checkAll();
	//corrupted eeprom or firs run
	if (g_eeGeneral.chkSum != evalChkSum())
	{
		chainMenu(menuFirstCalib);
	}
	#endif*/
}

void Init()
{
	eeReadAll();
	
	Start(); // All functions called in Start() are not used if a WDT reset occur.
	
	doMixerCalculations();

	//if(BIND_PRESSED)
	//{
	//debug("BIND on STARTUP");
	////if started with bind pressed
	//PROTO_SetBindState(500);
	//}
	
	//Start protocol here!
	startPulses(g_general.rfModuleType);
	//startPulses(MODULE_PPM);
	//Enable watchdog.
	wdt_enable(WDTO_500MS);
}

void perMain()
{

}

int main(void)
{
	MCUCSR = 0;
	wdt_disable();
	
	boardInit();
		
	//////////////////////////////////////////////////////////////////////////
	//setup UART for debug
	//////////////////////////////////////////////////////////////////////////
	init_debug(115200);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	sei();	// Enable global interrupts

	Init();
	
	/* Replace with your application code */
	while (1)
	{
		_delay_ms(15);
		//perMain();		
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