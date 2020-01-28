/*
 * translations.h
 *
 * Created: 1/25/2020 8:22:01 PM
 *  Author: Andrew
 */ 


#ifndef TRANSLATIONS_H_
#define TRANSLATIONS_H_

#include "_Config.h"

#define CENTER

//////////ENGLISH TRANSLATION//////////////////////////
/* Formatting octal codes available in TR_ strings:
*  \037\x           -sets LCD x-coord (x value in octal)
*  \036             -newline
*  \035             -horizontal tab (ARM only)
*  \001 to \034     -extended spacing (value * FW/2)
*  \0               -ends current string
*/

// NON ZERO TERMINATED STRINGS
#define LEN_OFFON              "\003"
#define TR_OFFON               "OFF""ON\0"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Quiet""Alarm""NoKey""All\0 "

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "No REaREb"

#define LEN_VBLMODE            "\004"
#define TR_VBLMODE             "OFF\0""Keys""Ctrl""Both""ON\0 "

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             "OFF"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_UART3MODES         "\015"
#define TR_UART3MODES          "OFF\0         ""S-Port Mirror""Telemetry\0   ""SBUS Trainer\0""Debug\0"

#define LEN_SWTYPES            "\006"
#define TR_SWTYPES             "None\0 ""Toggle""2POS\0 ""3POS\0"

#define LEN_POTTYPES           "\017"
#define TR_POTTYPES            "None\0          ""Pot with detent""Multipos Switch""Pot\0"

#define LEN_SLIDERTYPES        "\006"
#define TR_SLIDERTYPES         "None\0 ""Slider"

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DATE:""TIME:"

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "OFF\0        ""Flight\0     ""Manual Reset"

#define LEN_COUNTRYCODES       "\002"
#define TR_COUNTRYCODES        "US""JP""EU"

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "OFF\0""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "OFF\0""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           "\006"
#define TR_VTRIMINC            "Expo\0 ""ExFine""Fine\0 ""Medium""Coarse"

#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "No\0   ""Change""Yes\0"

#define LEN_VBEEPCOUNTDOWN     "\006"
#define TR_VBEEPCOUNTDOWN      "SilentBeeps\0Voice\0Haptic"

#define LEN_VVARIOCENTER       "\006"
#define TR_VVARIOCENTER        "Tone\0 ""Silent"

#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Standard""Custom\0"

#define LEN_RETA123            "\001"

#define TR_RETA123           "RETA123ab"

#define LEN_VPROTOS            "\006"

#if defined(PXX)
#define TR_PXX               "PXX\0  "
#elif defined(DSM2_SERIAL)
#define TR_PXX               "[PXX]\0"
#else
#define TR_PXX
#endif

#if defined(DSM2_SERIAL)
#define TR_DSM2              "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
#define TR_DSM2
#endif

#if defined(SPIMODULES)
#define TR_SPIM               "SPIRfMod"
#define TR_RFTUNECOARSE       INDENT "Freq.coarse"
#define TR_RFPOWER INDENT     "RF Power"
#endif

#if defined(MULTIMODULE)
#define LEN_MULTIPROTOCOLS    "\006"
#define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
#define TR_MULTI_CUSTOM       "Custom"
#endif

#if defined(MULTIMODULE) || defined(SPIMODULES)
#define TR_SUBTYPE            "SubType"
#define TR_RFTUNEFINE         INDENT "Freq.fine"
#define TR_TELEMETRY             "Telemetry"
#define TR_MULTI_VIDFREQ               INDENT "Vid. freq."
#define TR_MULTI_RFPOWER               INDENT "RF Power"
#define TR_MULTI_OPTION                INDENT "Option"
#define TR_AUTOBIND              INDENT "Autobind"
#define TR_MULTI_DSM_AUTODTECT         INDENT "Autodetect"
#define TR_MULTI_LOWPOWER              INDENT "Low power"
#define TR_MULTI_SERVOFREQ             INDENT "Servo rate"

#endif





#define LEN_POSNEG             "\003"
#define TR_POSNEG              "POS""NEG"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Add\0    ""Multiply""Replace\0"

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "OFF""ON\0""Rud""Ele""Thr""Ail"

#define TR_CSWTIMER          "Tim\0 "
#define TR_CSWSTICKY         "Glue\0"
#define TR_CSWRANGE
#define TR_CSWSTAY



#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\012"

#if defined(VARIO)
#define TR_VVARIO            "Vario\0    "
#else
#define TR_VVARIO            "[Vario]\0  "
#endif

#if defined(AUDIO)
#define TR_SOUND             "Play Sound"
#else
#define TR_SOUND             "Beep\0     "
#endif

#if defined(HAPTIC)
#define TR_HAPTIC            "Haptic\0   "
#else
#define TR_HAPTIC            "[Haptic]\0 "
#endif

#if defined(VOICE)
#define TR_PLAY_TRACK      "Play Track"
#define TR_PLAY_BOTH         "Play Both\0"
#define TR_PLAY_VALUE        "Play Val\0 "
#else
#define TR_PLAY_TRACK        "[Play Trk]"
#define TR_PLAY_BOTH         "[Play Bth]"
#define TR_PLAY_VALUE        "[Play Val]"
#endif

#define TR_SF_BG_MUSIC         "BgMusic\0  ""BgMusic ||"

#if defined(SDCARD)
#define TR_SDCLOGS           "SD Logs\0  "
#else
#define TR_SDCLOGS           "[SD Logs]\0"
#endif

#if defined(GVARS)
#define TR_ADJUST_GVAR       "Adjust \0  "
#else
#define TR_ADJUST_GVAR       "[AdjustGV]"
#endif

#define TR_SF_PLAY_SCRIPT   "[Lua]\0    "

#if defined(DEBUG)
#define TR_SF_TEST          "Test\0"
#else
#define TR_SF_TEST
#endif

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
#define TR_SF_SAFETY        "Overr.\0   "
#else
#define TR_SF_SAFETY        "---\0      "
#endif

#define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[reserve]\0"

#define TR_VFSWFUNC          TR_SF_SAFETY "Trainer\0  ""Inst. Trim""Reset\0    " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Backlight\0" TR_SF_TEST

#define LEN_VFSWRESET          "\004"

#if defined(FRSKY)
#define TR_FSW_RESET_TELEM   "Telm"
#define TR_MODELISON         INDENT "Modele is ON"
#else
#define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
#define TR_FSW_RESET_ROTENC  "REa\0""REb\0"
#elif ROTARY_ENCODERS == 1
#define TR_FSW_RESET_ROTENC  "R.E."
#else
#define TR_FSW_RESET_ROTENC
#endif

#define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"

#define TR_VFSWRESET           TR_FSW_RESET_TIMERS "All\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC

#define LEN_FUNCSOUNDS         "\004"
#define TR_FUNCSOUNDS          "Bp1\0""Bp2\0""Bp3\0""Wrn1""Wrn2""Chee""Rata""Tick""Sirn""Ring""SciF""Robt""Chrp""Tada""Crck""Alrm"

#define LEN_VTELEMCHNS         "\004"

#define TR_TELEM_RESERVE
#define TR_TELEM_TIME
#define TR_SWR
#define TR_RX_BATT
#define TR_A3_A4
#define TR_A3_A4_MIN

#define TR_ASPD_MAX            "ASp+"

#define TR_TELEM_RSSI_RX     "Rx\0 "

