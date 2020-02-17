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

#ifndef TIMERS_H_
#define TIMERS_H_

#include "main.h"


#define TMR_OFF      0
#define TMR_RUNNING  1
#define TMR_NEGATIVE 2
#define TMR_STOPPED  3

#define TIMER_MAX     (0xffff/2)

#define TIMER_MIN     (int16_t(-TIMER_MAX-1))

struct TimerState {
	uint16_t cnt;
	uint16_t sum;
	uint8_t  state;
	int16_t  val;
	uint8_t  val_10ms;
};

extern TimerState timersStates[TIMERS];

extern void timerReset(uint8_t idx);


extern void saveTimers();
extern void restoreTimers();

void evalTimers(int16_t throttle, uint8_t tick10ms);



#endif /* TIMERS_H_ */