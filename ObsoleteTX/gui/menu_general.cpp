/*
 * menu_general.cpp
 *
 * Created: 1/28/2020 7:21:59 PM
 *  Author: Andrew
 */ 

#include "menu_general.h"
#include "../verion.h"

const MenuFuncP_PROGMEM menuTabGeneral[7] PROGMEM = {
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

const pm_char STR_TR_ANAIN_POT_BAT[] PROGMEM = TR_ANAIN_POT_BAT;


//////////////////////////////////////////////////////////////////////////
///Radio setup
//////////////////////////////////////////////////////////////////////////
void menuGeneralSetup(uint8_t event)
{

#if defined(FAI_CHOICE)
  if (warningResult) {
    warningResult = false;
    g_eeGeneral.fai = true;
    eeDirty(EE_GENERAL);
  }
#endif

  MENU(STR_MENURADIOSETUP, menuTabGeneral, e_Setup, ITEM_SETUP_MAX+1, {0, CASE_RTCLOCK(3) CASE_RTCLOCK(2) CASE_RTCLOCK(0) CASE_BATTGRAPH(1) LABEL(SOUND), CASE_AUDIO(0) CASE_BUZZER(0) /*CASE_VOICE(0)*/ 0, CASE_AUDIO(0) CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) /*CASE_HAPTIC(0)*/ 0, LABEL(ALARMS), 0, 0, 0, 0, IF_ROTARY_ENCODERS(0) LABEL(BACKLIGHT), 0, 0, CASE_PWM_BACKLIGHT(0) CASE_PWM_BACKLIGHT(0) 0, CASE_SPLASH_PARAM(0) CASE_GPS(0) CASE_GPS(0) CASE_GPS(0) IF_FAI_CHOICE(0) 0, COL_TX_MODE, 1/*to force edit mode*/});

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
#if defined(BATTGRAPH)
    case ITEM_SETUP_BATT_RANGE:
      lcdDrawTextLeft(y, STR_BATTERY_RANGE);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y,  g_eeGeneral.vBatMin, (menuHorizontalPosition==0 ? attr : 0)|LEFT|NO_UNIT);
      lcdDrawChar(lcdLastPos, y, '-');
      lcdPutsVolts(lcdLastPos+FW, y, g_eeGeneral.vBatMax, (menuHorizontalPosition>0 ? attr : 0)|LEFT|NO_UNIT);
      if (attr && s_editMode>0) {
        if (menuHorizontalPosition==0)
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, 30, g_eeGeneral.vBatMax-10); // min=3.0V
        else
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin+10, 160); // max=16.0V
      }
      break;
#endif

    case ITEM_SETUP_SOUND_LABEL:
      lcdDrawTextLeft(y, STR_SOUND_LABEL);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_BEEP_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
      break;

#if defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.buzzerMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_BUZZER, STR_VBEEPMODE, g_eeGeneral.buzzerMode, -2, 1, attr, event);
      break;
#endif
#elif defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
      break;
#endif

#if defined(VOICE)
      /*case ITEM_SETUP_SPEAKER_VOLUME:
      {
        lcdDrawTextLeft(y, STR_SPEAKER_VOLUME);
        uint8_t b = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, b, VOLUME_LEVEL_MAX, attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, b, 0, VOLUME_LEVEL_MAX);
          if (checkIncDec_Ret) {
            g_eeGeneral.speakerVolume = (int8_t)b-VOLUME_LEVEL_DEF;
            setVolume(b);
          }
        }
        break;
      } Bracame not used but will be used ;-)*/
#endif


    case ITEM_SETUP_BEEP_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.beepLength, STR_BEEP_LENGTH, event, attr);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_SPEAKER_PITCH:
      lcdDrawTextLeft( y, STR_SPKRPITCH);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
      }
      break;
#endif


#if defined(HAPTIC)
    case ITEM_SETUP_HAPTIC_LABEL:
      lcdDrawTextLeft(y, STR_HAPTIC_LABEL);
      break;

    case ITEM_SETUP_HAPTIC_MODE:
      g_eeGeneral.hapticMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBEEPMODE, g_eeGeneral.hapticMode, -2, 1, attr, event);
      break;

    case ITEM_SETUP_HAPTIC_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.hapticLength, STR_LENGTH, event, attr);
      break;

      /*case ITEM_SETUP_HAPTIC_STRENGTH:
        SLIDER_5POS(y, g_eeGeneral.hapticStrength, STR_HAPTICSTRENGTH, event, attr);
        break; Bracame not used */