#define TR_TELEM_TIMERS      "Tmr1""Tmr2"

#define TR_VTELEMCHNS        "---\0""Batt" TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Time"

#define LENGTH_UNIT_IMP        "ft\0"
#define SPEED_UNIT_IMP         "mph"
#define LENGTH_UNIT_METR       "m\0 "
#define SPEED_UNIT_METR        "kmh"

#if defined(IMPERIAL_UNITS)
#define LENGTH_UNIT        LENGTH_UNIT_IMP
#define SPEED_UNIT         SPEED_UNIT_IMP
#else
#define LENGTH_UNIT        LENGTH_UNIT_METR
#define SPEED_UNIT         SPEED_UNIT_METR
#endif
#define LEN_VTELEMUNIT       "\003"
#define TR_VTELEMUNIT        "V\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "

#define STR_V                  (STR_VTELEMUNIT+1)
#define STR_A                  (STR_VTELEMUNIT+4)

#define LEN_VALARM             "\003"
#define TR_VALARM              "---""Yel""Org""Red"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "None\0  ""Hub\0   ""WSHHigh""S.Port\0"

#define LEN_AMPSRC           "\003"
#define TR_AMPSRC            "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
#define TR_VARIOSRC          "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
#define TR_VARIOSRC          "Alt\0""Alt+""VSpd""A1\0 ""A2\0"
#endif

#define LEN_VTELEMSCREENTYPE "\004"
#define TR_VTELEMSCREENTYPE  "Nums""Bars"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS\0""NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Clear Mixes\0\0""Simple 4-CH \0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               "Menu\0""Exit\0""Down\0""Up\0  ""Right""Left\0"

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#define TR_STICKS_VSRCRAW      "Rud\0""Ele\0""Thr\0""Ail\0"
#define TR_TRIMS_VSRCRAW       "TrmR""TrmE""TrmT""TrmA"

#define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
#define TR_SW_VSRCRAW        "3POS"

#define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

#define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"

#define TR_TRIMS_SWITCHES      "tRl""tRr""tEd""tEu""tTd""tTu""tAl""tAr"

#define TR_ROTARY_ENCODERS   "REa\0""REb\0"
#define TR_ROTENC_SWITCHES   "REA""REB""REN"

#define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"

#define TR_ON_ONE_SWITCHES     "ON\0""One"

#define TR_EXTRA_3POS_SWITCHES  "XD0""XD1""XD2"

#define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES

#if defined(HELI)
#define TR_CYC_VSRCRAW       "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW       "[C1]""[C2]""[C3]"
#endif

#define TR_EXTRA_VSRCRAW

#define TR_VSRCRAW             "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "OFF""ON\0""THs""TH%""THt"

#define LEN_VTRAINERMODES      "\022"
#define TR_VTRAINERMODES       "Master/Jack\0      ""Slave/Jack\0       ""Master/SBUS Module""Master/CPPM Module""Master/Battery\0"

#define LEN_VFAILSAFE          "\011"
#define TR_VFAILSAFE           "Not set\0 ""Hold\0    ""Custom\0  ""No pulses""Receiver\0"

#if defined(MAVLINK)
#define LEN_MAVLINK_BAUDS    "\006"
#define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
#define LEN_MAVLINK_AC_MODES "\011"
#define TR_MAVLINK_AC_MODES  "Stabilize""Acro     ""Alt Hold ""Auto     ""Guided   ""Loiter   ""RTL      ""Circle   ""Pos Hold ""Land     ""OF Loiter""Toy A    ""Toy M    ""INVALID  "
#define LEN_MAVLINK_AP_MODES "\015"
#define TR_MAVLINK_AP_MODES  "Manual       ""Circle       ""Stabilize    ""Training     ""Fly by Wire A""Fly by Wire A""Auto         ""RTL          ""Loiter       ""Guided       ""Initialising ""INVALID      "
#endif

#define LEN_VSENSORTYPES        "\012"
#define TR_VSENSORTYPES        "Custom\0   ""Calculated"

#define LEN_VFORMULAS          "\010"
#define TR_VFORMULAS           "Add\0    ""Average\0""Min\0    ""Max\0    ""Multiply""Totalize""Cell\0   ""Consumpt""Distance"

#define LEN_VPREC              "\004"
#define TR_VPREC               "0.--""0.0 ""0.00"

#define LEN_VCELLINDEX         "\007"
#define TR_VCELLINDEX          "Lowest\0""1\0     ""2\0     ""3\0     ""4\0     ""5\0     ""6\0     ""Highest""Delta\0"

// ZERO TERMINATED STRINGS
#define INDENT               "\001"
#define LEN_INDENT           1
#define INDENT_WIDTH         (FW/2)
#define BREAKSPACE           " "

#define TR_ENTER             "[MENU]"

#define TR_EXIT                "[EXIT]"

#define TR_POPUPS            TR_ENTER "\010" TR_EXIT
#define OFS_EXIT             sizeof(TR_ENTER)

#define TR_LOADING				"OpenAVRc is loading ..."
#define TR_MENUWHENDONE        CENTER "\006" TR_ENTER " WHEN DONE"
#define TR_FREE                "free"
#define TR_DELETEMODEL         "DELETE" BREAKSPACE "MODEL"
#define TR_COPYINGMODEL        "Copying model..."
#define TR_MOVINGMODEL         "Moving model..."
#define TR_LOADINGMODEL        "Loading model..."
#define TR_NAME                "Name"
#define TR_MODELNAME           "Model Name"
#define TR_PHASENAME           "Mode Name"
#define TR_MIXNAME             "Mix Name"
#define TR_INPUTNAME           "Input Name"
#define TR_EXPONAME          "Expo Name"
#define TR_BITMAP              "Model Image"
#define TR_TIMER               "Timer"
#define TR_ELIMITS             "E.Limits"
#define TR_ETRIMS              "E.Trims"
#define TR_TRIMINC             "Trim Step"
#define TR_DISPLAY_TRIMS       "Show Trims"
#define TR_TTRACE              "T-Source"
#define TR_TSWITCH             "T-Switch"
#define TR_TTRIM               "T-Trim"
#define TR_BEEPCTR             "Ctr Beep"
#define TR_USE_GLOBAL_FUNCS    "Glob.Funcs"
#define TR_PROTO               INDENT "Proto"
#define TR_PPMFRAME          "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Switch"
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Fade In"
#define TR_FADEOUT             "Fade Out"
#define TR_DEFAULT             "(default)"

#define TR_CHECKTRIMS          CENTER "\006Check\012Trims"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)

