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


#include "switches.h"

#define CS_LAST_VALUE_INIT -32768


int16_t lsLastValue[NUM_LOGICAL_SWITCH];
#define LS_LAST_VALUE(fm, idx) lsLastValue[idx]

volatile uint16_t s_last_switch_used = 0;
volatile uint16_t s_last_switch_value = 0;


uint8_t getLogicalSwitch(uint8_t idx)
{
	LogicalSwitchData * ls = lswAddress(idx);
	uint8_t result;

	uint8_t s = (ls->andsw);

	if (ls->func == LS_FUNC_NONE || (s && !getSwitch(s))) 
	{
		if (ls->func != LS_FUNC_STICKY) 
		{
			// AND switch must not affect STICKY and EDGE processing
			LS_LAST_VALUE(mixerCurrentFlightMode, idx) = CS_LAST_VALUE_INIT;
		}
		result = false;
	} 
	else if ((s=lswFamily(ls->func)) == LS_FAMILY_BOOL) 
	{
		uint8_t res1 = getSwitch(ls->v1);
		uint8_t res2 = getSwitch(ls->v2);
		switch (ls->func) 
		{
			case LS_FUNC_AND:
			result = (res1 && res2);
			break;
			case LS_FUNC_OR:
			result = (res1 || res2);
			break;
			// case LS_FUNC_XOR:
			default:
			result = (res1 ^ res2);
			break;
		}
	} 
	else if (s == LS_FAMILY_TIMER) 
	{
		result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) <= 0);
	} 
	else if (s == LS_FAMILY_STICKY) 
	{
		result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) & (1<<0));
	} 
	else 
	{
		int16_t x = getValue(ls->v1);
		int16_t y;
		if (s == LS_FAMILY_COMP) 
		{
			y = getValue(ls->v2);

			switch (ls->func) 
			{
				case LS_FUNC_EQUAL:
				result = (x==y);
				break;
				case LS_FUNC_GREATER:
				result = (x>y);
				break;
				default:
				result = (x<y);
				break;
			}
		} 
		else 
		{
			uint8_t v1 = ls->v1;
			
			if (v1 >= MIXSRC_GVAR1) 
			{
				y = ls->v2; // it's a GVAR
			} 
			else 
			{
				y = calc100toRESX(ls->v2);
			}
			
			switch (ls->func) 
			{
				case LS_FUNC_VEQUAL:// v==offset
					result = (x==y);
					break;

				case LS_FUNC_VALMOSTEQUAL:
					#if defined(GVARS)
					if (v1 >= MIXSRC_GVAR1 && v1 <= MIXSRC_LAST_GVAR)
						result = (x==y);
					else
					#endif
					result = (abs(x-y) < (1024 / STICK_TOLERANCE));
					break;
				case LS_FUNC_VPOS:
					result = (x>y);
					break;
				case LS_FUNC_VNEG:
					result = (x<y);
					break;
				case LS_FUNC_APOS:
					result = (abs(x)>y);
					break;
				case LS_FUNC_ANEG:
					result = (abs(x)<y);
					break;
				default: {
					if (LS_LAST_VALUE(mixerCurrentFlightMode, idx) == CS_LAST_VALUE_INIT) 
					{
						LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
					}
					int16_t diff = x - LS_LAST_VALUE(mixerCurrentFlightMode, idx);
					uint8_t update = false;
					if (ls->func == LS_FUNC_DIFFEGREATER) 
					{
						if (y >= 0) 
						{
							result = (diff >= y);
							if (diff < 0)
								update = true;
						} 
						else 
						{
							result = (diff <= y);
							if (diff > 0)
								update = true;
						}
					} 
					else 
					{
						result = (abs(diff) >= y);
					}
					if (result || update) 
					{
						LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
					}
					break;
				}
			}
		}
	}

	return result;
}

uint8_t getSwitch(int8_t swtch)
{
	uint8_t result;

	if (swtch == SWSRC_NONE)
		return true;

	uint8_t cs_idx = abs(swtch);

	if (cs_idx == SWSRC_ONE) 
	{
		result = !s_mixer_first_run_done;
	}
	else if (cs_idx == SWSRC_ON) 
	{
		result = true;
	} 
	else if (cs_idx <= SWSRC_LAST_SWITCH) 
	{
		result = switchState((EnumKeys)(SW_BASE+cs_idx-SWSRC_FIRST_SWITCH));
	}
	else if (cs_idx <= SWSRC_LAST_TRIM) 
	{
		uint8_t idx = cs_idx - SWSRC_FIRST_TRIM;
		idx = (CONVERT_MODE(idx/2) << 1) + (idx & 1);
		result = trimDown(idx);
	}
	else if (cs_idx == SWSRC_XD0) 
	{
		result = (calibratedStick[NUM_STICKS+EXTRA_3POS-1] > 0x200); // > 512
	}
	else if (cs_idx == SWSRC_XD1) 
	{
		result = IS_IN_RANGE(calibratedStick[NUM_STICKS+EXTRA_3POS-1],-(0x200),0x200); // -512 < X < 512
	}
	else if (cs_idx == SWSRC_XD2) 
	{
		result = (calibratedStick[NUM_STICKS+EXTRA_3POS-1] < -(0x200)); // < -512
	}
	else 
	{
		cs_idx -= SWSRC_FIRST_LOGICAL_SWITCH;
		uint16_t mask = ((uint16_t)1 << cs_idx);
		if (s_last_switch_used & mask) 
		{
			result = (s_last_switch_value & mask);
		} 
		else 
		{
			s_last_switch_used |= mask;
			result = getLogicalSwitch(cs_idx);
			if (result) 
			{
				s_last_switch_value |= mask;
			} 
			else 
			{
				s_last_switch_value &= ~mask;
			}
		}
	}

	return swtch > 0 ? result : !result;
}

void logicalSwitchesReset()
{
	s_last_switch_value = 0;

	for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
		LS_LAST_VALUE(fm, i) = CS_LAST_VALUE_INIT;
	}
}

uint8_t lswFamily(uint8_t func)
{
	if (func <= LS_FUNC_ANEG)
		return LS_FAMILY_OFS;
	else if (func <= LS_FUNC_XOR)
		return LS_FAMILY_BOOL;
	else if (func <= LS_FUNC_LESS)
		return LS_FAMILY_COMP;
	else if (func <= LS_FUNC_ADIFFEGREATER)
		return LS_FAMILY_DIFF;
	else
		return LS_FAMILY_TIMER+func-LS_FUNC_TIMER;
}

LogicalSwitchData * lswAddress(uint8_t idx)
{
	return &g_model.logicalSw[idx];
}