/*
 * menu_general_diaganas.cpp
 *
 * Created: 1/27/2020 7:56:40 PM
 *  Author: Andrew
 */ 


#include "menu_general.h"
#include "gui.h"




void menuGeneralDiagAna(uint8_t event)
{
	#define ANAS_ITEMS_COUNT 2

	SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

	STICK_SCROLL_DISABLE();

	uint8_t numLoop = NUM_STICKS+NUM_POTS;

	for (uint8_t i=0; i<numLoop+1; i++) { // Add battery input
		coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
		uint8_t x = i&1 ? 65 : 2;
		uint8_t attr = (analogIn(i) < 1 || analogIn(i) > 0x7FD) ? INVERS : 0;
		if (i<4) {
			putsMixerSource(x, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), attr);
			} else {
			lcdDrawSizedTextAtt(x, y, STR_TR_ANAIN_POT_BAT+3*(i-4), 3, attr);
		}
		lcdDrawChar(x+18, y, ':');
		lcd_outhex(4, x+3*FW+4, y, analogIn(i), attr);
		if (i<numLoop)
		lcdDrawNumberNAtt(x+10*FW+2, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
	}
	lcdDrawTextLeft(6*FH-2, STR_BATT_CALIB);
	lcdPutsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat10mV, (menuVerticalPosition==1 ? INVERS : 0)|PREC2);
	if (menuVerticalPosition==1)
	CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);

}
