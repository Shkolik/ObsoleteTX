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

uint8_t flashCounter;
uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;

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

FlightModeData *flightModeAddress(uint8_t idx)
{
	return &g_model.flightModeData[idx];
}

ExpoData *expoAddress(uint8_t idx )
{
	return &g_model.expoData[idx];
}

MixData *mixAddress(uint8_t idx)
{
	return &g_model.mixData[idx];
}

LimitData *limitAddress(uint8_t idx)
{
	return &g_model.limitData[idx];
}

int16_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
	FlightModeData *p = flightModeAddress(phase);
	return p->trim[idx];
}

int16_t getTrimValue(uint8_t phase, uint8_t idx)
{
	return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
}

void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim)
{
	FlightModeData *p = flightModeAddress(phase);
	p->trim[idx] = trim;
	eeDirty(EE_MODEL);
}

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx)
{
	for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++)
	{
		if (phase == 0) return 0;
		int16_t trim = getRawTrimValue(phase, idx);
		if (trim <= TRIM_EXTENDED_MAX) return phase;
		uint8_t result = trim-TRIM_EXTENDED_MAX-1;
		if (result >= phase) ++result;
		phase = result;
	}
	return 0;
}

uint8_t thrSource;
bool enableThr = false;

void setThrSource()
{
	uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
	if (idx > MIXSRC_Thr)
		++idx;
	if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
		idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
	thrSource = idx;
}

void checkBattery()
{
	uint32_t instant_vbat = analogIn(TX_VOLTAGE);

	// Replace with correct values on DX6I board
	#define BANDGAP 3300 // 3.3 Volts : We use AVCC.
	instant_vbat *= 2889L*(BANDGAP/100);
	instant_vbat /= 2047L*100L;
	instant_vbat += 38L;
	instant_vbat += g_general.voltageCalibration;
	// Schottky Diode drops 0.38V before a potential divider which reduces the input to the ADC by 1/2.8889.
	
	if (!g_vbat10mV)
	{
		g_vbat10mV = instant_vbat;
	}

	g_vbat10mV = ((g_vbat10mV << 1) + instant_vbat) / 3; // Simple low pass filter

	if (IS_TXBATT_WARNING() && (g_vbat10mV > (g_general.vBatMin*9)) && ((sessionTimer&0x0F)==0x04))
	{ 
		// No Audio Alarm if TX Battery < VCCMIN X .9 & 30 Sec
		AUDIO_TX_BATTERY_LOW();
	}
}

uint16_t s_analogFiltered[NUMBER_ANALOG];

uint16_t analogIn(uint8_t channel)
{
	static const pm_char crossAnalog[] PROGMEM = {3,1,2,0,4,5,6,7};
	uint16_t *p = &s_analogFiltered[pgm_read_byte_near(crossAnalog+channel)];
	return *p;
}

void evalTrims()
{
	uint8_t phase = mixerCurrentFlightMode;
	for (uint8_t i=0; i<NUM_STICKS; i++)
	{
		// do trim -> throttle trim if applicable
		int16_t trim = getTrimValue(phase, i);
		if (i==THR_STICK && g_model.thrTrim)
		{
			int16_t trimMin = g_model.extendedTrims ? TRIM_EXTENDED_MIN : TRIM_MIN;
			trim = enableThr? ((((int32_t)(trim-trimMin)) * (RESX-analogs[i])) >> (RESX_SHIFT+1)) : 0; //GAZ SECURITY
		}
		if (trimsCheckTimer)
		{
			trim = 0;
		}
		trims[i] = trim*2;
	}
}


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
	
	for (uint8_t i=0; i<NUM_STICKS; ++i)
	{
		MixData *mix = mixAddress(i);
		mix->destCh = i;
		mix->weight = 100;
		mix->srcRaw = MIXSRC_Rud - 1 + channel_order(i+1);
	}
	//g_model.PPMNCH = 2; // 8Ch	
}
#endif

void modelDefault(uint8_t id)
{
	memset(&g_model, 0, sizeof(g_model));
	applyDefaultTemplate();
	g_model.modelId = id+1;
}

void generalDefault()
{
	memclear(&g_general, sizeof(g_general));
	
	g_general.version  = EEPROM_VER;
	g_general.rfModuleType = MODULE_MULTI;
	g_general.currentModel = 0;
	g_general.contrast = 33;
	g_general.vBatWarning = 60;	
	g_general.vBatMin = 50;
	g_general.vBatMax = 90;
	g_general.stickMode = DEFAULT_MODE - 1;
	g_general.inactivityTimer = 10;
	g_general.checkSum = 0xFFFF;
}

