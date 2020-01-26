/*
 * ObsoleteTX.h
 *
 * Created: 1/24/2020 9:23:59 PM
 *  Author: Andrew
 */ 


#ifndef OBSOLETETX_H_
#define OBSOLETETX_H_

#include "boards/board.h"
#include "_Config.h"
#include "global_defs.h"


// CONFIG OPTIONS
#ifdef DX6I
#include "boards/dx6i/dx6i.h"
#endif // DX6I

/////////////////////////
// checkIncDec flags
#define EE_NO           0x00
#define EE_GENERAL      0x01
#define EE_MODEL        0x02
#define NO_INCDEC_MARKS 0x04
#define INCDEC_SWITCH   0x08
#define INCDEC_SOURCE   0x10
#define INCDEC_REP10    0x40
#define NO_DBLKEYS      0x80

#endif

#include <inttypes.h>
//#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

  #include <avr/pgmspace.h>
  #include <avr/cpufunc.h>
  #include "pgmtypes.h"
  #define ASSERT(x)
  #include <avr/io.h>
  #include <avr/eeprom.h>
  #include <avr/sleep.h>
  #include <avr/interrupt.h>
  #include <util/delay.h>
  #include <util/atomic.h>
  #include <avr/wdt.h>

  //#define FORCEINLINE inline __attribute__ ((always_inline))
  //#define NOINLINE __attribute__ ((noinline))
  #define TRACE(...)
  #define SIMU_SLEEP(x)
  #define SIMU_PROCESSEVENTS
  #define MYWDT_RESET(x) wdt_reset()
  #define SIMUBEEP1()
  #define SIMUBEEP2()
  #define SIMU_UNLOCK_MACRO_FALSE(x) (x)
  #define SIMU_UNLOCK_MACRO_TRUE(x) (x)
  #define CONVERT_PTR_UINT(x) ((uint32_t)(x))
  #define CONVERT_UINT_PTR(x) ((uint32_t *)(x))
  #define convertSimuPath(x) (x)
  // Fiddle to force compiler to use a pointer
  #define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
  

/////////////////DEBUG FUNCTION DEFINITION///////////////////

//#define SHOWDURATIONLCD 1  //Show LCD fast refresh duration
#if defined(SHOWDURATIONLCD)
#define SHOWDURATIONLCD1 SHOWDURATION1
#define SHOWDURATIONLCD2 SHOWDURATION2
#define SHOWDURATION 1
#else
#define SHOWDURATIONLCD1
#define SHOWDURATIONLCD2
#endif // defined LCD

//#define SHOWDURATION 1  //Show a duration

#if defined(SHOWDURATION)
static uint16_t DurationValue;
#define SHOWDURATION1                                                        \
uint16_t t0 = getTmr64uS();                                               \

#define SHOWDURATION2                                                        \
t0 = getTmr64uS() - t0;                                                   \
if (t0 > DurationValue) DurationValue = t0;                                \

#else
#define SHOWDURATION1
#define SHOWDURATION2
#endif

#define CASE_PERSISTENT_TIMERS(x) x,

#if defined(RTCLOCK)
#define CASE_RTCLOCK(x) x,
#else
#define CASE_RTCLOCK(x)
#endif

#if defined(BUZZER)
#define CASE_BUZZER(x) x,
#else
#define CASE_BUZZER(x)
#endif

#if defined(AUDIO)
#define CASE_AUDIO(x) x,
#else
#define CASE_AUDIO(x)
#endif

#if defined(VOICE)
#define CASE_VOICE(x) x,
#else
#define CASE_VOICE(x)
#endif

#if defined(PWM_BACKLIGHT)
#define CASE_PWM_BACKLIGHT(x) x,
#else
#define CASE_PWM_BACKLIGHT(x)
#endif

#if defined(FRSKY) && defined(GPS)
#define CASE_GPS(x) x,
#else
#define CASE_GPS(x)
#endif

#if defined(VARIO)
#define CASE_VARIO(x) x,
#else
#define CASE_VARIO(x)
#endif

#if defined(HAPTIC)
#define CASE_HAPTIC(x) x,
#else
#define CASE_HAPTIC(x)
#endif

#if defined(SPLASH)
#define CASE_SPLASH(x) x,
#else
#define CASE_SPLASH(x)
#endif

#if defined(FRSKY)
#define CASE_FRSKY(x) x,
#else
#define CASE_FRSKY(x)
#endif

#if defined(SDCARD)
#define CASE_SDCARD(x) x,
#else
#define CASE_SDCARD(x)
#define logDelay 0
#endif

