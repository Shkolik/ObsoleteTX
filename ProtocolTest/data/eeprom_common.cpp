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

#include "eeprom_common.h"

uint8_t		s_eeDirtyMsk;
uint16_t	s_eeDirtyTime10ms;

void eeDirty(uint8_t msk)
{
	s_eeDirtyMsk |= msk;
	s_eeDirtyTime10ms = g_tmr10ms;
}

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
	uint8_t i = id;
	for (;;) 
	{
		i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
		if (!eeModelExists(i))
			break;
		if (i == id) 
			return 0xff; // no free space in directory left
	}
	return i;
}

void selectModel(uint8_t sub)
{
	//TODO: GUI staff
	//POPUP(STR_LOADINGMODEL);
	//saveTimers();
	eeCheck(true); // force writing of current model data before this is changed
	g_general.currentModel = sub;
	eeDirty(EE_GENERAL);
	eeLoadModel(sub);
}

void eeReadAll()
{
	debugln("eeReadAll()");
	
	if (!eeLoadGeneral())//!eepromOpen() || !eeLoadGeneral())
	{
		debugln("eraising eeprom....");
		eeErase(true);
	}
	stickMode = g_general.stickMode;
	//eeLoadModel(g_general.currentModel);
}
