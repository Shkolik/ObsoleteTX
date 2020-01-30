/*
 * view.cpp
 *
 * Created: 1/29/2020 4:25:01 PM
 *  Author: andrew.shkolik
 */ 
#include "views.h"
//////////////////////////////////////////////////////////////////////////
///Main view
//////////////////////////////////////////////////////////////////////////
void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-5, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=5, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(x, LCD_H-8, len);
    }
  }
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];

#if defined(REV_EVO_V1)
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], -calibStickVert);
#else
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);
#endif

  calibStickVert = calibratedStick[CONVERT_MODE(2)];

#if defined(REV_EVO_V1)
  drawStick(RBOX_CENTERX, -calibratedStick[CONVERT_MODE(3)], calibStickVert);
#else
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);
#endif

  drawPotsBars();
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];
    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if defined(REV_EVO_V1)
    if (i==1 || i==3) {
      val = -val;
    }
#endif

    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }

    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    } else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    } else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }
    } else {
      ym = 60;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}

void displayTimers()
{
#if defined(TRANSLATIONS_CZ)
#define MAINTMR_LBL_COL (9*FW-FW/2-1)
#else
#define MAINTMR_LBL_COL (9*FW-FW/2+3)
#endif
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    LcdFlags att = DBLSIZE | (timerState.val<0 ? BLINK|INVERS : 0);
    putsTimer(12*FW+2+10*FWNUM-4, FH*2, timerState.val, att, att);
    uint8_t xLabel = (timerState.val >= 0 ? MAINTMR_LBL_COL : MAINTMR_LBL_COL-7);
    lcdPutsTimerMode(xLabel, FH*3, g_model.timers[0].mode);
  }
}

void displayVoltageOrAlarm()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, 0);
#if defined(SPIMODULES)
  if (!IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) || (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) && DOUBLE_BLINK_ON_PHASE))
    {
#endif
      lcdDrawFilledRect(VBATT_X-25, VBATT_Y+9, 21, 5);
      lcdDrawSolidVerticalLine(VBATT_X-4, VBATT_Y+10, 3);
      uint8_t count = GET_TXBATT_BARS();
      for (uint8_t i=0; i<count; i+=2)
        lcdDrawSolidVerticalLine(VBATT_X-24+i, VBATT_Y+10, 3);
      if (!IS_TXBATT_WARNING() || BLINK_ON_PHASE)
        lcdDrawFilledRect(VBATT_X-26, VBATT_Y, 24, 15);
#if defined(SPIMODULES)
    }
  else
    {
      lcdDrawFilledRect(VBATT_X-26, VBATT_Y, 24, 8);
      lcdDrawNumberNAtt(VBATT_X+1, VBATTUNIT_Y+1, RFPowerOut, CONDENSED|PREC2);
      lcdDrawText(VBATT_X+1,VBATTUNIT_Y+1,STR_mW);
      lcdDrawFilledRect(VBATT_X-26, VBATTUNIT_Y, 39, 9);
    }
#endif
#else
#if defined(SPIMODULES)
  if (!IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) || (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) && DOUBLE_BLINK_ON_PHASE))
    {
#endif
      LcdFlags att = (IS_TXBATT_WARNING() ? BLINK|INVERS : 0) | DBLSIZE;
      putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
      lcdDrawChar(VBATT_X, VBATTUNIT_Y, 'V');
#if defined(SPIMODULES)
    }
  else
    {
      lcdDrawNumberNAtt(VBATT_X, VBATTUNIT_Y, RFPowerOut, CONDENSED|PREC2);
      lcdDrawText(VBATT_X,VBATTUNIT_Y,STR_mW);
    }
#endif
#endif
}

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
  else if (result == STR_SAVE_TIMERS) {
    saveTimers();
    Close();
  }
#if TIMERS > 2
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
#endif
#if defined(FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    telemetryResetValue();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
}
#endif

