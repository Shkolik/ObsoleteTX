/*
 * GccApplication11.cpp
 *
 * Created: 1/24/2020 8:33:44 PM
 * Author : Andrew
 */ 


#include "ObsoleteTX.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

bool pwrCheck = true;
bool unexpectedShutdown = false;

/* AVR: mixer duration in 1/16ms */
uint16_t maxMixerDuration;
uint16_t lastMixerDuration;
uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = -1;
uint16_t g_guibuild_max;
uint16_t g_guibuild_min = -1;
uint16_t g_lcddraw_max;
uint16_t g_lcddraw_min = -1;

volatile uint8_t g_tmr128uS;
volatile uint16_t g_tmr10ms;

#if defined(AUDIO)
audioQueue  audio;
#endif

void checkMixer()
{
	
	// TODO duplicated code ...
	uint16_t t0 = getTmr64uS();
	int16_t delta = (nextMixerEndTime - lastMixerDuration) - t0;
	if ((delta > 0 && delta < US_TO_64US_TICK(MAX_MIXER_DELTA_US)) || (!s_mixer_first_run_done))
	{
		return;
	}

	nextMixerEndTime = t0 + US_TO_64US_TICK(MAX_MIXER_DELTA_US);

	doMixerCalculations();

	t0 = getTmr64uS() - t0;
	lastMixerDuration = t0;
	if (t0 > maxMixerDuration)
	maxMixerDuration = t0;
}


uint8_t heartbeat;
uint8_t stickMode;



#if defined(OVERRIDE_CHANNEL_FUNCTION)
safetych_t safetyCh[NUM_CHNOUT];
#endif

uint8_t gazSource;
bool enableGaz = false;

uint16_t (*timer_callback)(void);

const RfOptionSettingsvarstruct RfOpt_PPM_Ser[] PROGMEM = {
	/*rfProtoNeed*/BOOL1USED,
	/*rfSubTypeMax*/6,
	/*rfOptionValue1Min*/-20,
	/*rfOptionValue1Max*/35,
	/*rfOptionValue2Min*/-4,
	/*rfOptionValue2Max*/10,
	/*rfOptionValue3Max*/0,
};

void setGazSource()
{
	uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
	if (idx > MIXSRC_Thr)
	++idx;
	if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
	idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
	gazSource = idx;
}

bool rangeModeIsOn = false; // manage low power TX
uint8_t protoMode = NORMAL_MODE;

void sendOptionsSettingsPpm()
{
	SetRfOptionSettings(pgm_get_far_address(RfOpt_PPM_Ser), // Used by PPM
	STR_DUMMY,
	STR_DUMMY,
	STR_DUMMY,
	STR_DUMMY,
	STR_DUMMY,
	STR_DUMMY,
	STR_DUMMY);	
	g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
	protoMode = NORMAL_MODE;
}

struct RfOptionSettingsstruct RfOptionSettings; // used in menu model setup

void SetRfOptionSettings(uint_farptr_t RfOptSet,
						const pm_char* rfSubTypeNames,
						const pm_char* rfOptionValue1Name,
						const pm_char* rfOptionValue2Name,
						const pm_char* rfOptionValue3Name,
						const pm_char* rfOptionBool1Name,
						const pm_char* rfOptionBool2Name,
						const pm_char* rfOptionBool3Name)
{
	uint8_t tmp = pgm_read_byte_far(RfOptSet);
	RfOptionSettings.rfProtoNeed = (tmp & 0x0F);                                      // rfProtoNeed:4
	RfOptionSettings.rfSubTypeMax = (tmp >> 4);                                        // rfSubTypeMax:4
	RfOptionSettings.rfSubTypeNames = rfSubTypeNames;
	RfOptionSettings.rfOptionValue1Min = pgm_read_byte_far(++RfOptSet);
	RfOptionSettings.rfOptionValue1Max = pgm_read_byte_far(++RfOptSet);
	RfOptionSettings.rfOptionValue1Name = rfOptionValue1Name;
	RfOptionSettings.rfOptionValue2Min = pgm_read_byte_far(++RfOptSet);
	RfOptionSettings.rfOptionValue2Max = pgm_read_byte_far(++RfOptSet);
	RfOptionSettings.rfOptionValue2Name = rfOptionValue2Name;
	RfOptionSettings.rfOptionValue3Max = (pgm_read_byte_far(++RfOptSet) & 0b00011111); // rfOptionValue3Max/*:5*/
	RfOptionSettings.rfOptionValue3Name = rfOptionValue3Name;
	RfOptionSettings.rfOptionBool1Used = ((tmp & BOOL1USED) == BOOL1USED);
	RfOptionSettings.rfOptionBool1Name = rfOptionBool1Name;
	RfOptionSettings.rfOptionBool2Used = ((tmp & BOOL2USED) == BOOL2USED);
	RfOptionSettings.rfOptionBool2Name = rfOptionBool2Name;
	RfOptionSettings.rfOptionBool3Used = ((tmp & BOOL3USED) == BOOL3USED);
	RfOptionSettings.rfOptionBool3Name = rfOptionBool3Name;
}

