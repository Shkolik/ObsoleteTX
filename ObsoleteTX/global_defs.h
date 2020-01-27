/*
 * GlobalDefinitions.h
 *
 * Created: 1/26/2020 1:28:34 PM
 *  Author: Andrew
 */ 


#ifndef GLOBALDEFINITIONS_H_
#define GLOBALDEFINITIONS_H_
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "pgmtypes.h"
#include <string.h>

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
#define memclear(p, s) memset(p, 0, s)

//if not defined in config
#ifndef NUM_STICKS
#define NUM_STICKS           4
#endif

#if defined(EXTERNALEEPROM)
#define MAX_MODELS           55
#define NUM_CHNOUT           16 // number of real output channels CH1-CH16
#define MAX_FLIGHT_MODES     6
#define MAX_MIXERS           40
#define MAX_EXPOS            16
#define NUM_LOGICAL_SWITCH   20 // number of custom switches
#define GETSWITCH_RECURSIVE_TYPE uint32_t
#define NUM_CFN              32 // number of functions assigned to switches
#define MAX_GVARS            12
#define NUM_TRAINER          8
#define NUM_POTS             3

#else
#define MAX_MODELS           30
#define NUM_CHNOUT           16 // number of real output channels CH1-CH16
#define MAX_FLIGHT_MODES     6
#define MAX_MIXERS           32
#define MAX_EXPOS            16
#define NUM_LOGICAL_SWITCH   15 // number of custom switches
#define GETSWITCH_RECURSIVE_TYPE uint16_t
#define NUM_CFN              24 // number of functions assigned to switches
#define MAX_GVARS            6
#define NUM_TRAINER          8
#define NUM_POTS             3

#endif

// PPM Def
#define PPMFRAMELENGTH  rfOptionValue1 // 0=22.5ms  (10ms-30ms) 0.5ms increments
#define PPMDELAY        rfOptionValue2
#define PPMNCH          rfSubType
#define PULSEPOL        rfOptionBool1
//Multi Def
#define MULTIRFPROTOCOL rfOptionValue1
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3
//SPI Def
#define RFPOWER         rfOptionValue3



enum BaseCurves {
	CURVE_NONE,
	CURVE_X_GT0,
	CURVE_X_LT0,
	CURVE_ABS_X,
	CURVE_F_GT0,
	CURVE_F_LT0,
	CURVE_ABS_F,
	CURVE_BASE
};

enum {
	CYRF6936,
	A7105,
	CC2500,
	NRF24L01,
	MULTIMOD,
	TX_MODULE_LAST,
};

enum PROTO_MODE {
	NORMAL_MODE,
	BIND_MODE,
};

enum TxPower {
	TXPOWER_0,
	TXPOWER_1,
	TXPOWER_2,
	TXPOWER_3,
	TXPOWER_4,
	TXPOWER_5,
	TXPOWER_6,
	TXPOWER_7,
};

enum TXRX_State {
	TXRX_OFF,
	TX_EN,
	RX_EN,
};

enum CswFunctionFamilies {
	LS_FAMILY_OFS,
	LS_FAMILY_BOOL,
	LS_FAMILY_COMP,
	LS_FAMILY_DIFF,
	LS_FAMILY_TIMER,
	LS_FAMILY_STICKY,
	LS_FAMILY_RANGE,
	LS_FAMILY_EDGE
};

/* make sure the defines below always go in numeric order */
enum AUDIO_SOUNDS {
	AU_TADA,
	#if defined(VOICE)
	AU_THROTTLE_ALERT,
	AU_SWITCH_ALERT,
	AU_BAD_EEPROM,
	AU_EEPROM_FORMATTING,
	#endif
	AU_TX_BATTERY_LOW,
	AU_INACTIVITY,
	AU_ERROR,
	AU_KEYPAD_UP,
	AU_KEYPAD_DOWN,
	AU_MENUS,
	AU_TRIM_MOVE,
	AU_WARNING1,
	AU_WARNING2,
	AU_WARNING3,
	AU_TRIM_MIDDLE,
	AU_POT_MIDDLE,
	AU_MIX_WARNING_1,
	AU_MIX_WARNING_2,
	AU_MIX_WARNING_3,
	AU_TIMER_00,
	AU_TIMER_LT10,
	AU_TIMER_20,
	AU_TIMER_30,
	AU_FRSKY_FIRST,
	AU_FRSKY_BEEP1 = AU_FRSKY_FIRST,
	AU_FRSKY_BEEP2,
	AU_FRSKY_BEEP3,
	AU_FRSKY_WARN1,
	AU_FRSKY_WARN2,
	AU_FRSKY_CHEEP,
	AU_FRSKY_RATATA,
	AU_FRSKY_TICK,
	AU_FRSKY_SIREN,
	AU_FRSKY_RING,
	AU_FRSKY_SCIFI,
	AU_FRSKY_ROBOT,
	AU_FRSKY_CHIRP,
	AU_FRSKY_TADA,
	AU_FRSKY_CRICKET,
	AU_FRSKY_ALARMC,
	AU_FRSKY_LAST,
	AU_NONE=0xff
};

