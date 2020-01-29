/*
 * menu_model.h
 *
 * Created: 1/27/2020 7:57:43 PM
 *  Author: Andrew
 */ 


#ifndef MENU_MODEL_H_
#define MENU_MODEL_H_

#include "menus.h"
#include "../protocol/misc.h"
#include "widgets.h"

void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelXany(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuModelTelemetry(uint8_t event);
void menuModelTemplates(uint8_t event);
void menuModelExpoOne(uint8_t event);
uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay);
extern uint8_t s_curveChan;


// TODO elsewhere!
#define WCHART (LCD_H/2)
#define X0     (LCD_W-WCHART-2)
#define Y0     (LCD_H/2)
#define MIXES_2ND_COLUMN  (12*FW)

#define FlightModesType uint8_t

#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)


#define INCDEC_DECLARE_VARS(f)  uint8_t incdecFlag = (f)
#define INCDEC_SET_FLAG(f)      incdecFlag = (f)
#define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag)


extern uint8_t s_currIdx;
#define COPY_MODE 1
#define MOVE_MODE 2
extern uint8_t s_copyMode;
extern int8_t s_copySrcRow;
extern int8_t s_copyTgtOfs;

#define MODELSIZE_POS_X  170
#define MODELSEL_W       LCD_W


enum EnumTabModel {
	e_ModelSelect,
	e_ModelSetup,
	#ifdef HELI
	e_Heli,
	#endif
	#ifdef FLIGHT_MODES
	e_FlightModesAll,
	#endif
	e_InputsAll,
	e_MixAll,
	e_Limits,
	#ifdef CURVES
	e_CurvesAll,
	#endif
	e_LogicalSwitches,
	e_CustomFunctions,
	#ifdef FRSKY
	e_Telemetry,
	#endif
	#ifdef TEMPLATES
	e_Templates
	#endif
};

extern const MenuFuncP_PROGMEM menuTabModel[] PROGMEM;

#define MEMPROTO      shared_u8
#define PROTO_IS_SYNC (MEMPROTO == protocol)

enum menuModelSetupItems {
	ITEM_MODEL_NAME,
	ITEM_MODEL_TIMER1,
	CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER1_PERSISTENT)
	ITEM_MODEL_TIMER1_MINUTE_BEEP,
	ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
	ITEM_MODEL_TIMER2,
	CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER2_PERSISTENT)
	ITEM_MODEL_TIMER2_MINUTE_BEEP,
	ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
	ITEM_MODEL_EXTENDED_LIMITS,
	ITEM_MODEL_EXTENDED_TRIMS,
	ITEM_MODEL_TRIM_INC,
	ITEM_MODEL_THROTTLE_TRACE,
	ITEM_MODEL_THROTTLE_SWITCH,
	ITEM_MODEL_THROTTLE_TRIM,
	ITEM_MODEL_THROTTLE_WARNING,
	ITEM_MODEL_SWITCHES_WARNING,
	ITEM_MODEL_BEEP_CENTER,
	ITEM_MODEL_PROTOCOL,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_1,
	//Used with(MULTIMODULE) || (SPIMODULES)
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_2,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_3,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_4,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_5,
	//#endif
	//Used with(SPIMODULES)
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_6,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_7,
	ITEM_MODEL_PROTOCOL_PARAMS_LINE_8,
	ITEM_MODEL_SETUP_MAX

};

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-11*FW-MENUS_SCROLLBAR_WIDTH)
#define MODEL_SETUP_BIND_OFS          2*FW+1
#define MODEL_SETUP_RANGE_OFS         4*FW+3
#define MODEL_SETUP_SET_FAILSAFE_OFS  7*FW-2

enum menuModelHeliItems {
	ITEM_HELI_SWASHTYPE,
	ITEM_HELI_COLLECTIVE,
	ITEM_HELI_SWASHRING,
	ITEM_HELI_ELEDIRECTION,
	ITEM_HELI_AILDIRECTION,
	ITEM_HELI_COLDIRECTION
};

#define HELI_PARAM_OFS (14*FW)

enum LimitsItems {
	ITEM_LIMITS_OFFSET,
	ITEM_LIMITS_MIN,
	ITEM_LIMITS_MAX,
	ITEM_LIMITS_DIRECTION,
	#if defined(PPM_CENTER_ADJUSTABLE)
	ITEM_LIMITS_PPM_CENTER,
	#endif
	#if defined(PPM_LIMITS_SYMETRICAL)
	ITEM_LIMITS_SYMETRICAL,
	#endif
	ITEM_LIMITS_COUNT,
	ITEM_LIMITS_MAXROW = ITEM_LIMITS_COUNT-1
};