#endif

    case ITEM_SETUP_CONTRAST:
      lcdDrawTextLeft(y, STR_CONTRAST);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
        lcdSetContrast();
      }
      break;

    case ITEM_SETUP_ALARMS_LABEL:
      lcdDrawTextLeft(y, STR_ALARMS_LABEL);
      break;

    case ITEM_SETUP_BATTERY_WARNING:
      lcdDrawTextLeft(y, STR_BATTERYWARNING);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, attr|LEFT);
      if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, g_eeGeneral.vBatMin, g_eeGeneral.vBatMax); //Min- Max
      break;

    case ITEM_SETUP_MEMORY_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
      g_eeGeneral.disableMemoryWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_MEMORYWARNING, attr, event);
      break;
    }

    case ITEM_SETUP_ALARM_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
      g_eeGeneral.disableAlarmWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_ALARMWARNING, attr, event);
      break;
    }

    case ITEM_SETUP_INACTIVITY_ALARM:
      lcdDrawTextLeft( y,STR_INACTIVITYALARM);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 'm');
      if(attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
      break;

#if ROTARY_ENCODERS > 0
    case ITEM_SETUP_RE_NAVIGATION:
      g_eeGeneral.reNavigation = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_RENAVIG, STR_VRENAVIG, g_eeGeneral.reNavigation, 0, NUM_ROTARY_ENCODERS, attr, event);
      if (attr && checkIncDec_Ret) {
        g_rotenc[NAVIGATION_RE_IDX()] = 0;
      }
      break;
#endif

    case ITEM_SETUP_BACKLIGHT_LABEL:
      lcdDrawTextLeft(y, STR_BACKLIGHT_LABEL);
      break;

    case ITEM_SETUP_BACKLIGHT_MODE:
      g_eeGeneral.backlightMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBLMODE, g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on, attr, event);
      break;

    case ITEM_SETUP_FLASH_BEEP:
      g_eeGeneral.alarmsFlash = onoffMenuItem(g_eeGeneral.alarmsFlash, RADIO_SETUP_2ND_COLUMN, y, STR_ALARM, attr, event ) ;
      break;

    case ITEM_SETUP_BACKLIGHT_DELAY:
      lcdDrawTextLeft(y, STR_BLDELAY);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 's');
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
      break;


#if defined(PWM_BACKLIGHT)
    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF:
      lcdDrawTextLeft(y, STR_BLOFFBRIGHTNESS);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.blOffBright, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.blOffBright, 0, 15);
      break;

    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON:
      lcdDrawTextLeft(y, STR_BLONBRIGHTNESS);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.blOnBright, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.blOnBright, 0, 15);
      break;
#endif

#if defined(SPLASH) && !defined(FSPLASH)
    case ITEM_SETUP_DISABLE_SPLASH: {
      uint8_t b = 1-g_eeGeneral.splashMode;
      g_eeGeneral.splashMode = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_SPLASHSCREEN, attr, event);
      break;
    }
#endif

