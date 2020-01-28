/*
 * splash.cpp
 *
 * Created: 1/27/2020 8:01:24 PM
 *  Author: Andrew
 */ 


#include "../ObsoleteTX.h"

#if defined(SPLASH)

const pm_uchar zz_splashdata[] PROGMEM = {
	#if defined (LCDROT180)
	#include "../bitmaps/splash.lbmi"
	#else
	#include "../bitmaps/splash.lbm"
	#endif
};

void displaySplash()
{
	lcdClear();
	lcd_imgfar(0, 0, (pgm_get_far_address(zz_splashdata)), 0, 0); //use progmem "far" for splash working with all other options enabled

	#if MENUS_LOCK == 1
	if (readonly == false) {
		lcdDrawFilledRect((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 - 9, 50, (sizeof(TR_UNLOCKED)-1)*FW+16, 11, SOLID, ERASE|ROUND);
		lcdDrawText((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2, 53, STR_UNLOCKED);
	}
	#endif

	lcdRefresh();
}
#endif