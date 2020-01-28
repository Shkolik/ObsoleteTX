/*
 * menu_general_version.cpp
 *
 * Created: 1/27/2020 7:57:30 PM
 *  Author: Andrew
 */ 

#include "../ObsoleteTX.h"
#include "menu_general.h"


void menuGeneralVersion(uint8_t event)
{
	#define VERS_ITEMS_COUNT 2

	MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, VERS_ITEMS_COUNT, {0, 3});

	uint8_t sub = menuVerticalPosition - 1;
	uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
	uint8_t attr = (sub == menuVerticalOffset ? blink : 0);

	lcdDrawTextLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

	#define ID_OFFSET 4 + 3*FW

	lcdDrawTextLeft(7*FH, STR_RFID);
	for (uint8_t j=0; j<4; j++) {
		uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
		switch (j) {
			case 0:
			lcd_outhex(2, ID_OFFSET + 0*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[3], rowattr);
			if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[3], 0, 0xFF);
			break;
			case 1:
			lcd_outhex(2, ID_OFFSET + 2*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[2], rowattr);
			if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[2], 0, 0xFF);
			break;
			case 2:
			lcd_outhex(2, ID_OFFSET + 4*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[1], rowattr);
			if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[1], 0, 0xFF);
			break;
			case 3:
			lcd_outhex(2, ID_OFFSET + 6*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[0], rowattr);
			if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[0], 0, 0xFF);
			break;
		}
	}
}
