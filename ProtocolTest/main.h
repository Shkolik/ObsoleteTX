/*
 * main.h
 *
 * Created: 1/31/2020 6:26:04 PM
 *  Author: andrew.shkolik
 */


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include "pgmtypes.h"

//in case we need debug output
#ifdef USART_DBG
#include "USART_DBG/Serial.h"
#define debugln(ln) Serial0.println(ln)
#define debug(ln) Serial0.print(ln)
#define init_debug(baud) Serial0.init(baud)
#else
#define debugln(ln) 
#define debug(ln) 
#define init_debug(baud)
#endif

#include "usart_driver.h"
#include "keys.h"

#define MULTI

#ifndef PIN0_bm
#define PIN0_bm  0x01
#endif
#ifndef PIN1_bm
#define PIN1_bm  0x02
#endif
#ifndef PIN2_bm
#define PIN2_bm  0x04
#endif
#ifndef PIN3_bm
#define PIN3_bm  0x08
#endif
#ifndef PIN4_bm
#define PIN4_bm  0x10
#endif
#ifndef PIN5_bm
#define PIN5_bm  0x20
#endif
#ifndef PIN6_bm
#define PIN6_bm  0x40
#endif
#ifndef PIN7_bm
#define PIN7_bm  0x80
#endif

#define IS_PIN_HI(reg, pin) ((reg & (1 << pin)) > 0)
#define IS_PIN_LOW(reg, pin) !IS_PIN_HI(reg, pin)


#define DBG_UART_USB

#define CALCULATE_LAT_JIT()  dt = TCNT1 - OCR1A		// Calculate latency and jitter.

#define TIMER_10MS_VECT		TIMER0_COMP_vect //10ms timer
#define TIMER_10MS_COMPVAL	OCR0

#define RF_TIMER_COMPA_REG  OCR1A
#define RF_TIMER_COMPA_VECT TIMER1_COMPA_vect

#define MAX_MIXER_DELTA_US	(50000)	// 50ms max as an interval between 2 mixer calculations

#define PULSES_WORD_SIZE	19			// A 8 Channel PPM frame has 18 timing events + 1 int terminator
										// So, 19 should be enough, but will I use the same buffer for Multiprotocol module
										// and it has 26 bytes frame, that is 13 WORDs - still will work

#define PPM_CENTER			1500
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]

//for 16MHz we need multiply some timings by 2
#define TIMER_MULTIPLIER (F_CPU == 8000000L ? 1 : 2)
//for 8MHz we need divide some values by 2 (using bit-shift)
#define TIMER_DEMULTIPLIER (F_CPU == 8000000L ? 1 : 0)

#define OUT_PORT	PORTD
#define OUT_PIN		PIN0_bm

#define IN_PORT		PORTF
#define IN_PIN		PIN1_bm

#define LED_PORT	PORTE
#define LED_PIN		PIN4_bm

#define BIND_PRESSED	IS_PIN_LOW(PING, 3)
#define CHANGE_PRESSED	IS_PIN_LOW(PING, 4)

#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

//////////////////////////////////////////////////////////////////////////
//MULTI
//////////////////////////////////////////////////////////////////////////
#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)

#define MULTI_CHANS							16
#define MULTI_CHAN_BITS						11

#define MULTIRFPROTOCOL rfOptionValue1
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3

#define NO_SUBTYPE							0
#define MM_RF_CUSTOM_SELECTED				0xff

#define MULTI_RF_PROTO_LAST 100  // Use a large value

enum PROTO_MODE {
	NORMAL_MODE,
	BIND_MODE,
};

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

typedef struct {
	uint8_t protocol;
	const pm_char *subTypeString;
	uint8_t maxSubtype;
	const char *optionsstr;
} __attribute__((__packed__)) mm_protocol_definition;