#define TR_SWASHTYPE           "Swash Type"
#define TR_COLLECTIVE          "Collective"
#define TR_AILERON             "Lateral cyc. source"
#define TR_ELEVATOR            "Long. cyc. source"
#define TR_SWASHRING           "Swash Ring"
#define TR_ELEDIRECTION        "ELE Direction"
#define TR_AILDIRECTION        "AIL Direction"
#define TR_COLDIRECTION        "PIT Direction"
#define TR_MODE                INDENT "Mode"
#define TR_NOFREEEXPO          "No free expo!"
#define TR_NOFREEMIXER         "No free mixer!"
#define TR_INSERTMIX           "INSERT MIX "
#define TR_EDITMIX             "EDIT MIX "
#define TR_SOURCE              INDENT "Source"
#define TR_WEIGHT              "Weight"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Side"
#define TR_DIFFERENTIAL        "Differ"
#define TR_OFFSET              INDENT "Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define DREX_CHBOX_OFFSET      30
#define TR_CURVE               "Curve"
#define TR_FLMODE              "Mode"
#define TR_MIXWARNING          "Warning"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Delay Dn"
#define TR_DELAYUP             "Delay Up"
#define TR_SLOWDOWN            "Slow  Dn"
#define TR_SLOWUP              "Slow  Up"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004channel"
#define TR_RANGE               INDENT "Range"
#define TR_CENTER              INDENT "Center"
#define TR_BAR                 "Bar"
#define TR_ALARM               INDENT "Alarm"
#define TR_USRDATA             "UsrData"
#define TR_BLADES              INDENT "Blades"
#define TR_SCREEN              "Screen\001"
#define TR_SOUND_LABEL         "Sound"
#define TR_LENGTH              INDENT "Length"
#define TR_BEEP_LENGTH         INDENT "Beep Length"
#define TR_SPKRPITCH           INDENT "Beep Pitch"
#define TR_HAPTIC_LABEL        "Haptic"
#define TR_HAPTICSTRENGTH      INDENT "Strength"
#define TR_CONTRAST            "Contrast"
#define TR_ALARMS_LABEL        "Alarms"
#define TR_BATTERY_RANGE       "Battery range"
#define TR_BATTERYWARNING      INDENT "Battery Low"
#define TR_INACTIVITYALARM     INDENT "Inactivity"
#define TR_MEMORYWARNING       INDENT "Memory Low"
#define TR_ALARMWARNING        INDENT "Sound Off"
#define TR_RENAVIG             "RotEnc Navig"
#define TR_THROTTLE_LABEL      "Throttle"
#define TR_THROTTLEREVERSE     "T-Reverse"
#define TR_TIMER_NAME          INDENT "Name"
#define TR_MINUTEBEEP          INDENT "Minute"
#define TR_BEEPCOUNTDOWN       INDENT "Countdown"
#define TR_PERSISTENT          INDENT "Persist."
#define TR_BACKLIGHT_LABEL     "Backlight"
#define TR_BLDELAY             INDENT "Duration"
#define TR_BLONBRIGHTNESS      INDENT "ON Brightness"
#define TR_BLOFFBRIGHTNESS     INDENT "OFF Brightness"
#define TR_BLCOLOR             INDENT "Color"
#define TR_SPLASHSCREEN        "Splash screen"
#define TR_THROTTLEWARNING     "T-Warning"
#define TR_SWITCHWARNING       "S-Warning"
#define TR_POTWARNING          "Pot Warn."
#define TR_TIMEZONE            "Time Zone"
#define TR_ADJUST_RTC          "Adjust RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Rx Channel Ord"
#define TR_STICKS              "Sticks"
#define TR_POTS                "Pots"
#define TR_SWITCHES_DELAY      "Play delay"
#define TR_SLAVE               CENTER "Slave"
#define TR_MODESRC             "Mode\006% Source"
#define TR_MULTIPLIER          "Multiplier"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART       CENTER "\010" TR_ENTER " TO START"
#define TR_SETMIDPOINT       CENTER "\004SET STICKS MIDPOINT"
#define TR_MOVESTICKSPOTS    CENTER "\006MOVE STICKS/POTS"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "NO DATA"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                  "us"
#define TR_MENUTORESET         CENTER TR_ENTER " to reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FM"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM low mem"
#define TR_ALERT               "\016ALERT"
#define TR_PRESSANYKEYTOSKIP   "Press any key to skip"
#define TR_THROTTLENOTIDLE     "Throttle not idle"
#define TR_ALARMSDISABLED      "Alarms Disabled"
#define TR_PRESSANYKEY         "\010Press any Key"
#define TR_BADEEPROMDATA       "Bad EEprom Data"
#define TR_EEPROMFORMATTING    "Formatting EEPROM"
#define TR_EEPROMOVERFLOW      "EEPROM overflow"
#define TR_MENURADIOSETUP      "RADIO SETUP"
#define TR_MENUDATEANDTIME     "DATE AND TIME"
#define TR_MENUTRAINER         "TRAINER"
#define TR_MENUGLOBALFUNCS     "GLOBAL FUNCTIONS"
#define TR_MENUVERSION         "VERSION"
#define TR_MENUDIAG            "SWITCHES"
#define TR_MENUANA             "ANAS"
#define TR_MENUCALIBRATION     "CALIBRATION"
#define TR_TRIMS2OFFSETS     "\006Trims => Subtrims"
#define TR_MENUMODELSEL        "MODELSEL"
#define TR_MENUSETUP           "SETUP"
#define TR_MENUFLIGHTPHASES    "FLIGHT MODES"
#define TR_MENUFLIGHTPHASE     "FLIGHT MODE"
#define TR_MENUHELISETUP       "HELI SETUP"

#define TR_MENUINPUTS        "STICKS"
#define TR_MENULIMITS        "SERVOS"