enum ProtoCmds {
	PROTOCMD_INIT,
	PROTOCMD_BIND,
	PROTOCMD_RESET,
	PROTOCMD_GETOPTIONS,
};
enum PerOutMode {
	e_perout_mode_normal = 0,
	e_perout_mode_inactive_flight_mode = 1,
	e_perout_mode_notrainer = 2,
	e_perout_mode_notrims = 4,
	e_perout_mode_nosticks = 8,
	e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};


enum FunctionsActive {
	FUNCTION_TRAINER,
	FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
	FUNCTION_VARIO,
	FUNCTION_BACKLIGHT,
	#if defined(SDCARD)
	FUNCTION_LOGS,
	#endif
};

enum Analogs {
	STICK_RH,
	STICK_LV,
	STICK_RV,
	STICK_LH,
	POT1,
	POT2,
	POT3,
	POT_LAST = POT3,
	TX_VOLTAGE,
	NUMBER_ANALOG
};

enum TelemetryUnit {
	UNIT_VOLTS,
	UNIT_AMPS,
	UNIT_METERS_PER_SECOND,
	UNIT_RAW,
	UNIT_SPEED,
	UNIT_DIST,
	UNIT_TEMPERATURE,
	UNIT_PERCENT,
	UNIT_MILLIAMPS,
	UNIT_A1A2_MAX = UNIT_MILLIAMPS,
	UNIT_MAH,
	UNIT_WATTS,
	UNIT_DB,
	UNIT_FEET,
	UNIT_KTS,
	UNIT_HOURS,
	UNIT_MINUTES,
	UNIT_SECONDS,
	UNIT_RPMS,
	UNIT_G,
	UNIT_HDG,
};

#if defined(MULTIMODULE)

#define MULTI_RF_PROTO_LAST 100  // Use a large value

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

#endif

//Add more protocols if needed
enum Protocols {
	PROTOCOL_PPM,
	#ifdef DSM2_SERIAL
	PROTOCOL_DSM_SERIAL,
	#endif
	#ifdef MULTIMODULE
	PROTOCOL_MULTI,
	#endif
	PROTOCOL_COUNT
};

typedef const void* (*CMDS)(enum ProtoCmds);

typedef struct {
	enum Protocols Protocol;
	const pm_char* ProtoName;
	CMDS Cmds; // Cmds
} Proto_struct;

typedef struct {
	int32_t act:24;
	uint16_t delay:10;     // 10bits used 0 to DELAY_MAX*(100/DELAY_STEP)
	uint8_t activeMix:1;
	uint8_t activeExpo:1;
	int16_t hold:12;       // 12bits used -RESX to RESX
} MixVal;

struct t_inactivity{
	uint16_t counter;
	uint8_t  sum;
};

typedef struct {
	uint8_t activeFunctions;		// current max = 8 functions
	uint32_t  activeSwitches;		// current max = 32 function switches
	uint16_t lastFunctionTime[NUM_CFN];

	inline uint8_t isFunctionActive(uint8_t func)
	{
		return activeFunctions & ((uint8_t)1 << func);
	}

	void reset()
	{
		memclear(this, sizeof(*this));
	}
} CustomFunctionsContext;

#define BOOL1USED PIN0_bm
#define BOOL2USED PIN1_bm
#define BOOL3USED PIN2_bm

