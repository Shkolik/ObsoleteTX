/*
 * GccApplication11.cpp
 *
 * Created: 1/24/2020 8:33:44 PM
 * Author : Andrew
 */ 


#include "ObsoleteTX.h"
#include "timers.h"

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

uint16_t s_anaFilt[NUMBER_ANALOG];

uint16_t analogIn(uint8_t channel)
{	
	static const pm_char crossAnalog[] PROGMEM = {3,1,2,0,4,5,6,7};	
	uint16_t *p = &s_anaFilt[pgm_read_byte_near(crossAnalog+channel)];
	return *p;
}

void per10ms()
{

	#if defined(GUI)
	//if (lightOffCounter) --lightOffCounter;
	//if (flashCounter) --flashCounter;
	//if (noHighlightCounter) --noHighlightCounter;
	#endif

	//if (trimsCheckTimer) --trimsCheckTimer;
	//if (ppmInputValidityTimer) --ppmInputValidityTimer;


	#if defined(RTCLOCK)
	/* Update global Date/Time every 100 per10ms cycles */
	//if (++g_ms100 == 100) {
		//++g_rtcTime;   // inc global unix timestamp one second
		//g_ms100 = 0;
	//}
	#endif

	//readKeysAndTrims();

	#if defined(ROTARY_ENCODER_NAVIGATION)
	//if (IS_RE_NAVIGATION_ENABLE()) {
		//static rotenc_t rePreviousValue;
		//rotenc_t reNewValue = (g_rotenc[NAVIGATION_RE_IDX()] / ROTARY_ENCODER_GRANULARITY);
		//int8_t scrollRE = reNewValue - rePreviousValue;
		//if (scrollRE) {
			//rePreviousValue = reNewValue;
			//putEvent(scrollRE < 0 ? EVT_ROTARY_LEFT : EVT_ROTARY_RIGHT);
		//}
		//uint8_t evt = s_evt;
		//if (EVT_KEY_MASK(evt) == BTN_REa + NAVIGATION_RE_IDX()) {
			//if (IS_KEY_BREAK(evt)) {
				//putEvent(EVT_ROTARY_BREAK);
				//} else if (IS_KEY_LONG(evt)) {
				//putEvent(EVT_ROTARY_LONG);
			//}
		//}
	//}
	#endif

	#if defined(FRSKY)
	//if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
	//telemetryInterrupt10ms();
	#endif

	// These moved here from evalFlightModeMixes() to improve beep trigger reliability.
	#if defined(PWM_BACKLIGHT)
	//if (EVERY_PERIOD(g_tmr10ms, 4))
	//backlightFade(); // increment or decrement brightness until target brightness is reached
	#endif

	#if defined(VOICE_JQ6500) && !defined(SIMU)
	//JQ6500Check();
	#endif

	#if !defined(AUDIO)
	//if (mixWarning & 1) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,  0)) AUDIO_MIX_WARNING(1);
	//if (mixWarning & 2) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 64) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 72)) AUDIO_MIX_WARNING(2);
	//if (mixWarning & 4) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,128) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,136) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 144)) AUDIO_MIX_WARNING(3);
	#endif

	#if ROTARY_ENCODERS > 0
	//if (rotEncADebounce) {
		//if (!(rotEncADebounce >>= 1)) ENABLEROTENCAISR(); // Re enable rotencA isr (deboucing)
	//}
	#endif
	#if ROTARY_ENCODERS > 1
	//if (rotEncBDebounce) {
		//if (!(rotEncBDebounce >>= 1)) ENABLEROTENCBISR(); // Re enable rotencB isr (deboucing)
	//}
	#endif

	//if (Bind_tmr10ms)
	//{
		//if (!--Bind_tmr10ms)
		//protoMode = NORMAL_MODE;
	//}

	heartbeat |= HEART_TIMER_10MS;

}

void Init(uint8_t mcusr)
{
	//#ifdef DEBUG
	//Serial0.init(115200);
	//#endif
}