#define TR_MENUCURVES          "CURVES"
#define TR_MENUCURVE           "CURVE"
#define TR_MENULOGICALSWITCH   "LOGICAL SWITCH"
#define TR_MENULOGICALSWITCHES "LOGICAL SWITCHES"
#define TR_MENUCUSTOMFUNC      "SPECIAL FUNCTIONS"
#define TR_MENUCUSTOMSCRIPTS   "CUSTOM SCRIPTS"
#define TR_MENUCUSTOMSCRIPT    "CUSTOM SCRIPT"
#define TR_MENUTELEMETRY       "TELEMETRY"
#define TR_MENUTEMPLATES       "TEMPLATES"
#define TR_MENUSTAT            "STATS"
#define TR_MENUDEBUG           "DEBUG"
#define TR_RXNUM             "RxNum"
#define TR_SYNCMENU            "[Sync]"
#define TR_LIMIT               INDENT "Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            "GPS Coords"
#define TR_VARIO               "Vario"
#define TR_PITCH_AT_ZERO       INDENT "Pitch at Zero"
#define TR_PITCH_AT_MAX        INDENT "Pitch at Max"
#define TR_REPEAT_AT_ZERO      INDENT "Repeat at Zero"
#define TR_POWEROFF            "\006SHUT DOWN ?"
#define TR_SHUTDOWN            "SHUTTING DOWN"
#define TR_SAVEMODEL           "Saving model settings"
#define TR_BATT_CALIB          "Battery Calib"
#define TR_CURRENT_CALIB       "Current Calib"
#define TR_VOLTAGE             INDENT "Voltage"
#define TR_CURRENT             INDENT "Current"
#define TR_SELECT_MODEL        "Select Model"
#define TR_CREATE_MODEL        "Create Model"
#define TR_COPY_MODEL          "Copy Model"
#define TR_MOVE_MODEL          "Move Model"
#define TR_BACKUP_MODEL        "Backup Model"
#define TR_DELETE_MODEL        "Delete Model"
#define TR_RESTORE_MODEL       "Restore Model"
#define TR_SDCARD_ERROR        "SD Card Error"
#define TR_NO_SDCARD           "No SD Card"
#define TR_SDCARD_FULL         "SD Card Full"
#define TR_INCOMPATIBLE        "Incompatible"
#define TR_WARNING             "WARNING"
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "Converting EEPROM"
#define TR_THROTTLEWARN        "THROTTLE"
#define TR_ALARMSWARN          "ALARMS"
#define TR_SWITCHWARN          "SWITCH"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         "Failsafe not set"
#define TR_KEYSTUCK            "Key stuck"
#define TR_INVERT_THR          "Invert Thr?"
#define TR_SPEAKER_VOLUME      INDENT "Volume" // TODO could be TR_VOLUME ?
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Brightness"
#define TR_CPU_TEMP            "CPU Temp."
#define TR_CPU_CURRENT         "Current"
#define TR_CPU_MAH             "Consumpt."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp."
#define TR_CAPAWARNING         INDENT "Capacity Low"
#define TR_TEMPWARNING         INDENT "Overheat"
#define TR_FUNC                "Func"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duration"
#define TR_DELAY               "Delay"
#define TR_SD_CARD             "SD CARD"
#define TR_SDHC_CARD           "SD-HC CARD"
#define TR_NO_SOUNDS_ON_SD     "No Sounds" BREAKSPACE "on SD"
#define TR_NO_MODELS_ON_SD     "No Models" BREAKSPACE "on SD"
#define TR_NO_BITMAPS_ON_SD    "No Bitmaps" BREAKSPACE "on SD"
#define TR_NO_SCRIPTS_ON_SD    "No Scripts" BREAKSPACE "on SD"
#define TR_SCRIPT_SYNTAX_ERROR "Script syntax error"
#define TR_SCRIPT_PANIC        "Script panic"
#define TR_SCRIPT_KILLED       "Script killed"
#define TR_SCRIPT_ERROR        "Unknown error"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Delete"
#define TR_COPY_FILE           "Copy"
#define TR_RENAME_FILE         "Rename"
#define TR_ASSIGN_BITMAP       "Assign Bitmap"
#define TR_EXECUTE_FILE        "Execute"
#define TR_REMOVED             " removed"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatting..."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Time"
#if defined(IMPERIAL_UNITS)
#define TR_TXTEMP              "Temp. TX\037\164@F"
#else
#define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Type:"
#define TR_SD_SPEED            "Speed:"
#define TR_SD_SECTORS          "Sectors:"
#define TR_SD_SIZE             "Size:"
#define TR_TYPE                INDENT "Type"
#define TR_GLOBAL_VARS         "Global Variables"
#define TR_GLOBAL_V            "GLOBAL V."
#define TR_GLOBAL_VAR          "Global Variable"
#define TR_MENUGLOBALVARS      "GLOBAL VARIABLES"
#define TR_OWN                 "Own"
#define TR_DATE                "Date"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "CHANNELS MONITOR"
#define TR_MIXERS_MONITOR      "MIXERS MONITOR"
#define TR_PATH_TOO_LONG       "Path too long"
#define TR_VIEW_TEXT           "View text"
#define TR_FLASH_BOOTLOADER    "Flash BootLoader"
#define TR_FLASH_EXTERNAL_DEVICE "Flash External Device"
#define TR_FLASH_INTERNAL_MODULE "Flash Internal Module"
#define TR_WRITING             "\032Writing..."
#define TR_CONFIRM_FORMAT      "Confirm Format?"
#define TR_INTERNALRF          "Internal RF"
#define TR_EXTERNALRF          "External RF"
#define TR_FAILSAFE            INDENT "Failsafe"
#define TR_FAILSAFESET         "FAILSAFE SETTINGS"
#define TR_MENUSENSOR          "SENSOR"
#define TR_COUNTRYCODE         "Country Code"
#define TR_VOICELANG           "Voice Language"
#define TR_UNITSSYSTEM         "Units"
#define TR_EDIT                "Edit"
#define TR_INSERT_BEFORE       "Insert Before"
#define TR_INSERT_AFTER        "Insert After"
#define TR_COPY                "Copy"
#define TR_MOVE                "Move"
#define TR_PASTE               "Paste"
#define TR_DELETE              "Delete"
#define TR_INSERT              "Insert"
#define TR_RESET_FLIGHT        "Reset Flight"
#define TR_RESET_TIMER1        "Reset Timer1"
#define TR_RESET_TIMER2        "Reset Timer2"
#define TR_RESET_TIMER3        "Reset Timer3"
#define TR_RESET_TELEMETRY     "Reset Telemetry"
#define TR_STATISTICS          "Statistics"
#define TR_SAVE_TIMERS         "Save Timers"
#define TR_ABOUT_US            "About"
#define TR_AND_SWITCH          "AND Switch"
#define TR_SF                  "SF"
#define TR_GF                  "GF"
#define TR_SPEAKER             INDENT "Speaker"
#define TR_BUZZER              INDENT "Buzzer"
#define TR_BYTES               "bytes"
#define TR_MODULE_BIND         "[Bnd]"
#define TR_MODULE_RANGE        "[Rng]"
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Set]"
#define TR_TRAINER             "Trainer Port"
#define TR_ANTENNAPROBLEM      CENTER "TX Antenna problem!"
#define TR_MODELIDUSED         "ID already used"
#define TR_MODULE              INDENT "Module"
#define TR_TELEMETRY_TYPE      "Type"
#define TR_TELEMETRY_SENSORS   "Sensors"
#define TR_VALUE               "Value"
#define TR_TOPLCDTIMER         "Top LCD Timer"
#define TR_UNIT                "Unit"
#define TR_TELEMETRY_NEWSENSOR INDENT "Add a new sensor..."
#define TR_CHANNELRANGE        INDENT "Ch. Range"
#define TR_LOWALARM            INDENT "Low Alarm"
#define TR_CRITICALALARM       INDENT "Critical Alarm"
#define TR_ENABLE_POPUP        "Enable Popup"
#define TR_DISABLE_POPUP       "Disable Popup"
#define TR_CURVE_PRESET        "Preset..."
#define TR_PRESET              "Preset"
#define TR_MIRROR              "Mirror"
#define TR_CLEAR               "Clear"
#define TR_RESET               "Reset"
#define TR_RESET_SUBMENU       "Reset..."
#define TR_COUNT               "Count"
#define TR_PT                  "pt"
#define TR_PTS                 "pts"
#define TR_SMOOTH              "Smooth"
#define TR_COPY_STICKS_TO_OFS  "Copy sticks to subtrim"
#define TR_COPY_TRIMS_TO_OFS   "Copy trims to subtrim"
#define TR_INCDEC              "Inc/Decrement"
#define TR_GLOBALVAR           "Global Var"
#define TR_MIXSOURCE           "Mixer Source"
#define TR_CONSTANT            "Constant"
#define TR_PERSISTENT_MAH      INDENT "Str mAh"
#define TR_PREFLIGHT           "Preflight Checks"
#define TR_CHECKLIST           INDENT "Checklist"
#define TR_FAS_OFFSET          INDENT "FAS Ofs"
#define TR_UART3MODE           "Serial port"
#define TR_SCRIPT              "Script"
#define TR_INPUTS              "Inputs"
#define TR_OUTPUTS             "Outputs"
#define TR_EEBACKUP            "\012[ENTER Long]: EEPROM backup"
#define TR_FACTORYRESET        "\012[MENU Long]: Factory reset"


#if defined(X_ANY) || defined(BLUETOOTH)
#define TR_X_ANY                        "X ANY"
#define TR_ACTIVED                      "Active"
#define TR_NUMBER                       "Number"
#define TR_CHANNEL                      "Channel "
#define TR_NB_REPEAT                    "Nb repeat "
#define LEN_SWITCHES_VALUES             "\005"
#define TR_SWITCHES_VALUES              "Sw.--""L1-L4""L1-L8""Sw.4 ""Sw.8 ""Sw.16"
#define TR_ANGLE_SENSOR                 "0:360"
#define TR_PROP                         "Prop."
#define LEN_XANY_EXPO                   "\005"
#define TR_XANY_EXPO                    "0%\0  ""25%\0 ""37.5%""50%\0 "
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////

