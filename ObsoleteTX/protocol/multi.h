/*
 * multi.h
 *
 * Created: 1/26/2020 5:33:42 PM
 *  Author: Andrew
 */ 


#ifndef MULTI_H_
#define MULTI_H_

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)
#define MULTI_CHANS							16
#define MULTI_CHAN_BITS						11

#define NO_SUBTYPE							0
#define MM_RF_CUSTOM_SELECTED				0xff

extern const RfOptionSettingsvarstruct RfOpt_Multi_Ser[];

#endif /* MULTI_H_ */