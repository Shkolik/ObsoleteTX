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

extern void menuGeneralSetup(uint8_t event);
extern void menuGeneralSdManager(uint8_t event);
extern void menuGeneralBluetooth(uint8_t event);
extern void menuGeneralTrainer(uint8_t event);
extern void menuGeneralVersion(uint8_t event);
extern void menuGeneralDiagKeys(uint8_t event);
extern void menuGeneralDiagAna(uint8_t event);
extern void menuGeneralCalib(uint8_t event);


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

extern const pm_uchar zz_sticks[];

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
	menuGeneralSetup,
	#ifdef SDCARD
	menuGeneralSdManager,
	#endif
	menuGeneralTrainer,
	menuGeneralVersion,
	menuGeneralDiagKeys,
	menuGeneralDiagAna,
	menuGeneralCalib
};

const pm_uchar zz_sticks[] PROGMEM = {
	#ifdef LCDROT180
	#include "../bitmaps/sticks.lbmi"
	#else
	#include "../bitmaps/sticks.lbm"
	#endif
};
#endif /* MENU_GENERAL_H_ */