#define LEN_SPECIAL_CHARS 0

#define PSIZE(x) ( sizeof(x) - 1 )
#define EOFS(x)  ( OFS_##x + sizeof(TR_##x) )

// The non-0-terminated-strings

extern const pm_char STR_OPEN9X[];

#define OFS_OFFON              0
#define OFS_MMMINV             (OFS_OFFON + sizeof(TR_OFFON))
#define OFS_NCHANNELS          (OFS_MMMINV + sizeof(TR_MMMINV))
#if defined(GRAPHICS)
#define OFS_VBEEPMODE          (OFS_NCHANNELS + sizeof(TR_NCHANNELS))
#else
#define OFS_VBEEPLEN           (OFS_NCHANNELS + sizeof(TR_NCHANNELS))
#define OFS_VBEEPMODE          (OFS_VBEEPLEN + sizeof(TR_VBEEPLEN))
#endif
#if defined(ROTARY_ENCODERS)
#define OFS_VRENAVIG           (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_VRENCODERS         (OFS_VRENAVIG + sizeof(TR_VRENAVIG))
#define OFS_TRNMODE            (OFS_VRENCODERS + sizeof(TR_VRENCODERS))
#elif defined(ROTARY_ENCODER_NAVIGATION)
#define OFS_VRENCODERS         (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_TRNMODE            (OFS_VRENCODERS + sizeof(TR_VRENCODERS))
#else
#define OFS_TRNMODE            (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#endif
#define OFS_TRNCHN             (OFS_TRNMODE + sizeof(TR_TRNMODE))
#define OFS_VTRIMINC         (OFS_TRNCHN + sizeof(TR_TRNCHN))
#define OFS_RETA123          (OFS_VTRIMINC + sizeof(TR_VTRIMINC))
#if defined(MULTIMODULE)
#define OFS_MULTIPROTOCOLS   (OFS_RETA123 + sizeof(TR_RETA123))
#define OFS_POSNEG           (OFS_MULTIPROTOCOLS + sizeof(TR_MULTIPROTOCOLS))
#else
#define OFS_POSNEG           (OFS_RETA123 + sizeof(TR_RETA123))
#endif
#define OFS_VBLMODE          (OFS_POSNEG + sizeof(TR_POSNEG))
#define OFS_VCURVEFUNC         (OFS_VBLMODE + sizeof(TR_VBLMODE))
#define OFS_VMLTPX             (OFS_VCURVEFUNC + sizeof(TR_VCURVEFUNC))
#define OFS_VMLTPX2            (OFS_VMLTPX + sizeof(TR_VMLTPX))
#define OFS_VMIXTRIMS          (OFS_VMLTPX2 + sizeof(TR_VMLTPX2))
#define OFS_VCSWFUNC           (OFS_VMIXTRIMS + sizeof(TR_VMIXTRIMS))
#define OFS_VFSWFUNC           (OFS_VCSWFUNC + sizeof(TR_VCSWFUNC))
#define OFS_VFSWRESET          (OFS_VFSWFUNC + sizeof(TR_VFSWFUNC))
#define OFS_FUNCSOUNDS         (OFS_VFSWRESET + sizeof(TR_VFSWRESET))
#define OFS_VTELEMCHNS         (OFS_FUNCSOUNDS + sizeof(TR_FUNCSOUNDS))
#if defined(FRSKY)
#define OFS_VTELEMUNIT      (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#define OFS_VALARM          (OFS_VTELEMUNIT + sizeof(TR_VTELEMUNIT))
#define OFS_VALARMFN          (OFS_VALARM + sizeof(TR_VALARM))
#define OFS_VTELPROTO         (OFS_VALARMFN + sizeof(TR_VALARMFN))
#define OFS_GPSFORMAT         (OFS_VTELPROTO + sizeof(TR_VTELPROTO))
#define OFS_AMPSRC            (OFS_GPSFORMAT + sizeof(TR_GPSFORMAT))
#define OFS_VARIOSRC          (OFS_AMPSRC + sizeof(TR_AMPSRC))
#define OFS_VSCREEN           (OFS_VARIOSRC + sizeof(TR_VARIOSRC))
#define OFS_VTEMPLATES        (OFS_VSCREEN + sizeof(TR_VTELEMSCREENTYPE))
#else
#define OFS_VTEMPLATES        (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#endif
#if defined(TEMPLATES)
#define OFS_VSWASHTYPE        (OFS_VTEMPLATES + sizeof(TR_VTEMPLATES))
#else
#define OFS_VSWASHTYPE        (OFS_VTEMPLATES)
#endif
#if defined(HELI)
#define OFS_VKEYS             (OFS_VSWASHTYPE + sizeof(TR_VSWASHTYPE))
#else
#define OFS_VKEYS             (OFS_VSWASHTYPE)
#endif
#define OFS_VSWITCHES           (OFS_VKEYS + sizeof(TR_VKEYS))
#define OFS_VSRCRAW             (OFS_VSWITCHES + sizeof(TR_VSWITCHES))
#define OFS_VTMRMODES           (OFS_VSRCRAW + sizeof(TR_VSRCRAW))
#define OFS_DATETIME            (OFS_VTMRMODES + sizeof(TR_VTMRMODES))
#define OFS_VPERSISTENT         (OFS_DATETIME + sizeof(TR_DATETIME))
#if defined(X_ANY)
#define OFS_SWITCHES_VALUES     (OFS_VPERSISTENT + sizeof(TR_VPERSISTENT))
#define OFS_XANY_EXPO           (OFS_SWITCHES_VALUES + sizeof(TR_SWITCHES_VALUES))
#else
#define OFS_SWITCHES_VALUES     (OFS_VPERSISTENT)
#define OFS_XANY_EXPO           (OFS_SWITCHES_VALUES)
#endif

#define STR_OFFON               (STR_OPEN9X + OFS_OFFON)
#define STR_MMMINV              (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS           (STR_OPEN9X + OFS_NCHANNELS)
#if !defined(GRAPHICS)
#define STR_VBEEPLEN            (STR_OPEN9X + OFS_VBEEPLEN)
#endif
#define STR_VBEEPMODE           (STR_OPEN9X + OFS_VBEEPMODE)
#define STR_TRNMODE             (STR_OPEN9X + OFS_TRNMODE)
#define STR_TRNCHN              (STR_OPEN9X + OFS_TRNCHN)
#define STR_UART3MODES          (STR_OPEN9X + OFS_UART3MODES)
#define STR_SWTYPES             (STR_OPEN9X + OFS_SWTYPES)
#define STR_POTTYPES            (STR_OPEN9X + OFS_POTTYPES)
#define STR_SLIDERTYPES         (STR_OPEN9X + OFS_SLIDERTYPES)
#define STR_VTRIMINC            (STR_OPEN9X + OFS_VTRIMINC)
#define STR_VDISPLAYTRIMS       (STR_OPEN9X + OFS_VDISPLAYTRIMS)
#define STR_RETA123             (STR_OPEN9X + OFS_RETA123)
#if defined(MULTIMODULE)
#define STR_MULTIPROTOCOLS		(STR_OPEN9X + OFS_MULTIPROTOCOLS)
#endif
#define STR_POSNEG              (STR_OPEN9X + OFS_POSNEG)
#define STR_VBLMODE             (STR_OPEN9X + OFS_VBLMODE)
#define STR_VCURVEFUNC          (STR_OPEN9X + OFS_VCURVEFUNC)
#define STR_VSIDE               STR_VCURVEFUNC
#define LEN_VSIDE               LEN_VCURVEFUNC
#define STR_VMLTPX              (STR_OPEN9X + OFS_VMLTPX)
#define STR_VMLTPX2             (STR_OPEN9X + OFS_VMLTPX2)
#define STR_VMIXTRIMS           (STR_OPEN9X + OFS_VMIXTRIMS)
#define STR_VCSWFUNC            (STR_OPEN9X + OFS_VCSWFUNC)
#define STR_VFSWFUNC            (STR_OPEN9X + OFS_VFSWFUNC)
#define STR_VFSWRESET           (STR_OPEN9X + OFS_VFSWRESET)
#define STR_FUNCSOUNDS          (STR_OPEN9X + OFS_FUNCSOUNDS)