struct RfOptionSettingsvarstruct {
	uint8_t         rfProtoNeed:4;     // See usage in "PROTO_NEED_XX" Def
	uint8_t         rfSubTypeMax:4;       //16 max
	int8_t          rfOptionValue1Min;
	int8_t          rfOptionValue1Max;
	int8_t          rfOptionValue2Min;
	int8_t          rfOptionValue2Max;
	int8_t          rfOptionValue3Max;/*:5*/  //32 max -16 is min
} __attribute__((__packed__));

#define BOOL1USED PIN0_bm
#define BOOL2USED PIN1_bm
#define BOOL3USED PIN2_bm

void SetRfOptionSettings(uint_farptr_t RfOptSet,
const pm_char* rfSubTypeNames,
const pm_char* rfOptionValue1Name,
const pm_char* rfOptionValue2Name,
const pm_char* rfOptionValue3Name,
const pm_char* rfOptionBool1Name,
const pm_char* rfOptionBool2Name,
const pm_char* rfOptionBool3Name);

extern const pm_char STR_DUMMY[];
#define INDENT               "\001"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
///USART
//////////////////////////////////////////////////////////////////////////
//USART driver (static register dispatcher)

#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

#define _DOR_N(usart_no) DOR ## usart_no
#define _FE_N(usart_no) FE ## usart_no
#define _RXCIE_N(usart_no) RXCIE ## usart_no
#define _RXC_N(usart_no) RXC ## usart_no
#define _RXD_DDR_N(usart_no) RXD_DDR ## usart_no
#define _RXD_DDR_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXEN_N(usart_no) RXEN ## usart_no
#define _TXCIE_N(usart_no) TXCIE ## usart_no
#define _TXEN_N(usart_no) TXEN ## usart_no
#define _U2X_N(usart_no) U2X ## usart_no
#define _UBRRH_N(usart_no) UBRR ## usart_no ## H
#define _UBRRL_N(usart_no) UBRR ## usart_no ## L
#define _UCSRA_N(usart_no) UCSR ## usart_no ## A
#define _UCSRB_N(usart_no) UCSR ## usart_no ## B
#define _UCSRC_N(usart_no) UCSR ## usart_no ## C
#define _UCSZ0_N(usart_no) UCSZ ## usart_no ## 0
#define _UCSZ1_N(usart_no) UCSZ ## usart_no ## 1
#define _UCSZ2_N(usart_no) UCSZ ## usart_no ## 2
#define _UDRIE_N(usart_no) UDRIE ## usart_no
#define _UDR_N(usart_no) UDR ## usart_no
#define _UPE_N(usart_no) UPE ## usart_no
#define _USART_RX_vect_N(usart_no) USART ## usart_no ## _RX_vect
#define _USART_UDRE_vect_N(usart_no) USART ## usart_no ## _UDRE_vect

#define DOR_N(usart_no) _DOR_N(usart_no)
#define FE_N(usart_no) _FE_N(usart_no)
#define RXCIE_N(usart_no) _RXCIE_N(usart_no)
#define RXC_N(usart_no) _RXC_N(usart_no)
#define RXD_DDR_N(usart_no) _RXD_DDR_N(usart_no)
#define RXD_DDR_PIN_N(usart_no) _RXD_DDR_PIN_N(usart_no)
#define RXD_PORT_N(usart_no) _RXD_PORT_N(usart_no)
#define RXD_PORT_PIN_N(usart_no) _RXD_PORT_PIN_N(usart_no)
#define RXEN_N(usart_no) _RXEN_N(usart_no)
#define TXCIE_N(usart_no) _TXCIE_N(usart_no)
#define TXEN_N(usart_no) _TXEN_N(usart_no)
#define U2X_N(usart_no) _U2X_N(usart_no)
#define UBRRH_N(usart_no) _UBRRH_N(usart_no)
#define UBRRL_N(usart_no) _UBRRL_N(usart_no)
#define UCSRA_N(usart_no) _UCSRA_N(usart_no)
#define UCSRB_N(usart_no) _UCSRB_N(usart_no)
#define UCSRC_N(usart_no) _UCSRC_N(usart_no)
#define UCSZ0_N(usart_no) _UCSZ0_N(usart_no)
#define UCSZ1_N(usart_no) _UCSZ1_N(usart_no)
#define UCSZ2_N(usart_no) _UCSZ2_N(usart_no)
#define UDRIE_N(usart_no) _UDRIE_N(usart_no)
#define UDR_N(usart_no) _UDR_N(usart_no)
#define UPE_N(usart_no) _UPE_N(usart_no)
#define USART_RX_vect_N(usart_no) _USART_RX_vect_N(usart_no)
#define USART_UDRE_vect_N(usart_no) _USART_UDRE_vect_N(usart_no)