void LimitRfOptionSettings()
{
	g_model.rfSubType = limit( (uint8_t)0x0, (uint8_t)g_model.rfSubType, (uint8_t)RfOptionSettings.rfSubTypeMax);
	g_model.rfOptionValue1 = limit( RfOptionSettings.rfOptionValue1Min, g_model.rfOptionValue1, RfOptionSettings.rfOptionValue1Max);
	g_model.rfOptionValue2 = limit( RfOptionSettings.rfOptionValue2Min, g_model.rfOptionValue2, RfOptionSettings.rfOptionValue2Max);
	g_model.rfOptionValue3 = limit( (int8_t) -16, (int8_t) g_model.rfOptionValue3, (int8_t) RfOptionSettings.rfOptionValue3Max);
}

union ReusableBuffer reusableBuffer;

const pm_uint8_t bchout_ar[] PROGMEM = {
	0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
	0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
	0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
	0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4
};

uint8_t channel_order(uint8_t x)
{
	return ( ((pgm_read_byte_near(bchout_ar + g_eeGeneral.templateSetup) >> (6-(x-1) * 2)) & 3 ) + 1 );
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

uint16_t Bind_tmr10ms = 0;

void per10ms()
{
	#if defined(GUI)
	if (lightOffCounter) --lightOffCounter;
	if (flashCounter) --flashCounter;
	if (noHighlightCounter) --noHighlightCounter;
	#endif

	if (trimsCheckTimer) --trimsCheckTimer;
	if (ppmInputValidityTimer) --ppmInputValidityTimer;

	readKeysAndTrims();

	#if defined(ROTARY_ENCODER_NAVIGATION)
	if (IS_RE_NAVIGATION_ENABLE()) 
	{
		static rotenc_t rePreviousValue;
		rotenc_t reNewValue = (g_rotenc[NAVIGATION_RE_IDX()] / ROTARY_ENCODER_GRANULARITY);
		int8_t scrollRE = reNewValue - rePreviousValue;
		if (scrollRE) 
		{
			rePreviousValue = reNewValue;
			putEvent(scrollRE < 0 ? EVT_ROTARY_LEFT : EVT_ROTARY_RIGHT);
		}
		uint8_t evt = s_evt;
		if (EVT_KEY_MASK(evt) == BTN_REa + NAVIGATION_RE_IDX()) 
		{
			if (IS_KEY_BREAK(evt)) 
			{
				putEvent(EVT_ROTARY_BREAK);
			} 
			else if (IS_KEY_LONG(evt)) 
			{
				putEvent(EVT_ROTARY_LONG);
			}
		}
	}
	#endif

	#if defined(FRSKY)
	if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
		telemetryInterrupt10ms();
	#endif

	// These moved here from evalFlightModeMixes() to improve beep trigger reliability.
	#if defined(PWM_BACKLIGHT)
	if (EVERY_PERIOD(g_tmr10ms, 4))
		backlightFade(); // increment or decrement brightness until target brightness is reached
	#endif

	#if defined(VOICE_JQ6500)
		//JQ6500Check();
	#endif

	#if !defined(AUDIO)
	if (mixWarning & 1) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,  0)) AUDIO_MIX_WARNING(1);
	if (mixWarning & 2) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 64) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 72)) AUDIO_MIX_WARNING(2);
	if (mixWarning & 4) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,128) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,136) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 144)) AUDIO_MIX_WARNING(3);
	#endif

	#if ROTARY_ENCODERS > 0
	if (rotEncADebounce) 
	{
		if (!(rotEncADebounce >>= 1)) ENABLEROTENCAISR(); // Re enable rotencA isr (deboucing)
	}
	#endif
	#if ROTARY_ENCODERS > 1
	if (rotEncBDebounce) 
	{
		if (!(rotEncBDebounce >>= 1)) ENABLEROTENCBISR(); // Re enable rotencB isr (deboucing)
	}
	#endif

	if (Bind_tmr10ms)
	{
		if (!--Bind_tmr10ms)
			protoMode = NORMAL_MODE;
	}

	heartbeat |= HEART_TIMER_10MS;

}

#if ROTARY_ENCODERS > 0
uint8_t rotEncADebounce;
#endif
#if ROTARY_ENCODERS > 1
uint8_t rotEncBDebounce;
#endif

//////////////////////////////////////////////////////////////////////////
///TODO: WHY IT DOESN'T WORK!
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


void generalDefault()
{
	memclear(&g_eeGeneral, sizeof(g_eeGeneral));
	g_eeGeneral.version  = EEPROM_VER;
	g_eeGeneral.contrast = 33;
	g_eeGeneral.vBatMin = 50;
	g_eeGeneral.vBatMax = 90;
	g_eeGeneral.vBatWarn = 60;
	#if defined(DEFAULT_MODE)
		g_eeGeneral.stickMode = DEFAULT_MODE - 1;
	#endif
	g_eeGeneral.backlightMode = e_backlight_mode_all;
	g_eeGeneral.lightAutoOff = 2;
	g_eeGeneral.inactivityTimer = 10;
	g_eeGeneral.chkSum = 0xFFFF;	
}

uint16_t evalChkSum()
{
	uint16_t sum = 0;
	const int16_t *calibValues = (const int16_t *) &g_eeGeneral.calib[0];
	for (uint8_t i=0; i<12; i++)
		sum += calibValues[i];
	return sum;
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
	g_model.PPMNCH = 2; // 8Ch
}
#endif

void modelDefault(uint8_t id)
{
	memset(&g_model, 0, sizeof(g_model));
	applyDefaultTemplate();
	g_model.modelId = id+1;
}