#define STR_VTELEMCHNS          (STR_OPEN9X + OFS_VTELEMCHNS)
#define STR_VTELEMUNIT			(STR_OPEN9X + OFS_VTELEMUNIT)

#if defined(FRSKY)
#define STR_VALARM              (STR_OPEN9X + OFS_VALARM)
#define STR_VALARMFN            (STR_OPEN9X + OFS_VALARMFN)
#define STR_VTELPROTO           (STR_OPEN9X + OFS_VTELPROTO)
#define STR_GPSFORMAT           (STR_OPEN9X + OFS_GPSFORMAT)
#define STR_AMPSRC              (STR_OPEN9X + OFS_AMPSRC)
#define STR_VARIOSRC            (STR_OPEN9X + OFS_VARIOSRC)
#define STR_VTELEMSCREENTYPE    (STR_OPEN9X + OFS_VSCREEN)
#define STR_TELEMCHNS           (STR_OPEN9X + OFS_TELEMCHNS)
#endif

#if defined(TEMPLATES)
#define STR_VTEMPLATES        (STR_OPEN9X + OFS_VTEMPLATES)
#endif

#if defined(HELI)
#define STR_VSWASHTYPE        (STR_OPEN9X + OFS_VSWASHTYPE)
#endif

#define STR_VKEYS               (STR_OPEN9X + OFS_VKEYS)
#define STR_VSWITCHES           (STR_OPEN9X + OFS_VSWITCHES)
#define STR_VSRCRAW             (STR_OPEN9X + OFS_VSRCRAW)
#define STR_VTMRMODES           (STR_OPEN9X + OFS_VTMRMODES)

#if defined(ROTARY_ENCODERS)
#define STR_VRENAVIG          (STR_OPEN9X + OFS_VRENAVIG)
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
#define STR_VRENCODERS        (STR_OPEN9X + OFS_VRENCODERS)
#endif

#define STR_DATETIME          (STR_OPEN9X + OFS_DATETIME)
#define STR_VPERSISTENT       (STR_OPEN9X + OFS_VPERSISTENT)

#if defined(X_ANY)
#define STR_SWITCHES_VALUES (STR_OPEN9X + OFS_SWITCHES_VALUES)
#define STR_XANY_EXPO       (STR_OPEN9X + OFS_XANY_EXPO)
#endif

// The 0-terminated-strings
#define NO_INDENT(x) (x)+LEN_INDENT

extern const pm_char STR_POPUPS[];

#if defined(OFS_EXIT)
#define STR_EXIT (STR_POPUPS + OFS_EXIT)
#else
extern const pm_char STR_EXIT[];
#endif

