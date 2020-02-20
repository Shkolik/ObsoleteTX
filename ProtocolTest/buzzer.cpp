/*
 * buzzer.cpp
 *
 * Created: 2/18/2020 2:47:14 PM
 *  Author: andrew.shkolik
 */ 

#include "buzzer.h"

uint8_t g_beepCnt;
uint8_t beepAgain = 0;
uint8_t beepAgainOrig = 0;
uint8_t beepOn = false;
bool warble = false;
bool warbleC;

// The various "beep" tone lengths
static const pm_uint8_t beepTab[] PROGMEM = {
	// key, trim, warn2, warn1, error
	1,  1,  2, 10,  60, //xShort
	1,  1,  4, 20,  80, //short
	1,  1,  8, 30, 100, //normal
	2,  2, 15, 40, 120, //long
	5,  5, 30, 50, 150  //xLong
};

void beep(uint8_t val)
{
	//#if defined(HAPTIC) && !defined(AUDIO)
	//haptic.event(val==0 ? AU_KEYPAD_UP : (val==4 ? AU_ERROR : AU_TIMER_LT10+beepAgain));
	//#endif

	if (g_general.alarmsFlash && val>1) 
		flashCounter = FLASH_DURATION;
	
	if (g_general.beepMode > 0 || (g_general.beepMode == 0 && val !=0 ) || (g_general.beepMode == -1 && val >= 3)) 
	{
		_beep(pgm_read_byte_near(beepTab+5*(2+g_general.beepLength)+val));
	}
}