#if defined(AUTOSOURCE)
int8_t getMovedSource()
{
	int8_t result = 0;
	static tmr10ms_t lastMove10msTick = 0;
	static int16_t sourcesStates[NUM_STICKS+NUM_POTS];

	for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) 
	{
		if (abs(calibratedStick[i] - sourcesStates[i]) > 512) 
		{
			result = MIXSRC_Rud+i;
			break;
		}
	}

	uint8_t recent = (ELAPSED_10MS_TICK_SINCE(lastMove10msTick) >= MS_TO_10MS_TICK(100));
	if (recent) 
	{
		result = 0;
	}

	if (result || recent) 
	{
		memcpy(sourcesStates, calibratedStick, sizeof(sourcesStates));
	}

	lastMove10msTick = getTmr10ms();
	return result;
}
#endif

#if defined(FLIGHT_MODES)
uint8_t getFlightMode()
{
	for (uint8_t i=1; i<MAX_FLIGHT_MODES; i++) 
	{
		FlightModeData *phase = &g_model.flightModeData[i];
		if (phase->swtch && getSwitch(phase->swtch)) 
		{
			return i;
		}
	}
	return 0;
}
#endif

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

#if defined(ROTARY_ENCODERS)
uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
	uint8_t phase = mixerCurrentFlightMode;
	for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) 
	{
		if (phase == 0) return 0;
		int16_t value = flightModeAddress(phase)->rotaryEncoders[idx];
		if (value <= ROTARY_ENCODER_MAX) 
			return phase;
		uint8_t result = value-ROTARY_ENCODER_MAX-1;
		if (result >= phase) 
			++result;
		phase = result;
	}
	return 0;
}

uint16_t getRotaryEncoder(uint8_t idx)
{
	return flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
	g_rotenc[idx] += inc;
	int16_t *value = &(flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
	*value = limit((int16_t)-ROTARY_ENCODER_MAX, (int16_t)(*value + (inc * 8)), (int16_t)+ROTARY_ENCODER_MAX);
	eeDirty(EE_MODEL);
}
#endif

#if defined(GVARS)

uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx)
{
	for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
		if (phase == 0) return 0;
		int16_t val = GVAR_VALUE(idx, phase); // TODO phase at the end everywhere to be consistent!
		if (val <= GVAR_MAX) return phase;
		uint8_t result = val-GVAR_MAX-1;
		if (result >= phase) ++result;
		phase = result;
	}
	return 0;
}

int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase)
{
	if (GV_IS_GV_VALUE(x, min, max)) 
	{
		int8_t idx = GV_INDEX_CALCULATION(x, max);
		int8_t mul = 1;

		if (idx < 0) 
		{
			idx = -1-idx;
			mul = -1;
		}

		x = GVAR_VALUE(idx, getGVarFlightPhase(phase, idx)) * mul;
	}
	return limit(min, x, max);
}

void setGVarValue(uint8_t idx, int16_t value, int8_t phase)
{
	value = limit((int16_t)-GVAR_LIMIT,value,(int16_t)GVAR_LIMIT); //Limit Gvar value
	phase = getGVarFlightPhase(phase, idx);
	if (GVAR_VALUE(idx, phase) != value) {
		GVAR_VALUE(idx, phase) = value;  
		eeDirty(EE_MODEL);
	}
}

#endif

getvalue_t convert8bitsTelemValue(uint8_t channel, ls_telemetry_value_t value)
{
	getvalue_t result;
	switch (channel) 
	{
		case TELEM_TIMER1:
		case TELEM_TIMER2:
			result = value * 5;
			break;
		#if defined(FRSKY)
		case TELEM_ALT:
		case TELEM_GPSALT:
		case TELEM_MAX_ALT:
		case TELEM_MIN_ALT:
			result = value * 8 - 500;
			break;
		case TELEM_RPM:
		case TELEM_MAX_RPM:
			result = value * 50;
			break;
		case TELEM_T1:
		case TELEM_T2:
		case TELEM_MAX_T1:
		case TELEM_MAX_T2:
			result = (getvalue_t)value - 30;
			break;
		case TELEM_CELL:
		case TELEM_HDG:
		case TELEM_SPEED:
		case TELEM_MAX_SPEED:
			result = value * 2;
			break;
		case TELEM_ASPEED:
		case TELEM_MAX_ASPEED:
			result = value * 20;
			break;
		case TELEM_DIST:
		case TELEM_MAX_DIST:
			result = value * 8;
			break;
		case TELEM_CURRENT:
		case TELEM_POWER:
		case TELEM_MAX_CURRENT:
		case TELEM_MAX_POWER:
			result = value * 5;
			break;
		case TELEM_CONSUMPTION:
			result = value * 100;
			break;
		case TELEM_VSPEED:
			result = ((getvalue_t)value - 125) * 10;
			break;
		#endif
		default:
			result = value;
			break;
	}
	return result;
}

#if defined(FRSKY)
uint8_t checkIfModelIsOff()
{
	if (TELEMETRY_STREAMING())
	{
		pwrCheck = true; //reset shutdown command
		ALERT(STR_MODEL, STR_MODELISON, AU_FRSKY_WARN2);
		return true;
	}
	return false;
}

ls_telemetry_value_t maxTelemValue(uint8_t channel)
{
	switch (channel) {
		case TELEM_FUEL:
		case TELEM_RSSI_TX:
		case TELEM_RSSI_RX:
		return 100;
		case TELEM_HDG:
		return 180;
		default:
		return 255;
	}
}


