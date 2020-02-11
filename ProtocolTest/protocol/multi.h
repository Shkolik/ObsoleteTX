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

#ifndef MULTI_H_
#define MULTI_H_

#include "../main.h"

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)

#define MULTI_CHANS							16
#define MULTI_CHAN_BITS						11

#define MULTIRFPROTOCOL rfOptionValue1
#define OPTION			rfOptionValue2
#define SUB_TYPE		rfSubType
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3

#define NO_SUBTYPE							0
#define MM_RF_CUSTOM_SELECTED				0xff

#define MULTI_RF_PROTO_LAST 255  

enum MultiModuleRFProtocols {
	MM_RF_PROTO_CUSTOM = -1,
	MM_RF_PROTO_FIRST = MM_RF_PROTO_CUSTOM,
	MM_RF_PROTO_FLYSKY=0,
	MM_RF_PROTO_HUBSAN,
	MM_RF_PROTO_FRSKY,
	MM_RF_PROTO_HISKY,
	MM_RF_PROTO_V2X2,
	MM_RF_PROTO_DSM2,
	MM_RF_PROTO_DEVO,
	MM_RF_PROTO_YD717,
	MM_RF_PROTO_KN,
	MM_RF_PROTO_SYMAX,
	MM_RF_PROTO_SLT,
	MM_RF_PROTO_CX10,
	MM_RF_PROTO_CG023,
	MM_RF_PROTO_BAYANG,
	MM_RF_PROTO_ESky,
	MM_RF_PROTO_MT99XX,
	MM_RF_PROTO_MJXQ,
	MM_RF_PROTO_SHENQI,
	MM_RF_PROTO_FY326,
	MM_RF_PROTO_SFHSS,
	MM_RF_PROTO_J6PRO,
	MM_RF_PROTO_FQ777,
	MM_RF_PROTO_ASSAN,
	MM_RF_PROTO_HONTAI,
	MM_RF_PROTO_OLRS,
	MM_RF_PROTO_FS_AFHDS2A,
	MM_RF_PROTO_Q2X2,
	MM_RF_PROTO_WK_2X01,
	MM_RF_PROTO_Q303,
	MM_RF_PROTO_GW008,
	MM_RF_PROTO_DM002,
	MM_RF_PROTO_LAST= MM_RF_PROTO_DM002
};

enum MMDSM2Subtypes {
	MM_RF_DSM2_SUBTYPE_DSM2_22,
	MM_RF_DSM2_SUBTYPE_DSM2_11,
	MM_RF_DSM2_SUBTYPE_DSMX_22,
	MM_RF_DSM2_SUBTYPE_DSMX_11,
	MM_RF_DSM2_SUBTYPE_AUTO
};

enum MMRFrskySubtypes {
	MM_RF_FRSKY_SUBTYPE_D16,
	MM_RF_FRSKY_SUBTYPE_D8,
	MM_RF_FRSKY_SUBTYPE_D16_8CH,
	MM_RF_FRSKY_SUBTYPE_V8,
	MM_RF_FRSKY_SUBTYPE_D16_LBT,
	MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
};

extern const void *MULTI_Cmds(enum ProtoCmds cmd);

#endif /* MULTI_H_ */