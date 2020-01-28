/*
 * menu_model.cpp
 *
 * Created: 1/27/2020 7:57:49 PM
 *  Author: Andrew
 */ 
#include "../ObsoleteTX.h"
#include "menu_model.h"


uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
	lcdDrawTextLeft(y, str);
	lcdDrawNumberNAtt(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
	if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
	return delay;
}

uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_currIdx;
