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


#endif /* MENU_GENERAL_H_ */