#if defined(FRSKY) && defined(GPS)
    case ITEM_SETUP_TIMEZONE:
      lcdDrawTextLeft(y, STR_TIMEZONE);
      lcdDrawNumberNAtt(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
      break;

    case ITEM_SETUP_ADJUST_RTC:
        g_eeGeneral.adjustRTC = onoffMenuItem(g_eeGeneral.adjustRTC, RADIO_SETUP_2ND_COLUMN, y, STR_ADJUST_RTC, attr, event);
        break;

    case ITEM_SETUP_GPSFORMAT:
      g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSCOORD, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
      break;
#endif


#if defined(FAI_CHOICE)
    case ITEM_SETUP_FAI:
      onoffMenuItem(g_eeGeneral.fai, RADIO_SETUP_2ND_COLUMN, y, PSTR("FAI Mode"), attr, event);
      if (attr && checkIncDec_Ret) {
        if (g_eeGeneral.fai)
          POPUP_WARNING(PSTR("FAI\001mode blocked!"));
        else
          POPUP_CONFIRMATION(PSTR("FAI mode?"));
      }
      break;
#endif

    case ITEM_SETUP_RX_CHANNEL_ORD:
      lcdDrawTextLeft(y, STR_RXCHANNELORD); // RAET->AETR
      for (uint8_t i=1; i<=4; i++) {
        lcdPutsChnLetter(RADIO_SETUP_2ND_COLUMN - FW + i*FW, y, channel_order(i), attr);
      }
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
      break;

    case ITEM_SETUP_STICK_MODE_LABELS:
      lcdDrawTextLeft(y, NO_INDENT(STR_MODE));
      for (uint8_t i=0; i<4; i++) {
        lcd_imgfar((6+4*i)*FW, y, (pgm_get_far_address(zz_sticks)), i, 0);
      }
      break;

    case ITEM_SETUP_STICK_MODE:
      lcdDrawCharAtt(2*FW, y, '1'+g_eeGeneral.stickMode, attr);
      for (uint8_t i=0; i<4; i++) {
        putsMixerSource((6+4*i)*FW, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), 0);
      }
      if (attr && s_editMode>0) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickMode, 0, 3);
      } else if (stickMode != g_eeGeneral.stickMode) {
        sendStopPulses();
        stickMode = g_eeGeneral.stickMode;
        checkTHR();
        startPulses(PROTOCMD_INIT);
        clearKeyEvents();
      }
#if defined(ROTARY_ENCODER_NAVIGATION)
      MOVE_CURSOR_FROM_HERE();
#endif
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////
///Analogs
//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
///Keys
//////////////////////////////////////////////////////////////////////////
void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
	uint8_t t = switchState(key);
	lcdDrawCharAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
	SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

	lcdDrawText(14*FW, MENU_HEADER_HEIGHT+2*FH, STR_VTRIM);

	for(uint8_t i=0; i<9; i++) {
		coord_t y;

		if (i<8) {
			y = MENU_HEADER_HEIGHT + FH*3 + FH*(i/2);
			if (i&1) lcd_imgfar(14*FW, y, (pgm_get_far_address(zz_sticks)), i/2, 0);
			displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
		}

		if (i<6) {
			y = i*FH+MENU_HEADER_HEIGHT+FH;
			lcdDrawTextAtIndex(0, y, STR_VKEYS, (5-i), 0);
			displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+(5-i)));
		}

		if (i != SW_ID0-SW_BASE) {
			y = MENU_HEADER_HEIGHT+i*FH-2*FH;
			lcdPutsSwitches(8*FW, y, i+1, 0); //ohne off,on
			displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
		}
	}

	#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
	for(uint8_t i=0; i<DIM(g_rotenc); i++) {
		coord_t y = MENU_HEADER_HEIGHT /* ??? + 1 ??? */ + i*FH;
		lcdDrawTextAtIndex(14*FW, y, STR_VRENCODERS, i, 0);
		int16_t rex = getRotaryEncoder(i);
		rex /= 8;
		lcdDrawNumberNAtt(18*FW, y, rex, LEFT|(switchState((EnumKeys)(BTN_REa+i)) ? INVERS : 0));
	}
	#endif

}

