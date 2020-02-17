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

#include "maths.h"

#define CORRECT_NEGATIVE_SHIFTS

// open.20.fsguruh; shift right operations do the rounding different for negative values compared to positive values
// so all negative divisions round always further down, which give absolute values bigger compared to a usual division
// this is noticable on the display, because instead of -100.0 -99.9 is shown; While in praxis I doublt somebody will notice a
// difference this is more a mental thing. Maybe people are distracted, because the easy calculations are obviously wrong
// this define would correct this, but costs 34 bytes code for stock version

// return x*2.56
int16_t calc100to256_16Bits(int16_t x) 
{
	// y = 2*x + x/2 +x/16-x/512-x/2048
	// 512 and 2048 are out of scope from int8 input --> forget it
#ifdef CORRECT_NEGATIVE_SHIFTS
	int16_t res=(int16_t)x<<1;
	int8_t sign=(x < 0 ? 1 : 0);

	x-=sign;
	res+=(x>>1);
	res+=sign;
	res+=(x>>4);
	res+=sign;
	return res;
#else
	return ((int16_t)x<<1)+(x>>1)+(x>>4);
#endif
}

// return x*2.56
int16_t calc100to256(int8_t x) 
{
	return calc100to256_16Bits(x);
}

int16_t calc100toRESX_16Bits(int16_t x) // return x*10.24
{
#ifdef CORRECT_NEGATIVE_SHIFTS
	int16_t res= ((int16_t)x*41)>>2;
	int8_t sign=(x < 0 ? 1 : 0);
	x-=sign;
	res-=(x>>6);
	res-=sign;
	return res;
#else  
	return ((x*41)>>2) - (x>>6); // return (int16_t)x*10 + x/4 - x/64;
#endif
}

// return x*10.24
int16_t calc100toRESX(int8_t x) 
{
  return calc100toRESX_16Bits(x);
}

// return x*1.024
int16_t calc1000toRESX(int16_t x)
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x += y;
  y = y>>2;
  x -= y;
  return x + (y>>2);
}

// return x/1.024
int16_t calcRESXto1000(int16_t x)  
{
	return (x - (x>>5) + (x>>7)); // *1000/1024 = x - x/32 + x/128
}

int8_t calcRESXto100(int16_t x)
{
  return (x*25) >> 8;
}

#if defined(HELI)
uint16_t isqrt32(uint32_t n)
{
	uint16_t c = 0x8000;
	uint16_t g = 0x8000;

	for (;;)
	{
		if ((uint32_t)g*g > n)
		g ^= c;
		c >>= 1;
		if(c == 0)
		return g;
		g |= c;
	}
}
#endif

/*
  Division by 10 and rounding or fixed point arithmetic values

  Examples:
    value -> result
    105 ->  11
    104 ->  10
   -205 -> -21
   -204 -> -20
*/
int16_t div10_and_round(int16_t value)
{
  if (value >= 0 ) {
    value += 5;
  } else {
    value -= 5;
  }
  return value/10;
}

int16_t div100_and_round(int16_t value)
{
  if (value >= 0 ) {
    value += 50;
  } else {
    value -= 50;
  }
  return value/100;
}
