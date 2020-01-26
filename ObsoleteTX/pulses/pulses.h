/*
 * pulses.h
 *
 * Created: 1/25/2020 1:24:46 AM
 *  Author: Andrew
 */ 


#ifndef PULSES_H_
#define PULSES_H_


#define SCHEDULE_MIXER_END_IN_US(delay_us) nextMixerEndTime = getTmr64uS() + US_TO_64US_TICK(delay_us) - US_TO_64US_TICK(400) // 400 uS

#define IS_WAIT_PUPIL_STATE()       ((g_model.rfProtocol == (PROTOCOL_PPM16)) || (g_model.rfProtocol == (PROTOCOL_PPMSIM)))

#define IS_PPM_PROTOCOL(protocol)          (protocol<=PROTOCOL_PPMSIM)

#if defined(DSM2_SERIAL) // Todo check in needed in per10ms
#define IS_DSM2_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_DSM_SERIAL)
#else
#define IS_DSM2_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(MULTIMODULE)
#define IS_MULTIMODULE_PROTOCOL(protocol)  (protocol==PROTOCOL_MULTI)
#else
#define IS_MULTIMODULE_PROTOCOL(protocol)  (0)
#endif

#if defined(SPIMODULES)
#if defined(MULTIMODULE)
#define LASTPROTOMENU1 PROTOCOL_MULTI+1
#else
#if defined(DSM2_SERIAL)
#define LASTPROTOMENU1 PROTOCOL_DSM_SERIAL+1
#else
#define LASTPROTOMENU1 PROTOCOL_PPMSIM+1
#endif
#endif
#define IS_SPIMODULES_PROTOCOL(protocol)  (protocol>=LASTPROTOMENU1)
#else
#define IS_SPIMODULES_PROTOCOL(protocol)  (0)
#define LASTPROTOMENU1 PROTOCOL_COUNT-1
#endif

#endif /* PULSES_H_ */