/*
 * eeprom_common.cpp
 *
 * Created: 1/25/2020 10:06:41 PM
 *  Author: Andrew
 */ 

#include "ObsoleteTX.h"
#include "timers.h"

uint8_t   s_eeDirtyMsk;
uint16_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk)
{
	s_eeDirtyMsk |= msk;
	s_eeDirtyTime10ms = GET_10MS_TICK();
}

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
	uint8_t i = id;
	for (;;) 
	{
		i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
		if (!eeModelExists(i)) 
			break;
		if (i == id) // no free space left
			return 0xff; 
	}
	return i;
}

void selectModel(uint8_t sub)
{
	//POPUP(STR_LOADINGMODEL);
	saveTimers();
	eeCheck(true); // force writing of current model data before this is changed
	g_eeGeneral.currModel = sub;
	eeDirty(EE_GENERAL);
	eeLoadModel(sub);
}

void eeReadAll()
{
	if (!eepromOpen() || !eeLoadGeneral())
	{
		eeErase(true);
	}
	stickMode = g_eeGeneral.stickMode;
	eeLoadModel(g_eeGeneral.currModel);
}
