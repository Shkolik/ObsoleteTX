/*
 * verion.h
 *
 * Created: 1/29/2020 10:23:21 PM
 *  Author: andrew.shkolik
 */ 


#ifndef VERION_H_
#define VERION_H_

#include "ObsoleteTX.h"
#define FW_VER             1


#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

#define EEPROM_STR DEFNUMSTR(EEPROM_VER);
#define VERS_STR DEFNUMSTR(FW_VER);

extern const pm_char vers_stamp[];

#endif /* VERION_H_ */