//#define USART_RX_vect USART0_RX_vect
//END: USART driver (static register dispatcher)
//////////////////////////////////////////////////////////////////////////

typedef struct {
	//Rf data
	uint8_t   modelId:6;         //64 max
	uint8_t   rfProtocol:6;      //64 max
	uint8_t   rfSubType:4;       //16 max
	int8_t    rfOptionValue1;
	int8_t    rfOptionValue2;
	uint8_t   rfOptionValue3:5;  //32 max
	uint8_t   rfOptionBool1:1;
	uint8_t   rfOptionBool2:1;
	uint8_t   rfOptionBool3:1;
	//end of RF data	
} __attribute__((__packed__)) ModelData;

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
template<class t> FORCEINLINE t min(t a, t b){ return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b){ return a>b?a:b; }
template<class t> FORCEINLINE t limit(t mi, t x, t ma){	return min(max(mi,x),ma); }

enum Protocols {
	PROTOCOL_PPM,
	//PROTOCOL_DSM_SERIAL,
	PROTOCOL_MULTI,
	PROTOCOL_COUNT
};

enum ProtoCmds {
	PROTOCMD_INIT,
	PROTOCMD_BIND,
	PROTOCMD_RESET,
	PROTOCMD_GETOPTIONS
};

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum Protocols Protocol;
	CMDS Cmds; // Cmds
} __attribute__((__packed__)) Proto_struct;


#define PULSEPOL 0
#define PPMCHMAX 6

extern uint8_t heartbeat;

extern uint8_t s_current_protocol;

extern int16_t channelOutputs[PPMCHMAX];
extern uint16_t pulses1MHz[PULSES_WORD_SIZE];
extern uint16_t *pulses1MHzWPtr;
extern uint16_t nextMixerEndTime;
extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t dt;
extern uint8_t protoMode;
extern bool rangeModeIsOn;

extern uint16_t bind_tmr10ms;

extern uint16_t g_tmr10ms;
//extern void blinkLed(uint8_t count);
extern void setPinState(uint8_t state);
extern void togglePin();
extern void PROTO_Stop_Callback();
extern void PROTO_Start_Callback( uint16_t (*cb)());
extern void PROTO_SetBindState(uint16_t t10ms);

extern void setupPulsesPPM();
extern const void * PROTO_PPM_Cmds(enum ProtoCmds);
extern const void *MULTI_Cmds(enum ProtoCmds cmd);
extern const void * (*PROTO_Cmds)(enum ProtoCmds);  //protocol callback
extern uint16_t (*timer_callback)(void);			// Function pointer to add flexibility and simplicity to ISR.

extern void parseTelemFrskyByte(uint8_t data);

extern const ModelData g_model;

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2 // when multiple modules this is the first one
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)

#define IS_SPIMODULES_PROTOCOL(protocol)  (0)
#define LASTPROTOMENU1 PROTOCOL_COUNT-1

//  Dimension of Arrays
#define DIM(array) ((sizeof array) / (sizeof *array))
#define memclear(p, s) memset(p, 0, s)


#endif /* MAIN_H_ */