/*
 * interface.h
 *
 * Created: 1/26/2020 12:07:40 AM
 *  Author: Andrew
 */ 


#ifndef INTERFACE_H_
#define INTERFACE_H_


#include "misc.h"

#include "proto_ppm.cpp"

#ifdef DSM2_SERIAL
#include "dsm_serial.cpp"
#endif

//#ifdef PROTO_HAS_MULTI
#include "multi.cpp"
//#endif

#if defined(SPIMODULES)

#ifdef PROTO_HAS_CC2500
#include "iface_cc2500.h"
#include "cc2500.c"
#include "FRSKY_DEF_cc2500.cpp"
#include "FRSKYV_cc2500.c"
#include "FRSKYD_cc2500.c"
#include "FRSKYX_cc2500.c"
#include "SKYARTEC_cc2500.c"
#include "CORONA_cc2500.c"
#endif

#ifdef PROTO_HAS_CYRF6936
#include "iface_cyrf6936.h"
#include "cyrf6936.c"
#include "DEVO_cyrf6936.c"
#include "DSM_cyrf6936.c"
#include "J6PRO_cyrf6936.c"
#endif

#ifdef PROTO_HAS_A7105
#include "iface_a7105.h"
#include "a7105.c"
#include "FLYSKY_a7105.c"
#include "AFHDS2A_a7105.c"
#endif

#ifdef PROTO_HAS_NRF24L01
#include "iface_nrf24l01.h"
#include "nrf24l01.c"
#endif

#endif

#endif /* INTERFACE_H_ */