FORCEINLINE void convertUnit(getvalue_t & val, uint8_t & unit)
{
	if (IS_IMPERIAL_ENABLE()) {

		if (unit == UNIT_TEMPERATURE) {
			val += 18;
			val *= 115;
			val >>= 6;
		}
		if (unit == UNIT_DIST) {
			// m to ft *105/32
			val = val * 3 + (val >> 2) + (val >> 5);
		}
		if (unit == UNIT_FEET) {
			unit = UNIT_DIST;
		}
		if (unit == UNIT_KTS) {
			// kts to mph
			unit = UNIT_SPEED;
			val = (uint32_t)(val * 23) / 20;
		}

		} else {

		if (unit == UNIT_KTS) {
			// kts to km/h
			unit = UNIT_SPEED;
			val = (uint32_t)(val * 50) / 27;
		}
	}

	if (unit == UNIT_HDG) {
		unit = UNIT_TEMPERATURE;
	}
}
#endif

#define INAC_STICKS_SHIFT   6
#define INAC_SWITCHES_SHIFT 8

uint8_t inputsMoved()
{
	uint8_t sum = 0;
	for (uint8_t i=0; i<NUM_STICKS; i++)
		sum += analogIn(i) >> INAC_STICKS_SHIFT;
	for (uint8_t i=0; i<NUM_SWITCHES; i++)
		sum += getValue(MIXSRC_FIRST_SWITCH+i) >> INAC_SWITCHES_SHIFT;

	if (abs((int8_t)(sum-inactivity.sum)) > 5) 
	{
		inactivity.sum = sum;
		return true;
	} 
	else 
	{
		return false;
	}
}

void checkBacklight()
{
	static uint8_t tmr10ms ;

	uint8_t x = g_blinkTmr10ms;
	if (tmr10ms != x) 
	{
		tmr10ms = x;
		if (inputsMoved()) 
		{
			inactivity.counter = 0;
			if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
				backlightOn();
		}

		uint8_t backlightOn = (g_eeGeneral.backlightMode == e_backlight_mode_on || lightOffCounter || modelFunctionsContext.isFunctionActive(FUNCTION_BACKLIGHT));
		if (flashCounter) 
			backlightOn = !backlightOn;
		if (backlightOn)
			BACKLIGHT_ON();
		else
			BACKLIGHT_OFF();

	}
}

void backlightOn()
{
	lightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#if MENUS_LOCK == 1
bool readonly = true;
bool readonlyUnlocked()
{
	if (readonly) 
	{
		POPUP_WARNING(STR_MODS_FORBIDDEN);
		return false;
	} 
	else 
	{
		return true;
	}
}
#endif

#if defined(SPLASH)
void doSplash()
{
	uint8_t audioTadaPerformed = 0;

	if (SPLASH_NEEDED()) 
	{
		displaySplash();

		tmr10ms_t contrastStart10msTick = getTmr10ms();
		tmr10ms_t splashStart10msTick   = getTmr10ms();
		uint8_t contrast = 10;

		lcdSetRefVolt(contrast);

		getADC(); // init ADC array
		checkBattery();

		inputsMoved(); // Init struct inactivity with this function

		do{
			getADC();

			SIMU_SLEEP(1);

			#if defined(FSPLASH)
			// Splash is forced, we can't skip it
			if (!(g_eeGeneral.splashMode & 0x04)) 
			{
				#endif

				if (ELAPSED_10MS_TICK_SINCE(splashStart10msTick) >= MS_TO_10MS_TICK(SPLASH_TIMEOUT_MS / 2) && (!audioTadaPerformed)) 
				{
					AUDIO_TADA();
					audioTadaPerformed = 1;    // Splash duration/2 to play TADA sound
				}

				if (keyDown() || inputsMoved()) return;

				#if defined(FSPLASH)
			}
			#endif

			if (!pwrCheck) {
				return;
			}

			if (ELAPSED_10MS_TICK_SINCE(contrastStart10msTick) >= MS_TO_10MS_TICK(100)) 
			{
				contrastStart10msTick = getTmr10ms();
				if (contrast < g_eeGeneral.contrast) 
				{
					contrast += 1;
					lcdSetRefVolt(contrast);
				}
			}

			checkBacklight();
		}while(ELAPSED_10MS_TICK_SINCE(splashStart10msTick) < MS_TO_10MS_TICK(SPLASH_TIMEOUT_MS));
	}
}
#else
#define Splash()
#define doSplash()
#endif

#if defined(GUI)
void checkAll()
{
	checkLowEEPROM();
	checkTHR();
	checkSwitches();
	PLAY_MODEL_NAME();
	clearKeyEvents();
}
#endif // GUI

void checkLowEEPROM()
{
	if (g_eeGeneral.disableMemoryWarning) 
		return;
	if (EeFsGetFree() < 100) {
		ALERT(STR_EEPROMWARN, STR_EEPROMLOWMEM, AU_ERROR);
	}
}


void checkTHR()
{
	uint8_t thrchn = ((g_model.thrTraceSrc==0) || (g_model.thrTraceSrc>NUM_POTS)) ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1;
	// throttle channel is either the stick according stick mode (already handled in evalInputs)
	// or P1 to P3;
	// in case an output channel is chosen as throttle source (thrTraceSrc>NUM_POTS) we assume the throttle stick is the input
	// no other information available at the moment, and good enough to my option (otherwise too much exceptions...)

	if (g_model.disableThrottleWarning) 
	{
		return;
	}

	getADC();

	evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

	int16_t v = calibratedStick[thrchn];
	if (v <= THRCHK_DEADBAND-1024) 
	{
		return; // prevent warning if throttle input OK
	}

	// first - display warning; also deletes inputs if any have been before
	MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);


	while (1) 
	{
		getADC();

		evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

		v = calibratedStick[thrchn];

		if (!pwrCheck) 
		{
			break;
		}

		if (keyDown() || v <= THRCHK_DEADBAND-1024) 
		{
			break;
		}

		checkBacklight();

		wdt_reset();
	}
}

