/*
 * buzzer.h
 *
 * Created: 2/18/2020 2:47:32 PM
 *  Author: andrew.shkolik
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

#include "main.h"

#define FLASH_DURATION 20 /*200ms*/

#define AUDIO_TIMER_30()       { beepAgain=2; beep(2); }
#define AUDIO_TIMER_20()       { beepAgain=1; beep(2); }
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
#define AUDIO_TRIM(event, f)     { if (!IS_KEY_FIRST(event)) warble = true; beep(1); }
#define AUDIO_TRIM_MIDDLE(f)     beep(2)
#define AUDIO_TRIM_END(f)        beep(2)
#define AUDIO_PLAY(p)            beep(3)

#define IS_AUDIO_BUSY() (g_beepCnt || beepAgain || beepOn)

#define AUDIO_TADA()
#define AUDIO_BYE()
#define AUDIO_TX_BATTERY_LOW()
#define AUDIO_INACTIVITY()
#define AUDIO_ERROR_MESSAGE(e)
#define AUDIO_TIMER_MINUTE(t)

#define AUDIO_RESET()
#define AUDIO_FLUSH()

#define PLAY_PHASE_OFF(phase)
#define PLAY_PHASE_ON(phase)
#define PLAY_SWITCH_MOVED(sw)
#define PLAY_LOGICAL_SWITCH_OFF(sw)
#define PLAY_LOGICAL_SWITCH_ON(sw)

extern uint8_t g_beepCnt;
extern uint8_t beepAgain;
extern uint8_t beepAgainOrig;
extern uint8_t beepOn;
extern bool warble;
extern bool warbleC;
#if defined(HAPTIC)
extern uint8_t hapticTick;
#endif /* HAPTIC */

inline void BUZZER_HEARTBEAT()
{
	if (g_beepCnt) 
	{
		if (!beepAgainOrig) 
		{
			beepAgainOrig = g_beepCnt;
			beepOn = true;
		}
		g_beepCnt--;
	} 
	else 
	{
		if (beepAgain && beepAgainOrig) 
		{
			beepOn = !beepOn;
			g_beepCnt = beepOn ? beepAgainOrig : 8;
			if (beepOn) 
				beepAgain--;
		} 
		else 
		{
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

inline void _beep(uint8_t b)
{
	g_beepCnt = b;
}

extern void beep(uint8_t val);

#endif /* BUZZER_H_ */