/*
 * protocol.h
 *
 * Created: 1/26/2020 12:07:10 AM
 *  Author: Andrew
 */ 

#include "../global_defs.h"

extern const void * (*PROTO_Cmds)(enum ProtoCmds);

extern const pm_char STR_PPM[];
extern const void * PROTO_PPM_Cmds(enum ProtoCmds);
#ifdef DSM2_SERIAL
extern const pm_char STR_DSM_SERIAL[];
extern const void * DSM_SERIAL_Cmds(enum ProtoCmds);
#endif
#ifdef MULTIMODULE
extern const pm_char STR_MULTI[];
extern const void * MULTI_Cmds(enum ProtoCmds);
#endif
extern Proto_struct Protos[];