#if defined(PPM_UNIT_US)
#define LIMITS_MIN_POS          12*FW+1
#else
#define LIMITS_MIN_POS          12*FW
#endif
#define LIMITS_OFFSET_POS         8*FW
#if defined(PPM_LIMITS_SYMETRICAL)
#if defined(PPM_CENTER_ADJUSTABLE)
#define LIMITS_MAX_POS        15*FW
#define LIMITS_REVERT_POS     16*FW-3
#define LIMITS_PPM_CENTER_POS 20*FW+1
#else
#define LIMITS_DIRECTION_POS  12*FW+4
#define LIMITS_MAX_POS        16*FW+4
#define LIMITS_REVERT_POS     17*FW
#endif
#else
#if defined(PPM_CENTER_ADJUSTABLE)
#define LIMITS_MAX_POS        16*FW
#define LIMITS_REVERT_POS     17*FW-2
#define LIMITS_PPM_CENTER_POS 21*FW+2
#else
#define LIMITS_MAX_POS        17*FW
#define LIMITS_REVERT_POS     18*FW
#define LIMITS_DIRECTION_POS  12*FW+5
#endif
#endif

#define LIMITS_MIN_MAX_OFFSET 100
#define CONVERT_US_MIN_MAX(x) ((int16_t(x)*128)/25)
#define MIN_MAX_ATTR          attr

#if defined(PPM_UNIT_US)
#define SET_MIN_MAX(x, val)   x = ((val)*250)/128
#define MIN_MAX_DISPLAY(x)    CONVERT_US_MIN_MAX(x)
#else
#define MIN_MAX_DISPLAY(x)    ((int8_t)(x))
#endif

#ifdef TELEMETRY
enum menuModelTelemetryItems {
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_PROTO,
#endif
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  /*ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,*/
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  /*ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,*/
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  /*ITEM_TELEMETRY_RSSI_ALARM2,*/
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
  ITEM_TELEMETRY_FAS_OFFSET,
  CASE_VARIO(ITEM_TELEMETRY_VARIO_LABEL)
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_SOURCE,
#endif
  CASE_VARIO(ITEM_TELEMETRY_VARIO_RANGE)
  ITEM_TELEMETRY_SCREEN_LABEL1,
  ITEM_TELEMETRY_SCREEN_LINE1,
  ITEM_TELEMETRY_SCREEN_LINE2,
  ITEM_TELEMETRY_SCREEN_LINE3,
  ITEM_TELEMETRY_SCREEN_LINE4,
  ITEM_TELEMETRY_SCREEN_LABEL2,
  ITEM_TELEMETRY_SCREEN_LINE5,
  ITEM_TELEMETRY_SCREEN_LINE6,
  ITEM_TELEMETRY_SCREEN_LINE7,
  ITEM_TELEMETRY_SCREEN_LINE8,
  ITEM_TELEMETRY_MAX
};

#define TELEM_COL1                   INDENT_WIDTH
#define TELEM_COL2                   (8*FW)
#define TELEM_BARS_COLMIN            (56-3*FW)
#define TELEM_BARS_COLMAX            (14*FW-3)
#define TELEM_SCRTYPE_COL            TELEM_COL2

#define IS_RANGE_DEFINED(k)          (g_model.telemetry.channels[k].ratio > 0)

#define CHANNEL_ROWS(x)              LABEL(CHANNEL), 1, 0
#define CHANNELS_ROWS                CHANNEL_ROWS(0), CHANNEL_ROWS(1),
#define RSSI_ROWS                    0, 1,
#define SENSORS_ROWS

#define IF_FAS_OFFSET(x)             x,

#if defined(FRSKY)
#define USRPROTO                     1,
#define USRDATA_ROWS                 LABEL(UsrData), 0, 0, 0, IF_FAS_OFFSET(0)
#else
#define USRPROTO
#define USRDATA_ROWS                 0, 0, IF_FAS_OFFSET(0)
#endif

#define SCREEN_TYPE_ROWS             0

#define VARIO_RANGE_ROWS             3

#define TELEMETRY_SCREEN_ROWS(x)     SCREEN_TYPE_ROWS, 2, 2, 2, 2
#define TELEMETRY_CURRENT_CHANNEL(k) (k >= ITEM_TELEMETRY_A2_LABEL ? TELEM_ANA_A2 : TELEM_ANA_A1)


#endif


#if defined(GVARS)
#define CURVE_SELECTED(pos) (pos >= 0 && pos < MAX_CURVES)
#define GVAR_SELECTED(pos)  (pos >= MAX_CURVES)
#else
#define CURVE_SELECTED(pos) (pos >= 0)
#endif

struct point_t {
	coord_t x;
	coord_t y;
};


enum LogicalSwitchFields {
	LS_FIELD_FUNCTION,
	LS_FIELD_V1,
	LS_FIELD_V2,
	LS_FIELD_ANDSW,
	LS_FIELD_COUNT,
	LS_FIELD_LAST = LS_FIELD_COUNT-1
};

#define CSW_1ST_COLUMN  (4*FW-3)
#define CSW_2ND_COLUMN  (8*FW-3)
#define CSW_3RD_COLUMN  (13*FW-6)
#define CSW_4TH_COLUMN  (18*FW+2)

