/*
 * menu_general.h
 *
 * Created: 1/27/2020 7:56:00 PM
 *  Author: Andrew
 */ 


#ifndef MENU_GENERAL_H_
#define MENU_GENERAL_H_

#include "../ObsoleteTX.h"
#include "menus.h"
#include "gui.h"

//extern void menuGeneralSetup(uint8_t event);
//extern void menuGeneralSdManager(uint8_t event);
//extern void menuGeneralBluetooth(uint8_t event);
//extern void menuGeneralTrainer(uint8_t event);
//extern void menuGeneralVersion(uint8_t event);
//extern void menuGeneralDiagKeys(uint8_t event);
//extern void menuGeneralDiagAna(uint8_t event);
//extern void menuGeneralCalib(uint8_t event);

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */
#define TR_ANAIN_POT_BAT "Pt1""Pt2""Pt3""Bat"

enum EnumTabDiag {
	e_Setup,
	#ifdef SDCARD
	e_Sd,
	#endif
	e_Trainer,
	e_Vers,
	e_Keys,
	e_Ana,
	e_Calib
};

#if defined(BATTGRAPH)
#define CASE_BATTGRAPH(x) x,
#else
#define CASE_BATTGRAPH(x)
#endif

#define RADIO_SETUP_2ND_COLUMN  (LCD_W-6*FW-3-MENUS_SCROLLBAR_WIDTH)
#define RADIO_SETUP_TIME_COLUMN (FW*15+9)
#define RADIO_SETUP_DATE_COLUMN (FW*15+7)

#define SLIDER_5POS(y, value, label, event, attr) { \
	int8_t tmp = value; \
	displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
	value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
}

#if defined(SPLASH) && !defined(FSPLASH)
#define CASE_SPLASH_PARAM(x) x,
#else
#define CASE_SPLASH_PARAM(x)
#endif

#define COL_TX_MODE LABEL(TX_MODE)

enum menuGeneralSetupItems {
	CASE_RTCLOCK(ITEM_SETUP_DATE)
	CASE_RTCLOCK(ITEM_SETUP_TIME)
	CASE_RTCLOCK(ITEM_TEMP)
	CASE_BATTGRAPH(ITEM_SETUP_BATT_RANGE)
	ITEM_SETUP_SOUND_LABEL,
	CASE_AUDIO(ITEM_SETUP_BEEP_MODE)
	CASE_BUZZER(ITEM_SETUP_BUZZER_MODE)
	//CASE_VOICE(ITEM_SETUP_SPEAKER_VOLUME)
	ITEM_SETUP_BEEP_LENGTH,
	CASE_AUDIO(ITEM_SETUP_SPEAKER_PITCH)
	CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
	CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
	CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
	//CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
	ITEM_SETUP_CONTRAST,
	ITEM_SETUP_ALARMS_LABEL,
	ITEM_SETUP_BATTERY_WARNING,
	ITEM_SETUP_INACTIVITY_ALARM,
	ITEM_SETUP_MEMORY_WARNING,
	ITEM_SETUP_ALARM_WARNING,
	IF_ROTARY_ENCODERS(ITEM_SETUP_RE_NAVIGATION)
	ITEM_SETUP_BACKLIGHT_LABEL,
	ITEM_SETUP_BACKLIGHT_MODE,
	ITEM_SETUP_BACKLIGHT_DELAY,
	CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF)
	CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON)
	ITEM_SETUP_FLASH_BEEP,
	CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
	CASE_GPS(ITEM_SETUP_TIMEZONE)
	CASE_GPS(ITEM_SETUP_ADJUST_RTC)
	CASE_GPS(ITEM_SETUP_GPSFORMAT)
	IF_FAI_CHOICE(ITEM_SETUP_FAI)
	ITEM_SETUP_RX_CHANNEL_ORD,
	ITEM_SETUP_STICK_MODE_LABELS,
	ITEM_SETUP_STICK_MODE,
	ITEM_SETUP_MAX
};

#define TRAINER_CALIB_POS 8

#endif /* MENU_GENERAL_H_ */