struct RfOptionSettingsvarstruct {
	uint8_t         rfProtoNeed;//:4;     // See usage in "PROTO_NEED_XX" Def
	uint8_t         rfSubTypeMax;//:4;       //16 max
	int8_t          rfOptionValue1Min;
	int8_t          rfOptionValue1Max;
	int8_t          rfOptionValue2Min;
	int8_t          rfOptionValue2Max;
	int8_t          rfOptionValue3Max;/*:5*/  //32 max -16 is min
} ;

struct RfOptionSettingsstruct {
	uint8_t         rfProtoNeed;//:4;     // See usage in "PROTO_NEED_XX" Def
	uint8_t         rfSubTypeMax;//:4;       //16 max
	const pm_char*  rfSubTypeNames;
	int8_t          rfOptionValue1Min;
	int8_t          rfOptionValue1Max;
	const pm_char*  rfOptionValue1Name;
	int8_t          rfOptionValue2Min;
	int8_t          rfOptionValue2Max;
	const pm_char*  rfOptionValue2Name;
	int8_t          rfOptionValue3Max;//:5;  //32 max -16 is min
	const pm_char*  rfOptionValue3Name;
	uint8_t         rfOptionBool1Used;//:1;
	const pm_char*  rfOptionBool1Name;
	uint8_t         rfOptionBool2Used;//:1;
	const pm_char*  rfOptionBool2Name;
	uint8_t         rfOptionBool3Used;//:1;
	const pm_char*  rfOptionBool3Name;
} ;

typedef struct {
	uint8_t protocol;
	const pm_char *subTypeString;
	uint8_t maxSubtype;
	const char *optionsstr;
} mm_protocol_definition;

// Reusable byte array to save having multiple buffers
#define SD_SCREEN_FILE_LENGTH (26)
union ReusableBuffer {
	#if defined(GUI)
	// 240 bytes
	struct {
		char listnames[LCD_LINES-1][LEN_MODEL_NAME];
		uint16_t eepromfree;

		#if defined(SDCARD)
		char menu_bss[POPUP_MENU_MAX_LINES][MENU_LINE_LENGTH];
		char mainname[25]; // used in logs : modelnamex-2013-01-01.log & Voice List read function
		#else
		char mainname[LEN_MODEL_NAME];
		#endif
	} modelsel;
	#endif

	#if defined(BLUETOOTH)
	struct {
		uint8_t firstMenuRun;
		uint8_t eeWriteFlag;
		char name_zchar[LEN_BT_NAME];      // FW format
		char name_str[LEN_BT_NAME+1];      // ASCII format
		char pin_zchar[4];                 // FW format
		char pin_str[5];                   // ASCII format
		//char peer_name_zchar[LEN_BT_NAME]; // FW format
		char peer_name_str[LEN_BT_NAME+1]; // ASCII format
		BtScannSt_t scann;
	} bluetooth;
	#endif

	// 43 bytes
	struct {
		int16_t midVals[NUM_STICKS+NUM_POTS];
		int16_t loVals[NUM_STICKS+NUM_POTS];
		int16_t hiVals[NUM_STICKS+NUM_POTS];
		uint8_t state;
	} calib;

	#if defined(SDCARD)
	struct {
		char lines[LCD_LINES-1][SD_SCREEN_FILE_LENGTH+1+1]; // the last char is used to store the flags (directory) of the line
		uint32_t available;
		uint16_t offset;
		uint16_t count;
		char originalName[SD_SCREEN_FILE_LENGTH+1];
	} sdmanager;

	// 256 bytes
	struct {
		char data[240]; // Sd log buffer
	} logsbuffer;

	#endif
};

#undef min // Avoid double declaration in wingw
#undef max
template<class t> FORCEINLINE t min(t a, t b)
{
	return a<b?a:b;
}
template<class t> FORCEINLINE t max(t a, t b)
{
	return a>b?a:b;
}
template<class t> FORCEINLINE t sgn(t a)
{
	return a>0 ? 1 : (a < 0 ? -1 : 0);
}
template<class t> FORCEINLINE t limit(t mi, t x, t ma)
{
	return min(max(mi,x),ma);
}
template<class t> void SWAP(t & a, t & b)
{
	t tmp = b;
	b = a;
	a = tmp;
}


extern uint16_t (*timer_callback)(void);

extern const RfOptionSettingsvarstruct RfOpt_PPM_Ser[];

#endif /* GLOBALDEFINITIONS_H_ */