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

extern uint8_t mixerCurrentFlightMode;

extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];
extern int16_t channelOutputs[NUM_CHNOUT];
extern int16_t rawAnalogs[NUM_INPUTS];
extern int16_t analogs [NUM_INPUTS];
extern int32_t chans[NUM_CHNOUT];
extern int16_t trims[NUM_STICKS];
extern int16_t ex_chans[NUM_CHNOUT];

typedef struct {
	int32_t		act:24;
	uint16_t	delay:10;		// 10bits used 0 to DELAY_MAX*(100/DELAY_STEP)
	uint8_t		activeMix:1;
	uint8_t		activeExpo:1;
	int16_t		hold:12;		// 12bits used -RESX to RESX
} __attribute__((__packed__)) MixVal;

extern int16_t applyLimits(uint8_t channel, int32_t value);
extern void evalMixes(uint8_t tick10ms);
extern void applyExpos(int16_t *analogs, uint8_t mode);
extern int16_t getValue(uint8_t i);

union u_gvarint_t {
	struct {
		int8_t lo;
		uint8_t hi;
	} bytes_t;

	int16_t gvword;

	u_gvarint_t(int8_t l, uint8_t h)
	{
		bytes_t.lo=l;  // hi bit is negative sign
		bytes_t.hi=h?255:0;
	}

	private:
	// prevent unwanted constructors, also saves program
	u_gvarint_t() {}
	u_gvarint_t(const u_gvarint_t&) {}
} __attribute__((__packed__));

#define MD_WEIGHT(md) (u_gvarint_t(md->weight,md->weightMode).gvword)

union u_int8int16_t {
	struct {
		int8_t  lo;
		uint8_t hi;
	} bytes_t;
	int16_t gvword;
} __attribute__((__packed__));

#define MD_WEIGHT_TO_UNION(md, var) var.bytes_t.lo=md->weight; var.bytes_t.hi=md->weightMode?255:0
#define MD_UNION_TO_WEIGHT(var, md) md->weight=var.bytes_t.lo; if (var.gvword<0) md->weightMode=1; else md->weightMode=0
// #define MD_SETWEIGHT(md, val) md->weight=val; if (val<0) md->weightMode=1; else md->weightMode=0

#define MD_OFFSET(md) (u_gvarint_t(md->offset,md->offsetMode).gvword)
#define MD_OFFSET_TO_UNION(md, var) var.bytes_t.lo=md->offset; var.bytes_t.hi=md->offsetMode?255:0
#define MD_UNION_TO_OFFSET(var, md) md->offset=var.bytes_t.lo; if (var.gvword<0) md->offsetMode=1; else md->offsetMode=0 /* set negative sign */
// #define MD_SETOFFSET(md, val) md->offset=val; if (val<0) md->offsetMode=1; else md->offsetMode=0

#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))


#define MLTPX_ADD   0
#define MLTPX_MUL   1
#define MLTPX_REP   2

#endif /* MIXER_H_ */