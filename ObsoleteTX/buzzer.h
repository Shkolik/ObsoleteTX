/*
 * buzzer.h
 *
 * Created: 1/25/2020 11:54:06 PM
 *  Author: Andrew
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

#include "ObsoleteTX.h"

#if defined(BUZZER)
extern uint8_t g_beepCnt;
extern uint8_t beepAgain;
extern uint8_t beepAgainOrig;
extern uint8_t beepOn;
extern bool warble;
extern bool warbleC;
#if defined(HAPTIC)
extern uint8_t hapticTick;
#endif /* HAPTIC */
#endif /* BUZZER */

inline void _beep(uint8_t b)
{
	g_beepCnt = b;
}

extern void beep(uint8_t val);

#if !defined(AUDIO)

#if defined(BUZZER)
#if defined(VOICE)
#define AUDIO_TADA()           PUSH_SYSTEM_PROMPT(AU_TADA)
#define AUDIO_BYE()
#define AUDIO_TX_BATTERY_LOW() PUSH_SYSTEM_PROMPT(AU_TX_BATTERY_LOW)
#define AUDIO_INACTIVITY()     PUSH_SYSTEM_PROMPT(AU_INACTIVITY)
#define AUDIO_ERROR_MESSAGE(e) PUSH_SYSTEM_PROMPT((e))
#define AUDIO_TIMER_MINUTE(t)  playDuration(t)
#define AUDIO_TIMER_30()       PUSH_SYSTEM_PROMPT(AU_TIMER_30)
#define AUDIO_TIMER_20()       PUSH_SYSTEM_PROMPT(AU_TIMER_20)
#define PLAY_MODEL_NAME()      PUSH_CUSTOM_PROMPT(AU_MODEL_NAME_NUM_FILE,0); PUSH_CUSTOM_PROMPT(AU_MODEL_NAME_NUM_FILE + 1 + g_eeGeneral.currModel,0)
#define PLAY_TELEMETRY_GET()   PUSH_SYSTEM_PROMPT(AU_TELEMETRY_GET)
#define PLAY_TELEMETRY_LOSS()  PUSH_SYSTEM_PROMPT(AU_TELEMETRY_LOSS)

#else
#define AUDIO_TADA()
#define AUDIO_BYE()
#define AUDIO_TX_BATTERY_LOW() beep(4)
#define AUDIO_INACTIVITY()     beep(3)
#define AUDIO_ERROR_MESSAGE(e) beep(4)
#define AUDIO_TIMER_MINUTE(t)  beep(2)
#define AUDIO_TIMER_30()       { beepAgain=2; beep(2); }
#define AUDIO_TIMER_20()       { beepAgain=1; beep(2); }
#define PLAY_MODEL_NAME()
#define PLAY_TELEMETRY_GET()
#define PLAY_TELEMETRY_LOSS()

#endif

#define AUDIO_KEYPAD_UP()        beep(0)
#define AUDIO_KEYPAD_DOWN()      beep(0)
#define AUDIO_MENUS()            beep(0)
#define AUDIO_WARNING2()         beep(2)
#define AUDIO_WARNING1()         beep(3)
#define AUDIO_ERROR()            beep(4)
#define AUDIO_MIX_WARNING(x)     beep(1)
#define AUDIO_POT_MIDDLE()       beep(2)
#define AUDIO_TIMER_LT10(m, x)   beep(2)
#define AUDIO_TIMER_00(m)        beep(3)
#define AUDIO_VARIO_UP()         _beep(1)
#define AUDIO_VARIO_DOWN()       _beep(1)
#define AUDIO_TRIM(event, f)     { if (!IS_KEY_FIRST(event)) warble = true; beep(1); }
#define AUDIO_TRIM_MIDDLE(f)     beep(2)
#define AUDIO_TRIM_END(f)        beep(2)
#define AUDIO_PLAY(p)            beep(3)

#define IS_AUDIO_BUSY() (g_beepCnt || beepAgain || beepOn)
#else /* BUZZER */
#define AUDIO_TADA()
#define AUDIO_BYE()
#define AUDIO_TX_BATTERY_LOW()
#define AUDIO_INACTIVITY()
#define AUDIO_ERROR_MESSAGE(e)
#define AUDIO_TIMER_MINUTE(t)
#define AUDIO_TIMER_30()
#define AUDIO_TIMER_20()
#define AUDIO_KEYPAD_UP()
#define AUDIO_KEYPAD_DOWN()
#define AUDIO_MENUS()
#define AUDIO_WARNING2()
#define AUDIO_WARNING1()
#define AUDIO_ERROR()
#define AUDIO_MIX_WARNING(x)
#define AUDIO_POT_MIDDLE()
#define AUDIO_TIMER_LT10(m, x)
#define AUDIO_TIMER_00(m)
#define AUDIO_VARIO_UP()
#define AUDIO_VARIO_DOWN()
#define AUDIO_TRIM(event, f)
#define AUDIO_TRIM_MIDDLE(f)
#define AUDIO_TRIM_END(f)
#define AUDIO_PLAY(p)
#define IS_AUDIO_BUSY() false
#endif /* BUZZER */

#define AUDIO_RESET()
#define AUDIO_FLUSH()

#define PLAY_PHASE_OFF(phase)
#define PLAY_PHASE_ON(phase)
#define PLAY_SWITCH_MOVED(sw)
#define PLAY_LOGICAL_SWITCH_OFF(sw)
#define PLAY_LOGICAL_SWITCH_ON(sw)

#endif /* !AUDIO */

#if defined(BUZZER)
inline void BUZZER_HEARTBEAT()
{
	if (g_beepCnt) {
		if (!beepAgainOrig) {
			beepAgainOrig = g_beepCnt;
			beepOn = true;
		}
		g_beepCnt--;
		} else {
		if (beepAgain && beepAgainOrig) {
			beepOn = !beepOn;
			g_beepCnt = beepOn ? beepAgainOrig : 8;
			if (beepOn) beepAgain--;
			} else {
			beepAgainOrig = 0;
			beepOn = false;
			warble = false;
		}
	}

	if (beepOn) 
	{
		warbleC = warble && !warbleC;
		if (warbleC)
			buzzerOff();
		else
			buzzerOn();
	} 
	else 
	{
		buzzerOff();
	}
}
#else // BUZZER
#define BUZZER_HEARTBEAT()
#endif // BUZZER

#endif /* BUZZER_H_ */