/*
 * trainer_input.cpp
 *
 * Created: 1/26/2020 10:46:22 PM
 *  Author: Andrew
 */ 


#include "trainer_input.h"

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