void menuMainView(uint8_t event)
{
  if (warningResult)
    {
      // Power Off
      pwrCheck = false;
    }

  STICK_SCROLL_DISABLE();

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch(event) {

  case EVT_ENTRY:
    killEvents(KEY_EXIT);
    killEvents(KEY_UP);
    killEvents(KEY_DOWN);
    break;

  /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
  case EVT_KEY_BREAK(KEY_MENU):
    if (view_base == VIEW_TIMER2) {
      Timer2_running = !Timer2_running;
      AUDIO_KEYPAD_UP();
    }
  break;
  */

  case EVT_KEY_BREAK(KEY_RIGHT):
  case EVT_KEY_BREAK(KEY_LEFT):
    if (view_base < VIEW_TIMER2) {
      g_eeGeneral.view ^= ALTERNATE_VIEW;
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
    }
    break;

#if defined(NAVIGATION_MENUS)
  case EVT_KEY_CONTEXT_MENU:
    killEvents(event);


    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(FRSKY)
    POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
    POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
    POPUP_MENU_ADD_ITEM(STR_STATISTICS);
    POPUP_MENU_ADD_ITEM(STR_SAVE_TIMERS);
    popupMenuHandler = onMainViewMenu;
    break;
#endif

#if MENUS_LOCK != 2 /*no menus*/
  case EVT_KEY_LONG(KEY_MENU):// go to last menu
    pushMenu(lastPopMenu());
    killEvents(event);
    break;

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_MODEL_MENU:
    pushMenu(menuModelSelect);
    killEvents(event);
    break;

    CASE_EVT_ROTARY_LONG
  case EVT_KEY_GENERAL_MENU:
    pushMenu(menuGeneralSetup);
    killEvents(event);
    break;
#endif

  case EVT_KEY_BREAK(KEY_UP):
  case EVT_KEY_BREAK(KEY_DOWN):
    g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
    eeDirty(EE_GENERAL);
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_STATISTICS:
    chainMenu(menuStatisticsView);
    killEvents(event);
    break;

  case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
    if (!IS_FAI_ENABLED())
      chainMenu(menuTelemetryFrsky);
#else
    chainMenu(menuStatisticsDebug);
#endif
    killEvents(event);
    break;

  case EVT_KEY_FIRST(KEY_EXIT):

    if (view == VIEW_TIMER2) {
      timerReset(1);
    }
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_SHUTDOWN: // Shutdown confirmation
          POPUP_CONFIRMATION(STR_POWEROFF);
          killEvents(event);
    break;

#if !defined(NAVIGATION_MENUS)
  /*case EVT_KEY_LONG(KEY_EXIT):
    flightReset();
    AUDIO_KEYPAD_UP();
    break;*/
#endif

  }

  {
    // Flight Mode Name
    uint8_t mode = mixerCurrentFlightMode;
    lcdDrawSizedTextAtt(PHASE_X, PHASE_Y, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|PHASE_FLAGS);

    // Model Name
#if defined(DSM2_SERIAL) || defined(MULTIMODULE) || defined(SPIMODULES)
    if ((protoMode == BIND_MODE) && !BLINK_ON_PHASE)
    {
      lcdDrawTextAtt(MODELNAME_X, MODELNAME_Y, STR_BIND, BLINK|DBLSIZE);
    } else {
      putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, DBLSIZE);
    }
#else
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, DBLSIZE);
#endif

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timers
    displayTimers();

    // Trims sliders
    displayTrims(mode);

    DISPLAY_PROGRESS_BAR(20*FW+1);
  }

  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcdDrawSolidHorizontalLineStip(38, 34, 54, DOTTED);
    lcdDrawSolidHorizontalLine((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;

      int16_t val = channelOutputs[chan];

      switch(view_base) {
      case VIEW_OUTPUTS_VALUES:
        x0 = (i%4*9+3)*FW/2;
        y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
        lcdDrawNumberNAtt(x0+4*FW, y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumberNAtt(x0+4*FW, y0, calcRESXto1000(val), PREC1);
#else
        lcdDrawNumberNAtt(x0+4*FW, y0, calcRESXto1000(val)/10, 0);  // G: Don't like the decimal part*
#endif
        break;

      case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
        x0       = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
        y0       = 38+(i%4)*5;

        uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
        int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

        if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
        lcdDrawSolidHorizontalLineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
        lcdDrawSolidVerticalLine(x0,y0-2,5);
        if (val>0)
          x0+=1;
        else
          x0-=len;
        lcdDrawSolidHorizontalLine(x0,y0+1,len);
        lcdDrawSolidHorizontalLine(x0,y0-1,len);
        break;
      }
    }
  }
#if defined(GVARS) // Show current Gvars values
  else if (view_base == VIEW_GVARS) {
    uint8_t x0,y0;
    bool test;
#if defined(EXTERNALEEPROM)
    uint8_t offset = (g_eeGeneral.view & ALTERNATE_VIEW) ? MAX_GVARS/2 : 0;
#else
    uint8_t offset = 0;
#endif
    for (uint8_t i=0; i<6; i++) {
      x0 = (FW+FW/3)+(i%2)*2*FW*(LEN_GVAR_NAME-1)-(i%2)*FW/2;
      y0 = i/2*FH+33;
      test = false;
      for (uint8_t j=0; j<LEN_GVAR_NAME; j++) {
        if (g_model.gvars[i+offset].name[j]) {
          test = true;
          break;
        }
      }
      if (!test) lcdDrawStringWithIndex(x0+3*FW, y0, STR_GV, i+1+offset);
      else lcdDrawSizedTextAtt(x0, y0, g_model.gvars[i+offset].name, LEN_GVAR_NAME, ZCHAR|FIXEDWIDTH);
      x0 += (LEN_GVAR_NAME+3)*FW+FW/3;
      lcdDrawNumberNAtt(x0, y0, GVAR_VALUE(i+offset, getGVarFlightPhase(mixerCurrentFlightMode, i+offset)));
    }
  }
#endif
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i>=SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        lcdPutsSwitches(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
    } else {
#if defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len);
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len);
#endif
      }