#if defined(BLUETOOTH)
#define CASE_BLUETOOTH(x) x,
#include "bluetooth.h"
#else
#define CASE_BLUETOOTH(x)
#endif

#if defined(HELI)
#define CASE_HELI(x) x,
#else
#define CASE_HELI(x)
#endif

#if defined(TEMPLATES)
#define CASE_TEMPLATES(x) x,
#else
#define CASE_TEMPLATES(x)
#endif

#if defined(FLIGHT_MODES)
#define CASE_FLIGHT_MODES(x) x,
#else
#define CASE_FLIGHT_MODES(x)
#endif

#if defined(CURVES)
#define CASE_CURVES(x) x,
#else
#define CASE_CURVES(x)
#endif

#if defined(GVARS)
#define CASE_GVARS(x) x,
#else
#define CASE_GVARS(x)
#endif

#if defined(X_ANY)
#define CASE_X_ANY(x) x,
#else
#define CASE_X_ANY(x)
#endif

#if ROTARY_ENCODERS > 0
#define ROTARY_ENCODER_NAVIGATION
#endif

#if defined(FAI)
#define IS_FAI_ENABLED() true
#define IF_FAI_CHOICE(x)
#elif defined(FAI_CHOICE)
#define IS_FAI_ENABLED() g_eeGeneral.fai
#define IF_FAI_CHOICE(x) x,
#else
#define IS_FAI_ENABLED() false
#define IF_FAI_CHOICE(x)
#endif

#define IS_FAI_FORBIDDEN(idx) (IS_FAI_ENABLED() && idx >= MIXSRC_FIRST_TELEM)

// Build Protocol Mask used by simu to decrypt firmware used protocol
#ifdef DSM2_SERIAL
#define PROTOMASK0 _BV(0)
#else
#define PROTOMASK0 0
#endif
#ifdef PROTO_HAS_MULTISUPIIIK
#define PROTOMASK1 _BV(1)
#else
#define PROTOMASK1 0
#endif
#ifdef PROTO_HAS_CC2500
#define PROTOMASK2 _BV(2)
#else
#define PROTOMASK2 0
#endif
#ifdef PROTO_HAS_CYRF6936
#define PROTOMASK3 _BV(3)
#else
#define PROTOMASK3 0
#endif
#ifdef PROTO_HAS_A7105
#define PROTOMASK4 _BV(4)
#else
#define PROTOMASK4 0
#endif
#ifdef PROTO_HAS_NRF24L01
#define PROTOMASK5 _BV(5)
#else
#define PROTOMASK5 0
#endif
#define PROTOMASK PROTOMASK0|PROTOMASK1|PROTOMASK2|PROTOMASK3|PROTOMASK4|PROTOMASK5


// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l

#include "myeeprom.h"
#ifdef GUI
#include "gui/gui.h"
#endif

#define NUM_SWITCHES     7
#define IS_3POS(sw)      ((sw) == 0)
#define IS_MOMENTARY(sw) (sw == SWSRC_TRN)

#define NUM_PSWITCH      (SWSRC_LAST_SWITCH-SWSRC_FIRST_SWITCH+1)

#define KEY_ENTER        KEY_MENU
#define KEY_PLUS         KEY_RIGHT
#define KEY_MINUS        KEY_LEFT


//#define memclear(p, s) memset(p, 0, s)

#define IS_POT_AVAILABLE(x)       (true)
#define IS_POT_MULTIPOS(x)        (false)
#define IS_POT_WITHOUT_DETENT(x)  (true)


#define IS_POT(x)                   ((x)>=POT1 && (x)<=POT_LAST)

#define GET_LOWRES_POT_POSITION(i)  (getValue(MIXSRC_FIRST_POT+(i)) >> 4)
#define SAVE_POT_POSITION(i)        g_model.potsWarnPosition[i] = GET_LOWRES_POT_POSITION(i)


#if ROTARY_ENCODERS > 0
#define IF_ROTARY_ENCODERS(x) x,
#define ROTENCDEBOUNCEVAL _BV(2)
extern uint8_t rotEncADebounce;
#if ROTARY_ENCODERS > 1
extern uint8_t rotEncBDebounce;
#endif
#else
#define IF_ROTARY_ENCODERS(x)
#endif

#define PPM_CENTER 1500

#if defined(PPM_CENTER_ADJUSTABLE)
#define PPM_CH_CENTER(ch) (PPM_CENTER+limitAddress(ch)->ppmCenter)
#define FULL_CHANNEL_OUTPUTS(ch) (channelOutputs[ch] + ((limitAddress(ch)->ppmCenter)>>2))
#else
#define PPM_CH_CENTER(ch) (PPM_CENTER)
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]
#endif

