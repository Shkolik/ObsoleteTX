/*
 * protocol.cpp
 *
 * Created: 1/26/2020 3:50:20 PM
 *  Author: Andrew
 */ 

#include "protocol.h"

const void * (*PROTO_Cmds)(enum ProtoCmds);

const pm_char STR_PPM[] PROGMEM = "PPM";
#ifdef DSM2_SERIAL
const pm_char STR_DSM_SERIAL[] PROGMEM = "DSM-SERI"
#endif
#ifdef MULTIMOD
const pm_char STR_MULTI[] PROGMEM = "MultiMod"
#endif

Proto_struct Protos[] = {
	{ PROTOCOL_PPM, STR_PPM, PROTO_PPM_Cmds },
	#ifdef DSM2_SERIAL
	{ PROTOCOL_DSM_SERIAL, STR_DSM_SERIAL, DSM_SERIAL_Cmds },
	#endif
	#ifdef MULTIMOD
	{ PROTOCOL_MULTI, STR_MULTI, MULTI_Cmds }
	#endif
};