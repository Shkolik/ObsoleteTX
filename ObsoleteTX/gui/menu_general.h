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

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralBluetooth(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralCalib(uint8_t event);


enum EnumTabDiag {
	e_Setup,
	CASE_SDCARD(e_Sd)
	CASE_BLUETOOTH(e_Bluetooth)
	e_Trainer,
	e_Vers,
	e_Keys,
	e_Ana,
	e_Calib
};

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
	menuGeneralSetup,
	CASE_SDCARD(menuGeneralSdManager)
	CASE_BLUETOOTH(menuGeneralBluetooth)
	menuGeneralTrainer,
	menuGeneralVersion,
	menuGeneralDiagKeys,
	menuGeneralDiagAna,
	menuGeneralCalib
};

const pm_uchar zz_sticks[] PROGMEM = {
	#if defined (LCDROT180)
	#include "../bitmaps/sticks.lbmi"
	#else
	#include "../bitmaps/sticks.lbm"
	#endif
};



#endif /* MENU_GENERAL_H_ */