#include "misclib.h"

//Return the current 10 ms tick counter
#define GET_10MS_TICK()					getTmr10ms() 
//Returns the amount of elapsed 10msTick since a starting point expressed in 10msTick
#define ELAPSED_10MS_TICK_SINCE(Start10msTick)    ((uint16_t)(getTmr10ms() - (Start10msTick)))
//Returns the amount of 10msTick corresponding to a duration expressed in ms
#define MS_TO_10MS_TICK(Ms)                       ((tmr10ms_t)(((Ms) + 9) / 10))
//Returns the amount of ms corresponding to a duration expressed in 10msTick
#define _10MS_TICK_TO_MS(_10MsTick)               ((_10MsTick) * 10)
//Returns the amount of elapsed 64uSTick since a starting point expressed in 64uSTick
#define ELAPSED_64uS_TICK_SINCE(Start64uSTick)  ((uint16_t)(getTmr64uS() - (Start64uSTick)))
//Returns the amount of 64uSTick corresponding to a duration expressed in us
#define US_TO_64US_TICK(Us) ((Us)>>6) /* counter is 16MHz/1024=156251 not real 16Khz tick = 64us */

//Yield CPU to the specified task(s) for the specified time (in ms)
#define YIELD_TO_TASK_FOR_MS(TaskList, StartDurationMsVar, DurationMs) StartDurationMsVar = GET_10MS_TICK(); do{do{MYWDT_RESET();TaskList;}while((GET_10MS_TICK() - StartDurationMsVar) < MS_TO_10MS_TICK(DurationMs));}while(0)
//Yield CPU to the specified task(s) (one shot)
#define YIELD_TO_TASK(TaskList) do{ MYWDT_RESET(); TaskList; }while(0)


#define T1900_OFFSET 1900
extern time_t g_rtcTime;
extern uint8_t g_ms100; // global to allow time set function to reset to zero
void rtcInit();
void rtcSetTime(struct tm * t);
uint8_t rtcReadTemp(int16_t * temp);

extern volatile uint8_t g_tmr128uS;
extern volatile uint16_t g_tmr10ms;
extern uint16_t Bind_tmr10ms;

extern uint16_t getTmr10ms();
extern uint16_t getTmr64uS();
FORCEINLINE uint16_t getTmr128uS();

#ifdef GUI
#define POPUP(x) displayPopup(x);
#else
#define POPUP(...)
#define DISPLAY_WARNING(...)
#define POPUP_WARNING(...)
#define POPUP_CONFIRMATION(...)
#define POPUP_INPUT(...)
#define WARNING_INFO_FLAGS           0
#define SET_WARNING_INFO(...)




void checkMixer();

typedef int8_t rotenc_t;
typedef int16_t getvalue_t;
typedef uint8_t mixsrc_t;
typedef int8_t swsrc_t;


#if defined(NAVIGATION_STICKS)
extern uint8_t getSticksNavigationEvent();
extern uint8_t StickScrollAllowed;
extern uint8_t StickScrollTimer;
#define STICK_SCROLL_TIMEOUT          9
#define STICK_SCROLL_DISABLE()        StickScrollAllowed = 0
#else
#define STICK_SCROLL_DISABLE()
#endif

#include "telemetry_driver.h"
#include "pulses/pulses_avr.h"
#include "pulses/pulses.h"
#include "eeprom_common.h"
#include "eeprom_rlc.h"


//#define MASK_CFN_TYPE  uint32_t  // current max = 32 function switches
//#define MASK_FUNC_TYPE uint8_t   // current max = 8 functions
//
//typedef struct {
	//MASK_FUNC_TYPE activeFunctions;
	//MASK_CFN_TYPE  activeSwitches;
	//uint16_t lastFunctionTime[NUM_CFN];
//
	//inline uint8_t isFunctionActive(uint8_t func)
	//{
		//return activeFunctions & ((MASK_FUNC_TYPE)1 << func);
	//}
//
	//void reset()
	//{
		//memclear(this, sizeof(*this));
	//}
//} CustomFunctionsContext;

//  Dimension of Arrays
#define DIM(a) ((sizeof a) / (sizeof *a))

#if defined(TEMPLATES)
#include "templates.h"
#endif


extern const pm_uint8_t bchout_ar[];
extern const pm_uint8_t modn12x3[];

extern uint8_t stickMode;

