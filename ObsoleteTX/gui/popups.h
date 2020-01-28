/*
 * popups.h
 *
 * Created: 1/27/2020 8:01:10 PM
 *  Author: Andrew
 */ 


#ifndef POPUPS_H_
#define POPUPS_H_


#include "menus.h"

const pm_char * warningText = NULL;
const pm_char * warningInfoText;
uint8_t         warningInfoLength;
uint8_t         warningType;
bool            warningResult = false;

const pm_uchar zz_asterisk_lbm[] PROGMEM = {
	#if defined (LCDROT180)
	#include "../bitmaps/asterisk.lbmi"
	#else
	#include "../bitmaps/asterisk.lbm"
	#endif
};

#if defined(NAVIGATION_MENUS)
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint8_t popupMenuNoItems = 0;
uint8_t popupMenuFlags = 0;
uint8_t popupMenuOffset = 0;
#endif // defined

#endif /* POPUPS_H_ */