#endif // ROTARY_ENCODERS

      // Logical Switches
      for (uint8_t i=0; i<15; i++) //Can't draw all NUM_LOGICAL_SWITCH
        lcdPutsSwitches(2*FW-3 + (i/3)*(i/3>2 ? 3*FW+2 : (3*FW-1)) + (i/3>2 ? 2*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
    }
  } else { // timer2
#if defined(TRANSLATIONS_CZ)
#define TMR2_LBL_COL (20-FW/2+1)
#else
#define TMR2_LBL_COL (20-FW/2+5)
#endif
    putsTimer(33+FW+2+10*FWNUM-4, FH*5, timersStates[1].val, DBLSIZE, DBLSIZE);
    lcdPutsTimerMode(timersStates[1].val >= 0 ? TMR2_LBL_COL : TMR2_LBL_COL-7, FH*6, g_model.timers[1].mode);
  }

  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcdDrawCharAtt(REBOOT_X, 0, '!', INVERS);
  }

}

//////////////////////////////////////////////////////////////////////////
///Stats
//////////////////////////////////////////////////////////////////////////
void menuStatisticsView(uint8_t event)
{
	TITLE(STR_MENUSTAT);

	switch(event) {
		case EVT_KEY_FIRST(KEY_UP):
		chainMenu(menuStatisticsDebug);
		break;

		case EVT_KEY_FIRST(KEY_EXIT):
		chainMenu(menuMainView);
		break;
	}

	lcdDrawText(  1*FW, FH*0, STR_TOTTM1TM2THRTHP);
	putsTimer(    5*FW+5*FWNUM+1, FH*1, timersStates[0].val, 0, 0);
	putsTimer(   12*FW+5*FWNUM+1, FH*1, timersStates[1].val, 0, 0);

	putsTimer(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, 0, 0);
	putsTimer(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, 0, 0);

	putsTimer(   12*FW+5*FWNUM+1, FH*0, sessionTimer, 0, 0);

	#if defined(THRTRACE)
	coord_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
	const coord_t x = 5;
	const coord_t y = 60;
	lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
	lcdDrawSolidVerticalLine(x, y-32, 32+3);

	for (coord_t i=0; i<MAXTRACE; i+=6) {
		lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
	}
	for (coord_t i=1; i<=MAXTRACE; i++) {
		lcdDrawSolidVerticalLine(x+i, y-s_traceBuf[traceRd], s_traceBuf[traceRd]);
		traceRd++;
		if (traceRd>=MAXTRACE) traceRd = 0;
		if (traceRd==s_traceWr) break;
	}
	#endif
}