//convert from mode 1 to mode stickMode
//NOTICE!  =>  0..3 -> 0..3
#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3
#define CONVERT_MODE(x)  (((x)<=AIL_STICK) ? pgm_read_byte_near(modn12x3 + 4*stickMode + (x)) : (x) )

extern uint8_t channel_order(uint8_t x);




#define THRCHK_DEADBAND               16

#define SPLASH_NEEDED()               (!g_eeGeneral.splashMode)

#if defined(FSPLASH)
#define SPLASH_INFINITE_MS          (10*60*1000UL)  // 10 minutes = infinite
#define SPLASH_DEFAULT_TIMEOUT_MS   (4000)  // 4 seconds
#define SPLASH_TIMEOUT_MS           (g_eeGeneral.splashMode == 0 ? MS_TO_10MS_TICK(SPLASH_INFINITE_MS) : (MS_TO_10MS_TICK(SPLASH_DEFAULT_TIMEOUT_MS) * (g_eeGeneral.splashMode & 0x03)))
#else
#define SPLASH_TIMEOUT_MS           (4000)  // 4 seconds
#endif

#if defined(ROTARY_ENCODERS)
#define NAVIGATION_RE_IDX()         (g_eeGeneral.reNavigation - 1)
#define IS_RE_NAVIGATION_ENABLE()   g_eeGeneral.reNavigation
#elif defined(ROTARY_ENCODER_NAVIGATION)
#define IS_RE_NAVIGATION_ENABLE()   true
#define NAVIGATION_RE_IDX()         0
#endif

#define HEART_TIMER_10MS              1
#define HEART_TIMER_PULSES            2 // when multiple modules this is the first one
#define HEART_WDT_CHECK               (HEART_TIMER_10MS + HEART_TIMER_PULSES)


extern uint8_t heartbeat;


#define MAX_ALERT_TIME   60

//typedef struct {
	//uint16_t counter;
	//uint8_t  sum;
//} t_inactivity;

extern t_inactivity inactivity;

#define LEN_STD_CHARS 40

#if defined(TRANSLATIONS_CZ)
#define ZCHAR_MAX (LEN_STD_CHARS)
#else
#define ZCHAR_MAX (LEN_STD_CHARS + LEN_SPECIAL_CHARS)
#endif

char hex2zchar(uint8_t hex);
char idx2char(int8_t idx);
void str2zchar(char *dest, const char *src, uint8_t size); // ASCII to FW
uint8_t zchar2str(char *dest, const char *src, uint8_t size); // FW to ASCII

//TODO: add later
#include "keys.h"

uint8_t switchState(EnumKeys enuk);
uint8_t trimDown(uint8_t idx);

uint16_t evalChkSum();

#if !defined(GUI)
#define MESSAGE_SOUND_ARG
#define MESSAGE(...)
#define ALERT(...)
#elif defined(VOICE)
#define MESSAGE_SOUND_ARG , uint8_t sound
#define MESSAGE(title, msg, info, sound) message(title, msg, info, sound)
#define ALERT(title, msg, sound) alert(title, msg, sound)
#else
#define MESSAGE_SOUND_ARG
#define MESSAGE(title, msg, info, sound) message(title, msg, info)
#define ALERT(title, msg, sound) alert(title, msg)
#endif

extern void message(const pm_char *title, const pm_char *s, const char *last MESSAGE_SOUND_ARG);
extern void alert(const pm_char * t, const pm_char * s MESSAGE_SOUND_ARG);

//enum PerOutMode {
	//e_perout_mode_normal = 0,
	//e_perout_mode_inactive_flight_mode = 1,
	//e_perout_mode_notrainer = 2,
	//e_perout_mode_notrims = 4,
	//e_perout_mode_nosticks = 8,
	//e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
//};

extern uint8_t mixerCurrentFlightMode;
extern uint8_t lastFlightMode;
extern uint8_t flightModeTransitionLast;

#define bitfield_channels_t uint16_t

void evalFlightModeMixes(uint8_t mode, uint8_t tick10ms);
void evalMixes(uint8_t tick10ms);
void doMixerCalculations();

void perMain();
NOINLINE void per10ms();

getvalue_t getValue(mixsrc_t i);

uint8_t getSwitch(swsrc_t swtch);
uint8_t getLogicalSwitch(uint8_t idx);

void logicalSwitchesTimerTick();
void logicalSwitchesReset();

extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used;
extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value;
#define LS_RECURSIVE_EVALUATION_RESET() s_last_switch_used = 0

