/*
 * mixer.h
 *
 * Created: 1/26/2020 9:06:33 PM
 *  Author: Andrew
 */ 


#ifndef MIXER_H_
#define MIXER_H_


extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];
extern void applyExpos(int16_t *anas, uint8_t mode);

#endif /* MIXER_H_ */