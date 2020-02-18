/*
 * functions.cpp
 *
 * Created: 2/18/2020 3:34:34 PM
 *  Author: andrew.shkolik
 */ 

#include "functions.h"

CustomFunctionsContext modelFunctionsContext = { 0 };
	
#define functions g_model.customFn
#define functionsContext modelFunctionsContext

void evalFunctions()
{
	uint8_t newActiveFunctions  = 0;
	uint32_t  newActiveSwitches = 0;

	#define PLAY_INDEX   (i+1)

	#if defined(ROTARY_ENCODERS) && defined(GVARS)
	static rotenc_t rePreviousValues[ROTARY_ENCODERS];
	#endif

	#if defined(OVERRIDE_CHANNEL_FUNCTION)
	for (uint8_t i=0; i<NUM_CHNOUT; i++) {
		safetyCh[i] = OVERRIDE_CHANNEL_UNDEFINED;
	}
	#endif

	#if defined(GVARS)
	for (uint8_t i=0; i<NUM_STICKS; i++) {
		trimGvar[i] = -1;
	}
	#endif

	for (uint8_t i=0; i<NUM_CFN; i++) {
		const CustomFunctionData * cfn = &functions[i];
		int8_t swtch = CFN_SWITCH(cfn);
		if (swtch) {
			uint32_t switch_mask = ((uint32_t)1 << i);

			bool active = getSwitch(swtch);

			if (HAS_ENABLE_PARAM(CFN_FUNC(cfn))) {
				active &= (bool)CFN_ACTIVE(cfn);
			}

			if (active || IS_PLAY_BOTH_FUNC(CFN_FUNC(cfn))) {

				switch (CFN_FUNC(cfn)) {

					#if defined(OVERRIDE_CHANNEL_FUNCTION)
					case FUNC_OVERRIDE_CHANNEL:
					safetyCh[CFN_CH_INDEX(cfn)] = CFN_PARAM(cfn);
					break;
					#endif

					case FUNC_TRAINER: {
						uint8_t mask = 0x0f;
						if (CFN_CH_INDEX(cfn) > 0) {
							mask = (1<<(CFN_CH_INDEX(cfn)-1));
						}
						newActiveFunctions |= mask;
						break;
					}

					case FUNC_INSTANT_TRIM:
					newActiveFunctions |= (1 << FUNCTION_INSTANT_TRIM);
					if (!modelFunctionsContext.isFunctionActive(FUNCTION_INSTANT_TRIM)) {
						#if defined(GUI)
						if (menuHandlers[0] == menuMainView)
						#endif
						{
							//TODO: LAter
							//instantTrim();
						}
					}
					break;

					case FUNC_RESET:
					switch (CFN_PARAM(cfn)) {
						case FUNC_RESET_TIMER1:
						case FUNC_RESET_TIMER2:
						timerReset(CFN_PARAM(cfn));
						break;
						case FUNC_RESET_FLIGHT:
						flightReset();
						break;
						#if defined(FRSKY)
						case FUNC_RESET_TELEMETRY:
						telemetryResetValue();
						break;
						#endif
						#if ROTARY_ENCODERS > 0
						case FUNC_RESET_ROTENC1:
						#if ROTARY_ENCODERS > 1
						case FUNC_RESET_ROTENC2:
						#endif
						g_rotenc[CFN_PARAM(cfn)-FUNC_RESET_ROTENC1] = 0;
						break;
						#endif
					}
					break;

					#if defined(GVARS)
					case FUNC_ADJUST_GVAR:
					if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_CONSTANT) {
						SET_GVAR(CFN_GVAR_INDEX(cfn), CFN_PARAM(cfn), mixerCurrentFlightMode);
						} else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_GVAR) {
						SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_PARAM(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_PARAM(cfn))), mixerCurrentFlightMode);
						} else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_INC) {
						if (!(functionsContext.activeSwitches & switch_mask)) {
							SET_GVAR(CFN_GVAR_INDEX(cfn), limit(-GVAR_MAX, GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + (CFN_PARAM(cfn) ? +1 : -1), GVAR_MAX), mixerCurrentFlightMode);
						}
						} else if (CFN_PARAM(cfn) >= MIXSRC_TrimRud && CFN_PARAM(cfn) <= MIXSRC_TrimAil) {
						trimGvar[CFN_PARAM(cfn)-MIXSRC_TrimRud] = CFN_GVAR_INDEX(cfn);
					}
					#if defined(ROTARY_ENCODERS)
					else if (CFN_PARAM(cfn) >= MIXSRC_REa && CFN_PARAM(cfn) < MIXSRC_TrimRud) {
						int8_t scroll = rePreviousValues[CFN_PARAM(cfn)-MIXSRC_REa] - (g_rotenc[CFN_PARAM(cfn)-MIXSRC_REa] / ROTARY_ENCODER_GRANULARITY);
						if (scroll) {
							SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + scroll, mixerCurrentFlightMode);
						}
					}
					#endif
					else {
						SET_GVAR(CFN_GVAR_INDEX(cfn), calcRESXto100(getValue(CFN_PARAM(cfn))), mixerCurrentFlightMode);
					}
					break;
					#endif


					#if defined(VOICE)
					case FUNC_PLAY_SOUND:
					case FUNC_PLAY_TRACK:
					case FUNC_PLAY_BOTH:
					case FUNC_PLAY_VALUE: {
						tmr10ms_t tmr10ms = getTmr10ms();
						uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
						if (!functionsContext.lastFunctionTime[i] || (CFN_FUNC(cfn)==FUNC_PLAY_BOTH && active!=(bool)(functionsContext.activeSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
							functionsContext.lastFunctionTime[i] = tmr10ms;
							uint8_t param = CFN_PARAM(cfn);
							if (CFN_FUNC(cfn) == FUNC_PLAY_SOUND) {
								AUDIO_PLAY(AU_FRSKY_FIRST+param);
								} else if (CFN_FUNC(cfn) == FUNC_PLAY_VALUE) {
								PLAY_VALUE(param, PLAY_INDEX);
								} else {
								#if defined(GVARS)
								if (CFN_FUNC(cfn) == FUNC_PLAY_TRACK && param > 250)
								param = GVAR_VALUE(param-251, getGVarFlightPhase(mixerCurrentFlightMode, param-251));
								#endif
								PUSH_CUSTOM_PROMPT(active ? param : param+1, PLAY_INDEX);
							}
						}
						if (!active) {
							// PLAY_BOTH would change activeFnSwitches otherwise
							switch_mask = 0;
						}
						break;
					}
					#else
					case FUNC_PLAY_SOUND: {
						uint16_t tmr10ms = g_tmr10ms;
						uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
						if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
							functionsContext.lastFunctionTime[i] = tmr10ms;
							AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(cfn));
						}
						break;
					}
					#endif

					#if defined(FRSKY) && defined(VARIO)
					case FUNC_VARIO:
					newActiveFunctions |= (1 << FUNCTION_VARIO);
					break;
					#endif

					#if defined(HAPTIC)
					case FUNC_HAPTIC: {
						tmr10ms_t tmr10ms = getTmr10ms();
						uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
						if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
							functionsContext.lastFunctionTime[i] = tmr10ms;
							haptic.event(AU_FRSKY_LAST+CFN_PARAM(cfn));
						}
						break;
					}
					#endif

					#if defined(SDCARD)
					case FUNC_LOGS:
					if (CFN_PARAM(cfn)) {
						newActiveFunctions |= (1 << FUNCTION_LOGS);
						logDelay = CFN_PARAM(cfn);
					}
					break;
					#endif

					case FUNC_BACKLIGHT:
					newActiveFunctions |= (1 << FUNCTION_BACKLIGHT);
					break;

				}

				newActiveSwitches |= switch_mask;
				} else {
				functionsContext.lastFunctionTime[i] = 0;
			}
		}
	}

	functionsContext.activeSwitches   = newActiveSwitches;
	functionsContext.activeFunctions  = newActiveFunctions;

	#if defined(ROTARY_ENCODERS) && defined(GVARS)
	for (uint8_t i=0; i<ROTARY_ENCODERS; i++) {
		rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
	}
	#endif
}

#undef functions
#undef functionsContext