extern swarnstate_t switches_states;
swsrc_t getMovedSwitch();

int8_t getMovedSource();
#define GET_MOVED_SOURCE(min, max) getMovedSource()

#if defined(FLIGHT_MODES)
extern uint8_t getFlightMode();
#else
#define getFlightMode() 0
#endif

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);

#if defined(GVARS)
extern int8_t trimGvar[NUM_STICKS];
#define TRIM_REUSED(idx) trimGvar[idx] >= 0
#else
#define TRIM_REUSED(idx) 0
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx);
int16_t getTrimValue(uint8_t phase, uint8_t idx);

void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim);

#if defined(ROTARY_ENCODERS)
int16_t getRotaryEncoder(uint8_t idx);
void incRotaryEncoder(uint8_t idx, int8_t inc);
#endif

#define ROTARY_ENCODER_GRANULARITY (1)

#if defined(GVARS)
uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx);
int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase);
void setGVarValue(uint8_t x, int16_t value, int8_t phase);
#define GET_GVAR(x, min, max, p) getGVarValue(x, min, max, p)
#define SET_GVAR(idx, val, p) setGVarValue(idx, val, p)
#else
#define GET_GVAR(x, ...) (x)
#endif

// highest bit used for small values in mix 128 --> 8 bit is enough
#define GV1_SMALL  128
// highest bit used for large values in mix 256 --> 9 bits is used (8 bits + 1 extra bit from weightMode/offsetMode)
#define GV1_LARGE  256

#define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE )
#define GV_INDEX_CALCULATION(x,max)  ( (max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE) )
#define GV_IS_GV_VALUE(x,min,max)    ( (x>max) || (x<min) )

#define GV_INDEX_CALC_DELTA(x,delta) ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL      (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG  (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE      (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG  (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))
// for stock we just use as much as possible
#define GV_RANGELARGE_WEIGHT      GV_RANGELARGE
#define GV_RANGELARGE_WEIGHT_NEG  GV_RANGELARGE_NEG
#define GV_RANGELARGE_OFFSET      GV_RANGELARGE
#define GV_RANGELARGE_OFFSET_NEG  GV_RANGELARGE_NEG

extern uint16_t sessionTimer;
extern uint16_t s_timeCumThr;
extern uint16_t s_timeCum16ThrP;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
typedef int8_t safetych_t;
#define OVERRIDE_CHANNEL_UNDEFINED -128
extern safetych_t safetyCh[NUM_CHNOUT];
#endif

extern uint8_t trimsCheckTimer;


void flightReset();

void setGazSource();
extern uint8_t gazSource;
extern bool enableGaz;
extern bool pwrCheck;
extern bool unexpectedShutdown;

extern uint16_t maxMixerDuration;

extern uint16_t g_tmr1Latency_max;
extern uint16_t g_tmr1Latency_min;
extern uint16_t g_guibuild_max;
extern uint16_t g_guibuild_min;
extern uint16_t g_lcddraw_max;
extern uint16_t g_lcddraw_min;
extern uint16_t lastMixerDuration;

#define DURATION_MS_PREC2(x) (((x)*100)>>4)

#if defined(THRTRACE)
#define MAXTRACE (LCD_W - 8)
extern uint8_t  s_traceBuf[MAXTRACE];
#if LCD_W >= 255
extern int16_t  s_traceWr;
extern int16_t  s_traceCnt;
#else
extern uint8_t  s_traceWr;
extern int16_t  s_traceCnt;
#endif
extern uint8_t  s_cnt_10s;
extern uint16_t s_cnt_samples_thr_10s;
extern uint16_t s_sum_samples_thr_10s;
#define RESET_THR_TRACE() s_traceCnt = s_traceWr = s_cnt_10s = s_cnt_samples_thr_10s = s_sum_samples_thr_10s = s_timeCum16ThrP = s_timeCumThr = 0
#else
#define RESET_THR_TRACE() s_timeCum16ThrP = s_timeCumThr = 0
#endif

uint16_t stackAvailable();

#if defined(SPLASH)
void doSplash();
#endif

#if MENUS_LOCK == 1
extern bool readonly;
extern bool readonlyUnlocked();
#define READ_ONLY() readonly
#define READ_ONLY_UNLOCKED() readonlyUnlocked()
#else
#define READ_ONLY() false
#define READ_ONLY_UNLOCKED() true
#endif

void checkLowEEPROM();
void checkTHR();
void checkSwitches();
void checkAlarm();
void checkAll();

//read analog inputs
void getADC();

extern void backlightOn();

