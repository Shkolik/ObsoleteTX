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

#include "trainer.h"

int16_t ppmInput[NUM_TRAINER];
uint8_t ppmInputValidityTimer;

/*
 * Trainer PPM input capture ISR.
 * Timer 1 is free running as it serves other purposes.
 * 1 count is 0.5us
*/
//ISR(TRAINER_TC_VECT) // G: High frequency noise can cause stack overflow with ISR_NOBLOCK
//{
  //uint16_t capture = TRAINER_TC_REG;
//
  //static uint16_t lastCapt = 0;
  //static uint8_t channelNumber = 0;
//
  //uint16_t val = (uint16_t) (capture - lastCapt) / 2; // Convert from timer counts to usec.
  //lastCapt = capture;
//
  //if(channelNumber && val > 800 && val < 2200 && channelNumber <= NUM_TRAINER)
  //{ // Accepted range is 800 to 2200 us  1500us+/-700.
    //ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
    //ppmInput[channelNumber++ -1] =
      //(int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10;
  //}
  //else if(val > 4000 && val < 19000)
  //{ // Frame sync pulse >4 <19 ms.
    //channelNumber = 1; // Indicates start of new frame.
  //}
  //else channelNumber = 0; /* Glitches (<800us) or long channel pulses (2200 to 4000us) or
  //pulses > 19000us reset the process */
//}