void checkAlarm()
{
	if (!g_eeGeneral.disableAlarmWarning && IS_SOUND_OFF()) 
	{
		ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
	}
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
	MESSAGE(t, s, STR_PRESSANYKEY, sound);

	while(1)
	{
		if (keyDown())
			return;  // wait for key release

		checkBacklight();

		if (!pwrCheck)
		{
			boardOff(); // turn power off now
		}
		wdt_reset();
	}
}

#if defined(GVARS)
int8_t trimGvar[NUM_STICKS] = { -1, -1, -1, -1 };
#endif

uint8_t checkTrim(uint8_t event)
{
	int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
	if (k>=0 && k<8 && !IS_KEY_BREAK(event)) 
	{
		// LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
		uint8_t idx = CONVERT_MODE((uint8_t)k/2);
		uint8_t phase;
		int16_t before;
		uint8_t thro;


		#if defined(GVARS)
		if (TRIM_REUSED(idx)) 
		{
			phase = getGVarFlightPhase(mixerCurrentFlightMode, trimGvar[idx]);
			before = GVAR_VALUE(trimGvar[idx], phase);
			thro = false;
		} 
		else 
		{
			phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
			before = getRawTrimValue(phase, idx);
			thro = (idx==THR_STICK && g_model.thrTrim);
		}
		#else
		phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
		before = getRawTrimValue(phase, idx);
		thro = (idx==THR_STICK && g_model.thrTrim);
		#endif
		int8_t trimInc = g_model.trimInc + 1;
		int8_t v = (trimInc==-1) ? min(32, abs(before)/4+1) : (1 << trimInc); // TODO flash saving if (trimInc < 0)
		
		if (thro)
			v = 4; // if throttle trim and trim throttle then step=4
		int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
		int8_t beepTrim = false;
		
		for (int16_t mark=TRIM_MIN; mark<=TRIM_MAX; mark+=TRIM_MAX) 
		{
			if ((mark!=0 || !thro) && ((mark!=TRIM_MIN && after>=mark && before<mark) || (mark!=TRIM_MAX && after<=mark && before>mark))) 
			{
				after = mark;
				beepTrim = (mark == 0 ? 1 : 2);
			}
		}

		if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) 
		{
			if (!g_model.extendedTrims || TRIM_REUSED(idx)) 
				after = before;
		}

		limit<int16_t>(TRIM_EXTENDED_MIN,after,TRIM_EXTENDED_MAX);


		#if defined(GVARS)
		if (TRIM_REUSED(idx)) 
		{
			SET_GVAR_VALUE(trimGvar[idx], phase, after);
		} 
		else
		#endif
		{
			setTrimValue(phase, idx, after);
		}

		#if defined(AUDIO)
		// toneFreq higher/lower according to trim position
		// limit the frequency, range -125 to 125 = toneFreq: 19 to 101

    
		limit<int16_t>(TRIM_MIN,after,TRIM_MAX);

		after >>= 2;
		after += 60;
		#endif

		if (beepTrim) 
		{
			if (beepTrim == 1) 
			{
				AUDIO_TRIM_MIDDLE(after);
				pauseEvents(event);
			}
			else 
			{
				AUDIO_TRIM_END(after);
				killEvents(event);
			}
		} 
		else 
		{
			AUDIO_TRIM(event, after);
		}
		return 0;
	}
	return event;
}

uint16_t s_anaFilt[NUMBER_ANALOG];

uint16_t analogIn(uint8_t channel)
{
	static const pm_char crossAnalog[] PROGMEM = {3,1,2,0,4,5,6,7};
	uint16_t *p = &s_anaFilt[pgm_read_byte_near(crossAnalog+channel)];
	return *p;
}

uint16_t g_vbat10mV = 0;
uint16_t lightOffCounter;
uint8_t flashCounter = 0;

uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;

void flightReset()
{
	// we don't reset the whole audio here (the tada.wav would be cut, if a prompt is queued before FlightReset, it should be played)
	// TODO check if the vario / background music are stopped correctly if switching to a model which doesn't have these functions enabled

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

	#if defined(FRSKY)
	telemetryResetValue();
	#endif

	s_mixer_first_run_done = false;

	RESET_THR_TRACE();
}

#if defined(THRTRACE)
uint8_t  s_traceBuf[MAXTRACE];
uint8_t  s_traceWr;
int16_t  s_traceCnt;
uint8_t  s_cnt_10s;
uint16_t s_cnt_samples_thr_10s;
uint16_t s_sum_samples_thr_10s;
#endif

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
			trim = enableGaz? ((((int32_t)(trim-trimMin)) * (RESX-anas[i])) >> (RESX_SHIFT+1)) : 0; //GAZ SECURITY
		}
		if (trimsCheckTimer) 
		{
			trim = 0;
		}
		trims[i] = trim*2;
	}
}

bool s_mixer_first_run_done = false;