//enum Analogs {
	//STICK_RH,
	//STICK_LV,
	//STICK_RV,
	//STICK_LH,
	//POT1,
	//POT2,
	//POT3,
	//POT_LAST = POT3,
	//TX_VOLTAGE,
	//NUMBER_ANALOG
//};

extern uint16_t s_anaFilt[NUMBER_ANALOG];

void checkBacklight();


//#undef min // Avoid double declaration in wingw
//#undef max
//template<class t> FORCEINLINE t min(t a, t b)
//{
	//return a<b?a:b;
//}
//template<class t> FORCEINLINE t max(t a, t b)
//{
	//return a>b?a:b;
//}
//template<class t> FORCEINLINE t sgn(t a)
//{
	//return a>0 ? 1 : (a < 0 ? -1 : 0);
//}
//template<class t> FORCEINLINE t limit(t mi, t x, t ma)
//{
	//return min(max(mi,x),ma);
//}
//template<class t> void SWAP(t & a, t & b)
//{
	//t tmp = b;
	//b = a;
	//a = tmp;
//}

#define IS_IN_RANGE(value,lowest,highest) (!(value<(lowest)) && (value<((highest)+1)))

uint16_t isqrt32(uint32_t n);

void generalDefault();
void modelDefault(uint8_t id);


