/*
 * functions.h
 *
 * Created: 2/18/2020 3:34:25 PM
 *  Author: andrew.shkolik
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "main.h"

typedef struct {
	uint8_t activeFunctions;		// current max = 8 functions
	uint32_t  activeSwitches;		// current max = 32 function switches
	uint16_t lastFunctionTime[NUM_CFN];

	inline uint8_t isFunctionActive(uint8_t func)
	{
		return activeFunctions & ((uint8_t)1 << func);
	}

	void reset()
	{
		memclear(this, sizeof(*this));
	}
} __attribute__((__packed__)) CustomFunctionsContext;

enum FunctionsActive {
	FUNCTION_TRAINER,
	FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
	FUNCTION_VARIO,
	FUNCTION_BACKLIGHT
};

enum ResetFunctionParam {
	FUNC_RESET_TIMER1,
	FUNC_RESET_TIMER2,
	FUNC_RESET_FLIGHT,
	#if defined(FRSKY)
	FUNC_RESET_TELEMETRY,
	#endif
	#if ROTARY_ENCODERS > 0
	FUNC_RESET_ROTENC1,
	#endif
	#if ROTARY_ENCODERS > 1
	FUNC_RESET_ROTENC2,
	#endif
	FUNC_RESET_PARAMS_COUNT,
	FUNC_RESET_PARAM_LAST = FUNC_RESET_PARAMS_COUNT-1,
};

enum AdjustGvarFunctionParam {
	FUNC_ADJUST_GVAR_CONSTANT,
	FUNC_ADJUST_GVAR_SOURCE,
	FUNC_ADJUST_GVAR_GVAR,
	FUNC_ADJUST_GVAR_INC,
};

#if defined(OVERRIDE_CHANNEL_FUNCTION)
#define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE)
#else
#define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE && (func) != FUNC_OVERRIDE_CHANNEL)
#endif

#if defined(VOICE)
#define IS_PLAY_FUNC(func)      ((func) >= FUNC_PLAY_SOUND && func <= FUNC_PLAY_VALUE)
#else
#define IS_PLAY_FUNC(func)      ((func) == FUNC_PLAY_SOUND)
#endif

#define IS_PLAY_BOTH_FUNC(func) ((func) == FUNC_PLAY_BOTH)
#define IS_VOLUME_FUNC(func)    (0)

#if defined(GVARS)
#define IS_ADJUST_GV_FUNC(func) ((func) == FUNC_ADJUST_GVAR)
#else
#define IS_ADJUST_GV_FUNC(func) (0)
#endif

#if defined(HAPTIC)
#define IS_HAPTIC_FUNC(func)    ((func) == FUNC_HAPTIC)
#else
#define IS_HAPTIC_FUNC(func)    (0)
#endif

#define HAS_REPEAT_PARAM(func)    (IS_PLAY_FUNC(func) || IS_HAPTIC_FUNC(func))


extern CustomFunctionsContext modelFunctionsContext;

extern void evalFunctions();

#endif /* FUNCTIONS_H_ */