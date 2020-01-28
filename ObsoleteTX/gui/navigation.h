/*
 * navigation.h
 *
 * Created: 1/27/2020 8:00:49 PM
 *  Author: Andrew
 */ 


#ifndef NAVIGATION_H_
#define NAVIGATION_H_


vertpos_t menuVerticalOffset;
uint8_t noHighlightCounter;
int8_t s_editMode;
uint8_t calibrationState;
vertpos_t menuVerticalPosition;
horzpos_t menuHorizontalPosition;

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

#if defined(NAVIGATION_POT2)
int8_t p2valdiff;
#endif




#endif /* NAVIGATION_H_ */