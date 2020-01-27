/*
 * main_avr.cpp
 *
 * Created: 1/26/2020 11:16:47 PM
 *  Author: Andrew
 */ 

#include "ObsoleteTX.h"


void perMain()
{

	//SIMU_PROCESSEVENTS;
	//LEDON();
	//uint16_t t0 = getTmr64uS();
	//int16_t delta = (nextMixerEndTime - lastMixerDuration) - t0;
	//if (delta > 0 && delta < US_TO_64US_TICK(MAX_MIXER_DELTA_US)) {
//
		//LEDOFF();
		//return;
	//}
//
	//nextMixerEndTime = t0 + US_TO_64US_TICK(MAX_MIXER_DELTA_US);
	//// this is a very tricky implementation; lastMixerEndTime is just like a default value not to stop mixcalculations totally;
	//// the real value for lastMixerEndTime is calculated inside protocol files
//
	//doMixerCalculations();
//
	////SIMU_PROCESSEVENTS;
//
	//t0 = getTmr64uS() - t0;
	//lastMixerDuration = t0;
	//if (t0 > maxMixerDuration)
	//maxMixerDuration = t0;
//
//
	//if (!eeprom_buffer_size) {
		//if (theFile.isWriting())
		//theFile.nextWriteStep();
		//else if (TIME_TO_WRITE())
		//eeCheck(false);
	//}
//
//
	////#if defined(SDCARD)
	////#if defined(FRSKY)
	////// ReusableBuffer is the log's buffer, just use it in main view or telemetry view
	////if (menuHandlers[menuLevel] == menuMainView || menuHandlers[menuLevel] == menuTelemetryFrsky)
	////#else
	////// ReusableBuffer is the log's buffer, just use it in main view
	////if (menuHandlers[menuLevel] == menuMainView)
	////#endif
	////{
		////writeLogs();
	////}
	////#endif
//
	//uint8_t evt = getEvent();
	//evt = checkTrim(evt);
//
	//#if defined(FRSKY) && defined(VARIO)
	//if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
		//varioWakeup();
	//}
	//#endif
//
	//#if defined(NAVIGATION_STICKS)
	//uint8_t sticks_evt = getSticksNavigationEvent();
	//if (sticks_evt)
	//evt = EVT_KEY_FIRST(sticks_evt);
	//#endif
//
	//if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys))
	//backlightOn(); // on key press turn the light on
	//checkBacklight();
//
	//#if defined(GUI)
	//const char *warn = warningText;
	//bool popupMenuActive = (popupMenuNoItems > 0);
//
	//lcdClear();
//
	//if (menuEvent) {
		//menuVerticalPosition = menuEvent == EVT_ENTRY_UP ? menuVerticalPositions[menuLevel] : 0;
		//menuHorizontalPosition = 0;
		//evt = menuEvent;
		//menuEvent = 0;
		//AUDIO_MENUS();
	//}
//
	//uint16_t tguibuid = getTmr64uS(); // gui build duration
	//menuHandlers[menuLevel]((warn || popupMenuActive) ? 0 : evt);
	//tguibuid = getTmr64uS() - tguibuid;
	//if (tguibuid > g_guibuild_max) g_guibuild_max = tguibuid;
	//if (tguibuid < g_guibuild_min) g_guibuild_min = tguibuid;
//
	//if (warn)
	//DISPLAY_WARNING(evt);
//
	//#if defined(NAVIGATION_MENUS)
	//if (popupMenuActive) {
		//const char * result = displayPopupMenu(evt);
		//if (result) {
			//popupMenuHandler(result);
			//putEvent(EVT_MENU_UP);
		//}
	//}
	//#endif
//
	//drawStatusLine();
//
	//uint16_t tlcddraw = getTmr64uS(); // lcd draw (fast) duration
	//lcdRefreshFast();
	//tlcddraw = getTmr64uS() - tlcddraw;
	//if (tlcddraw > g_lcddraw_max) g_lcddraw_max = tlcddraw;
	//if (tlcddraw < g_lcddraw_min) g_lcddraw_min = tlcddraw;
//
	//#endif // if defined(GUI)

}
