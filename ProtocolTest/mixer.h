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


#ifndef MIXER_H_
#define MIXER_H_

#include "main.h"

extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];
extern int16_t channelOutputs[NUM_CHNOUT];

typedef struct {
	int32_t		act:24;
	uint16_t	delay:10;		// 10bits used 0 to DELAY_MAX*(100/DELAY_STEP)
	uint8_t		activeMix:1;
	uint8_t		activeExpo:1;
	int16_t		hold:12;		// 12bits used -RESX to RESX
} __attribute__((__packed__)) MixVal;


extern int16_t getValue(uint8_t i);
#endif /* MIXER_H_ */