extern int16_t calc100to256_16Bits(int16_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100to256(int8_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100toRESX_16Bits(int16_t x); // @@@ open.20.fsguruh
extern int16_t calc100toRESX(int8_t x);
extern int16_t calc1000toRESX(int16_t x);
extern int16_t calcRESXto1000(int16_t x);
extern int8_t  calcRESXto100(int16_t x);

extern const char vers_stamp[];

extern uint16_t           g_vbat10mV;
#define GET_TXBATT_BARS() (limit<uint8_t>(2, 20 * ((uint8_t)(g_vbat10mV/10) - g_eeGeneral.vBatMin) / (g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 20))
#define IS_TXBATT_WARNING() (g_vbat10mV < (g_eeGeneral.vBatWarn*10))


#define g_blinkTmr10ms    (*(uint8_t*)&g_tmr10ms)
extern uint8_t            g_beepCnt;
extern uint8_t            g_beepVal[5];

//uncomment later
//#include "trainer_input.h"

extern int32_t            chans[NUM_CHNOUT];
extern int16_t            ex_chans[NUM_CHNOUT]; // Outputs (before LIMITS) of the last perMain
extern int16_t            channelOutputs[NUM_CHNOUT];

#define NUM_INPUTS      (NUM_STICKS)

int16_t intpol(int16_t x, uint8_t idx);
int16_t expo(int16_t x, int16_t k);

#if defined(CURVES)
int16_t applyCurve(int16_t x, int8_t idx);
#else
#define applyCurve(x, idx) (x)
#endif


#define applyCustomCurve(x, idx) intpol(x, idx)

#define APPLY_EXPOS_EXTRA_PARAMS_INC
#define APPLY_EXPOS_EXTRA_PARAMS


void applyExpos(int16_t *anas, uint8_t mode APPLY_EXPOS_EXTRA_PARAMS_INC);
int16_t applyLimits(uint8_t channel, int32_t value);

void evalInputs(uint8_t mode);
uint16_t anaIn(uint8_t chan);
extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];

#define FLASH_DURATION 20 /*200ms*/

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

FlightModeData *flightModeAddress(uint8_t idx);
ExpoData *expoAddress(uint8_t idx);
MixData *mixAddress(uint8_t idx);
LimitData *limitAddress(uint8_t idx);
int8_t *curveAddress(uint8_t idx);
LogicalSwitchData *lswAddress(uint8_t idx);

// static variables used in evalFlightModeMixes - moved here so they don't interfere with the stack
// It's also easier to initialize them here.
extern int16_t rawAnas[NUM_INPUTS];

extern int16_t  anas[NUM_INPUTS];
extern int16_t  trims[NUM_STICKS];
extern BeepANACenter bpanaCenter;

extern bool s_mixer_first_run_done;

extern int8_t s_currCh;
uint8_t getExpoMixCount(uint8_t expo);
void deleteExpoMix(uint8_t expo, uint8_t idx);
void insertExpoMix(uint8_t expo, uint8_t idx);
void applyDefaultTemplate();

void incSubtrim(uint8_t idx, int16_t inc);
void instantTrim();
FORCEINLINE void evalTrims();
void copyTrimsToOffset(uint8_t ch);
void copySticksToOffset(uint8_t ch);
void moveTrimsToOffsets();

//typedef struct {
	//int32_t act:24;
	//uint16_t delay:10;     // 10bits used 0 to DELAY_MAX*(100/DELAY_STEP)
	//uint8_t activeMix:1;
	//uint8_t activeExpo:1;
	//int16_t hold:12;       // 12bits used -RESX to RESX
//} MixVal;

extern MixVal mixVal[MAX_MIXERS];

#ifdef BOLD_FONT
inline uint8_t isExpoActive(uint8_t expo)
{
	return mixVal[expo].activeExpo;
}

inline uint8_t isMixActive(uint8_t mix)
{
	return mixVal[mix].activeMix;
}
#else
#define isExpoActive(x) false
#define isMixActive(x) false
#endif

//enum CswFunctionFamilies {
	//LS_FAMILY_OFS,
	//LS_FAMILY_BOOL,
	//LS_FAMILY_COMP,
	//LS_FAMILY_DIFF,
	//LS_FAMILY_TIMER,
	//LS_FAMILY_STICKY,
	//LS_FAMILY_RANGE,
	//LS_FAMILY_EDGE
//};

uint8_t lswFamily(uint8_t func);
int16_t lswTimerValue(int8_t val);

//enum FunctionsActive {
	//FUNCTION_TRAINER,
	//FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
	//FUNCTION_VARIO,
	//FUNCTION_BACKLIGHT,
	//#if defined(SDCARD)
	//FUNCTION_LOGS,
	//#endif
//};

#define VARIO_FREQUENCY_ZERO   700/*Hz*/
#define VARIO_FREQUENCY_RANGE  1000/*Hz*/
#define VARIO_REPEAT_ZERO      500/*ms*/
#define VARIO_REPEAT_MAX       80/*ms*/

extern CustomFunctionsContext modelFunctionsContext;

//#define isFunctionActive(func) modelFunctionsContext.isFunctionActive(func)
void evalFunctions();
//#define customFunctionsReset() modelFunctionsContext.reset()

#if defined(ROTARY_ENCODERS)
// Global rotary encoder registers
extern volatile rotenc_t g_rotenc[ROTARY_ENCODERS];
#elif defined(ROTARY_ENCODER_NAVIGATION)
extern volatile rotenc_t g_rotenc[1];
#endif

extern void parseTelemFrskyByte(uint8_t data);
#if defined (FRSKY)
// FrSky Telemetry
#include "telemetry/frsky.h"
#endif


#define PLAY_REPEAT(x)            (x)                 /* Range 0 to 15 */
#define PLAY_NOW                  0x10
#define PLAY_BACKGROUND           0x20
#define PLAY_INCREMENT(x)         ((uint8_t)(((uint8_t)x) << 6))   /* -1, 0, 1, 2 */

#define AU_MODEL_NAME_NUM_FILE    0xC8 //200 in build audio file models name range 201 to 255
#define AU_TELEMETRY_GET          0x1E //510
#define AU_TELEMETRY_LOSS         0x1F //511


///* make sure the defines below always go in numeric order */
//enum AUDIO_SOUNDS {
	//AU_TADA,
	//#if defined(VOICE)
	//AU_THROTTLE_ALERT,
	//AU_SWITCH_ALERT,
	//AU_BAD_EEPROM,
	//AU_EEPROM_FORMATTING,
	//#endif
	//AU_TX_BATTERY_LOW,
	//AU_INACTIVITY,
	//AU_ERROR,
	//AU_KEYPAD_UP,
	//AU_KEYPAD_DOWN,
	//AU_MENUS,
	//AU_TRIM_MOVE,
	//AU_WARNING1,
	//AU_WARNING2,
	//AU_WARNING3,
	//AU_TRIM_MIDDLE,
	//AU_POT_MIDDLE,
	//AU_MIX_WARNING_1,
	//AU_MIX_WARNING_2,
	//AU_MIX_WARNING_3,
	//AU_TIMER_00,
	//AU_TIMER_LT10,
	//AU_TIMER_20,
	//AU_TIMER_30,
//
	//AU_FRSKY_FIRST,
	//AU_FRSKY_BEEP1 = AU_FRSKY_FIRST,
	//AU_FRSKY_BEEP2,
	//AU_FRSKY_BEEP3,
	//AU_FRSKY_WARN1,
	//AU_FRSKY_WARN2,
	//AU_FRSKY_CHEEP,
	//AU_FRSKY_RATATA,
	//AU_FRSKY_TICK,
	//AU_FRSKY_SIREN,
	//AU_FRSKY_RING,
	//AU_FRSKY_SCIFI,
	//AU_FRSKY_ROBOT,
	//AU_FRSKY_CHIRP,
	//AU_FRSKY_TADA,
	//AU_FRSKY_CRICKET,
	//AU_FRSKY_ALARMC,
	//AU_FRSKY_LAST,
//
	//AU_NONE=0xff
//};

//TODO: Replace with DX6I message
// First message
//const pm_char STR_OPENAVRCISLOADING[] PROGMEM = "OpenAVRc is loading ...";
//#define  LOADINGMESSAGE() lcdDrawTextAtt(1*FW, 3*FH, STR_OPENAVRCISLOADING, BOLD)

#if defined(AUDIO)
#include "audio_avr.h"
#endif

#include "buzzer.h"

#if defined(VOICE)
#include "boards/dx6i/voice.h"
#endif

#include "translations.h"

#if defined(PERSONAMES)
#include "personames.h"
#endif

#if defined(GUI)
#include "fonts.h"
#endif

#if defined(HAPTIC)
#include "haptic.h"
#endif

#if defined(SDCARD)
#include "sdcard.h"
#endif

#if defined(MULTIMODULE)
//struct mm_protocol_definition {
	//uint8_t protocol;
	//const pm_char *subTypeString;
	//uint8_t maxSubtype;
	//const char *optionsstr;
//};
const mm_protocol_definition *getMultiProtocolDefinition (uint8_t protocol);

#endif

///////////////// PROTOCOLS ///////////////////

extern bool rangeModeIsOn;
extern uint8_t protoMode;



#define PROTO_NEED_SPI PIN3_bm // bitmask
#define IS_PROTO_NEED_SPI if (RfOptionSettings.rfProtoNeed & PROTO_NEED_SPI)

//#define BOOL1USED PIN0_bm
//#define BOOL2USED PIN1_bm
//#define BOOL3USED PIN2_bm

//struct RfOptionSettingsvarstruct {
	//uint8_t         rfProtoNeed:4;     // See usage in "PROTO_NEED_XX" Def
	//uint8_t         rfSubTypeMax:4;       //16 max
	//int8_t          rfOptionValue1Min;
	//int8_t          rfOptionValue1Max;
	//int8_t          rfOptionValue2Min;
	//int8_t          rfOptionValue2Max;
	//int8_t          rfOptionValue3Max/*:5*/;  //32 max -16 is min
//};

//struct RfOptionSettingsstruct {
	//uint8_t         rfProtoNeed:4;     // See usage in "PROTO_NEED_XX" Def
	//uint8_t         rfSubTypeMax:4;       //16 max
	//const pm_char*  rfSubTypeNames;
	//int8_t          rfOptionValue1Min;
	//int8_t          rfOptionValue1Max;
	//const pm_char*  rfOptionValue1Name;
	//int8_t          rfOptionValue2Min;
	//int8_t          rfOptionValue2Max;
	//const pm_char*  rfOptionValue2Name;
	//int8_t          rfOptionValue3Max:5;  //32 max -16 is min
	//const pm_char*  rfOptionValue3Name;
	//uint8_t         rfOptionBool1Used:1;
	//const pm_char*  rfOptionBool1Name;
	//uint8_t         rfOptionBool2Used:1;
	//const pm_char*  rfOptionBool2Name;
	//uint8_t         rfOptionBool3Used:1;
	//const pm_char*  rfOptionBool3Name;
//};

void SetRfOptionSettings(uint_farptr_t RfOptSet,
const pm_char* rfSubTypeNames,
const pm_char* rfOptionValue1Name,
const pm_char* rfOptionValue2Name,
const pm_char* rfOptionValue3Name,
const pm_char* rfOptionBool1Name,
const pm_char* rfOptionBool2Name,
const pm_char* rfOptionBool3Name);

void LimitRfOptionSettings();

//PPM Default

void sendOptionsSettingsPpm();
//PPM

extern struct RfOptionSettingsstruct RfOptionSettings;
extern uint8_t * packet; //protocol global packet

//uint16_t (*timer_callback)(void);

#include "protocol/protocol.h"

extern void startPulses(enum ProtoCmds Command);

/* Load Protocols */
//#include "protocol/interface.h"

///////////////// PROTOCOLS ///////////////////

getvalue_t convert8bitsTelemValue(uint8_t channel, ls_telemetry_value_t value);
getvalue_t convertLswTelemValue(LogicalSwitchData * cs);

//////////////////////////////////////////////////////////////////////////
#endif /* OBSOLETETX_H_ */