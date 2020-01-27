/*
 * trainer_input.h
 *
 * Created: 1/26/2020 10:46:13 PM
 *  Author: Andrew
 */ 


#ifndef TRAINER_INPUT_H_
#define TRAINER_INPUT_H_

#include "ObsoleteTX.h"

// Trainer input channels
extern int16_t ppmInput[NUM_TRAINER];

// Timer gets decremented in per10ms()
#define PPM_IN_VALID_TIMEOUT 100 // 1s
extern uint8_t ppmInputValidityTimer;

#define IS_TRAINER_INPUT_VALID() (ppmInputValidityTimer != 0)


#endif /* TRAINER_INPUT_H_ */