#if defined(SDCARD) && defined(VOICE)
#define PROMPTTEXT  reusableBuffer.modelsel.mainname
#define PROMPTMEM   shared_u8
#endif

#define MODEL_CUSTOM_FUNC_1ST_COLUMN          (3)
#define MODEL_CUSTOM_FUNC_2ND_COLUMN          (5*FW-2)
#define MODEL_CUSTOM_FUNC_3RD_COLUMN          (15*FW+2)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN          (20*FW)
#if defined(GRAPHICS)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (20*FW)
#else
#define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (18*FW+2)
#endif

enum menuModelPhaseItems {
	ITEM_MODEL_PHASE_NAME,
	ITEM_MODEL_PHASE_SWITCH,
	ITEM_MODEL_PHASE_TRIMS,
	IF_ROTARY_ENCODERS(ITEM_MODEL_PHASE_ROTARY_ENCODERS)
	ITEM_MODEL_PHASE_FADE_IN,
	ITEM_MODEL_PHASE_FADE_OUT,
	#if defined(GVARS)
	ITEM_MODEL_PHASE_GVARS_LABEL,
	ITEM_MODEL_PHASE_GV1,
	ITEM_MODEL_PHASE_GV2,
	ITEM_MODEL_PHASE_GV3,
	ITEM_MODEL_PHASE_GV4,
	ITEM_MODEL_PHASE_GV5,
	ITEM_MODEL_PHASE_GV6,
	#if defined(EXTERNALEEPROM)
	ITEM_MODEL_PHASE_GV7,
	ITEM_MODEL_PHASE_GV8,
	ITEM_MODEL_PHASE_GV9,
	ITEM_MODEL_PHASE_GV10,
	ITEM_MODEL_PHASE_GV11,
	ITEM_MODEL_PHASE_GV12,
	#endif
	#endif
	ITEM_MODEL_PHASE_MAX
};

#if defined(ROTARY_ENCODERS)
#if ROTARY_ENCODERS > 2
#define NAME_OFS (-4-12)
#define SWITCH_OFS (-FW/2-2-13)
#define TRIMS_OFS  (-FW/2-4-15)
#define ROTARY_ENC_OFS (0)
#else
#define NAME_OFS (-4)
#define SWITCH_OFS (-FW/2-2)
#define TRIMS_OFS  (-FW/2-4)
#define ROTARY_ENC_OFS (2)
#endif
#else
#define NAME_OFS 0
#define SWITCH_OFS (FW/2)
#define TRIMS_OFS  (FW/2)
#endif

//mixes

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_ONE_2ND_COLUMN (7*FW+3*FW+2)
#define EXPO_ONE_FM_WIDTH   (5*FW)

#define EXPO_LINE_WEIGHT_POS	7*FW+1
#define EXPO_LINE_EXPO_POS		11*FW
#define EXPO_LINE_SWITCH_POS	11*FW+4
#if MAX_FLIGHT_MODES == 6
#define EXPO_LINE_SIDE_POS		15*FW
#else
#define EXPO_LINE_SIDE_POS		15*FW+2
#endif
#define EXPO_LINE_FM_POS		LCD_W-FW-MENUS_SCROLLBAR_WIDTH
#define EXPO_LINE_SELECT_POS	24
#define MIX_LINE_SRC_POS		4*FW-1
#define MIX_LINE_WEIGHT_POS		11*FW+3
#define MIX_LINE_CURVE_POS		12*FW+2
#define MIX_LINE_SWITCH_POS		16*FW
#define MIX_LINE_DELAY_POS		19*FW+7



#define CURVE_ROWS 0

#ifdef CURVES
#define CASE_CURVES(x) x,
#else
#define CASE_CURVES(x)
#endif

#ifdef FLIGHT_MODES
#define CASE_FLIGHT_MODES(x) x,
#else
#define CASE_FLIGHT_MODES(x)
#endif
	
enum ExposFields {
	EXPO_FIELD_WEIGHT,
	EXPO_FIELD_EXPO,
	#ifdef CURVES
	EXPO_FIELD_CURVE,
	#endif
	#ifdef FLIGHT_MODES
	EXPO_FIELD_FLIGHT_MODES,
	#endif
	EXPO_FIELD_SWITCH,
	EXPO_FIELD_SIDE,
	EXPO_FIELD_MAX
};

enum MixFields {
	MIX_FIELD_SOURCE,
	MIX_FIELD_WEIGHT,
	MIX_FIELD_OFFSET,
	MIX_FIELD_TRIM,
	CASE_CURVES(MIX_FIELD_CURVE)
	CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_PHASE)
	MIX_FIELD_SWITCH,
	MIX_FIELD_WARNING,
	MIX_FIELD_MLTPX,
	MIX_FIELD_DELAY_UP,
	MIX_FIELD_DELAY_DOWN,
	MIX_FIELD_SLOW_UP,
	MIX_FIELD_SLOW_DOWN,
	MIX_FIELD_COUNT
};

#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
#endif /* MENU_MODEL_H_ */