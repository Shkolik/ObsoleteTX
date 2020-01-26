/*
 * protocol.h
 *
 * Created: 1/26/2020 12:07:10 AM
 *  Author: Andrew
 */ 

#include "../global_defs.h"

#if defined(PROTO_INIT)

#else

#define PROTO_INIT

extern const void * (*PROTO_Cmds)(enum ProtoCmds);
typedef const void* (*CMDS)(enum ProtoCmds);


//Add more protocols if needed
enum Protocols {
	PROTOCOL_PPM,
#ifdef DSM2_SERIAL
	PROTOCOL_DSM_SERIAL,
#endif
#ifdef MULTIMOD
	PROTOCOL_MULTI,
#endif
	PROTOCOL_COUNT
};

typedef struct {
	enum Protocols Protocol;
	const pm_char* ProtoName;
	CMDS Cmds; // Cmds
} Proto_struct;


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

#endif
