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


#ifndef TRAINER_H_
#define TRAINER_H_

#include "main.h"
// Timer gets decremented in per10ms()
#define PPM_IN_VALID_TIMEOUT 100 // 1s
#define IS_TRAINER_INPUT_VALID() (ppmInputValidityTimer != 0)

// Trainer input channels
extern int16_t ppmInput[NUM_TRAINER];
extern uint8_t ppmInputValidityTimer;



#endif /* TRAINER_H_ */