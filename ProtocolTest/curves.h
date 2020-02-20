/*
 * curves.h
 *
 * Created: 2/18/2020 4:45:04 PM
 *  Author: andrew.shkolik
 */ 


#ifndef CURVES_H_
#define CURVES_H_

#include "main.h"


enum BaseCurves {
	CURVE_NONE,
	CURVE_X_GT0,
	CURVE_X_LT0,
	CURVE_ABS_X,
	CURVE_F_GT0,
	CURVE_F_LT0,
	CURVE_ABS_F,
	CURVE_BASE
};

#define applyCustomCurve(x, idx) intpol(x, idx)

typedef struct {
	int8_t * curve;
	uint8_t points:7;
	uint8_t custom:1;
} __attribute__((__packed__)) CurveInfo;

extern int16_t intpol(int16_t x, uint8_t idx);
extern int16_t expo(int16_t x, int16_t k);
extern int16_t applyCurve(int16_t x, int8_t idx);


#endif /* CURVES_H_ */