void doMixerCalculations()
{
	static uint16_t lastTMR = 0;

	uint16_t tmr10ms = getTmr10ms();
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

			if (gModelMax!=0 && gModelMax!=2048) 
				val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs

			if (val<0) 
				val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
		} 
		else 
		{
			val = RESX + (g_model.thrTraceSrc == 0 ? rawAnalogs[THR_STICK] : calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1]);
		}

		val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)

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
				if ((((uint8_t)ptrInactivity->counter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && ptrInactivity->counter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
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


void Start() // Run only if it is not a WDT reboot
{
	#if defined(RTCLOCK)
	rtcInit();
	#endif
	#if defined(SDCARD)
	if (!MountSD()) 
		master_spi_disable(); // Mount SD, disable master SPI port on error
	if sdMounted()
	{
		sdCreateSystemDir();   // Create LOGS & MODELS dir if they don't exist
		sdFindFileStruct(ROOT_PATH); // Nedeed to initialize SD driver buffer (Lcd buffer reused ;-)
	}
	#endif
	#if defined(BLUETOOTH)
	bluetooth_init(&Serial1);
	#endif
	doSplash();
	#if defined(GUI)
	checkAlarm();
	checkAll();
	//corrupted eeprom or firs run
	if (g_eeGeneral.chkSum != evalChkSum()) 
	{
		chainMenu(menuFirstCalib);
	}
	#endif
}

void Init(uint8_t mcusr)
{
	eeReadAll();

	#if MENUS_LOCK == 1
	getMovedSwitch();
	if (TRIMS_PRESSED() && g_eeGeneral.switchUnlockStates==switches_states) 
	{
		readonly = false;
	}
	#endif

	#if defined(VOICE)
	//  setVolume(g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF);
	#endif

	if (g_eeGeneral.backlightMode != e_backlight_mode_off) 
		backlightOn(); // on Tx start turn the light on

	if (UNEXPECTED_SHUTDOWN())
	{
		unexpectedShutdown = true;
	}
	else
	{
		Start(); // All functions called in Start() are not used if a WDT reset occur.
	}

	//#if defined(U_CLI)
	//Serial1.print(F("\nWelcome to OpenAVRc V3.0 !\n"));
	//#endif

	if (!g_eeGeneral.unexpectedShutdown) 
	{
		g_eeGeneral.unexpectedShutdown = 1;
		eeDirty(EE_GENERAL);
	}

	#if defined(GUI)
	lcdSetContrast();
	#endif
	backlightOn();

	doMixerCalculations();

	startPulses(PROTOCMD_INIT);

	// Enable watchdog.
	wdt_enable(WDTO_500MS);	
}



void Close()
{
	AUDIO_BYE();

	#if defined(HAPTIC)
	hapticOff();
	#endif

	saveTimers();

	#ifdef GUI
	if (s_eeDirtyMsk & EE_MODEL) 
	{
		displayPopup(STR_SAVEMODEL);
	}
	#endif

	g_eeGeneral.unexpectedShutdown = 0;

	eeDirty(EE_GENERAL);
	eeCheck(true);

	#if defined(SDCARD)
	closeLogs();
	UmountSD();
	#endif
}

void checkBattery()
{
	uint32_t instant_vbat = analogIn(TX_VOLTAGE);

	// Replace with correct values on DX6I board
	#define BANDGAP 5000 // 5 Volts : We use AVCC.
	instant_vbat *= 2889L*(BANDGAP/100);
	instant_vbat /= 2047L*100L;
	instant_vbat += 38L;
	instant_vbat += g_eeGeneral.txVoltageCalibration;
	// Schottky Diode drops 0.38V before a potential divider which reduces the input to the ADC by 1/2.8889.
	
	if (!g_vbat10mV) 
	{
		g_vbat10mV = instant_vbat;
	}

	g_vbat10mV = ((g_vbat10mV << 1) + instant_vbat) / 3; // Simple low pass filter

	if (IS_TXBATT_WARNING() && (g_vbat10mV > (g_eeGeneral.vBatMin*9)) && ((sessionTimer&0x0F)==0x04)) 
	{ // No Audio Alarm if TX Battery < VCCMIN X .9 & 30 Sec
		AUDIO_TX_BATTERY_LOW();
	}
}

#define INSTANT_TRIM_MARGIN 15 /* around 1.5% */

void instantTrim()
{
	evalInputs(e_perout_mode_notrainer);

	for (uint8_t stick=0; stick<NUM_STICKS; stick++) 
	{
		if (stick!=THR_STICK) 
		{
			// don't instant trim the throttle stick
			uint8_t trim_phase = getTrimFlightPhase(mixerCurrentFlightMode, stick);
			int16_t delta = anas[stick];
			if (abs(delta) >= INSTANT_TRIM_MARGIN) 
			{
				int16_t trim = limit<int16_t>(TRIM_EXTENDED_MIN, (delta + trims[stick]) / 2, TRIM_EXTENDED_MAX);
				setTrimValue(trim_phase, stick, trim);
			}
		}
	}
	eeDirty(EE_MODEL);
	AUDIO_WARNING2();
}

void copySticksToOffset(uint8_t ch)
{
	int32_t zero = (int32_t)channelOutputs[ch];

	evalFlightModeMixes(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
	int32_t val = chans[ch];
	LimitData *ld = limitAddress(ch);
	limit_min_max_t lim = LIMIT_MIN(ld);
	if (val < 0) 
	{
		val = -val;
		lim = LIMIT_MIN(ld);
	}
	zero = (zero*25600 - val*lim) / (26214-val);
	ld->offset = (ld->revert ? -zero : zero);
	eeDirty(EE_MODEL);
}

void copyTrimsToOffset(uint8_t ch)
{
	int16_t zero;

	evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
	zero = applyLimits(ch, chans[ch]);

	evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

	int16_t output = applyLimits(ch, chans[ch]) - zero;
	int16_t v = g_model.limitData[ch].offset;
	if (g_model.limitData[ch].revert) output = -output;
	v += output;
	g_model.limitData[ch].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire

	eeDirty(EE_MODEL);
}

void moveTrimsToOffsets() // copy state of 3 primary to sub trim
{
	int16_t zeros[NUM_CHNOUT];

	evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
	for (uint8_t i=0; i<NUM_CHNOUT; i++) {
		zeros[i] = applyLimits(i, chans[i]);
	}

	evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

	for (uint8_t i=0; i<NUM_CHNOUT; i++) {
		int16_t output = applyLimits(i, chans[i]) - zeros[i];
		int16_t v = g_model.limitData[i].offset;
		if (g_model.limitData[i].revert) output = -output;
		v += output;
		g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
	}

	// reset all trims, except throttle (if throttle trim)
	for (uint8_t i=0; i<NUM_STICKS; i++) {
		if (i!=THR_STICK || !g_model.thrTrim) {
			int16_t original_trim = getTrimValue(mixerCurrentFlightMode, i);
			for (uint8_t phase=0; phase<MAX_FLIGHT_MODES; ++phase) {
				int16_t trim = getRawTrimValue(phase, i);
				if (trim <= TRIM_EXTENDED_MAX)
				setTrimValue(phase, i, trim - original_trim);
			}
		}
	}

	eeDirty(EE_MODEL);
	AUDIO_WARNING2();
}

#if defined(ROTARY_ENCODERS)
volatile rotenc_t g_rotenc[ROTARY_ENCODERS] = {0};
#elif defined(ROTARY_ENCODER_NAVIGATION)
volatile rotenc_t g_rotenc[1] = {0};
#endif

#define STACKPTR     _SFR_IO16(0x3D)
extern unsigned char __bss_end ;
void stackPaint()
{
	// Init Stack while interrupts are disabled
	unsigned char *p ;
	unsigned char *q ;

	p = (unsigned char *) STACKPTR ;
	q = &__bss_end ;
	p -= 2 ;
	while ( p > q ) {
		*p-- = 0x55 ;
	}
}


uint16_t stackAvailable()
{
	unsigned char *p ;

	p = &__bss_end + 1 ;
	while ( *p++ == 0x55 );
	return p - &__bss_end ;
}

//Back magic! Need more info how it's work
void ResetToBootloaderWithFlag()
{
	/* Build the flag at the top of SRAM (Not erased in WDT reboot) */
	uint8_t * p = (uint8_t*)RAMEND;
	*p-- = 0x0F;
	*p   = 0xF0;

	wdt_enable(WDTO_15MS); // set 15mS watchdog

	for (;;) ; // Reset in 15mS ...
}

int main(void)
{
	// G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
	// important to disable it before commencing with system initialization (or
	// we could put a bunch more MYWDT_RESET()s in. But I don't like that approach
	// during boot up.)

	uint8_t mcusr = MCUCSR; // save the WDT (etc) flags

	MCUCSR = 0; // must be zeroed before disabling the WDT
	MCUCSR |= (1<<JTD);    // Disable JTAG port that can interfere with POT3
	MCUCSR |= (1<<JTD);   // Must be done twice within four cycles
	wdt_disable();

	stackPaint();
	
	//Init board configuration
	boardInit();

	sei();	// Needed to catch first 10mS interrupt

	#if defined(GUI)
	menuHandlers[0] = menuMainView;
	#if MENUS_LOCK != 2 /*no menus*/
	menuHandlers[1] = menuModelSelect;
	#endif
	#endif
	
	#if defined(GUI)
	lcdSetRefVolt(25);
	#endif

	Init(mcusr);

	//uint16_t lastPrint = getTmr10ms();
	
    while (1) 
    {
		//#if defined(U_CLI)
		//#if defined(BLUETOOTH)
		//if (menuHandlers[menuLevel] != menuGeneralBluetooth) // Do not process uCli when BT is in AT mode (In BT menu)
		////#endif
		////{
			////uCli_process();
		////}
		////#endif
//
		//#if defined(TINY_DBG_UART_USB) || (defined(TINY_DBG_UART_BT) && !defined(U_CLI))
		//TinyDbg_event();
		//#endif

		if (!pwrCheck)
			break;

		perMain();

		if (heartbeat == HEART_WDT_CHECK) 
		{
			wdt_reset();
			heartbeat = 0;
		}

		//Serial0.println(lastPrint, 10);
		//uint16_t time = g_tmr10ms;
		//_delay_ms(20);
		//if(time - lastPrint >= 10)
		//{
			//lastPrint = time;
			//uint16_t t = s_anaFilt[1];
			////#if DEBUG
			////Serial0.println(t, 10);
			////#endif
		//}
		//if(heartbeat == 0x3)
		//{
			//wdt_reset();
			//heartbeat = 0;
		//}
		
    }
}

uint16_t getTmr10ms()
{
	//uint16_t time;
	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		//time = g_tmr10ms;
	//}
	//return time;
return g_tmr10ms;
}

uint16_t getTmr128uS()
{
	while(1)
	{
		uint8_t hb  = g_tmr128uS;
		uint8_t lb  = TCNT0;
		if(hb - g_tmr128uS==0) return (hb << 8) | lb;
	}
}

//uint16_t getTmr64uS()
//{
	//return getTmr128uS() * 2;
//}

ISR(TIMER0_OVF_vect)	//continuous timer 32.64ms (8MHz/1024)
{
	g_tmr128uS++;
}

ISR(TIMER_10MS_VECT) //10ms timer
{
	// Clocks every 9.984ms & 10.112ms
	static uint8_t accuracy = 0;

	// Clock correction
	// 7 out of 8 times count to 9.984ms and 1 of 8 to 10.112ms
	OCR0 += (++accuracy & 0x07) ? 78 : 79;

	++g_tmr10ms;

	sei(); // ISR no block from here

	getADC();
	#if defined(AUDIO)
	AUDIO_HEARTBEAT();
	#endif

	#if defined(BUZZER)
	BUZZER_HEARTBEAT();
	#endif

	#if defined(HAPTIC)
	HAPTIC_HEARTBEAT();
	#endif

	per10ms();
}


ISR(INT5_vect)
{
	//if (!(PINE & ROT_ENC_1_MASK))
	//{
	//debounceRotEncA();
	//incRotaryEncoder(0, -1);
	//}
}

ISR(INT6_vect)
{
	//if (!(PINE & ROT_ENC_1_MASK)) {debounceRotEncA(); incRotaryEncoder(0, +1);}
}


//////////////////////////////////////////////////////////////////////////
///helpers
//////////////////////////////////////////////////////////////////////////


const pm_char s_charTab[] PROGMEM = "_-.,";

char hex2zchar(uint8_t hex)
{
  return (hex >= 10 ? hex-9 : 27+hex);
}

char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= 40) return pgm_read_byte_near(s_charTab+idx-37);
#if LEN_SPECIAL_CHARS > 0
  if (idx <= (LEN_STD_CHARS + LEN_SPECIAL_CHARS)) return 'z' + 5 + idx - 40;
#endif
  return ' ';
}

int8_t char2idx(char c)
{
  if (c == '_') return 37;
#if LEN_SPECIAL_CHARS > 0
  if (c < 0 && c+128 <= LEN_SPECIAL_CHARS) return 41 + (c+128);
#endif
  if (c >= 'a') return 'a' - c - 1;
  if (c >= 'A') return c - 'A' + 1;
  if (c >= '0') return c - '0' + 27;
  if (c == '-') return 38;
  if (c == '.') return 39;
  if (c == ',') return 40;
  return 0;
}

void str2zchar(char *dest, const char *src, uint8_t size) // ASCII to FW
{
  memset(dest, 0, size);
  for (uint8_t c=0; c<size && src[c]; c++) {
    dest[c] = char2idx(src[c]);
  }
}

uint8_t zchar2str(char *dest, const char *src, uint8_t size) // FW to ASCII
{
  for (uint8_t c=0; c<size; c++) {
    dest[c] = idx2char(src[c]);
  }
  do {
    dest[size--] = '\0';
  } while ((size) && dest[size] == ' ');
  return size+1;
}


#if defined(SDCARD)
char *strAppend(char *dest, const char *source, uint8_t len)
{
  while ((*dest++ = *source++)) {
    if (--len == 0) {
      *dest = '\0';
      return dest;
    }
  }
  return dest - 1;
}

char *strSetCursor(char *dest, uint8_t position)
{
  *dest++ = 0x1F;
  *dest++ = position;
  *dest = '\0';
  return dest;
}

char *strAppendFilename(char *dest, const char *filename, const uint8_t size)
{
  memset(dest, 0, size);
  for (uint8_t i=0; i<size; i++) {
    char c = *filename++;
    if (c == '\0' || c == '.')
      break;
    *dest++ = c;
  }
  return dest;
}

#define LEN_FILE_EXTENSION 4
char *getFileExtension(char *filename, uint8_t size)
{
  uint8_t len = min<uint8_t>(size, strlen(filename));
  for (uint8_t i=len; i>=len-LEN_FILE_EXTENSION; --i) {
    if (filename[i] == '.') {
      return &filename[i];
    }
  }
  return NULL;
}

#if defined(RTCLOCK)

char * strAppendDate(char * str)//, bool time)
{
  str[0] = '-';
  struct tm * utm;
  utm = localtime(&g_rtcTime);
  div_t qr = div(utm->tm_year+T1900_OFFSET, 10);
  str[4] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[3] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[2] = '0' + qr.rem;
  str[1] = '0' + qr.quot;
  str[5] = '-';
  qr = div(utm->tm_mon+1, 10);
  str[7] = '0' + qr.rem;
  str[6] = '0' + qr.quot;
  str[8] = '-';
  qr = div(utm->tm_mday, 10);
  str[10] = '0' + qr.rem;
  str[9] = '0' + qr.quot;

  /*if (time) {
    str[11] = '-';
    div_t qr = div(utm->tm_hour, 10);
    str[13] = '0' + qr.rem;
    str[12] = '0' + qr.quot;
    qr = div(utm->tm_min, 10);
    str[15] = '0' + qr.rem;
    str[14] = '0' + qr.quot;
    qr = div(utm->tm_sec, 10);
    str[17] = '0' + qr.rem;
    str[16] = '0' + qr.quot;
    str[18] = '\0';
    return &str[18];
  } else {*/
    str[11] = '\0';
    return &str[11];
  //}
}
#endif
#endif