//////////////////////////////////////////////////////////////////////////
///Calibration
//////////////////////////////////////////////////////////////////////////
void menuCommonCalib(uint8_t event)
{
	for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
		int16_t vt = analogIn(i);
		reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
		reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
		if (i >= POT1 && i <= POT_LAST) {
			if (IS_POT_WITHOUT_DETENT(i)) {
				reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
			}
		}
	}

	calibrationState = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

	switch (event) {
		case EVT_ENTRY:
		reusableBuffer.calib.state = 0;
		break;

		case EVT_KEY_BREAK(KEY_ENTER):
		reusableBuffer.calib.state++;
		break;
	}

	switch (reusableBuffer.calib.state) {
		case 0:
		// START CALIBRATION
		if (!READ_ONLY()) {
			lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUTOSTART);
		}
		break;

		case 1:
		// SET MIDPOINT
		lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
		lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

		for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
			reusableBuffer.calib.loVals[i] = 15000;
			reusableBuffer.calib.hiVals[i] = -15000;
			reusableBuffer.calib.midVals[i] = analogIn(i);
		}
		break;

		case 2:
		// MOVE STICKS/POTS
		STICK_SCROLL_DISABLE();
		lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
		lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

		for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
			if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i]) > 50) {
				g_eeGeneral.calib[i].mid = reusableBuffer.calib.midVals[i];
				int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
				g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
				v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
				g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
			}
		}
		break;

		case 3:
		g_eeGeneral.chkSum = evalChkSum();
		eeDirty(EE_GENERAL);
		reusableBuffer.calib.state = 4;
		break;

		default:
		reusableBuffer.calib.state = 0;
		break;
	}

	doMainScreenGraphics();
}

void menuGeneralCalib(uint8_t event)
{
	check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

	if (menuEvent) {
		calibrationState = 0;
	}

	TITLE(STR_MENUCALIBRATION);
	menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
	if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
		calibrationState = 0;
		chainMenu(menuMainView);
		} else {
		lcdDrawTextCenter(0*FH, MENUCALIBRATION);
		lcdInvertLine(0);
		menuCommonCalib(event);
	}
}

//////////////////////////////////////////////////////////////////////////
///Version
//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
///Trainer
//////////////////////////////////////////////////////////////////////////
void menuGeneralTrainer(uint8_t event)
{
	uint8_t slave = IS_WAIT_PUPIL_STATE();

	MENU(STR_MENUTRAINER, menuTabGeneral, e_Trainer, (slave ? 1 : 7), {0, 2, 2, 2, 2, 0/*, 0*/});

	if (slave) {
		lcdDrawText(7*FW, 4*FH, STR_SLAVE);
		} else {
		uint8_t attr;
		uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

		lcdDrawText(3*FW, MENU_HEADER_HEIGHT+1, STR_MODESRC);

		uint8_t y = MENU_HEADER_HEIGHT + 1 + FH;

		for (uint8_t i=1; i<=NUM_STICKS; i++) {
			uint8_t chan = channel_order(i);
			volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

			putsMixerSource(0, y, MIXSRC_Rud-1+chan, (menuVerticalPosition==i && CURSOR_ON_LINE()) ? INVERS : 0);

			for (uint8_t j=0; j<3; j++) {

				attr = ((menuVerticalPosition==i && menuHorizontalPosition==j) ? blink : 0);

				switch(j) {
					case 0:
					lcdDrawTextAtIndex(4*FW, y, STR_TRNMODE, td->mode, attr);
					if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
					break;

					case 1:
					lcdDrawNumberNAtt(11*FW, y, td->studWeight, attr);
					if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -125, 125);
					break;

					case 2:
					lcdDrawTextAtIndex(12*FW, y, STR_TRNCHN, td->srcChn, attr);
					if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);
					break;
				}
			}
			y += FH;
		}

		attr = (menuVerticalPosition==5) ? blink : 0;
		lcdDrawTextLeft(MENU_HEADER_HEIGHT+1+5*FH, STR_MULTIPLIER);
		lcdDrawNumberNAtt(LEN_MULTIPLIER*FW+3*FW, MENU_HEADER_HEIGHT+1+5*FH, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
		if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

		attr = (menuVerticalPosition==6) ? INVERS : 0;
		if (attr) s_editMode = 0;
		lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+1+6*FH, STR_CAL, attr);
		for (uint8_t i=0; i<4; i++) {
			uint8_t x = (i*TRAINER_CALIB_POS+16)*FW/2;
			#if defined (PPM_UNIT_PERCENT_PREC1)
			lcdDrawNumberNAtt(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
			#else
			lcdDrawNumberNAtt(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])/5, 0);
			#endif
		}

		if (attr) {
			if (event==EVT_KEY_LONG(KEY_ENTER)) {
				memcpy(g_eeGeneral.trainer.calib, ppmInput, sizeof(g_eeGeneral.trainer.calib));
				eeDirty(EE_GENERAL);
				AUDIO_WARNING1();
			}
		}
	}
}