extern const pm_char STR_MENUWHENDONE[];
extern const pm_char STR_FREE[];
#define LEN_FREE PSIZE(TR_FREE)
extern const pm_char STR_DELETEMODEL[];
extern const pm_char STR_COPYINGMODEL[];
extern const pm_char STR_MOVINGMODEL[];
extern const pm_char STR_LOADINGMODEL[];
extern const pm_char STR_NAME[];
extern const pm_char STR_BITMAP[];
extern const pm_char STR_TIMER[];
extern const pm_char STR_ELIMITS[];
extern const pm_char STR_ETRIMS[];
extern const pm_char STR_TRIMINC[];
extern const pm_char STR_DISPLAY_TRIMS[];
extern const pm_char STR_TTRACE[];
extern const pm_char STR_TSWITCH[];
extern const pm_char STR_TTRIM[];
extern const pm_char STR_BEEPCTR[];
extern const pm_char STR_USE_GLOBAL_FUNCS[];
extern const pm_char STR_PROTO[];
extern const pm_char STR_PPMFRAME[];
extern const pm_char STR_MS[];
extern const pm_char STR_SWITCH[];
extern const pm_char STR_TRIMS[];
extern const pm_char STR_FADEIN[];
extern const pm_char STR_FADEOUT[];
extern const pm_char STR_DEFAULT[];
extern const pm_char STR_CHECKTRIMS[];
extern const pm_char STR_SWASHTYPE[];
extern const pm_char STR_COLLECTIVE[];
extern const pm_char STR_AILERON[];
extern const pm_char STR_ELEVATOR[];
extern const pm_char STR_SWASHRING[];
extern const pm_char STR_ELEDIRECTION[];
extern const pm_char STR_AILDIRECTION[];
extern const pm_char STR_COLDIRECTION[];
extern const pm_char STR_MODE[];
#if defined(AUDIO) && defined(BUZZER)
extern const pm_char STR_SPEAKER[];
extern const pm_char STR_BUZZER[];
#else
#define STR_SPEAKER STR_MODE
#define STR_BUZZER  STR_MODE
#endif
extern const pm_char STR_NOFREEEXPO[];
extern const pm_char STR_NOFREEMIXER[];
extern const pm_char STR_INSERTMIX[];
extern const pm_char STR_EDITMIX[];
extern const pm_char STR_SOURCE[];
extern const pm_char STR_WEIGHT[];
extern const pm_char STR_EXPO[];
extern const pm_char STR_SIDE[];
extern const pm_char STR_DIFFERENTIAL[];
extern const pm_char STR_OFFSET[];
extern const pm_char STR_TRIM[];
extern const pm_char STR_DREX[];
extern const pm_char STR_CURVE[];
extern const pm_char STR_FLMODE[];
extern const pm_char STR_MIXWARNING[];
extern const pm_char STR_OFF[];
extern const pm_char STR_MULTPX[];
extern const pm_char STR_DELAYDOWN[];
extern const pm_char STR_DELAYUP[];
extern const pm_char STR_SLOWDOWN[];
extern const pm_char STR_SLOWUP[];
extern const pm_char STR_MIXER[];
extern const pm_char STR_CV[];
extern const pm_char STR_GV[];
extern const pm_char STR_ACHANNEL[];
extern const pm_char STR_RANGE[];
extern const pm_char STR_CENTER[];
extern const pm_char STR_BAR[];
extern const pm_char STR_ALARM[];
extern const pm_char STR_USRDATA[];
extern const pm_char STR_BLADES[];
extern const pm_char STR_SCREEN[];
extern const pm_char STR_SOUND_LABEL[];
extern const pm_char STR_LENGTH[];
extern const pm_char STR_BEEP_LENGTH[];
#define STR_BEEP_LENGTH STR_LENGTH
extern const pm_char STR_SPKRPITCH[];
extern const pm_char STR_HAPTIC_LABEL[];
extern const pm_char STR_HAPTICSTRENGTH[];
extern const pm_char STR_CONTRAST[];
extern const pm_char STR_ALARMS_LABEL[];
#if defined(BATTGRAPH)
extern const pm_char STR_BATTERY_RANGE[];
#endif
extern const pm_char STR_BATTERYWARNING[];
extern const pm_char STR_INACTIVITYALARM[];
extern const pm_char STR_MEMORYWARNING[];
extern const pm_char STR_ALARMWARNING[];
extern const pm_char STR_RENAVIG[];
//extern const pm_char STR_THROTTLEREVERSE[];
extern const pm_char STR_TIMER_NAME[];
extern const pm_char STR_MINUTEBEEP[];
extern const pm_char STR_BEEPCOUNTDOWN[];
extern const pm_char STR_PERSISTENT[];
extern const pm_char STR_BACKLIGHT_LABEL[];
extern const pm_char STR_BLDELAY[];
#if defined(PWM_BACKLIGHT)
extern const pm_char STR_BLONBRIGHTNESS[];
extern const pm_char STR_BLOFFBRIGHTNESS[];
#endif
extern const pm_char STR_SPLASHSCREEN[];
extern const pm_char STR_THROTTLEWARNING[];
extern const pm_char STR_SWITCHWARNING[];
extern const pm_char STR_TIMEZONE[];
extern const pm_char STR_ADJUST_RTC[];
extern const pm_char STR_GPS[];
extern const pm_char STR_GPSCOORD[];
extern const pm_char STR_VARIO[];
extern const pm_char STR_PITCH_AT_ZERO[];
extern const pm_char STR_PITCH_AT_MAX[];
extern const pm_char STR_REPEAT_AT_ZERO[];
extern const pm_char STR_RXCHANNELORD[];
extern const pm_char STR_STICKS[];
extern const pm_char STR_POTS[];
extern const pm_char STR_SWITCHES_DELAY[];
extern const pm_char STR_SLAVE[];
extern const pm_char STR_MODESRC[];
extern const pm_char STR_MULTIPLIER[];
#define LEN_MULTIPLIER PSIZE(TR_MULTIPLIER)
extern const pm_char STR_CAL[];
extern const pm_char STR_VTRIM[];
extern const pm_char STR_BG[];
extern const pm_char STR_MENUTOSTART[];
extern const pm_char STR_SETMIDPOINT[];
extern const pm_char STR_MOVESTICKSPOTS[];
extern const pm_char STR_RXBATT[];
extern const pm_char STR_TX[];
#define STR_RX (STR_TX+OFS_RX)
extern const pm_char STR_ACCEL[];
extern const pm_char STR_NODATA[];
extern const pm_char STR_TOTTM1TM2THRTHP[];
extern const pm_char STR_US[];
extern const pm_char STR_MENUTORESET[];
extern const pm_char STR_PPM_TRAINER[];
extern const pm_char STR_CH[];
extern const pm_char STR_MODEL[];
extern const pm_char STR_FP[];
extern const pm_char STR_EEPROMLOWMEM[];
extern const pm_char STR_ALERT[];
extern const pm_char STR_PRESSANYKEYTOSKIP[];
extern const pm_char STR_THROTTLENOTIDLE[];
extern const pm_char STR_ALARMSDISABLED[];
extern const pm_char STR_PRESSANYKEY[];
#define LEN_PRESSANYKEY PSIZE(TR_PRESSANYKEY)
extern const pm_char STR_BADEEPROMDATA[];
extern const pm_char STR_EEPROMFORMATTING[];
extern const pm_char STR_EEPROMOVERFLOW[];
extern const pm_char STR_TRIMS2OFFSETS[];
extern const pm_char STR_MENURADIOSETUP[];
extern const pm_char STR_MENUDATEANDTIME[];
//extern const pm_char STR_FREERAMINB[];
extern const pm_char STR_MENUTRAINER[];
extern const pm_char STR_MENUGLOBALFUNCS[];
extern const pm_char STR_MENUVERSION[];
extern const pm_char STR_MENUDIAG[];
extern const pm_char STR_MENUANA[];
extern const pm_char STR_MENUCALIBRATION[];
extern const pm_char STR_MENUMODELSEL[];
extern const pm_char STR_MENUSETUP[];
extern const pm_char STR_MENUFLIGHTPHASE[];
extern const pm_char STR_MENUFLIGHTPHASES[];
extern const pm_char STR_MENUHELISETUP[];
extern const pm_char STR_MENUINPUTS[];
extern const pm_char STR_MENULIMITS[];
extern const pm_char STR_MENUCURVES[];
extern const pm_char STR_MENUCURVE[];
extern const pm_char STR_MENULOGICALSWITCH[];
extern const pm_char STR_MENULOGICALSWITCHES[];
extern const pm_char STR_MENUCUSTOMFUNC[];
extern const pm_char STR_MENUCUSTOMSCRIPTS[];
extern const pm_char STR_MENUCUSTOMSCRIPT[];
extern const pm_char STR_MENUTELEMETRY[];
extern const pm_char STR_MENUTEMPLATES[];
extern const pm_char STR_MENUSTAT[];
extern const pm_char STR_MENUDEBUG[];
extern const pm_char STR_MENUGLOBALVARS[];
extern const pm_char STR_INVERT_THR[];
extern const pm_char STR_AND_SWITCH[];
extern const pm_char STR_SF[];
extern const pm_char STR_GF[];

extern const pm_char STR_FAS_OFFSET[];

#if defined(SPIMODULES)
extern const pm_char STR_SPIM[];
extern const pm_char STR_RFPOWER[];
#endif

#if defined(MULTIMODULE)
extern const pm_char STR_MULTI_CUSTOM[];
#endif

#if defined(MULTIMODULE) || defined(SPIMODULES)
extern const pm_char STR_SUBTYPE[];
extern const pm_char STR_MULTI_VIDFREQ[];
extern const pm_char STR_RFTUNEFINE[];
//extern const pm_char STR_RFTUNECOARSE[];
extern const pm_char STR_TELEMETRY[];
extern const pm_char STR_MULTI_RFPOWER[];
extern const pm_char STR_MULTI_SERVOFREQ[];
extern const pm_char STR_MULTI_OPTION[];
extern const pm_char STR_MULTI_DSM_AUTODTECT[];
extern const pm_char STR_AUTOBIND[];
extern const pm_char STR_MULTI_LOWPOWER[];
#endif

#if defined(FRSKY)
extern const pm_char STR_LIMIT[];
extern const pm_char STR_MINRSSI[];
extern const pm_char STR_LATITUDE[];
extern const pm_char STR_LONGITUDE[];
#endif

extern const pm_char STR_SHUTDOWN[];
extern const pm_char STR_SAVEMODEL[];


extern const pm_char STR_BATT_CALIB[];

extern const pm_char STR_VOLTAGE[];

#if defined(FRSKY)

extern const pm_char STR_CURRENT[];
extern const pm_char STR_MODELISON[];
#endif

#define LEN_CALIB_FIELDS PSIZE(TR_BATT_CALIB)