void Start() // Run only if it is not a WDT reboot
{
	//#if defined(RTCLOCK)
	//rtcInit();
	//#endif
	//#if defined(SDCARD)
	//if (!MountSD()) master_spi_disable(); // Mount SD, disable master SPI port on error
	//if sdMounted()
	//{
		//sdCreateSystemDir();   // Create LOGS & MODELS dir if they don't exist
		//sdFindFileStruct(ROOT_PATH); // Nedeed to initialize SD driver buffer (Lcd buffer reused ;-)
	//}
	//#endif
	//#if defined(BLUETOOTH)
	//bluetooth_init(&Serial1);
	//#endif
	//doSplash();
	//#if defined(GUI)
	//checkAlarm();
	//checkAll();
	//if (g_eeGeneral.chkSum != evalChkSum()) {
		//chainMenu(menuFirstCalib);
	//}
	//#endif
}

void Close()
{
	//AUDIO_BYE();
//
	//#if defined(HAPTIC)
	//hapticOff();
	//#endif
//
	//saveTimers();
//
	//if (s_eeDirtyMsk & EE_MODEL) {
		//displayPopup(STR_SAVEMODEL);
	//}
//
	//g_eeGeneral.unexpectedShutdown = 0;
//
	//eeDirty(EE_GENERAL);
	//eeCheck(true);
//
	//#if defined(SDCARD)
	//closeLogs();
	//UmountSD();
	//#endif
}

int main(void)
{
	
	//Init board configuration
	boardInit();
	
	#if DEBUG
	Serial0.init(115200);
	#endif

	sei();
	
	uint16_t lastPrint = getTmr10ms();
	
    while (1) 
    {
		//Serial0.println(lastPrint, 10);
		uint16_t time = g_tmr10ms;
		//_delay_ms(20);
		if(time - lastPrint >= 10)
		{
			lastPrint = time;
			//uint16_t t = s_anaFilt[1];
			//#if DEBUG
			//Serial0.println(t, 10);
			//#endif
		}
		//if(heartbeat == 0x3)
		//{
			//wdt_reset();
			//heartbeat = 0;
		//}
		
    }
}

FORCEINLINE uint16_t getTmr10ms()
{
	//uint16_t time;
	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		//time = g_tmr10ms;
	//}
	return g_tmr10ms;//time;
}

FORCEINLINE uint16_t getTmr128uS()
{
	while(1)
	{
		uint8_t hb  = g_tmr128uS;
		uint8_t lb  = TCNT0;
		if(hb - g_tmr128uS==0) return (hb << 8) | lb;
	}
}

FORCEINLINE uint16_t getTmr64uS()
{
	return getTmr128uS() * 2;
}




volatile uint8_t g_tmr128uS;
volatile uint16_t g_tmr10ms;
uint8_t heartbeat;
uint8_t stickMode;

ISR(TIMER0_OVF_vect, ISR_NOBLOCK)	//continuous timer 32.64ms (6MHz/1024)
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
	//#if defined(AUDIO)
	//AUDIO_HEARTBEAT();
	//#endif
//
	//#if defined(BUZZER)
	//BUZZER_HEARTBEAT();
	//#endif
//
	//#if defined(HAPTIC)
	//HAPTIC_HEARTBEAT();
	//#endif

	per10ms();
		
	//#ifdef FRSKY
	//check_frsky() ;
	//#endif
	
	// See if time for alarm checking
	//struct t_alarmControl *pac = &AlarmControl ;
	//FORCE_INDIRECT(pac) ;
	//
	//if (--pac->AlarmTimer == 0 )
	//{
		//pac->AlarmTimer = 100 ;		// Restart timer
		////			pac->AlarmCheckFlag += 1 ;	// Flag time to check alarms
		//pac->OneSecFlag = 1 ;
	//}
	//if (--pac->VoiceFtimer == 0 )
	//{
		//pac->VoiceFtimer = 10 ;		// Restart timer
		//pac->VoiceCheckFlag |= 1 ;	// Flag time to check alarms
	//}

	//  } // end 10ms event

}