void menuStatisticsDebug(uint8_t event)
{
	TITLE(STR_MENUDEBUG);

	switch(event) {
		case EVT_KEY_FIRST(KEY_ENTER):
		g_tmr1Latency_min = -1;
		g_tmr1Latency_max = 0;
		g_guibuild_min = -1;
		g_guibuild_max = 0;
		g_lcddraw_min = -1;
		g_lcddraw_max = 0;
		maxMixerDuration  = 0;
		AUDIO_KEYPAD_UP();
		break;

		case EVT_KEY_FIRST(KEY_DOWN):
		chainMenu(menuStatisticsView);
		break;
		case EVT_KEY_FIRST(KEY_EXIT):
		chainMenu(menuMainView);
		break;
	}

	#define COLDEBUG1 10*FW
	#define COLDEBUG2 15*FW
	#define COLDEBUG3 19*FW
	#define OFSDEBUG  3*FW

	lcdDrawTextLeft(1*FH, STR_COMPUTE);
	lcdDrawText(COLDEBUG1, 1*FH, STR_MAX);
	lcdDrawText(COLDEBUG2, 1*FH, STR_MIN);

	lcdDrawText(FW/2, 2*FH, STR_PROTOCOL);
	lcdDrawNumberNAtt(COLDEBUG1+OFSDEBUG, 2*FH, (g_tmr1Latency_max/2), UNSIGN);
	lcdDrawNumberNAtt(COLDEBUG2+OFSDEBUG, 2*FH, (g_tmr1Latency_min/2), UNSIGN);

	lcdDrawText(FW/2, 3*FH, STR_GUIBUILD);
	lcdDrawNumberNAtt(COLDEBUG1+OFSDEBUG, 3*FH, DURATION_MS_PREC2(g_guibuild_max), PREC2);
	lcdDrawNumberNAtt(COLDEBUG2+OFSDEBUG, 3*FH, DURATION_MS_PREC2(g_guibuild_min), PREC2);

	lcdDrawText(FW/2, 4*FH, STR_LCDDRAW);
	lcdDrawNumberNAtt(COLDEBUG1+OFSDEBUG, 4*FH, DURATION_MS_PREC2(g_lcddraw_max), PREC2);
	lcdDrawNumberNAtt(COLDEBUG2+OFSDEBUG, 4*FH, DURATION_MS_PREC2(g_lcddraw_min), PREC2);

	lcdDrawText(FW/2, 5*FH, STR_MIXERlowcase);
	lcdDrawNumberNAtt(COLDEBUG2+OFSDEBUG, 5*FH, DURATION_MS_PREC2(maxMixerDuration), PREC2);
	lcdDrawText(FW/2, 6*FH, STR_FREESRAM);
	lcdDrawNumberNAtt(COLDEBUG2+OFSDEBUG, 6*FH, stackAvailable(), UNSIGN);

	//  lcdDrawTextLeft(6*FH, STR_FREERAMINB);
	//  lcdDrawNumberNAtt(14*FW, 6*FH, freeRam(), UNSIGN);

	lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
	lcd_status_line();
}

