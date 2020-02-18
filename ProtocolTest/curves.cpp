/*
 * curves.cpp
 *
 * Created: 2/18/2020 4:44:57 PM
 *  Author: andrew.shkolik
 */ 
#include "curves.h"

int8_t *curveAddress(uint8_t idx)
{
	return &g_model.points[idx==0 ? 0 : 5*idx+g_model.curves[idx-1]];
}

CurveInfo curveInfo(uint8_t idx)
{
	CurveInfo result;
	result.curve = curveAddress(idx);
	int8_t *next = curveAddress(idx+1);
	uint8_t size = next - result.curve;
	if ((size & 1) == 0) 
	{
		result.points = (size / 2) + 1;
		result.custom = true;
	} 
	else 
	{
		result.points = size;
		result.custom = false;
	}
	return result;
}

int16_t intpol(int16_t x, uint8_t idx) // -100, -75, -50, -25, 0 ,25 ,50, 75, 100
{
	CurveInfo info = curveInfo(idx);
	int8_t *points = info.curve;
	uint8_t count = info.points;
	bool custom = info.custom;
	int16_t erg = 0;

	x += RESXu;

	if (x <= 0) 
	{
		erg = (int16_t)points[0] * (RESX/4);
	} 
	else if (x >= (RESX*2)) 
	{
		erg = (int16_t)points[count-1] * (RESX/4);
	} 
	else
	{
		uint16_t a=0, b=0;
		uint8_t i;
		if (custom) 
		{
			for (i=0; i<count-1; i++) 
			{
				a = b;
				b = (i==count-2 ? 2*RESX : RESX + calc100toRESX(points[count+i]));
				if ((uint16_t)x<=b) break;
			}
		} 
		else
		{
			uint16_t d = (RESX * 2) / (count-1);
			i = (uint16_t)x / d;
			a = i * d;
			b = a + d;
		}
		erg = (int16_t)points[i]*(RESX/4) + ((int32_t)(x-a) * (points[i+1]-points[i]) * (RESX/4)) / ((b-a));
	}

	return erg / 25; // 100*D5/RESX;
}

#if   defined(CURVES)
int16_t applyCurve(int16_t x, int8_t idx)
{
	/* already tried to have only one return at the end */
	switch(idx) 
	{
		case CURVE_NONE:
		return x;
		case CURVE_X_GT0:
		if (x < 0) x = 0; //x|x>0
		return x;
		case CURVE_X_LT0:
		if (x > 0) x = 0; //x|x<0
		return x;
		case CURVE_ABS_X: // x|abs(x)
		return abs(x);
		case CURVE_F_GT0: //f|f>0
		return x > 0 ? RESX : 0;
		case CURVE_F_LT0: //f|f<0
		return x < 0 ? -RESX : 0;
		case CURVE_ABS_F: //f|abs(f)
		return x > 0 ? RESX : -RESX;
	}
	if (idx < 0) 
	{
		x = -x;
		idx = -idx + CURVE_BASE - 1;
	}
	return applyCustomCurve(x, idx - CURVE_BASE);
}
#endif


// input parameters;
//  x 0 to 1024;
//  k 0 to 100;
// output between 0 and 1024
uint16_t expou(uint16_t x, uint16_t k)
{
	#if defined(EXTENDED_EXPO)
	bool extended;
	if (k>80) 
	{
		extended=true;
	} 
	else 
	{
		k += (k>>2);  // use bigger values before extend, because the effect is anyway very very low
		extended=false;
	}
	#endif

	k = calc100to256(k);

	uint32_t value = (uint32_t) x*x;
	value *= (uint32_t)k;
	value >>= 8;
	value *= (uint32_t)x;

	#if defined(EXTENDED_EXPO)
	if (extended)
	{  // for higher values do more multiplications to get a stronger expo curve
		value >>= 16;
		value *= (uint32_t)x;
		value >>= 4;
		value *= (uint32_t)x;
	}
	#endif

	value >>= 12;
	value += (uint32_t)(256-k)*x+128;

	return value>>8;
}

int16_t expo(int16_t x, int16_t k)
{
	if (k == 0) return x;
	int16_t y;
	bool neg = (x < 0);

	if (neg) x = -x;
	if (k<0) 
	{
		y = RESXu-expou(RESXu-x, -k);
	} 
	else 
	{
		y = expou(x, k);
	}
	return neg? -y : y;
}