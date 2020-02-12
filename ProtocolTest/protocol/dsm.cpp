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

#include "dsm.h"

bool dsmRange = 0;

static void DSM2_Reset()
{
	UsartDisableTx();
#ifdef TELEMETRY
	UsartDisableRx();
#endif
}

static uint16_t DSM_cb()
{
	// Schedule next Mixer calculations.
	//SCHEDULE_MIXER_END_IN_US(22000);
	
	UsartTxBufferCount = 0;

	uint8_t dsmTxBufferCount = 14;

	uint8_t dsm_header;

	switch(g_model.DSM_TYPE)
	{
		case Sub_LP45:
			dsm_header = 0x00;
			break;
		case Sub_DSM2:
			dsm_header = 0x10;
			break;
		default: 
			dsm_header = 0x10 | DSMX_BIT; // PROTO_DSM2_DSMX
	}
	
	if(protoMode == BIND_MODE)
		dsm_header |= DSM2_SEND_BIND;
	else if(protoMode == RANGE_CHECK_MODE)
		dsm_header |= DSM2_SEND_RANGECHECK;

	UsartTxBuffer[--dsmTxBufferCount] = dsm_header;

	UsartTxBuffer[--dsmTxBufferCount] = g_model.modelId; // DSM2 Header. Second byte for model match.


	for (uint8_t i = 0; i < DSM2_CHANS; i++) 
	{
		//some dark magic here. Why not just limit(0, FULL_CHANNEL_OUTPUTS(i) + PPMCENTER,1023)? Maybe limiting throws to 90% ?
		uint16_t pulse = limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
		UsartTxBuffer[--dsmTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
		UsartTxBuffer[--dsmTxBufferCount] = pulse & 0xff; // Low byte
	}
	UsartTxBufferCount = 14; // Indicates data to transmit.

	UsartTransmitBuffer();

	heartbeat |= HEART_TIMER_PULSES;
	CALCULATE_LAT_JIT(); // Calculate latency and jitter.
	return 22000U * TIMER_MULTIPLIER; // 22 mSec Frame.
}

static void DSM_initialize()
{
	// 125K 8N1
	UsartSet125000BAUDS();
	UsartSet8N1();
	UsartEnableTx();
	UsartTxBufferCount = 0;
	PROTO_Start_Callback( DSM_cb);
}

const void *DSM_Cmds(enum ProtoCmds cmd)
{
	switch(cmd) 
	{
		case PROTOCMD_INIT:
			DSM_initialize();
			debugln("DSM_initialize");
			return 0;		
		case PROTOCMD_RESET:
			debugln("DSM2_Reset");
			PROTO_Stop_Callback();
			DSM2_Reset();
			return 0;
		//Same iu crap - enable later
		/*case PROTOCMD_GETOPTIONS:
			SetRfOptionSettings(pgm_get_far_address(RfOpt_Dsm_Ser),
			STR_DSM_PROTOCOLS,
			STR_DUMMY,
			STR_DUMMY,
			STR_DUMMY,
			STR_DUMMY,
			STR_DUMMY,
			STR_DUMMY);
			return 0;*/
		default:break;
	}
	return 0;
}