//////////////////////////////////////////////////////////////////////////
///TELEMETRY
//////////////////////////////////////////////////////////////////////////
#ifdef TELEMETRY
	NOINLINE uint8_t getRssiAlarmValue()
	{
		return (45 + g_model.telemetry.rssiAlarm);
	}

	void displayVoltageScreenLine(uint8_t y, uint8_t index)
	{
		lcdDrawStringWithIndex(0, y, STR_A, index+1, 0);
		if (TELEMETRY_STREAMING()) {
			lcdPutsTelemetryChannelValue(3*FW+6*FW+4, y-FH, index+TELEM_A1-1, telemetryData.analog[index].value, DBLSIZE);
			lcdDrawChar(12*FW-1, y-FH, '<');
			lcdPutsTelemetryChannelValue(17*FW, y-FH, index+TELEM_A1-1, telemetryData.analog[index].min, NO_UNIT);
			lcdDrawChar(12*FW, y, '>');
			lcdPutsTelemetryChannelValue(17*FW, y, index+TELEM_A1-1, telemetryData.analog[index].max, NO_UNIT);
		}
	}

	uint8_t barCoord(int16_t value, int16_t min, int16_t max)
	{
		return limit<uint8_t>(0, (((int32_t)(BAR_WIDTH-1) * (value - min)) / (max - min)), BAR_WIDTH);
	}

	void displayVoltagesScreen()
	{
		// Volts / Amps / Watts / mAh
		uint8_t analog = 0;
		lcdDrawTextAtIndex(0, 2*FH, STR_AMPSRC, g_model.telemetry.voltsSource+1, 0);
		switch (g_model.telemetry.voltsSource) {
			case FRSKY_VOLTS_SOURCE_A1:
			case FRSKY_VOLTS_SOURCE_A2:
			displayVoltageScreenLine(2*FH, g_model.telemetry.voltsSource);
			analog = 1+g_model.telemetry.voltsSource;
			break;
			case FRSKY_VOLTS_SOURCE_FAS:
			lcdPutsTelemetryChannelValue(3*FW+6*FW+4, FH, TELEM_VFAS-1, telemetryData.value.vfas, DBLSIZE);
			break;
			case FRSKY_VOLTS_SOURCE_CELLS:
			lcdPutsTelemetryChannelValue(3*FW+6*FW+4, FH, TELEM_CELLS_SUM-1, telemetryData.value.cellsSum, DBLSIZE);
			break;
		}

		if (g_model.telemetry.currentSource) {
			lcdDrawTextAtIndex(0, 4*FH, STR_AMPSRC, g_model.telemetry.currentSource, 0);
			switch(g_model.telemetry.currentSource) {
				case FRSKY_CURRENT_SOURCE_A1:
				case FRSKY_CURRENT_SOURCE_A2:
				displayVoltageScreenLine(4*FH, g_model.telemetry.currentSource-1);
				break;
				case FRSKY_CURRENT_SOURCE_FAS:
				lcdPutsTelemetryChannelValue(3*FW+6*FW+4, 3*FH, TELEM_CURRENT-1, telemetryData.value.current, DBLSIZE);
				break;
			}

			lcdPutsTelemetryChannelValue(4, 5*FH, TELEM_POWER-1, telemetryData.value.power, LEFT|DBLSIZE);
			lcdPutsTelemetryChannelValue(3*FW+4+4*FW+6*FW+FW, 5*FH, TELEM_CONSUMPTION-1, telemetryData.value.currentConsumption, DBLSIZE);
			} else {
			displayVoltageScreenLine(analog > 0 ? 5*FH : 4*FH, analog ? 2-analog : 0);
			if (analog == 0) displayVoltageScreenLine(6*FH, 1);
		}

		// Cells voltage
		if (telemetryData.value.cellsCount > 0) {
			uint8_t y = 1*FH;
			for (uint8_t k=0; k<telemetryData.value.cellsCount && k<6; k++) {
				uint8_t attr = (barsThresholds[THLD_CELL] && telemetryData.value.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
				lcdDrawNumberNAtt(LCD_W, y, TELEMETRY_CELL_VOLTAGE(k), attr, 4);
				y += 1*FH;
			}
			lcdDrawSolidVerticalLine(LCD_W-3*FW-2, 8, 47);
		}

		displayRssiLine();
	}

	void displayAfterFlightScreen()
	{
		uint8_t line=1*FH+1;
		if (IS_GPS_AVAILABLE()) {
			// Latitude
			lcdDrawTextLeft(line, STR_LATITUDE);
			displayGpsCoord(line, telemetryData.value.gpsLatitudeNS, telemetryData.value.gpsLatitude_bp, telemetryData.value.gpsLatitude_ap);
			// Longitude
			line+=1*FH+1;
			lcdDrawTextLeft(line, STR_LONGITUDE);
			displayGpsCoord(line, telemetryData.value.gpsLongitudeEW, telemetryData.value.gpsLongitude_bp, telemetryData.value.gpsLongitude_ap);
			displayGpsTime();
			line+=1*FH+1;
			lcdDrawTextAtIndex(0, line, STR_VTELEMCHNS, TELEM_GPSALT, 0);
			lcdPutsTelemetryChannelValue(TELEM_2ND_COLUMN-2*FW, line, TELEM_GPSALT-1, telemetryData.value.gpsAltitude, 0);
			lcdDrawTextAtIndex(TELEM_2ND_COLUMN+FW, line, STR_VTELEMCHNS, TELEM_SPEED, 0);
			lcdPutsTelemetryChannelValue(LCD_W-3*FW, line, TELEM_SPEED-1, telemetryData.value.gpsSpeed_bp, 0);
			line+=1*FH+1;
			lcdDrawTextAtIndex(0, line, STR_VTELEMCHNS, TELEM_DIST, 0);
			lcdPutsTelemetryChannelValue(TELEM_2ND_COLUMN-2*FW, line, TELEM_DIST-1, telemetryData.value.gpsDistance, 0);
			lcdDrawTextAtIndex(TELEM_2ND_COLUMN+FW, line, STR_VTELEMCHNS, TELEM_HDG, 0);
			lcdPutsTelemetryChannelValue(LCD_W-3*FW, line, TELEM_HDG-1, telemetryData.value.gpsCourse_bp, 0);
			line+=1*FH+1;
		}
		// Rssi
		lcdDrawTextLeft(line, STR_MINRSSI);
		lcdDrawText(TELEM_2ND_COLUMN, line, STR_TX);
		lcdDrawNumberNAtt(TELEM_2ND_COLUMN+3*FW, line, telemetryData.rssi[1].min, LEFT|LEADING0, 2);
		lcdDrawText(TELEM_2ND_COLUMN+6*FW, line, STR_RX);
		lcdDrawNumberNAtt(TELEM_2ND_COLUMN+9*FW, line, telemetryData.rssi[0].min, LEFT|LEADING0, 2);
	}

	bool displayGaugesTelemetryScreen(telemetryScreenData & screen)
	{
		// Custom Screen with gauges
		uint8_t barHeight = 5;
		for (int8_t i=3; i>=0; i--) {
			FrSkyBarData & bar = screen.bars[i];
			source_t source = bar.source;
			getvalue_t barMin = convertBarTelemValue(source, bar.barMin);
			getvalue_t barMax = convertBarTelemValue(source, 255-bar.barMax);
			if (source && barMax > barMin) {
				uint8_t y = barHeight+6+i*(barHeight+6);
				lcdDrawTextAtIndex(0, y+barHeight-5, STR_VTELEMCHNS, source, 0);
				lcdDrawRect(BAR_LEFT, y, BAR_WIDTH+1, barHeight+2);
				getvalue_t value = getValue(MIXSRC_FIRST_TELEM+source-1);

				uint8_t thresholdX = 0;

				getvalue_t threshold = 0;
				if (source <= TELEM_TIMER_MAX)
				threshold = 0;
				else if (source <= TELEM_RSSI_RX)
				threshold = getRssiAlarmValue();
				else if (source <= TELEM_A2)
				threshold = 0;
				else {
					threshold = convertBarTelemValue(source, barsThresholds[source-TELEM_ALT]);
				}

				if (threshold) {
					thresholdX = barCoord(threshold, barMin, barMax);
					if (thresholdX == 100)
					thresholdX = 0;
				}

				uint8_t width = barCoord(value, barMin, barMax);

				// reversed barshade for T1/T2
				uint8_t barShade = ((threshold > value) ? DOTTED : SOLID);
				if (source == TELEM_T1 || source == TELEM_T2) {
					barShade = -barShade;
				}

				lcdDrawFilledRect(BAR_LEFT+1, y+1, width, barHeight, barShade);

				for (uint8_t j=24; j<99; j+=25) {
					if (j>thresholdX || j>width) {
						lcdDrawSolidVerticalLine(j*BAR_WIDTH/100+BAR_LEFT+1, y+1, barHeight);
					}
				}

				if (thresholdX) {
					lcdDrawSolidVerticalLineStip(BAR_LEFT+1+thresholdX, y-2, barHeight+3, DOTTED);
					lcdDrawSolidHorizontalLine(BAR_LEFT+thresholdX, y-2, 3);
				}
				} else {
				barHeight += 2;
			}
		}
		displayRssiLine();
		return barHeight < 13;
	}

	bool displayNumbersTelemetryScreen(telemetryScreenData & screen)
	{
		// Custom Screen with numbers
		uint8_t fields_count = 0;
		for (uint8_t i=0; i<4; i++) {
			for (uint8_t j=0; j<NUM_LINE_ITEMS; j++) {
				uint8_t field = screen.lines[i].sources[j];
				if (field > 0) {
					fields_count++;
				}
				if (i==3) {
					lcdDrawSolidVerticalLine(63, 8, 48);
					if (TELEMETRY_STREAMING()) {
						if (field == TELEM_ACC) {
							lcdDrawTextLeft(STATUS_BAR_Y, STR_ACCEL);
							lcdDrawNumberNAtt(4*FW, STATUS_BAR_Y, telemetryData.value.accelX, LEFT|PREC2);
							lcdDrawNumberNAtt(10*FW, STATUS_BAR_Y, telemetryData.value.accelY, LEFT|PREC2);
							lcdDrawNumberNAtt(16*FW, STATUS_BAR_Y, telemetryData.value.accelZ, LEFT|PREC2);
							break;
						}
						} else {
						displayRssiLine();
						return fields_count;
					}
				}
				if (field) {
					getvalue_t value = getValue(MIXSRC_FIRST_TELEM+field-1);
					uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
					coord_t pos[] = {0, 65, 130};
					lcdPutsTelemetryChannelValue(pos[j+1]-2, FH+2*FH*i, field-1, value, att);

					if (field >= TELEM_TIMER1 && field <= TELEM_TIMER_MAX && i!=3) {
						// there is not enough space on LCD for displaying "Tmr1" or "Tmr2" and still see the - sign, we write "T1" or "T2" instead
						field = field-TELEM_TIMER1+TELEM_T1;
					}

					lcdDrawTextAtIndex(pos[j], 1+FH+2*FH*i, STR_VTELEMCHNS, field, 0);
				}
			}
		}
		lcd_status_line();
		return fields_count;
	}

	bool displayCustomTelemetryScreen(uint8_t index)
	{
		telemetryScreenData & screen = g_model.telemetry.screens[index];

		if (IS_BARS_SCREEN(s_frsky_view)) {
			return displayGaugesTelemetryScreen(screen);
		}

		return displayNumbersTelemetryScreen(screen);
	}

	bool displayTelemetryScreen()
	{

		lcdDrawTelemetryTopBar();

		if (s_frsky_view < MAX_TELEMETRY_SCREENS) {
			return displayCustomTelemetryScreen(s_frsky_view);
		}

		if (s_frsky_view == TELEMETRY_VOLTAGES_SCREEN) {
			displayVoltagesScreen();
		}

		else {
			displayAfterFlightScreen();
		}

		return true;
	}

	void decrTelemetryScreen()
	{
		if (s_frsky_view-- == 0)
		s_frsky_view = TELEMETRY_VIEW_MAX;
	}
	void incrTelemetryScreen()
	{
		if (s_frsky_view++ == TELEMETRY_VIEW_MAX)
		s_frsky_view = 0;
	}

	void menuTelemetryFrsky(uint8_t event)
	{

		switch (event) {
			case EVT_KEY_FIRST(KEY_EXIT):
			killEvents(event);
			chainMenu(menuMainView);
			break;

			case EVT_KEY_FIRST(KEY_UP):
			decrTelemetryScreen();
			break;

			case EVT_KEY_FIRST(KEY_DOWN):
			incrTelemetryScreen();
			break;

			case EVT_KEY_FIRST(KEY_ENTER):
			telemetryResetValue();
			break;
		}

		if (!displayTelemetryScreen()) {
			putEvent(event == EVT_KEY_FIRST(KEY_UP) ? event : EVT_KEY_FIRST(KEY_DOWN));
		}
	}

bar_threshold_t barsThresholds[THLD_MAX];

uint8_t s_frsky_view = 0;



void displayRssiLine()
{
	if (TELEMETRY_STREAMING()) {
		lcdDrawSolidHorizontalLine(0, 55, 128, 0); // separator
		uint8_t rssi;
		rssi = min((uint8_t)99, telemetryData.rssi[1].value);
		lcdDrawTextLeft(STATUS_BAR_Y, STR_TX);
		lcdDrawNumberNAtt(4*FW+1, STATUS_BAR_Y, rssi, LEADING0, 2);
		lcdDrawRect(BAR_LEFT+1, 57, 38, 7);
		lcdDrawFilledRect(BAR_LEFT+1, 58, 4*rssi/11, 5, (rssi < getRssiAlarmValue()) ? DOTTED : SOLID);
		rssi = min((uint8_t)99, TELEMETRY_RSSI());
		lcdDrawText(104, STATUS_BAR_Y, STR_RX);
		lcdDrawNumberNAtt(105+4*FW, STATUS_BAR_Y, rssi, LEADING0, 2);
		lcdDrawRect(65, 57, 38, 7);
		uint8_t v = 4*rssi/11;
		lcdDrawFilledRect(66+36-v, 58, v, 5, (rssi < getRssiAlarmValue()) ? DOTTED : SOLID);
		} else {
		lcdDrawTextAtt(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
		lcd_status_line();
	}
}

#if defined(GPS)
void displayGpsTime()
{
	uint8_t att = (TELEMETRY_STREAMING() ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
	lcdDrawNumberNAtt(CENTER_OFS+6*FW+7, STATUS_BAR_Y, telemetryData.value.hour, att, 2);
	lcdDrawCharAtt(CENTER_OFS+8*FW+4, STATUS_BAR_Y, ':', att);
	lcdDrawNumberNAtt(CENTER_OFS+9*FW+2, STATUS_BAR_Y, telemetryData.value.min, att, 2);
	lcdDrawCharAtt(CENTER_OFS+11*FW-1, STATUS_BAR_Y, ':', att);
	lcdDrawNumberNAtt(CENTER_OFS+12*FW-3, STATUS_BAR_Y, telemetryData.value.sec, att, 2);
	lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, uint16_t bp, uint16_t ap)
{
	IF_GPS_IS_FIXED
	{
		if (!direction) direction = '-';
		lcdDrawNumberNAtt(TELEM_2ND_COLUMN, y, bp / 100, LEFT); // ddd before '.'
		lcdDrawChar(lcdLastPos, y, '@');
		uint8_t mn = bp % 100; // TODO div_t
		if (g_eeGeneral.gpsFormat == 0)
		{
			lcdDrawChar(lcdLastPos+FWNUM, y, direction);
			lcdDrawNumberNAtt(lcdLastPos+FW+FW+1, y, mn, LEFT|LEADING0, 2); // mm before '.'
			lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
			uint16_t ss = ap * 6;
			lcdDrawNumberNAtt(lcdLastPos+3, y, ss / 1000, LEFT|LEADING0, 2); // ''
			lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
			lcdDrawNumberNAtt(lcdLastPos+2, y, ss % 1000, LEFT|LEADING0, 3); // ''
			lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
			lcdDrawSolidVerticalLine(lcdLastPos+2, y, 2);
			} else {
			lcdDrawNumberNAtt(lcdLastPos+FW, y, mn, LEFT|LEADING0, 2); // mm before '.'
			lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
			lcdDrawNumberNAtt(lcdLastPos+2, y, ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
			lcdDrawChar(lcdLastPos+1, y, direction);
		}
	}
	else
	{
		// no fix
		lcdDrawText(TELEM_2ND_COLUMN, y, STR_VCSWFUNC+1/*----*/);
	}
}
#else
#define displayGpsTime()
#define displayGpsCoord(...)
#endif

#endif


//////////////////////////////////////////////////////////////////////////
///view_text
//////////////////////////////////////////////////////////////////////////

//#ifdef VOICE
//#define VOICE_PROMPT_TEXT_LEN  25
//
//FORCEINLINE uint8_t loadVoiceTextLine(uint8_t Numline, char * PromptText)
//{
	//closeLogIfActived();
	//uint8_t ret = false;
//
	//if (sdChangeCurDir(VOICETXT_PATH))
	//{
		//if (sdFindFileStruct(VOICETXT_FILE))
		//{
			//SD_file = fat_open_file(SD_filesystem, &SD_dir_entry);
			//int32_t seekofs = Numline*(VOICE_PROMPT_TEXT_LEN+2);
			//if(fat_seek_file(SD_file, &seekofs, FAT_SEEK_SET))
			//{
				//if (fat_read_file(SD_file, (uint8_t*)PromptText, VOICE_PROMPT_TEXT_LEN) == VOICE_PROMPT_TEXT_LEN)
				//{
					//PromptText[VOICE_PROMPT_TEXT_LEN] = '\0';
					//ret = true;
				//}
			//}
			//fat_close_file(SD_file);
		//}
	//}
	//return ret;
//}
//
//
//// Keep unused code ;-)
//
//#endif // VOICE

/*#define TEXT_FILE_MAXSIZE     2048

char s_text_file[TEXT_FILENAME_MAXLEN];
char s_text_screen[LCD_LINES-1][LCD_COLS+1];

void readTextFile(int & lines_count)
{
  FIL file;
  int result;
  char c;
  unsigned int sz;
  int line_length = 0;
  int escape = 0;
  char escape_chars[2];
  int current_line = 0;

  memset(s_text_screen, 0, sizeof(s_text_screen));

  result = f_open(&file, s_text_file, FA_OPEN_EXISTING | FA_READ);
  if (result == 0) {
    for (int i=0; i<TEXT_FILE_MAXSIZE && f_read(&file, &c, 1, &sz)==0 && sz==1 && (lines_count==0 || current_line-menuVerticalOffset<LCD_LINES-1); i++) {
      if (c == '\n') {
        ++current_line;
        line_length = 0;
        escape = 0;
      } else if (c!='\r' && current_line>=menuVerticalOffset && current_line-menuVerticalOffset<LCD_LINES-1 && line_length<LCD_COLS) {
        if (c=='\\' && escape==0) {
          escape = 1;
          continue;
        } else if (c!='\\' && escape>0 && escape<3) {
          escape_chars[escape-1] = c;
          if (escape == 2 && !strncmp(escape_chars, "up", 2)) {
            c = '\300';
            escape = 0;
          } else if (escape == 2 && !strncmp(escape_chars, "dn", 2)) {
            c = '\301';
            escape = 0;
          } else {
            escape++;
            continue;
          }
        } else if (c=='~') {
          c = 'z'+1;
        } else if (c=='\t') {
          c = 0x1D; //tab
        }
        escape = 0;
        s_text_screen[current_line-menuVerticalOffset][line_length++] = c;
      }
    }
    if (c != '\n') {
      current_line += 1;
    }
    f_close(&file);
  }

  if (lines_count == 0) {
    lines_count = current_line;
  }
}

void menuTextView(uint8_t event)
{
  static int lines_count;

  switch (event) {
  case EVT_ENTRY:
    menuVerticalOffset = 0;
    lines_count = 0;
    readTextFile(lines_count);
    break;

  case EVT_KEY_FIRST(KEY_UP):
    if (menuVerticalOffset == 0)
      break;
    else
      menuVerticalOffset--;
  // no break;

  case EVT_KEY_FIRST(KEY_DOWN):
    // if (event == EVT_KEY_BREAK(KEY_DOWN)) {
    if (menuVerticalOffset+LCD_LINES-1 >= lines_count)
      break;
    else
      ++menuVerticalOffset;
    // }
    readTextFile(lines_count);
    break;

  case EVT_KEY_BREAK(KEY_EXIT):
    popMenu();
    break;
  }

  for (int i=0; i<LCD_LINES-1; i++) {
    lcdDrawTextAtt(0, i*FH+FH+1, s_text_screen[i], FIXEDWIDTH);
  }

  char *title = s_text_file;
#if defined(SIMU)
  if (!strncmp(title, "./", 2)) title += 2;
#else
  // TODO?
#endif
  lcdDrawText(LCD_W/2-strlen(title)*FW/2, 0, title);
  lcdInvertLine(0);

  if (lines_count > LCD_LINES-1) {
    displayScrollbar(LCD_W-1, FH, LCD_H-FH, menuVerticalOffset, lines_count, LCD_LINES-1);
  }
}*/