#if defined(NAVIGATION_MENUS)
extern const pm_char STR_SELECT_MODEL[];
extern const pm_char STR_CREATE_MODEL[];
extern const pm_char STR_COPY_MODEL[];
extern const pm_char STR_MOVE_MODEL[];
extern const pm_char STR_DELETE_MODEL[];
extern const pm_char STR_EDIT[];
extern const pm_char STR_INSERT_BEFORE[];
extern const pm_char STR_INSERT_AFTER[];
extern const pm_char STR_COPY[];
extern const pm_char STR_MOVE[];
extern const pm_char STR_PASTE[];
extern const pm_char STR_INSERT[];
extern const pm_char STR_DELETE[];
extern const pm_char STR_RESET_FLIGHT[];
extern const pm_char STR_RESET_TIMER1[];
extern const pm_char STR_RESET_TIMER2[];
extern const pm_char STR_RESET_TIMER3[];
extern const pm_char STR_RESET_TELEMETRY[];
extern const pm_char STR_STATISTICS[];
extern const pm_char STR_SAVE_TIMERS[];
#endif

extern const pm_char STR_RESET_BTN[];

#if defined(SDCARD)
extern const pm_char STR_BACKUP_MODEL[];
extern const pm_char STR_RESTORE_MODEL[];
extern const pm_char STR_SDCARD_ERROR[];
extern const pm_char STR_NO_SDCARD[];
extern const pm_char STR_SDCARD_FULL[];
extern const pm_char STR_INCOMPATIBLE[];
extern const pm_char STR_LOGS_PATH[];
extern const pm_char STR_LOGS_EXT[];
extern const pm_char STR_MODELS_PATH[];
extern const pm_char STR_MODELS_EXT[];
#define STR_UPDATE_LIST STR_DELAYDOWN
#endif

extern const pm_char STR_WARNING[];
extern const pm_char STR_EEPROMWARN[];
extern const pm_char STR_EEPROM_CONVERTING[];
extern const pm_char STR_THROTTLEWARN[];
extern const pm_char STR_ALARMSWARN[];
extern const pm_char STR_SWITCHWARN[];
extern const pm_char STR_FAILSAFEWARN[];
extern const pm_char STR_NO_FAILSAFE[];
extern const pm_char STR_KEYSTUCK[];

extern const pm_char STR_SPEAKER_VOLUME[];
extern const pm_char STR_LCD[];
extern const pm_char STR_FUNC[];
extern const pm_char STR_V1[];
extern const pm_char STR_V2[];
extern const pm_char STR_DURATION[];
extern const pm_char STR_DELAY[];
extern const pm_char STR_SD_CARD[];
extern const pm_char STR_SDHC_CARD[];
extern const pm_char STR_NO_SOUNDS_ON_SD[];
extern const pm_char STR_NO_MODELS_ON_SD[];
extern const pm_char STR_NO_BITMAPS_ON_SD[];
extern const pm_char STR_NO_SCRIPTS_ON_SD[];
extern const pm_char STR_SCRIPT_SYNTAX_ERROR[];
extern const pm_char STR_SCRIPT_PANIC[];
extern const pm_char STR_SCRIPT_KILLED[];
extern const pm_char STR_SCRIPT_ERROR[];
extern const pm_char STR_PLAY_FILE[];
extern const pm_char STR_ASSIGN_BITMAP[];
extern const pm_char STR_EXECUTE_FILE[];
extern const pm_char STR_DELETE_FILE[];
extern const pm_char STR_COPY_FILE[];
extern const pm_char STR_RENAME_FILE[];
extern const pm_char STR_REMOVED[];
extern const pm_char STR_SD_INFO[];
extern const pm_char STR_SD_FORMAT[];
extern const pm_char STR_NA[];
extern const pm_char STR_HARDWARE[];
extern const pm_char STR_FORMATTING[];
extern const pm_char STR_TEMP_CALIB[];
extern const pm_char STR_TIME[];
extern const pm_char STR_BAUDRATE[];
extern const pm_char STR_SD_INFO_TITLE[];
extern const pm_char STR_SD_TYPE[];
extern const pm_char STR_SD_SPEED[];
extern const pm_char STR_YEAR[];
extern const pm_char STR_SD_SIZE[];
extern const pm_char STR_TYPE[];
extern const pm_char STR_GLOBAL_VARS[];
extern const pm_char STR_GLOBAL_V[];
extern const pm_char STR_GLOBAL_VAR[];
extern const pm_char STR_OWN[];
extern const pm_char STR_ROTARY_ENCODER[];
extern const pm_char STR_DATE[];
extern const pm_char STR_CHANNELS_MONITOR[];
extern const pm_char STR_MIXERS_MONITOR[];
extern const pm_char STR_PATH_TOO_LONG[];
extern const pm_char STR_VIEW_TEXT[];
extern const pm_char STR_FLASH_BOOTLOADER[];
extern const pm_char STR_FLASH_EXTERNAL_DEVICE[];
extern const pm_char STR_FLASH_INTERNAL_MODULE[];
extern const pm_char STR_WRITING[];
extern const pm_char STR_CONFIRM_FORMAT[];
extern const pm_char STR_EEBACKUP[];
extern const pm_char STR_DUMMY[];
extern const pm_char STR_POWEROFF[];
extern const pm_char STR_RFID[];
extern const pm_char STR_BIND[];


#if defined(VOICE)
PLAY_FUNCTION(playNumber, getvalue_t number, uint8_t unit, uint8_t att);
PLAY_FUNCTION(playDuration, int16_t seconds);
#define LANGUAGE_PACK_DECLARE(lng, name)
#define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#else
#define LANGUAGE_PACK_DECLARE(lng, name)
#define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#endif

#if MENUS_LOCK == 1
extern const pm_char STR_UNLOCKED[];
extern const pm_char STR_MODS_FORBIDDEN[];
#endif

#if defined(SPIMODULES)
extern const pm_char STR_mW[];
#endif

#if defined(DSM2_SERIAL) || defined(SPIMODULES) || defined(MULTIMODULE)
extern const pm_char STR_MODULE_RANGE[];
extern const pm_char STR_MODULE_BIND[];
extern const pm_char STR_RXNUM[];
extern const pm_char STR_DSM_PROTOCOLS[];
#endif

#define CHR_SHORT  TR_CHR_SHORT
#define CHR_LONG   TR_CHR_LONG
#define CHR_TOGGLE TR_CHR_TOGGLE
#define CHR_HOUR   TR_CHR_HOUR
#define CHR_INPUT  TR_CHR_INPUT

#if defined(X_ANY)
extern const pm_char STR_X_ANY[];
extern const pm_char STR_ACTIVED[];
extern const pm_char STR_NUMBER[];
extern const pm_char STR_CHANNEL[];
extern const pm_char STR_NB_REPEAT[];
extern const pm_char STR_ANGLE_SENSOR[];
extern const pm_char STR_PROP[];
#endif

// New Debug Menu
extern const pm_char STR_COMPUTE[];
extern const pm_char STR_MIN[];
extern const pm_char STR_MAX[];
extern const pm_char STR_PROTOCOL[];
extern const pm_char STR_GUIBUILD[];
extern const pm_char STR_LCDDRAW[];
extern const pm_char STR_MIXERlowcase[];
extern const pm_char STR_FREESRAM[];

#if defined(RTCLOCK)
extern const pm_char STR_TXTEMP[];
#endif

#endif /* TRANSLATIONS_H_ */