void flightReset()
{	
	if (!IS_MANUAL_RESET_TIMER(0))
	{
		timerReset(0);
	}

	#if TIMERS > 1
	if (!IS_MANUAL_RESET_TIMER(1))
	{
		timerReset(1);
	}
	#endif
	
	s_mixer_first_run_done = false;

	//RESET_THR_TRACE();
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

bool s_mixer_first_run_done = false;

void doMixerCalculations()
{
	static uint16_t lastTMR = 0;

	uint16_t tmr10ms = g_tmr10ms;
	uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
	// handle tick10ms overrun
	// correct overflow handling costs a lot of code; happens only each 11 min;
	// therefore forget the exact calculation and use only 1 instead; good compromise

	lastTMR = tmr10ms;

	getADC();

	evalMixes(tick10ms);

	if (tick10ms)
	{
		/* Throttle trace */
		int16_t val;

		if (g_model.thrTraceSrc > NUM_POTS)
		{
			uint8_t ch = g_model.thrTraceSrc-NUM_POTS-1;
			val = channelOutputs[ch];

			LimitData *lim = limitAddress(ch);
			int16_t gModelMax = LIMIT_MAX_RESX(lim);
			int16_t gModelMin = LIMIT_MIN_RESX(lim);

			if (lim->revert)
				val = -val + gModelMax;
			else
			val = val - gModelMin;

			#if defined(PPM_LIMITS_SYMETRICAL)
				if (lim->symetrical)
				{
					val -= calc1000toRESX(lim->offset);
				}
			#endif

			gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
			// usually max is 1024 min is -1024 --> max-min = 2048 full range

			if (gModelMax != 0 && gModelMax != 2048)
				val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs

			if (val < 0)
				val = 0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
		}
		else
		{
			val = RESX + (g_model.thrTraceSrc == 0 ? rawAnalogs[THR_STICK] : calibratedStick[g_model.thrTraceSrc + NUM_STICKS-1]);
		}

		val >>= (RESX_SHIFT - 6); // calibrate it (resolution increased by factor 4)

		evalTimers(val, tick10ms);

		static uint8_t  s_cnt_100ms;
		static uint8_t  s_cnt_1s;
		static uint8_t  s_cnt_samples_thr_1s;
		static uint16_t s_sum_samples_thr_1s;

		++s_cnt_samples_thr_1s;
		s_sum_samples_thr_1s+=val;

		if ((s_cnt_100ms += tick10ms) >= 10)
		{ // 0.1sec
			s_cnt_100ms -= 10;
			s_cnt_1s += 1;

			logicalSwitchesTimerTick();

			if (s_cnt_1s >= 10)
			{ // 1sec
				s_cnt_1s -= 10;
				++sessionTimer;
				checkBattery();

				if ((rangeModeIsOn) && !(MENU_MODEL_SETUP))
				{
					rangeModeIsOn = false; // Reset range mode if not in menuModelSetup
				}

				struct t_inactivity *ptrInactivity = &inactivity;
				FORCE_INDIRECT(ptrInactivity) ;
				ptrInactivity->counter++;
				if ((((uint8_t)ptrInactivity->counter)&0x07)==0x01 && g_general.inactivityTimer && ptrInactivity->counter > ((uint16_t)g_general.inactivityTimer*60))
				AUDIO_INACTIVITY();

				#if defined(AUDIO)
				if (mixWarning & 1) if ((sessionTimer&0x03)==0) AUDIO_MIX_WARNING(1);
				if (mixWarning & 2) if ((sessionTimer&0x03)==1) AUDIO_MIX_WARNING(2);
				if (mixWarning & 4) if ((sessionTimer&0x03)==2) AUDIO_MIX_WARNING(3);
				#endif

				val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
				s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
				if (val) s_timeCumThr += 1;
				s_sum_samples_thr_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this

				#if defined(THRTRACE)
				// throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
				// in case buffer runs out, it wraps around
				// resolution for y axis is only 32, therefore no higher value makes sense
				s_cnt_samples_thr_10s += s_cnt_samples_thr_1s;
				s_sum_samples_thr_10s += s_sum_samples_thr_1s;

				if (++s_cnt_10s >= 10)
				{ // 10s
					s_cnt_10s -= 10;
					val = s_sum_samples_thr_10s / s_cnt_samples_thr_10s;
					s_sum_samples_thr_10s = 0;
					s_cnt_samples_thr_10s = 0;
					s_traceBuf[s_traceWr++] = val;
					if (s_traceWr >= MAXTRACE)
					s_traceWr = 0;
					if (s_traceCnt >= 0)
					++s_traceCnt;
				}
				#endif

				s_cnt_samples_thr_1s = 0;
				s_sum_samples_thr_1s = 0;
			}
		}
	}
	s_mixer_first_run_done = true;
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
	
	doMixerCalculations();
	
	//Start protocol here!
	startPulses(g_general.rfModuleType);
	
	//Enable watchdog.
	wdt_enable(WDTO_500MS);
	debugln("WD Timer enabled!");
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
	
	//sendStopPulses();
	//g_general.rfModuleType = MODULE_MULTI;
	//eeDirty(EE_GENERAL);
	//eeDirty(EE_MODEL);
	//startPulses(g_general.rfModuleType);
	
	
	/* Replace with your application code */
	while (1)
	{
		_delay_ms(1);
		//perMain();		
		if (heartbeat == HEART_WDT_CHECK) {			
			wdt_reset();
			//debugln("heartbeat");
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