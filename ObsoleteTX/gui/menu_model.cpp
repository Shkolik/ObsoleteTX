/*
 * menu_model.cpp
 *
 * Created: 1/27/2020 7:57:49 PM
 *  Author: Andrew
 */ 
#include "menu_model.h"

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
	menuModelSelect,
	menuModelSetup,
	#ifdef HELI
	menuModelHeli,
	#endif
	#ifdef FLIGHT_MODES
	menuModelFlightModesAll,
	#endif
	menuModelExposAll,
	menuModelMixAll,
	menuModelLimits,
	#ifdef CURVES
	menuModelCurvesAll,
	#endif
	menuModelLogicalSwitches,
	menuModelCustomFunctions,
	#ifdef FRSKY
	menuModelTelemetry,
	#endif
	#ifdef TEMPLATES
	menuModelTemplates,
	#endif
};

uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_currIdx;

uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
	lcdDrawTextLeft(y, str);
	lcdDrawNumberNAtt(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
	if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
	return delay;
}

//////////////////////////////////////////////////////////////////////////
///Model select
//////////////////////////////////////////////////////////////////////////
#if defined(NAVIGATION_MENUS)
void onModelSelectMenu(const char *result)
{
	int8_t sub = menuVerticalPosition;

	if (result == STR_SELECT_MODEL || result == STR_CREATE_MODEL) {
		selectModel(sub);
		popMenu();
		} else if (result == STR_COPY_MODEL) {
		s_copyMode = COPY_MODE;
		s_copyTgtOfs = 0;
		s_copySrcRow = -1;
		} else if (result == STR_MOVE_MODEL) {
		s_copyMode = MOVE_MODE;
		s_copyTgtOfs = 0;
		s_copySrcRow = -1;
	}
	#if defined(SDCARD)
	else if (result == STR_BACKUP_MODEL) {
		eeCheck(true); // force writing of current model data before this is changed
		if (!eeBackupModel(sub)) POPUP_WARNING(STR_SDCARD_ERROR);
		} else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
		if (!listSdFiles(MODELS_PATH, MODELS_EXT, MENU_LINE_LENGTH-1, NULL, 0)) {
			POPUP_WARNING(STR_NO_MODELS_ON_SD);
			popupMenuFlags = 0;
		}
	}
	#endif
	else if (result == STR_DELETE_MODEL) {
		POPUP_CONFIRMATION(STR_DELETEMODEL);
		char * name = reusableBuffer.modelsel.mainname;
		eeLoadModelName(sub, name);
		SET_WARNING_INFO(name, sizeof(g_model.name), ZCHAR);
	}
	#if defined(SDCARD)
	else {
		// The user choosed a file on SD to restore
		POPUP_WARNING(eeRestoreModel(sub, (char *)result));
		fat_close_file(SD_file);
		if (!warningText && g_eeGeneral.currModel == sub) {
			eeLoadModel(sub);
		}
	}
	#endif
}
#endif

void menuModelSelect(uint8_t event)
{
	if (warningResult) {
		warningResult = false;
		eeDeleteModel(menuVerticalPosition); // delete file
		s_copyMode = 0;
		event = EVT_ENTRY_UP;
	}

	uint8_t _event_ = (IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event) ? 0 : event);

	if ((s_copyMode && EVT_KEY_MASK(event) == KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
		_event_ -= KEY_EXIT;
	}

	int8_t oldSub = menuVerticalPosition;

	check_submenu_simple(_event_, MAX_MODELS-1);

	#if defined(NAVIGATION_POT2)
	if (event==0 && p2valdiff<0) {
		event = EVT_KEY_FIRST(KEY_RIGHT);
	}
	#endif

	if (s_editMode > 0) s_editMode = 0;

	if (event) {
		eeFlush(); // flush eeprom write
	}

	int8_t sub = menuVerticalPosition;

	switch (event) {
		case EVT_ENTRY:
		menuVerticalPosition = sub = g_eeGeneral.currModel;
		if (sub >= LCD_LINES-1) menuVerticalOffset = sub-LCD_LINES+2;
		s_copyMode = 0;
		s_editMode = EDIT_MODE_INIT;
		eeCheck(true);
		break;

		case EVT_KEY_LONG(KEY_EXIT):
		killEvents(event);
		if (s_copyMode && s_copyTgtOfs == 0 && g_eeGeneral.currModel != sub && eeModelExists(sub)) {
			POPUP_CONFIRMATION(STR_DELETEMODEL);
			char * name = reusableBuffer.modelsel.mainname;
			eeLoadModelName(sub, name);
			SET_WARNING_INFO(name, sizeof(g_model.name), ZCHAR);
			} else {
			s_copyMode = 0;
			menuVerticalPosition = g_eeGeneral.currModel;
		}
		break;

		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_LONG:
		killEvents(event);
		if (s_editMode < 0) {
			popMenu();
			break;
			} else if (!s_copyMode) {
			menuVerticalPosition = sub = g_eeGeneral.currModel;
			s_copyMode = 0;
			s_editMode = EDIT_MODE_INIT;
		}
		// no break
		#endif

		case EVT_KEY_BREAK(KEY_EXIT):
		if (s_copyMode) {
			sub = menuVerticalPosition = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
			s_copyMode = 0;
			} else if (uint8_t(menuVerticalPosition) != g_eeGeneral.currModel) {
			menuVerticalPosition = g_eeGeneral.currModel;
			} else {
			popMenu();
		}
		break;

		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_BREAK:
		if (s_editMode == -1) {
			s_editMode = 0;
			break;
		}
		// no break;
		#endif

		case EVT_KEY_LONG(KEY_ENTER):
		case EVT_KEY_BREAK(KEY_ENTER):
		s_editMode = 0;
		if (READ_ONLY()) {
			if (g_eeGeneral.currModel != sub && eeModelExists(sub)) {
			}
			} else if (s_copyMode && (s_copyTgtOfs || s_copySrcRow>=0)) {
			displayPopup(s_copyMode==COPY_MODE ? STR_COPYINGMODEL : STR_MOVINGMODEL);
			eeCheck(true); // force writing of current model data before this is changed

			uint8_t cur = (MAX_MODELS + sub + s_copyTgtOfs) % MAX_MODELS;

			if (s_copyMode == COPY_MODE) {
				if (!eeCopyModel(cur, s_copySrcRow)) {
					cur = sub;
				}
			}

			s_copySrcRow = g_eeGeneral.currModel; // to update the currModel value
			while (sub != cur) {
				uint8_t src = cur;
				cur = (s_copyTgtOfs > 0 ? cur+MAX_MODELS-1 : cur+1) % MAX_MODELS;
				eeSwapModels(src, cur);
				if (src == s_copySrcRow)
				s_copySrcRow = cur;
				else if (cur == s_copySrcRow)
				s_copySrcRow = src;
			}

			if (s_copySrcRow != g_eeGeneral.currModel) {
				g_eeGeneral.currModel = s_copySrcRow;
				eeDirty(EE_GENERAL);
			}

			s_copyMode = 0;
			event = EVT_ENTRY_UP;
			} else if (event == EVT_KEY_LONG(KEY_ENTER) || IS_ROTARY_BREAK(event)) {
			s_copyMode = 0;
			killEvents(event);
			#if defined(NAVIGATION_MENUS)
			if (g_eeGeneral.currModel != sub) {
				if (eeModelExists(sub)) {
					POPUP_MENU_ADD_ITEM(STR_SELECT_MODEL);
					POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
					POPUP_MENU_ADD_ITEM(STR_COPY_MODEL);
					POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
					POPUP_MENU_ADD_ITEM(STR_DELETE_MODEL);
					} else {
					#if defined(SDCARD)
					POPUP_MENU_ADD_ITEM(STR_CREATE_MODEL);
					POPUP_MENU_ADD_ITEM(STR_RESTORE_MODEL);
					#else
					selectModel(sub);
					#endif
				}
				} else {
				POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
				POPUP_MENU_ADD_ITEM(STR_COPY_MODEL);
				POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
			}
			popupMenuHandler = onModelSelectMenu;
			#else
			if (g_eeGeneral.currModel != sub) {
				selectModel(sub);
			}
			#endif
			} else if (eeModelExists(sub)) {
			s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
			s_copyTgtOfs = 0;
			s_copySrcRow = -1;
		}
		break;

		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_LEFT:
		case EVT_ROTARY_RIGHT:
		#endif
		case EVT_KEY_FIRST(KEY_LEFT):
		case EVT_KEY_FIRST(KEY_RIGHT):
		#if defined(ROTARY_ENCODER_NAVIGATION)
		if ((!IS_ROTARY_RIGHT(event) && !IS_ROTARY_LEFT(event)) || s_editMode < 0) {
			#endif
			if (sub == g_eeGeneral.currModel) {
				chainMenu((IS_ROTARY_RIGHT(event) || event == EVT_KEY_FIRST(KEY_RIGHT)) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
				} else {
				AUDIO_WARNING2();
			}
			break;
			#if defined(ROTARY_ENCODER_NAVIGATION)
		}
		// no break
		#endif

		case EVT_KEY_FIRST(KEY_MOVE_UP):
		case EVT_KEY_REPT(KEY_MOVE_UP):
		case EVT_KEY_FIRST(KEY_MOVE_DOWN):
		case EVT_KEY_REPT(KEY_MOVE_DOWN):
		if (s_copyMode) {
			int8_t next_ofs = s_copyTgtOfs + oldSub - menuVerticalPosition;
			if (next_ofs == MAX_MODELS || next_ofs == -MAX_MODELS)
			next_ofs = 0;

			if (s_copySrcRow < 0 && s_copyMode==COPY_MODE) {
				s_copySrcRow = oldSub;
				// find a hole (in the first empty slot above / below)
				sub = eeFindEmptyModel(s_copySrcRow, IS_ROTARY_DOWN(event) || event==EVT_KEY_FIRST(KEY_MOVE_DOWN));
				if (sub < 0) {
					// no free room for duplicating the model
					AUDIO_ERROR();
					sub = oldSub;
					s_copyMode = 0;
				}
				next_ofs = 0;
				menuVerticalPosition = sub;
			}
			s_copyTgtOfs = next_ofs;
		}
		break;
	}

	lcdDrawText(9*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
	if (event) reusableBuffer.modelsel.eepromfree = EeFsGetFree();
	lcdDrawNumberNAtt(17*FW, 0, reusableBuffer.modelsel.eepromfree, 0);

	#if defined(ROTARY_ENCODER_NAVIGATION)
	displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? ((IS_RE_NAVIGATION_ENABLE() && s_editMode < 0) ? INVERS|BLINK : INVERS) : 0);
	#else
	displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? INVERS : 0);
	#endif

	TITLE(STR_MENUMODELSEL);

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i+menuVerticalOffset;

		lcdDrawNumberNAtt(3*FW+2, y, k+1, LEADING0+((!s_copyMode && sub==k) ? INVERS : 0), 2);

		if (s_copyMode == MOVE_MODE || (s_copyMode == COPY_MODE && s_copySrcRow >= 0)) {
			if (k == sub) {
				if (s_copyMode == COPY_MODE) {
					k = s_copySrcRow;
					lcdDrawChar(MODELSEL_W-FW, y, '+');
					} else {
					k = sub + s_copyTgtOfs;
				}
			} else if (s_copyTgtOfs < 0 && ((k < sub && k >= sub+s_copyTgtOfs) || (k-MAX_MODELS < sub && k-MAX_MODELS >= sub+s_copyTgtOfs)))
			k += 1;
			else if (s_copyTgtOfs > 0 && ((k > sub && k <= sub+s_copyTgtOfs) || (k+MAX_MODELS > sub && k+MAX_MODELS <= sub+s_copyTgtOfs)))
			k += MAX_MODELS-1;
		}

		k %= MAX_MODELS;

		if (eeModelExists(k)) {
			char * name = reusableBuffer.modelsel.listnames[i];
			if (event) eeLoadModelName(k, name);
			putsModelName(4*FW, y, name, k, 0);
			lcdDrawNumberNAtt(20*FW, y, eeModelSize(k), 0);
			if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+menuVerticalOffset!=(vertpos_t)sub))
			lcdDrawChar(1, y, '*');
		}

		if (s_copyMode && (vertpos_t)sub==i+menuVerticalOffset) {
			lcdDrawFilledRect(9, y, MODELSEL_W-1-9, 7);
			lcdDrawRect(8, y-1, MODELSEL_W-1-7, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
///Model setup
//////////////////////////////////////////////////////////////////////////
void menuModelSetup(uint8_t event)
{
	#define CURSOR_ON_CELL         (true)
	#define MODEL_SETUP_MAX_LINES  (IS_PPM_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_1+2 : \
	(IS_DSM2_SERIAL_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_2+2 : \
	(IS_MULTIMODULE_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_5+2 :  \
	(IS_SPIMODULES_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_8+2 :   \
	1

	uint8_t protocol = g_model.rfProtocol;
	MEMPROTO = protocol;
	MENU_TAB({ 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 1, 0, 0, 0, 0, 0, NUM_SWITCHES, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX,
		2,2,0,2,0,0,0,0
	});


	MENU_CHECK(menuTabModel, e_ModelSetup, MODEL_SETUP_MAX_LINES+2);


	TITLE(STR_MENUSETUP);

	uint8_t sub = menuVerticalPosition - 1;
	int8_t editMode = s_editMode;

	for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i+menuVerticalOffset;

		LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
		LcdFlags attr = (sub == k ? blink : 0);

		switch(k) {
			case ITEM_MODEL_NAME:
			editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_NAME, g_model.name, sizeof(g_model.name), event, attr, EE_MODEL, RANGE_NONE);
			break;

			case ITEM_MODEL_TIMER1:
			case ITEM_MODEL_TIMER2:
			case ITEM_MODEL_TIMER1_MINUTE_BEEP:
			case ITEM_MODEL_TIMER2_MINUTE_BEEP:
			case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
			case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP: {
				TimerData *timer = &g_model.timers[k>=ITEM_MODEL_TIMER2 ? 1 : 0];
				if (k==ITEM_MODEL_TIMER1_MINUTE_BEEP || k==ITEM_MODEL_TIMER2_MINUTE_BEEP) {
					timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
					} else if (k==ITEM_MODEL_TIMER1_COUNTDOWN_BEEP || k==ITEM_MODEL_TIMER2_COUNTDOWN_BEEP) {
					timer->countdownBeep = onoffMenuItem(timer->countdownBeep, MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, attr, event);
					} else {
					lcdDrawStringWithIndex(0*FW, y, STR_TIMER, k>=ITEM_MODEL_TIMER2 ? 2 : 1);
					lcdPutsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
					putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, menuHorizontalPosition==1 ? attr : 0, menuHorizontalPosition==2 ? attr : 0);
					if (attr && (editMode>0 || p1valdiff)) {
						div_t qr = div(timer->start, 60);
						switch (menuHorizontalPosition) {
							case 0:
							CHECK_INCDEC_MODELVAR_CHECK(event, timer->mode, SWSRC_FIRST, TMRMODE_COUNT+SWSRC_LAST-1/*SWSRC_None removed*/, isSwitchAvailableInTimers);
							break;
							case 1:
							CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 59);
							timer->start = qr.rem + qr.quot*60;
							break;
							case 2:
							qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
							timer->start -= qr.rem ;
							if ((int16_t)timer->start < 0) timer->start=0;
							if ((int16_t)timer->start > 5999) timer->start=32399; // 8:59:59
							break;
						}
					}
				}
				break;
			}

			case ITEM_MODEL_TIMER1_PERSISTENT:
			case ITEM_MODEL_TIMER2_PERSISTENT: {
				TimerData &timer = g_model.timers[k==ITEM_MODEL_TIMER2_PERSISTENT];
				timer.persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer.persistent, 0, 2, attr, event);
				break;
			}

			case ITEM_MODEL_EXTENDED_LIMITS:
			ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
			break;

			case ITEM_MODEL_EXTENDED_TRIMS:
			ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
			lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
			if (attr && menuHorizontalPosition>0) {
				s_editMode = 0;
				if (event==EVT_KEY_LONG(KEY_ENTER)) {
					START_NO_HIGHLIGHT();
					for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
						memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
					}
					eeDirty(EE_MODEL);
					AUDIO_WARNING1();
				}
			}
			break;

			case ITEM_MODEL_TRIM_INC:
			g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
			break;

			case ITEM_MODEL_THROTTLE_TRACE: {
				lcdDrawTextLeft(y, STR_TTRACE);
				if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT);
				setGazSource();
				putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, gazSource, attr);
				break;
			}

			case ITEM_MODEL_THROTTLE_SWITCH: {
				lcdDrawTextLeft(y, STR_TSWITCH);
				if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrSwitch, NUM_SWITCHES-2);
				uint8_t idx = g_model.thrSwitch;
				if (idx) idx +=3;
				lcdPutsSwitches(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
				break;
			}

			case ITEM_MODEL_THROTTLE_TRIM:
			ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
			break;

			case ITEM_MODEL_THROTTLE_WARNING:
			g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
			break;

			case ITEM_MODEL_SWITCHES_WARNING: {
				lcdDrawTextLeft(y, STR_SWITCHWARNING);
				int8_t states = g_model.switchWarningState;
				char c;
				if (attr) {
					s_editMode = 0;
					if (!READ_ONLY()) {
						switch (event) {
							CASE_EVT_ROTARY_BREAK
							case EVT_KEY_BREAK(KEY_ENTER):
							if (menuHorizontalPosition < NUM_SWITCHES-1) {
								g_model.switchWarningEnable ^= (1 << menuHorizontalPosition);
								eeDirty(EE_MODEL);
							}
							break;

							case EVT_KEY_LONG(KEY_ENTER):
							if (menuHorizontalPosition == NUM_SWITCHES-1) {
								START_NO_HIGHLIGHT();
								getMovedSwitch();
								g_model.switchWarningState = switches_states;
								AUDIO_WARNING1();
								eeDirty(EE_MODEL);
							}
							killEvents(event);
							break;
						}
					}
				}

				LcdFlags line = attr;

				for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++) {
					uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
					attr = 0;

					if (IS_3POS(i)) {
						c = '0'+(states & 0x03);
						states >>= 2;
						} else {
						if ((states & 0x01) && swactive)
						attr = INVERS;
						c = pgm_read_byte_near(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
						states >>= 1;
					}
					if (line && (menuHorizontalPosition == i)) {
						attr = BLINK;
						if (swactive)
						attr |= INVERS;
					}
					lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+i*FW, y, (swactive || (attr & BLINK)) ? c : '-', attr);
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+(NUM_SWITCHES*FW), y, PSTR("<]"), (menuHorizontalPosition == NUM_SWITCHES-1 && !NO_HIGHLIGHT()) ? line : 0);
				}
				break;
			}

			case ITEM_MODEL_BEEP_CENTER:
			lcdDrawTextLeft(y, STR_BEEPCTR);
			for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {
				// TODO flash saving, \001 not needed in STR_RETA123
				coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
				lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((uint16_t)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
			}
			if (attr && CURSOR_ON_CELL) {
				if (event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff) {
					if (READ_ONLY_UNLOCKED()) {
						s_editMode = 0;
						g_model.beepANACenter ^= ((uint16_t)1<<menuHorizontalPosition);
						eeDirty(EE_MODEL);
					}
				}
			}
			break;

			int16_t tmp; // used for CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE

			case ITEM_MODEL_PROTOCOL:
			lcdDrawTextLeft(y, NO_INDENT(STR_PROTO));
			if IS_SPIMODULES_PROTOCOL(protocol) {
				#if defined(SPIMODULES)
				lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, STR_SPIM, menuHorizontalPosition<=0 ? attr : 0);
				#endif
				} else {
				lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition<=0 ? attr : 0);
			}
			if (IS_PPM_PROTOCOL(protocol)) {
				lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.PPMNCH, menuHorizontalPosition!=0 ? attr : 0);
				} else if (menuHorizontalPosition>0 && attr) {
				MOVE_CURSOR_FROM_HERE();
			}
			if (attr && (editMode>0 || p1valdiff)) {
				switch (menuHorizontalPosition) {
					case 0:
					#if defined(SPIMODULES)
					if IS_SPIMODULES_PROTOCOL(protocol) {
						uint8_t prototemp = LASTPROTOMENU1;
						CHECK_INCDEC_MODELVAR_ZERO(event, prototemp, LASTPROTOMENU1);
						if (prototemp < LASTPROTOMENU1) {
							protocol = prototemp;
						}
					} else
					#endif
					{
						CHECK_INCDEC_MODELVAR_ZERO(event, protocol, LASTPROTOMENU1);
					}
					break;
					case 1:
					CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PPMNCH, /*protocol!=PROTOCOL_PPM16 ? 6 : 2*/ 6); //limit 8 channels for PPMSim and PPM16
					g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
					break;
				}
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_1:
			if PROTO_IS_SYNC {
				if (IS_PPM_PROTOCOL(protocol))
				{
					lcdDrawTextLeft(y, STR_PPMFRAME);
					lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
					lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.PPMFRAMELENGTH*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
					lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
					lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.PPMDELAY*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
					lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.PULSEPOL ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
					if (attr && (editMode>0 || p1valdiff)) {
						switch (menuHorizontalPosition) {
							case 0:
							CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMFRAMELENGTH, -20, 35);
							break;
							case 1:
							CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMDELAY, -4, 10);
							break;
							case 2:
							CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PULSEPOL, 1);
							break;
						}
					}
				}
				#if defined(DSM2_SERIAL)
				if (IS_DSM2_SERIAL_PROTOCOL(protocol))
				{
					lcdDrawTextLeft(y, STR_TYPE);
					lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 0 ? attr : 0);
					if (attr && (editMode>0 || p1valdiff)) {
						CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax);
					}
				}
				#endif
				#if defined(SPIMODULES)
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition == 0 ? attr : 0);
					if (RfOptionSettings.rfSubTypeMax) { // Check if Subtype exist
						lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 1 ? attr : 0);
					}
					if (attr  && (editMode>0)) {

						switch (menuHorizontalPosition) {
							case 0: {
								CHECK_INCDEC_MODELVAR(event, protocol, LASTPROTOMENU1, PROTOCOL_COUNT-1);
								break;
							}
							case 1: {
								if (RfOptionSettings.rfSubTypeMax) {
									CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax); // TODO SUBTYPE
								} else break;
							}
							break;
						}
					}
				}
				#endif
				#if defined(MULTIMODULE)
				else if IS_MULTIMODULE_PROTOCOL(protocol)
				{
					uint8_t multi_rfProto = g_model.MULTIRFPROTOCOL;

					lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
					if(g_model.CUSTOMPROTO) {
						lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==0 ? attr : 0);
						} else {
						lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTIPROTOCOLS, multi_rfProto, menuHorizontalPosition==0 ? attr : 0);
					}
					const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
					if(g_model.CUSTOMPROTO) {
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 3 * FW, y, g_model.MULTIRFPROTOCOL, (menuHorizontalPosition == 1 ? attr : 0));
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfSubType, (menuHorizontalPosition == 2 ? attr : 0));
						} else {
						if (pdef->subTypeString != 0)
						lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+ 2 * FW, y, pdef->subTypeString, g_model.rfSubType, (menuHorizontalPosition == 1 ? attr : 0));
					}

					if (attr  && (editMode>0)) {

						switch (menuHorizontalPosition) {
							case 0: {
								int8_t multiRfProto = g_model.CUSTOMPROTO == true ? MM_RF_PROTO_CUSTOM : g_model.MULTIRFPROTOCOL;
								CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
								if (checkIncDec_Ret) {
									g_model.CUSTOMPROTO = (multiRfProto == MM_RF_PROTO_CUSTOM);
									if (!g_model.CUSTOMPROTO)
									g_model.MULTIRFPROTOCOL = multiRfProto;
									g_model.rfSubType = 0;
									if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2) {
										g_model.AUTOBINDMODE = 1;
										} else {
										g_model.AUTOBINDMODE = 0;
									}
									g_model.rfOptionValue2 = 0;
								}
							}
							break;
							case 1: {
								if (g_model.CUSTOMPROTO) {
									CHECK_INCDEC_MODELVAR_ZERO(event, multi_rfProto, MULTI_RF_PROTO_LAST);
									g_model.MULTIRFPROTOCOL = multi_rfProto;
								} else if (pdef->maxSubtype > 0)
								CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, pdef->maxSubtype);

							}
							break;
							case 2:
							// Custom protocol, third column is subtype
							CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, 7);
							break;

						}
					}
				}
				#endif
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_2:
			if PROTO_IS_SYNC {
				#if defined(DSM2_SERIAL)
				if (IS_DSM2_SERIAL_PROTOCOL(protocol))
				{
					horzpos_t l_posHorz = menuHorizontalPosition;
					lcdDrawTextLeft(y, STR_RXNUM);
					coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
					if (xOffsetBind) lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
					if (attr && l_posHorz==0) {
						if (editMode>0 || p1valdiff) {
							CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
						}
					}
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

					if (attr && l_posHorz>0 && s_editMode>0) {
						if (l_posHorz == 1) {
							PROTOCOL_SetBindState(1000); // 10 Sec
							} else if (l_posHorz == 2) {
							rangeModeIsOn = true;
						}
					}
				}
				#endif
				#if defined(MULTIMODULE)
				if IS_MULTIMODULE_PROTOCOL(protocol)
				{
					horzpos_t l_posHorz = menuHorizontalPosition;
					lcdDrawTextLeft(y, STR_RXNUM);
					coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
					if (xOffsetBind) lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
					if (attr && l_posHorz==0) {
						if (editMode>0 || p1valdiff) {
							CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, 15);
						}
					}
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

					if (attr && l_posHorz>0 && s_editMode>0) {
						if (l_posHorz == 1)
						PROTOCOL_SetBindState(500); // 5 Sec
						else if (l_posHorz == 2) {
							rangeModeIsOn = true;
						}
					}
				}
				#endif
				#if defined(SPIMODULES)
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					horzpos_t l_posHorz = menuHorizontalPosition;
					lcdDrawTextLeft(y, STR_RXNUM);
					coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
					if (xOffsetBind) lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
					if (attr && l_posHorz==0) {
						if (editMode>0 || p1valdiff) {
							CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
						}
					}
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
					lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

					if (attr && l_posHorz>0 && s_editMode>0) {
						if (l_posHorz == 1) {
							if (protoMode != BIND_MODE)
							{
								protoMode = BIND_MODE;
								startPulses(PROTOCMD_BIND);
							}
						}
						else if (l_posHorz == 2)
						{
							rangeModeIsOn = true;
						}
					}
					if (attr && l_posHorz == 1 && protoMode == BIND_MODE && s_editMode<1)
					{
						protoMode = NORMAL_MODE;
						startPulses(PROTOCMD_INIT);
					}
				}
				#endif
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_3:
			if PROTO_IS_SYNC {
				#if defined(MULTIMODULE)
				if IS_MULTIMODULE_PROTOCOL(protocol)
				{
					if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2)
					ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
					else
					ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_AUTOBIND, attr, event);
				}
				#endif
				#if defined(SPIMODULES)
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionValue1Max != RfOptionSettings.rfOptionValue1Min) {
						lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue1Name);
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue1, attr);
						if (attr) {
							CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue1, RfOptionSettings.rfOptionValue1Min, RfOptionSettings.rfOptionValue1Max);
						}
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
				#endif
			}
			break;
			case  ITEM_MODEL_PROTOCOL_PARAMS_LINE_4: //Option1 for SPIMOD
			if PROTO_IS_SYNC {
				#if defined(MULTIMODULE)
				if IS_MULTIMODULE_PROTOCOL(protocol)
				{
					ON_OFF_MENU_ITEM(g_model.LOWPOWERMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
				}
				#endif
				#if defined(SPIMODULES)
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionValue2Max != RfOptionSettings.rfOptionValue2Min) {
						lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue2Name);
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue2, attr);
						if (attr) {
							CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, RfOptionSettings.rfOptionValue2Min, RfOptionSettings.rfOptionValue2Max);
						}
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
				#endif
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_5:
			if PROTO_IS_SYNC {
				#if defined(MULTIMODULE)
				if IS_MULTIMODULE_PROTOCOL(protocol)
				{
					int8_t optionValue =  g_model.rfOptionValue2;
					const uint8_t multi_proto = g_model.MULTIRFPROTOCOL;
					const mm_protocol_definition* pdef = getMultiProtocolDefinition(multi_proto);
					if (pdef->optionsstr) {
						lcdDrawTextLeft(y, pdef->optionsstr);
						if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) optionValue = 50 + 5 * optionValue;
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 2 * FW, y, optionValue, attr);
						if (attr) {
							if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) {
								CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, 0, 70);
								} else if (multi_proto == MM_RF_PROTO_OLRS) {
								CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -1, 7);
								} else {
								CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -128, 127);
							}
						}
					}
				}
				#endif
				#if defined(SPIMODULES)
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionValue3Max) {
						lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue3Name);
						lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue3, attr);
						if (attr) {
							CHECK_INCDEC_MODELVAR_ZERO(event, g_model.rfOptionValue3, RfOptionSettings.rfOptionValue3Max);
							#if defined(SIMU)
							switch (protocol)
							{
								case PROTOCOL_FRSKYD:
								case PROTOCOL_FRSKYV:
								case PROTOCOL_FRSKYX:
								case PROTOCOL_SKYARTEC:
								case PROTOCOL_CORONA:
								CC2500_ManagePower(); // Change value on gui

								case PROTOCOL_DEVO:
								case PROTOCOL_DSM:
								CYRF_ManagePower();
								case PROTOCOL_FLYSKY:
								case PROTOCOL_AFHDS2A:
								A7105_ManagePower();
							}
							#endif
						}
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
				#endif
			}
			break;
			#if defined(SPIMODULES)
			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_6:
			if PROTO_IS_SYNC {
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionBool1Used) {
						ON_OFF_MENU_ITEM(g_model.rfOptionBool1, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool1Name, attr, event);
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_7:
			if PROTO_IS_SYNC {
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionBool2Used) {
						ON_OFF_MENU_ITEM(g_model.rfOptionBool2, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool2Name, attr, event);
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
			}
			break;

			case ITEM_MODEL_PROTOCOL_PARAMS_LINE_8:
			if PROTO_IS_SYNC {
				if IS_SPIMODULES_PROTOCOL(protocol)
				{
					if (RfOptionSettings.rfOptionBool3Used) {
						ON_OFF_MENU_ITEM(g_model.rfOptionBool3, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool3Name, attr, event);
						} else {
						lcdDrawTextAtt(0,y,STR_DUMMY,attr);
					}
				}
			}
			break;

			#endif
		}
	}

	if (!PROTO_IS_SYNC) {
		flightReset();
		rangeModeIsOn = false;
		g_model.rfProtocol = protocol;
		protoMode = NORMAL_MODE;
		startPulses(PROTOCMD_INIT);
	}
}

//////////////////////////////////////////////////////////////////////////
///Model heli
//////////////////////////////////////////////////////////////////////////
#ifdef HELI
void menuModelHeli(uint8_t event)
{
	SIMPLE_MENU(STR_MENUHELISETUP, menuTabModel, e_Heli, 7);

	uint8_t sub = menuVerticalPosition - 1;

	for (uint8_t i=0; i<6; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t attr = (sub == i ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
		switch(i) {
			case ITEM_HELI_SWASHTYPE:
			g_model.swashR.type = selectMenuItem(HELI_PARAM_OFS, y, STR_SWASHTYPE, STR_VSWASHTYPE, g_model.swashR.type, 0, SWASH_TYPE_MAX, attr, event);
			break;

			case ITEM_HELI_COLLECTIVE:
			g_model.swashR.collectiveSource = selectMenuItem(HELI_PARAM_OFS, y, STR_COLLECTIVE, NULL, g_model.swashR.collectiveSource, 0, MIXSRC_LAST_CH, attr, event);
			putsMixerSource(HELI_PARAM_OFS, y, g_model.swashR.collectiveSource, attr);
			break;

			case ITEM_HELI_SWASHRING:
			lcdDrawTextLeft(y, STR_SWASHRING);
			lcdDrawNumberNAtt(HELI_PARAM_OFS, y, g_model.swashR.value,  LEFT|attr);
			if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.swashR.value, 100);
			break;

			case ITEM_HELI_ELEDIRECTION:
			g_model.swashR.invertELE = selectMenuItem(HELI_PARAM_OFS, y, STR_ELEDIRECTION, STR_MMMINV, g_model.swashR.invertELE, 0, 1, attr, event);
			break;

			case ITEM_HELI_AILDIRECTION:
			g_model.swashR.invertAIL = selectMenuItem(HELI_PARAM_OFS, y, STR_AILDIRECTION, STR_MMMINV, g_model.swashR.invertAIL, 0, 1, attr, event);
			break;

			case ITEM_HELI_COLDIRECTION:
			g_model.swashR.invertCOL = selectMenuItem(HELI_PARAM_OFS, y, STR_COLDIRECTION, STR_MMMINV, g_model.swashR.invertCOL, 0, 1, attr, event);
			break;
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
///Model Limits
//////////////////////////////////////////////////////////////////////////
bool isThrottleOutput(uint8_t ch)
{
	for (uint8_t i=0; i<MAX_MIXERS; i++) {
		MixData *mix = mixAddress(i);
		if (mix->destCh==ch && mix->srcRaw==MIXSRC_Thr)
		return true;
	}
	return false;
}

void menuModelLimits(uint8_t event)
{
	uint8_t sub = menuVerticalPosition - 1;

	if (sub < NUM_CHNOUT) {
		#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_UNIT_US)
		lcdDrawNumberNAtt(13*FW, 0, PPM_CH_CENTER(sub)+channelOutputs[sub]/2, 0);
		lcdDrawText(13*FW, 0, STR_US);
		#else
		lcdDrawNumberNAtt(13*FW, 0, calcRESXto1000(channelOutputs[sub]), PREC1);
		#endif
	}

	MENU(STR_MENULIMITS, menuTabModel, e_Limits, 1+NUM_CHNOUT+1, {0, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, 0});

	if (warningResult) {
		warningResult = false;
		LimitData *ld = limitAddress(sub);
		ld->revert = !ld->revert;
		eeDirty(EE_MODEL);
	}

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i+menuVerticalOffset;

		if (k==NUM_CHNOUT) {
			// last line available - add the "copy trim menu" line
			uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
			lcdDrawTextAtt(CENTER_OFS, y, STR_TRIMS2OFFSETS, NO_HIGHLIGHT() ? 0 : attr);
			if (attr) {
				s_editMode = 0;
				if (event==EVT_KEY_LONG(KEY_ENTER)) {
					START_NO_HIGHLIGHT();
					killEvents(event);
					moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
				}
			}
			return;
		}

		LimitData *ld = limitAddress(k);

		#if !defined(PPM_CENTER_ADJUSTABLE)
		int16_t v = (ld->revert) ? -LIMIT_OFS(ld) : LIMIT_OFS(ld);
		char swVal = '-';  // '-', '<', '>'
		if ((channelOutputs[k] - v) > 50) swVal = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
		if ((channelOutputs[k] - v) < -50) swVal = (ld->revert ? 126 : 127);
		putsChn(0, y, k+1, 0);
		lcdDrawChar(LIMITS_DIRECTION_POS, y, swVal);
		#endif

		int8_t limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 100);

		putsChn(0, y, k+1, 0);

		for (uint8_t j=0; j<ITEM_LIMITS_COUNT; j++) {
			uint8_t attr = ((sub==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
			uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;
			if (active) { STICK_SCROLL_DISABLE(); }
			switch(j) {
				case ITEM_LIMITS_OFFSET:
				#if defined(PPM_UNIT_US)
				lcdDrawNumberNAtt(LIMITS_OFFSET_POS, y, ((int32_t)ld->offset*128) / 25, attr|PREC1);
				#else
				lcdDrawNumberNAtt(LIMITS_OFFSET_POS, y, ld->offset, attr|PREC1);
				#endif
				if (active) {
					ld->offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL|NO_INCDEC_MARKS);
					} else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
					copySticksToOffset(k);
					s_editMode = 0;
				}
				break;

				case ITEM_LIMITS_MIN:
				lcdDrawNumberNAtt(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR);
				if (active) ld->min = LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->min-LIMITS_MIN_MAX_OFFSET, -limit, 0, EE_MODEL);
				break;

				case ITEM_LIMITS_MAX:
				lcdDrawNumberNAtt(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR);
				if (active) ld->max = -LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->max+LIMITS_MIN_MAX_OFFSET, 0, +limit, EE_MODEL);
				break;

				case ITEM_LIMITS_DIRECTION: {
					uint8_t revert = ld->revert;
					#if defined(PPM_CENTER_ADJUSTABLE)
					lcdDrawCharAtt(LIMITS_REVERT_POS, y, revert ? 127 : 126, attr);
					#else
					lcdDrawTextAtIndex(LIMITS_REVERT_POS, y, STR_MMMINV, revert, attr);
					#endif
					if (active) {
						uint8_t revert_new = checkIncDecModel(event, revert, 0, 1);
						if (checkIncDec_Ret && isThrottleOutput(k)) {
							POPUP_CONFIRMATION(STR_INVERT_THR);
							} else {
							ld->revert = revert_new;
						}
					}
					break;
				}

				#if defined(PPM_CENTER_ADJUSTABLE)
				case ITEM_LIMITS_PPM_CENTER:
				lcdDrawNumberNAtt(LIMITS_PPM_CENTER_POS, y, PPM_CENTER+ld->ppmCenter, attr);
				if (active) {
					CHECK_INCDEC_MODELVAR(event, ld->ppmCenter, -PPM_CENTER_MAX, +PPM_CENTER_MAX);
				}
				break;
				#endif

				#if defined(PPM_LIMITS_SYMETRICAL)
				case ITEM_LIMITS_SYMETRICAL:
				lcdDrawCharAtt(LCD_W-FW-MENUS_SCROLLBAR_WIDTH, y, ld->symetrical ? '=' : '^', attr);
				if (active) {
					CHECK_INCDEC_MODELVAR_ZERO(event, ld->symetrical, 1);
				}
				break;
				#endif
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
///Model Templates
//////////////////////////////////////////////////////////////////////////
#ifdef TEMPLATES
void menuModelTemplates(uint8_t event)
{
	SIMPLE_MENU(STR_MENUTEMPLATES, menuTabModel, e_Templates, 1+TMPL_COUNT);

	uint8_t sub = menuVerticalPosition - 1;

	if (sub < TMPL_COUNT) {
		if (warningResult) {
			warningResult = false;
			applyTemplate(sub);
			AUDIO_WARNING2();
		}
		if (event==EVT_KEY_BREAK(KEY_ENTER)) {
			POPUP_CONFIRMATION(STR_VTEMPLATES+1 + (sub * LEN2_VTEMPLATES));
			s_editMode = 0;
		}
	}

	coord_t y = MENU_HEADER_HEIGHT + 1;
	uint8_t k = 0;
	for (uint8_t i=0; i<LCD_LINES-1 && k<TMPL_COUNT; i++) {
		k = i+menuVerticalOffset;
		lcdDrawNumberNAtt(3*FW, y, k, (sub==k ? INVERS : 0)|LEADING0, 2);
		lcdDrawTextAtIndex(4*FW, y, STR_VTEMPLATES, k, (sub==k ? INVERS  : 0));
		y+=FH;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
///Model Telemetry
//////////////////////////////////////////////////////////////////////////
#ifdef TELEMETRY
void menuModelTelemetry(uint8_t event)
{

	MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {USRPROTO 0, CHANNELS_ROWS RSSI_ROWS SENSORS_ROWS USRDATA_ROWS CASE_VARIO(LABEL(Vario)) CASE_VARIO(0) CASE_VARIO(VARIO_RANGE_ROWS) TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1), });

	uint8_t sub = menuVerticalPosition - 1;

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i + menuVerticalOffset;

		uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
		uint8_t attr = (sub == k ? blink : 0);

		uint8_t ch = TELEMETRY_CURRENT_CHANNEL(k);
		TelemetryChannelData & channel = g_model.telemetry.channels[ch];
		uint8_t dest = TELEM_A1-1+ch;


		switch (k) {


			case ITEM_TELEMETRY_A1_LABEL:
			case ITEM_TELEMETRY_A2_LABEL:
			lcdDrawTextLeft(y, STR_ACHANNEL);
			lcdDrawNumberNAtt(2*FW, y, ch+1, 0);
			lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, telemetryData.analog[ch].value, LEFT);
			break;

			case ITEM_TELEMETRY_A1_RANGE:
			case ITEM_TELEMETRY_A2_RANGE:
			lcdDrawTextLeft(y, STR_RANGE);
			lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 255-channel.offset, (menuHorizontalPosition<=0 ? attr : 0)|NO_UNIT|LEFT);
			lcdDrawTextAtIndex(lcdLastPos, y, STR_VTELEMUNIT, channel.type, menuHorizontalPosition!=0 ? attr : 0);
			if (attr && (s_editMode>0 || p1valdiff)) {
				if (menuHorizontalPosition == 0) {
					uint16_t ratio = checkIncDec(event, channel.ratio, 0, 256, EE_MODEL);
					if (checkIncDec_Ret) {
						if (ratio == 127 && channel.multiplier > 0) {
							channel.multiplier--;
							channel.ratio = 255;
							} else if (ratio == 256) {
							#ifdef FRSKY
							if (channel.multiplier < FRSKY_MULTIPLIER_MAX) {
								channel.multiplier++;
								channel.ratio = 128;
							}
							#endif
							} else {
							channel.ratio = ratio;
						}
					}
					} else {
					CHECK_INCDEC_MODELVAR_ZERO(event, channel.type, UNIT_A1A2_MAX);
				}
			}
			break;

			case ITEM_TELEMETRY_A1_OFFSET:
			case ITEM_TELEMETRY_A2_OFFSET:
			lcdDrawTextLeft(y, STR_OFFSET);
			lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 0, LEFT|attr);
			if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
			break;

			case ITEM_TELEMETRY_RSSI_LABEL:
			lcdDrawTextLeft(y, PSTR("RSSI"));
			break;

			case ITEM_TELEMETRY_RSSI_ALARM1:
			lcdDrawTextLeft(y, STR_ALARM);
			lcdDrawChar(TELEM_COL2-2*FW, y, '<');
			lcdDrawNumberNAtt(TELEM_COL2+2*FW, y, getRssiAlarmValue(), attr, 3);
			if (attr) {CHECK_INCDEC_MODELVAR(event, g_model.telemetry.rssiAlarm, -30, 30);}
			break;

			#if defined(FRSKY)
			case ITEM_TELEMETRY_USR_LABEL:
			lcdDrawTextLeft(y, STR_USRDATA);
			break;

			case ITEM_TELEMETRY_USR_PROTO:
			lcdDrawTextLeft(y, STR_PROTO);
			lcdDrawTextAtIndex(TELEM_COL2, y, STR_VTELPROTO, g_model.telemetry.usrProto, attr);
			if (attr) {
				CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.usrProto, USR_PROTO_LAST);
				if ((checkIncDec_Ret) && (!IS_MULTIMODULE_PROTOCOL(g_model.rfProtocol))) { // No reset in Multi mode
					telemetryResetValue();
				}
			}
			break;

			case ITEM_TELEMETRY_USR_BLADES:
			lcdDrawTextLeft(y, STR_BLADES);
			lcdDrawNumberNAtt(TELEM_COL2+FWNUM, y, 2+g_model.telemetry.blades, attr);
			if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.blades, MAX_BLADES);
			break;
			#endif

			case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
			lcdDrawTextLeft(y, STR_VOLTAGE);
			lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.voltsSource+1, attr);
			if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.voltsSource, FRSKY_VOLTS_SOURCE_LAST);
			break;

			case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
			lcdDrawTextLeft(y, STR_CURRENT);
			lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.currentSource, attr);
			if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.currentSource, FRSKY_CURRENT_SOURCE_LAST);
			break;

			case ITEM_TELEMETRY_FAS_OFFSET:
			lcdDrawTextLeft(y, STR_FAS_OFFSET);
			lcdDrawNumberNAtt(TELEM_COL2, y, g_model.telemetry.fasOffset, attr|LEFT|PREC1);
			lcdDrawNumberNAtt(TELEM_COL2+6*FW, y, telemetryData.value.current, LEFT|PREC1);
			lcdDrawChar(TELEM_COL2+8*FW, y, 'A');
			if (attr) g_model.telemetry.fasOffset = checkIncDec(event, g_model.telemetry.fasOffset, -120, 120, EE_MODEL);
			break;

			#if defined(VARIO)
			case ITEM_TELEMETRY_VARIO_LABEL:
			lcdDrawTextLeft(y, STR_VARIO);
			break;

			case ITEM_TELEMETRY_VARIO_SOURCE:
			lcdDrawTextLeft(y, STR_SOURCE);
			lcdDrawTextAtIndex(TELEM_COL2, y, STR_VARIOSRC, g_model.telemetry.varioSource, attr);
			if (attr) CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioSource, 0, VARIO_SOURCE_LAST);
			break;

			case ITEM_TELEMETRY_VARIO_RANGE:
			lcdDrawTextLeft(y, STR_LIMIT);
			lcdDrawNumberNAtt(TELEM_COL2, y, -10+g_model.telemetry.varioMin, (menuHorizontalPosition<=0 ? attr : 0)|LEFT);
			lcdDrawNumberNAtt(TELEM_COL2+7*FW-2, y, -5+g_model.telemetry.varioCenterMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|PREC1);
			lcdDrawNumberNAtt(TELEM_COL2+10*FW, y, 5+g_model.telemetry.varioCenterMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0)|PREC1);
			lcdDrawNumberNAtt(TELEM_COL2+13*FW+2, y, 10+g_model.telemetry.varioMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==3) ? attr : 0));
			if (attr && (s_editMode>0 || p1valdiff)) {
				switch (menuHorizontalPosition) {
					case 0:
					CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMin, -7, 7);
					break;
					case 1:
					CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMin, -16, 5+min<int8_t>(10, g_model.telemetry.varioCenterMax+5));
					break;
					case 2:
					CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMax, -5+max<int8_t>(-10, g_model.telemetry.varioCenterMin-5), +15);
					break;
					case 3:
					CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMax, -7, 7);
					break;
				}
			}
			break;
			#endif

			case ITEM_TELEMETRY_SCREEN_LABEL1:
			case ITEM_TELEMETRY_SCREEN_LABEL2: {
				uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
				lcdDrawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex);
				bool screenType = g_model.telemetry.screensType & screenIndex;
				if (screenType != (bool)selectMenuItem(TELEM_SCRTYPE_COL, y, PSTR(""), STR_VTELEMSCREENTYPE, screenType, 0, 1, attr, event))
				g_model.telemetry.screensType ^= screenIndex;
				break;
			}

			case ITEM_TELEMETRY_SCREEN_LINE1:
			case ITEM_TELEMETRY_SCREEN_LINE2:
			case ITEM_TELEMETRY_SCREEN_LINE3:
			case ITEM_TELEMETRY_SCREEN_LINE4:
			case ITEM_TELEMETRY_SCREEN_LINE5:
			case ITEM_TELEMETRY_SCREEN_LINE6:
			case ITEM_TELEMETRY_SCREEN_LINE7:
			case ITEM_TELEMETRY_SCREEN_LINE8:

			{
				uint8_t screenIndex, lineIndex;
				if (k < ITEM_TELEMETRY_SCREEN_LABEL2) {
					screenIndex = 0;
					lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE1;
					} else {
					screenIndex = 1;
					lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
				}

				if (IS_BARS_SCREEN(screenIndex)) {
					FrSkyBarData & bar = g_model.telemetry.screens[screenIndex].bars[lineIndex];
					source_t barSource = bar.source;
					lcdDrawTextAtIndex(TELEM_COL1, y, STR_VTELEMCHNS, barSource, menuHorizontalPosition==0 ? attr : 0);
					if (barSource) {
						lcdPutsTelemetryChannelValue(TELEM_BARS_COLMIN, y, barSource-1, convertBarTelemValue(barSource, bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
						lcdPutsTelemetryChannelValue(TELEM_BARS_COLMAX, y, barSource-1, convertBarTelemValue(barSource, 255-bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
						} else if (attr && menuHorizontalPosition>0) {
						menuHorizontalPosition = 0;
					}
					if (attr && (s_editMode>0 || p1valdiff)) {
						switch (menuHorizontalPosition) {
							case 0:
							bar.source = CHECK_INCDEC_MODELVAR_ZERO(event, barSource, TELEM_DISPLAY_MAX);
							if (checkIncDec_Ret) {
								bar.barMin = 0;
								bar.barMax = 255 - maxBarTelemValue(bar.source);
							}
							break;
							case 1:
							bar.barMin = checkIncDec(event, bar.barMin, 0, 254-bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
							break;
							case 2:
							bar.barMax = 255 - checkIncDec(event, 255-bar.barMax, bar.barMin+1, maxBarTelemValue(barSource), EE_MODEL|NO_INCDEC_MARKS);
							break;
						}
					}
				} else
				{
					for (uint8_t c=0; c<NUM_LINE_ITEMS; c++) {
						uint8_t cellAttr = (menuHorizontalPosition==c ? attr : 0);
						source_t & value = g_model.telemetry.screens[screenIndex].lines[lineIndex].sources[c];
						uint8_t pos[] = {INDENT_WIDTH, TELEM_COL2};
						lcdDrawTextAtIndex(pos[c], y, STR_VTELEMCHNS, value, cellAttr);
						if (cellAttr && (s_editMode>0 || p1valdiff)) {
							CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, value, (lineIndex==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX, isTelemetrySourceAvailable);
						}
					}
					if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
						REPEAT_LAST_CURSOR_MOVE();
					}
				}
				break;
			}
		}
	}
}
#endif // TELEMETRY

//////////////////////////////////////////////////////////////////////////
///Model Curves
//////////////////////////////////////////////////////////////////////////
#ifdef CURVES
uint8_t s_curveChan;

int16_t curveFn(int16_t x)
{
	return applyCustomCurve(x, s_curveChan);
}

point_t getPoint(uint8_t i)
{
	point_t result = {0, 0};
	CurveInfo crv = curveInfo(s_curveChan);
	int8_t *points = crv.crv;
	bool custom = crv.custom;
	uint8_t count = crv.points;
	if (i < count) {
		result.x = X0-1-WCHART+i*WCHART/(count/2);
		result.y = (LCD_H-1) - (100 + points[i]) * (LCD_H-1) / 200;
		if (custom && i>0 && i<count-1)
		result.x = X0-1-WCHART + (100 + (100 + points[count+i-1]) * (2*WCHART)) / 200;
	}
	return result;
}

void DrawCurve(uint8_t offset=0)
{
	DrawFunction(curveFn, offset);

	uint8_t i = 0;
	do {
		point_t point = getPoint(i);
		i++;
		if (point.x == 0) break;
		lcdDrawFilledRect(point.x-offset, point.y-1, 3, 3, SOLID, FORCE); // do markup square
	} while(1);
}

bool moveCurve(uint8_t index, int8_t shift, int8_t custom=0)
{
	if (g_model.curves[MAX_CURVES-1] + shift > NUM_POINTS-5*MAX_CURVES) {
		AUDIO_WARNING2();
		return false;
	}

	int8_t *crv = curveAddress(index);
	if (shift < 0) {
		for (uint8_t i=0; i<custom; i++)
		crv[i] = crv[2*i];
	}

	int8_t *nextCrv = curveAddress(index+1);
	memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+g_model.curves[MAX_CURVES-1]-g_model.curves[index]);
	if (shift < 0) memclear(&g_model.points[NUM_POINTS-1] + shift, -shift);
	while (index<MAX_CURVES)
	g_model.curves[index++] += shift;

	for (uint8_t i=0; i<custom-2; i++)
	crv[custom+i] = -100 + ((200 * (i+1) + custom/2) / (custom-1)) ;

	eeDirty(EE_MODEL);
	return true;
}

void menuModelCurveOne(uint8_t event)
{
	TITLE(STR_MENUCURVE);
	lcdDrawNumberNAtt(PSIZE(TR_MENUCURVE)*FW+1, 0, s_curveChan+1, INVERS|LEFT);

	CurveInfo crv = curveInfo(s_curveChan);

	switch(event) {
		case EVT_ENTRY:
		s_editMode = 1;
		break;
		CASE_EVT_ROTARY_BREAK
		case EVT_KEY_BREAK(KEY_ENTER):
		if (s_editMode <= 0)
		menuHorizontalPosition = 0;
		if (s_editMode == 1 && crv.custom)
		s_editMode = 2;
		else
		s_editMode = 1;
		break;
		case EVT_KEY_LONG(KEY_ENTER):
		if (s_editMode <= 0) {
			if (int8_t(++menuHorizontalPosition) > 4)
			menuHorizontalPosition = -4;
			for (uint8_t i=0; i<crv.points; i++)
			crv.crv[i] = (i-(crv.points/2)) * int8_t(menuHorizontalPosition) * 50 / (crv.points-1);
			eeDirty(EE_MODEL);
			killEvents(event);
		}
		break;
		case EVT_KEY_BREAK(KEY_EXIT):
		if (s_editMode > 0) {
			if (--s_editMode == 0)
			menuHorizontalPosition = 0;
			} else {
			popMenu();
		}
		break;

		/* CASE_EVT_ROTARY_LEFT */
		case EVT_KEY_REPT(KEY_LEFT):
		case EVT_KEY_FIRST(KEY_LEFT):
		if (s_editMode==1 && menuHorizontalPosition>0) menuHorizontalPosition--;
		if (s_editMode <= 0) {
			if (crv.custom) {
				moveCurve(s_curveChan, -crv.points+2);
				} else if (crv.points > MIN_POINTS) {
				moveCurve(s_curveChan, -1, (crv.points+1)/2);
				} else {
				AUDIO_WARNING2();
			}
			return;
		}
		break;

		/* CASE_EVT_ROTARY_RIGHT */
		case EVT_KEY_REPT(KEY_RIGHT):
		case EVT_KEY_FIRST(KEY_RIGHT):
		if (s_editMode==1 && menuHorizontalPosition<(crv.points-1)) menuHorizontalPosition++;
		if (s_editMode <= 0) {
			if (!crv.custom) {
				moveCurve(s_curveChan, crv.points-2, crv.points);
				} else if (crv.points < MAX_POINTS) {
				if (moveCurve(s_curveChan, 1)) {
					for (int8_t i=crv.points+crv.points-2; i>=0; i--) {
						if (i%2)
						crv.crv[i] = (crv.crv[i/2] + crv.crv[1+i/2]) / 2;
						else
						crv.crv[i] = crv.crv[i/2];
					}
				}
				} else {
				AUDIO_WARNING2();
			}
		}
		break;
	}

	lcdDrawTextLeft(7*FH, STR_TYPE);
	uint8_t attr = (s_editMode <= 0 ? INVERS : 0);
	lcdDrawNumberNAtt(5*FW-2, 7*FH, crv.points, LEFT|attr);
	lcdDrawTextAtt(lcdLastPos, 7*FH, crv.custom ? PSTR("pt'") : PSTR("pt"), attr);

	DrawCurve();

	if (s_editMode>0) {
		uint8_t i = menuHorizontalPosition;
		point_t point = getPoint(i);

		if (s_editMode==1 || !BLINK_ON_PHASE) {
			// do selection square
			lcdDrawFilledRect(point.x-1, point.y-2, 5, 5, SOLID, FORCE);
			lcdDrawFilledRect(point.x, point.y-1, 3, 3, SOLID);
		}

		int8_t x = -100 + 200*i/(crv.points-1);
		if (crv.custom && i>0 && i<crv.points-1) x = crv.crv[crv.points+i-1];
		lcdDrawText(7, 2*FH, PSTR("x="));
		lcdDrawNumberNAtt(7+2*FW, 2*FH, x, LEFT);
		lcdDrawText(7, 3*FH, PSTR("y="));
		lcdDrawNumberNAtt(7+2*FW, 3*FH, crv.crv[i], LEFT);
		lcdDrawRect(3, 1*FH+4, 7*FW-2, 3*FH-2);

		if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
		CHECK_INCDEC_MODELVAR(event, crv.crv[i], -100, 100);  // edit Y on up/down

		if (i>0 && i<crv.points-1 && s_editMode==2 && (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_LEFT) || event==EVT_KEY_REPT(KEY_RIGHT)))
		CHECK_INCDEC_MODELVAR(event, crv.crv[crv.points+i-1], i==1 ? -99 : crv.crv[crv.points+i-2]+1, i==crv.points-2 ? 99 : crv.crv[crv.points+i]-1);  // edit X on left/right
	}
}

void menuModelCurvesAll(uint8_t event)
{
	SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, 1+MAX_CURVES);

	int8_t  sub = menuVerticalPosition - 1;

	switch (event) {
		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_BREAK:
		#endif
		case EVT_KEY_FIRST(KEY_RIGHT):
		case EVT_KEY_FIRST(KEY_ENTER):
		if (CURVE_SELECTED(sub) && !READ_ONLY()) {
			s_curveChan = sub;
			pushMenu(menuModelCurveOne);
		}
		break;
	}

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i + menuVerticalOffset;
		uint8_t attr = (sub == k ? INVERS : 0);
		{
			lcdDrawStringWithIndex(0, y, STR_CV, k+1, attr);
		}
	}

	if (CURVE_SELECTED(sub)) {
		s_curveChan = sub;
		DrawCurve(23);
	}
}
#endif // CURVES

//////////////////////////////////////////////////////////////////////////
///Model Logical Switches
//////////////////////////////////////////////////////////////////////////
void menuModelLogicalSwitches(uint8_t event)
{
	INCDEC_DECLARE_VARS(EE_MODEL);

	MENU(STR_MENULOGICALSWITCHES, menuTabModel, e_LogicalSwitches, NUM_LOGICAL_SWITCH+1, {0, NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/});

	int8_t    sub = menuVerticalPosition - 1;
	horzpos_t horz = menuHorizontalPosition;

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i+menuVerticalOffset;
		uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
		uint8_t attr1 = (horz==1 ? attr : 0);
		uint8_t attr2 = (horz==2 ? attr : 0);
		LogicalSwitchData * cs = lswAddress(k);

		// CSW name
		uint8_t sw = SWSRC_SW1+k;
		lcdPutsSwitches(0, y, sw, (getSwitch(sw) ? BOLD : 0) | ((sub==k && CURSOR_ON_LINE()) ? INVERS : 0));

		// CSW func
		lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, horz==0 ? attr : 0);

		// CSW params
		uint8_t cstate = lswFamily(cs->func);
		int8_t v1_min=0, v1_max=MIXSRC_LAST_TELEM, v2_min=0, v2_max=MIXSRC_LAST_TELEM;

		if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
			lcdPutsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr1);
			lcdPutsSwitches(CSW_3RD_COLUMN, y, cs->v2, attr2);
			v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
			v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
			v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
			v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
			INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
			} else if (cstate == LS_FAMILY_COMP) {
			putsMixerSource(CSW_2ND_COLUMN, y, cs->v1, attr1);
			putsMixerSource(CSW_3RD_COLUMN, y, cs->v2, attr2);
			INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
			} else if (cstate == LS_FAMILY_TIMER) {
			lcdDrawNumberNAtt(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr1);
			lcdDrawNumberNAtt(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr2);
			v1_min = v2_min = -128;
			v1_max = v2_max = 122;
			INCDEC_SET_FLAG(EE_MODEL);
			} else {
			putsMixerSource(CSW_2ND_COLUMN, y, cs->v1, attr1);
			if (horz == 1) {
				INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
				} else {
				INCDEC_SET_FLAG(EE_MODEL);
			}
			#if   defined(FRSKY)
			if (cs->v1 >= MIXSRC_FIRST_TELEM) {
				lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, cs->v1 - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
				v2_max = maxTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1);
				if (cstate == LS_FAMILY_OFS) {
					v2_min = -128;
					v2_max -= 128;
					} else {
					v2_max = min((uint8_t)127, (uint8_t)v2_max);
					v2_min = -v2_max;
				}
				if (cs->v2 > v2_max) {
					cs->v2 = v2_max;
					eeDirty(EE_MODEL);
				}
				} else {
				lcdDrawNumberNAtt(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
				{
					v2_min = -LIMIT_EXT_PERCENT;
					v2_max = +LIMIT_EXT_PERCENT;
				}
			}
			#else
			if (cs->v1 >= MIXSRC_FIRST_TELEM) {
				lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, cs->v1 - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
				v2_min = -128;
				v2_max = 127;
				} else {
				lcdDrawNumberNAtt(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
				v2_min = -LIMIT_EXT_PERCENT;
				v2_max = +LIMIT_EXT_PERCENT;
			}
			#endif
		}

		// CSW AND switch
		lcdPutsSwitches(CSW_4TH_COLUMN, y, (cs->andsw), horz==LS_FIELD_ANDSW ? attr : 0);


		if ((s_editMode>0 || p1valdiff) && attr) {
			switch (horz) {
				case LS_FIELD_FUNCTION: {
					CHECK_INCDEC_MODELVAR_ZERO(event, cs->func, LS_FUNC_MAX);
					uint8_t new_cstate = lswFamily(cs->func);
					if (cstate != new_cstate) {
						cs->v1 = cs->v2 = (new_cstate==LS_FAMILY_TIMER ? -119/*1.0*/ : 0);
					}
					break;
				}
				case LS_FIELD_V1:
				cs->v1 = CHECK_INCDEC_PARAM(event, cs->v1, v1_min, v1_max);
				#if defined(SIMU) // Not needed but ...
				if (checkIncDec_Ret) TRACE("v1=%d", cs->v1);
				#endif
				break;
				case LS_FIELD_V2:
				cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
				#if defined(SIMU)
				if (checkIncDec_Ret) TRACE("v2=%d", cs->v2);
				#endif
				break;
				case LS_FIELD_ANDSW:
				CHECK_INCDEC_MODELVAR(event, cs->andsw, SWSRC_FIRST, SWSRC_LAST);
				#if defined(SIMU)
				if (checkIncDec_Ret) TRACE("andsw=%d", cs->andsw);
				#endif
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
///Model Custom Functions
//////////////////////////////////////////////////////////////////////////
void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext)
{
	int8_t sub = menuVerticalPosition - 1;

	uint8_t eeFlags = EE_MODEL;

	for (uint8_t i=0; i<LCD_LINES-1; i++) {
		coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
		uint8_t k = i+menuVerticalOffset;

		CustomFunctionData *cfn = &functions[k];
		uint8_t func = CFN_FUNC(cfn);
		for (uint8_t j=0; j<5; j++) {
			uint8_t attr = ((sub==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
			uint8_t active = (attr && (s_editMode>0 || p1valdiff));
			switch (j) {
				case 0:
				lcdPutsSwitches(MODEL_CUSTOM_FUNC_1ST_COLUMN, y, CFN_SWITCH(cfn), attr | ((functionsContext->activeSwitches & ((uint32_t)1 << k)) ? BOLD : 0));
				if (active || AUTOSWITCH_ENTER_LONG()) CHECK_INCDEC_SWITCH(event, CFN_SWITCH(cfn), SWSRC_FIRST, SWSRC_LAST, eeFlags, isSwitchAvailableInCustomFunctions);
				break;

				case 1:
				if (CFN_SWITCH(cfn)) {
					lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_2ND_COLUMN, y, STR_VFSWFUNC, func, attr);
					if (active) {
						CHECK_INCDEC_MODELVAR_ZERO(event, CFN_FUNC(cfn), FUNC_MAX-1);
						if (checkIncDec_Ret) CFN_RESET(cfn);
					}
					} else {
					j = 4; // skip other fields
					if (sub==k && menuHorizontalPosition > 0) {
						REPEAT_LAST_CURSOR_MOVE();
					}
				}
				break;

				case 2: {
					int8_t maxParam = NUM_CHNOUT-1;
					#if defined(OVERRIDE_CHANNEL_FUNCTION)
					if (func == FUNC_OVERRIDE_CHANNEL) {
						putsChn(lcdNextPos, y, CFN_CH_INDEX(cfn)+1, attr);
					} else
					#endif
					if (func == FUNC_TRAINER) {
						maxParam = 4;
						putsMixerSource(lcdNextPos, y, MIXSRC_Rud+CFN_CH_INDEX(cfn)-1, attr);
					}
					#if defined(GVARS)
					else if (func == FUNC_ADJUST_GVAR) {
						maxParam = MAX_GVARS-1;
						lcdDrawStringWithIndex(lcdNextPos, y, STR_GV, CFN_GVAR_INDEX(cfn)+1, attr);
						if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_GVAR_INDEX(cfn), maxParam);
						break;
					}
					#endif
					else if (attr) {
						REPEAT_LAST_CURSOR_MOVE();
					}
					if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_CH_INDEX(cfn), maxParam);
					break;
				}

				case 3: {
					INCDEC_DECLARE_VARS(eeFlags);
					int16_t val_displayed = CFN_PARAM(cfn);
					int8_t val_min = 0;
					uint8_t val_max = 255;
					if (func == FUNC_RESET) {
						val_max = FUNC_RESET_PARAM_LAST;
						lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VFSWRESET, CFN_PARAM(cfn), attr);
					}
					#if defined(OVERRIDE_CHANNEL_FUNCTION)
					else if (func == FUNC_OVERRIDE_CHANNEL) {
						val_displayed = (int8_t)CFN_PARAM(cfn);
						val_min = -LIMIT_EXT_PERCENT;
						val_max = +LIMIT_EXT_PERCENT;
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
					}
					#endif
					#if defined(AUDIO)
					else if (func == FUNC_PLAY_SOUND) {
						val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
						lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_FUNCSOUNDS, val_displayed, attr);
					}
					#endif
					#if defined(HAPTIC)
					else if (func == FUNC_HAPTIC) {
						val_max = 3;
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
					}
					#endif
					#if defined(VOICE)
					else if (func == FUNC_PLAY_TRACK) {
						#if defined(GVARS)
						if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
							killEvents(event);
							s_editMode = !s_editMode;
							active = true;
							val_displayed = (val_displayed > 250 ? 0 : 251);
						}
						if (val_displayed > 250) {
							lcdDrawStringWithIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed-250, attr);
							} else {
							lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
						}
						#else
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
						#endif

						#if defined(SDCARD)
						if (active)
						{
							lcdDrawTextAtt(0,0, PROMPTTEXT, BSS|INVERS|BLINK);
							if ((uint8_t)val_displayed == PROMPTMEM)
							{
								PROMPTMEM = 0; // PROMPTTEXT is OK
							}
							else
							{
								PROMPTMEM = (uint8_t)val_displayed;
							}
						}
						#endif

						} else if (func == FUNC_PLAY_BOTH) {
						lcdDrawCharAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, '|', attr);
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, val_displayed+PROMPT_CUSTOM_BASE, attr);
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+2+3*FWNUM, y, (val_displayed+PROMPT_CUSTOM_BASE+1)%10, attr|LEFT);
						} else if (func == FUNC_PLAY_VALUE) {
						val_max = MIXSRC_FIRST_TELEM + TELEM_DISPLAY_MAX - 1;
						putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
					}
					#endif
					#if defined(SDCARD)
					else if (func == FUNC_LOGS) {
						if (val_displayed) {
							lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed*10, attr|PREC1|LEFT);
							lcdDrawChar(lcdLastPos, y, 's');
							} else {
							lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_MMMINV, 0, attr);
						}
					}
					#endif
					#if defined(GVARS)
					else if (func == FUNC_ADJUST_GVAR) {
						switch (CFN_GVAR_MODE(cfn)) {
							case FUNC_ADJUST_GVAR_CONSTANT:
							val_displayed = (int16_t)CFN_PARAM(cfn);
							val_min = -CFN_GVAR_CST_MAX;
							val_max = +CFN_GVAR_CST_MAX;
							lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
							break;
							case FUNC_ADJUST_GVAR_SOURCE:
							val_max = MIXSRC_LAST_CH;
							putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
							INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
							break;
							case FUNC_ADJUST_GVAR_GVAR:
							val_max = MAX_GVARS-1;
							lcdDrawStringWithIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed+1, attr);
							break;
							default: // FUNC_ADJUST_GVAR_INC
							val_max = 1;
							lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, PSTR("\003-=1+=1"), val_displayed, attr);
							break;
						}

						if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
							killEvents(event);
							s_editMode = !s_editMode;
							active = true;
							CFN_GVAR_MODE(cfn) += 1;
							val_displayed = 0;
						}
					}
					#endif
					else if (attr) {
						REPEAT_LAST_CURSOR_MOVE();
					}

					if (active) {
						CFN_PARAM(cfn) = CHECK_INCDEC_PARAM(event, val_displayed, val_min, val_max);
					}
					break;
				}

				case 4:
				if (HAS_ENABLE_PARAM(func)) {
					menu_lcd_onoff(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF, y, CFN_ACTIVE(cfn), attr);
					if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_ACTIVE(cfn), 1);
					} else if (HAS_REPEAT_PARAM(func)) {
					if (CFN_PLAY_REPEAT(cfn) == 0) {
						lcdDrawCharAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF+3, y, '-', attr);
						} else {
						lcdDrawNumberNAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN+2+FW, y, CFN_PLAY_REPEAT(cfn)*CFN_PLAY_REPEAT_MUL, attr);
					}
					if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_PLAY_REPEAT(cfn), 60/CFN_PLAY_REPEAT_MUL);
					} else if (attr) {
					REPEAT_LAST_CURSOR_MOVE();
				}
				break;
			}
		}
	}
}

void menuModelCustomFunctions(uint8_t event)
{
	#if defined(VOICE) && defined(SDCARD)
	if (PROMPTMEM)
	{
		if (!loadVoiceTextLine(PROMPTMEM,PROMPTTEXT)) // Load the prompt text file if exist
		{
			PROMPTMEM = 0; // Reset on error
			PROMPTTEXT[0] = '\0';
		}
		SdBufferClear(); // Clear reused buffer
	}
	#endif

	MENU(STR_MENUCUSTOMFUNC, menuTabModel, e_CustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
	return menuCustomFunctions(event, g_model.customFn, &modelFunctionsContext);
}

#if defined(SDCARD) && defined(VOICE)
FORCEINLINE uint8_t loadVoiceTextLine(uint8_t Numline, char * PromptText)
{
	closeLogIfActived();
	uint8_t ret = false;

	if (sdChangeCurDir(VOICETXT_PATH))
	{
		if (sdFindFileStruct(VOICETXT_FILE))
		{
			SD_file = fat_open_file(SD_filesystem, &SD_dir_entry);
			int32_t seekofs = Numline*(VOICE_PROMPT_TEXT_LEN+2);
			if(fat_seek_file(SD_file, &seekofs, FAT_SEEK_SET))
			{
				if (fat_read_file(SD_file, (uint8_t*)PromptText, VOICE_PROMPT_TEXT_LEN) == VOICE_PROMPT_TEXT_LEN)
				{
					PromptText[VOICE_PROMPT_TEXT_LEN] = '\0';
					ret = true;
				}
			}
			fat_close_file(SD_file);
		}
	}
	return ret;
}
#endif

//////////////////////////////////////////////////////////////////////////
///Model Flight Modes
//////////////////////////////////////////////////////////////////////////
void displayFlightModes(coord_t x, coord_t y, FlightModesType value)
{
	uint8_t p = MAX_FLIGHT_MODES;
	do {
		--p;
		if (!(value & (1<<p)))
		lcdDrawChar(x, y, '0'+p);
		x -= FWNUM;
	} while (p!=0);
}

void menuModelPhaseOne(uint8_t event)
{
	FlightModeData *fm = flightModeAddress(s_currIdx);
	putsFlightMode(13*FW, 0, s_currIdx+1, (getFlightMode()==s_currIdx ? BOLD : 0));

	#if defined(GVARS)
	static const pm_uint8_t mstate_tab_fm1[] PROGMEM = {0, 0, 0, (uint8_t)-1, 1, 1, 1, 1, 1 , 1, 1, 1, 1, 1, 1, 1};
	static const pm_uint8_t mstate_tab_others[] PROGMEM = {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0, 0, (uint8_t)-1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

	check(event, 0, NULL, 0, (s_currIdx == 0) ? mstate_tab_fm1 : mstate_tab_others, DIM(mstate_tab_others)-1, ITEM_MODEL_PHASE_MAX - 1 - (s_currIdx==0 ? (ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH) : 0));

	TITLE(STR_MENUFLIGHTPHASE);

	#define PHASE_ONE_FIRST_LINE (1+1*FH)
	#else
	SUBMENU(STR_MENUFLIGHTPHASE, 3 + (s_currIdx==0 ? 0 : 2 + (bool)NUM_ROTARY_ENCODERS), {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0/*, 0*/});
	#define PHASE_ONE_FIRST_LINE (1+1*FH)
	#endif

	int8_t sub = menuVerticalPosition;
	int8_t editMode = s_editMode;

	#if defined(GVARS)
	if (s_currIdx == 0 && sub>=ITEM_MODEL_PHASE_SWITCH) sub += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;

	for (uint8_t k=0; k<LCD_LINES-1; k++) 
	{
		coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
		int8_t i = k + menuVerticalOffset;
		if (s_currIdx == 0 && i>=ITEM_MODEL_PHASE_SWITCH) i += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;
		uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
		#else
		for (uint8_t i=0, k=0, y=PHASE_ONE_FIRST_LINE; i<ITEM_MODEL_PHASE_MAX; i++, k++, y+=FH) 
		{
			if (s_currIdx == 0 && i==ITEM_MODEL_PHASE_SWITCH) i = ITEM_MODEL_PHASE_FADE_IN;
			uint8_t attr = (sub==k ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
			#endif
			switch(i) 
			{
				case ITEM_MODEL_PHASE_NAME:
				editSingleName(MIXES_2ND_COLUMN, y, STR_NAME, fm->name, sizeof(fm->name), event, attr, EE_MODEL, RANGE_NONE);
				break;
				case ITEM_MODEL_PHASE_SWITCH:
				fm->swtch = switchMenuItem(MIXES_2ND_COLUMN, y, fm->swtch, attr, event);
				break;
				case ITEM_MODEL_PHASE_TRIMS:
				lcdDrawTextLeft(y, STR_TRIMS);
				for (uint8_t t=0; t<NUM_STICKS; t++) 
				{
					lcdPutsTrimMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, menuHorizontalPosition==t ? attr : 0);
					if (attr && menuHorizontalPosition==t && ((editMode>0) || p1valdiff)) 
					{
						int16_t v = getRawTrimValue(s_currIdx, t);
						if (v < TRIM_EXTENDED_MAX) v = TRIM_EXTENDED_MAX;
						v = checkIncDec(event, v, TRIM_EXTENDED_MAX, TRIM_EXTENDED_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
						if (checkIncDec_Ret) 
						{
							if (v == TRIM_EXTENDED_MAX) v = 0;
							setTrimValue(s_currIdx, t, v);
						}
					}
				}
				break;

				#if ROTARY_ENCODERS > 0
				case ITEM_MODEL_PHASE_ROTARY_ENCODERS:
				lcdDrawTextLeft(y, STR_ROTARY_ENCODER);
				for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) 
				{
					lcdPutsRotaryEncoderMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, menuHorizontalPosition==t ? attr : 0);
					if (attr && menuHorizontalPosition==t && ((editMode>0) || p1valdiff)) 
					{
						int16_t v = flightModeAddress(s_currIdx)->rotaryEncoders[t];
						if (v < ROTARY_ENCODER_MAX) v = ROTARY_ENCODER_MAX;
						v = checkIncDec(event, v, ROTARY_ENCODER_MAX, ROTARY_ENCODER_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
						if (checkIncDec_Ret) 
						{
							if (v == ROTARY_ENCODER_MAX) v = 0;
							flightModeAddress(s_currIdx)->rotaryEncoders[t] = v;
						}
					}
				}
				break;
				#endif

				case ITEM_MODEL_PHASE_FADE_IN:
				fm->fadeIn = EDIT_DELAY(0, y, event, attr, STR_FADEIN, fm->fadeIn);
				break;

				case ITEM_MODEL_PHASE_FADE_OUT:
				fm->fadeOut = EDIT_DELAY(0, y, event, attr, STR_FADEOUT, fm->fadeOut);
				break;

				#if defined(GVARS)
				case ITEM_MODEL_PHASE_GVARS_LABEL:
				lcdDrawTextLeft(y, STR_GLOBAL_VARS);
				break;

				default: 
				{
					uint8_t idx = i-ITEM_MODEL_PHASE_GV1;
					uint8_t posHorz = menuHorizontalPosition;
					if (attr && posHorz > 0 && s_currIdx==0) posHorz++;

					lcdDrawStringWithIndex(INDENT_WIDTH, y, STR_GV, idx+1);

					editName(4*FW+2, y, g_model.gvars[idx].name, LEN_GVAR_NAME, event, posHorz==0 ? attr : 0, EE_MODEL, RANGE_NONE);

					int16_t v = fm->gvars[idx];
					if (v > GVAR_MAX) {
						uint8_t p = v - GVAR_MAX - 1;
						if (p >= s_currIdx) p++;
						putsFlightMode(11*FW, y, p+1, posHorz==1 ? attr : 0);
						} else {
						lcdDrawTextAtt(11*FW, y, STR_OWN, posHorz==1 ? attr : 0);
					}
					if (attr && s_currIdx>0 && posHorz==1 && (editMode>0 || p1valdiff)) {
						if (v < GVAR_MAX) v = GVAR_MAX;
						v = checkIncDec(event, v, GVAR_MAX, GVAR_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
						if (checkIncDec_Ret) {
							if (v == GVAR_MAX) v = 0;
							fm->gvars[idx] = v;
						}
					}

					uint8_t p = getGVarFlightPhase(s_currIdx, idx);
					lcdDrawNumberNAtt(21*FW, y, GVAR_VALUE(idx, p), posHorz==2 ? attr : 0);
					if (attr && posHorz==2 && ((editMode>0) || p1valdiff)) {
						GVAR_VALUE(idx, p) = checkIncDec(event, GVAR_VALUE(idx, p), -GVAR_LIMIT, GVAR_LIMIT, EE_MODEL);
					}

					break;
				}
				#endif
			}
		}
	}
	
	
void menuModelFlightModesAll(uint8_t event)
{
	SIMPLE_MENU(STR_MENUFLIGHTPHASES, menuTabModel, e_FlightModesAll, 1+MAX_FLIGHT_MODES+1);

	int8_t sub = menuVerticalPosition - 1;

	switch (event) {
		CASE_EVT_ROTARY_BREAK
		case EVT_KEY_FIRST(KEY_ENTER):
		if (sub == MAX_FLIGHT_MODES) {
			s_editMode = 0;
			trimsCheckTimer = 200; // 2 seconds
		}
		// no break
		case EVT_KEY_FIRST(KEY_RIGHT):
		if (sub >= 0 && sub < MAX_FLIGHT_MODES) {
			s_currIdx = sub;
			pushMenu(menuModelPhaseOne);
		}
		break;
	}

	for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
		uint8_t y = 1 + (i+1)*FH;
		uint8_t att = (i==sub ? INVERS : 0);
		FlightModeData *p = flightModeAddress(i);
		putsFlightMode(0, y, i+1, att|(getFlightMode()==i ? BOLD : 0));

		lcdDrawSizedTextAtt(4*FW+NAME_OFS, y, p->name, sizeof(p->name), ZCHAR);
		if (i == 0) {
			lcdDrawText((5+LEN_FLIGHT_MODE_NAME)*FW+SWITCH_OFS, y, STR_DEFAULT);
			} else {
			lcdPutsSwitches((5+LEN_FLIGHT_MODE_NAME)*FW+SWITCH_OFS, y, p->swtch, 0);
			for (uint8_t t=0; t<NUM_STICKS; t++) {
				lcdPutsTrimMode((9+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS, y, i, t, 0);
			}
			#ifdef ROTARY_ENCODERS
			for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
				lcdPutsRotaryEncoderMode((13+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS+ROTARY_ENC_OFS, y, i, t, 0);
			}
			#endif
		}

		if (p->fadeIn || p->fadeOut) {
			lcdDrawChar(LCD_W-FW-MENUS_SCROLLBAR_WIDTH, y, (p->fadeIn && p->fadeOut) ? '*' : (p->fadeIn ? 'I' : 'O'));
		}
	}


	lcdDrawTextLeft((LCD_LINES-1)*FH+1, STR_CHECKTRIMS);
	putsFlightMode(OFS_CHECKTRIMS, (LCD_LINES-1)*FH+1, mixerCurrentFlightMode+1);
	if (sub==MAX_FLIGHT_MODES && !trimsCheckTimer) {
		lcd_status_line();
	}
}


//////////////////////////////////////////////////////////////////////////
///Model MIXES
//////////////////////////////////////////////////////////////////////////
#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value);
FlightModesType editFlightModes(coord_t x, coord_t y, uint8_t event, FlightModesType value, uint8_t attr)
{
	lcdDrawTextColumnLeft(x, y, STR_FLMODE);

	uint8_t posHorz = menuHorizontalPosition;

	for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
		lcdDrawCharAtt(x, y, '0'+p, ((posHorz==p) && attr) ? BLINK|INVERS : ((value & (1<<p)) ? 0 : INVERS));
		x += FW;
	}

	if (attr) {
		if (s_editMode && ((event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff))) {
			s_editMode = 0;
			value ^= (1<<posHorz);
			eeDirty(EE_MODEL);
		}
	}

	return value;
}
#else
#define displayFlightModes(...)
#endif

static uint8_t s_maxLines = 8;
static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;
int8_t s_currCh;

int16_t expoFn(int16_t x)
{
	ExpoData *ed = expoAddress(s_currIdx);
	int16_t anas[NUM_INPUTS] = {0};
	anas[ed->chn] = x;
	applyExpos(anas, e_perout_mode_inactive_flight_mode);
	return anas[ed->chn];
}

void DrawFunction(FnFuncP fn, uint8_t offset)
{
	lcdDrawSolidVerticalLineStip(X0-offset, 0, LCD_H, 0xee);
	lcdDrawSolidHorizontalLineStip(X0-WCHART-offset, Y0, WCHART*2, 0xee);

	coord_t prev_yv = (coord_t)-1;

	for (int8_t xv=-WCHART; xv<=WCHART; xv++) {
		coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/WCHART))) / 2 * (LCD_H-1) / RESX);
		if (prev_yv != (coord_t)-1) {
			if (abs((int8_t)yv-prev_yv) <= 1) {
				lcdDrawPoint(X0+xv-offset-1, prev_yv, FORCE);
				} else {
				uint8_t tmp = (prev_yv < yv ? 0 : 1);
				lcdDrawSolidVerticalLine(X0+xv-offset-1, yv+tmp, prev_yv-yv);
			}
		}
		prev_yv = yv;
	}
}

uint8_t getExpoMixCount(uint8_t expo)
{
	uint8_t count = 0;

	for(int8_t i=(expo ? MAX_EXPOS-1 : MAX_MIXERS-1); i>=0; i--) {
		uint8_t ch = (expo ? EXPO_VALID(expoAddress(i)) : mixAddress(i)->srcRaw);
		if (ch != 0) {
			count++;
		}
	}
	return count;
}

bool reachExpoMixCountLimit(uint8_t expo)
{
	// check mixers count limit
	if (getExpoMixCount(expo) >= (expo ? MAX_EXPOS : MAX_MIXERS)) {
		POPUP_WARNING(expo ? STR_NOFREEEXPO : STR_NOFREEMIXER);
		return true;
	}
	return false;
}

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
	if (expo) {
		ExpoData *expo = expoAddress(idx);
		memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
		memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
		} else {
		MixData *mix = mixAddress(idx);
		memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
		memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
	}
	eeDirty(EE_MODEL);
}

void insertExpoMix(uint8_t expo, uint8_t idx)
{
	if (expo) {
		ExpoData *expo = expoAddress(idx);
		memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
		memclear(expo, sizeof(ExpoData));
		expo->mode = 3; // pos&neg
		expo->chn = s_currCh - 1;
		expo->weight = 100;
		} else {
		MixData *mix = mixAddress(idx);
		memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
		memclear(mix, sizeof(MixData));
		mix->destCh = s_currCh-1;
		mix->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
		mix->weight = 100;
	}
	eeDirty(EE_MODEL);
}

void copyExpoMix(uint8_t expo, uint8_t idx)
{
	if (expo) {
		ExpoData *expo = expoAddress(idx);
		memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
		} else {
		MixData *mix = mixAddress(idx);
		memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
	}
	eeDirty(EE_MODEL);
}

void memswap(void *a, void *b, uint8_t size)
{
	uint8_t *x = (uint8_t*)a;
	uint8_t *y = (uint8_t*)b;

	while (size--) {
		uint8_t temp = *x;
		*x++ = *y;
		*y++ = temp;
	}
}

bool swapExpoMix(uint8_t expo, uint8_t &idx, uint8_t up)
{
	void *x, *y;
	uint8_t size;
	int8_t tgt_idx = (up ? idx-1 : idx+1);

	if (expo) {
		x = (ExpoData *)expoAddress(idx);

		if (tgt_idx < 0) {
			if (((ExpoData *)x)->chn == 0)
			return false;
			((ExpoData *)x)->chn--;
			return true;
		}

		if (tgt_idx == MAX_EXPOS) {
			if (((ExpoData *)x)->chn == NUM_INPUTS-1)
			return false;
			((ExpoData *)x)->chn++;
			return true;
		}

		y = (ExpoData *)expoAddress(tgt_idx);
		if(((ExpoData *)x)->chn != ((ExpoData *)y)->chn || !EXPO_VALID((ExpoData *)y)) {
			if (up) {
				if (((ExpoData *)x)->chn>0) ((ExpoData *)x)->chn--;
				else return false;
				} else {
				if (((ExpoData *)x)->chn<NUM_INPUTS-1) ((ExpoData *)x)->chn++;
				else return false;
			}
			return true;
		}

		size = sizeof(ExpoData);
		} else {
		x = (MixData *)mixAddress(idx);

		if (tgt_idx < 0) {
			if (((MixData *)x)->destCh == 0)
			return false;
			((MixData *)x)->destCh--;
			return true;
		}

		if (tgt_idx == MAX_MIXERS) {
			if (((MixData *)x)->destCh == NUM_CHNOUT-1)
			return false;
			((MixData *)x)->destCh++;
			return true;
		}

		y = (MixData *)mixAddress(tgt_idx);
		uint8_t destCh = ((MixData *)x)->destCh;
		if(!((MixData *)y)->srcRaw || destCh != ((MixData *)y)->destCh) {
			if (up) {
				if (destCh>0) ((MixData *)x)->destCh--;
				else return false;
				} else {
				if (destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
				else return false;
			}
			return true;
		}

		size = sizeof(MixData);
	}
	memswap(x, y, size);
	idx = tgt_idx;
	return true;
}

void menuModelExpoOne(uint8_t event)
{
	ExpoData *ed = expoAddress(s_currIdx);
	putsMixerSource(7*FW+FW/2, 0, MIXSRC_Rud+ed->chn, 0);

	SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, {0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

	SET_SCROLLBAR_X(EXPO_ONE_2ND_COLUMN+10*FW);

	int8_t sub = menuVerticalPosition;

	coord_t y = MENU_HEADER_HEIGHT + 1;

	for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
		uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
		switch(i) {

			case EXPO_FIELD_WEIGHT:
			lcdDrawTextLeft(y, STR_WEIGHT);
			ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr, 0, event);
			break;

			case EXPO_FIELD_EXPO:
			lcdDrawTextLeft(y, STR_EXPO);
			if (ed->curveMode==MODE_EXPO || ed->curveParam==0) {
				ed->curveMode = MODE_EXPO;
				ed->curveParam = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->curveParam, -100, 100, attr, 0, event);
				} else {
				lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
			}
			break;

			#if defined(CURVES)
			case EXPO_FIELD_CURVE:
			lcdDrawTextLeft(y, STR_CURVE);
			if (ed->curveMode!=MODE_EXPO || ed->curveParam==0) {
				lcdDrawCurveName(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->curveParam, attr);
				if (attr) {
					CHECK_INCDEC_MODELVAR_ZERO(event, ed->curveParam, CURVE_BASE+MAX_CURVES-1);
					if (ed->curveParam) ed->curveMode = MODE_CURVE;
					if (ed->curveParam>=CURVE_BASE && event==EVT_KEY_LONG(KEY_ENTER)) {
						s_curveChan = ed->curveParam - CURVE_BASE;
						pushMenu(menuModelCurveOne);
					}
				}
				} else {
				lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
			}
			break;
			#endif

			#if defined(FLIGHT_MODES)
			case EXPO_FIELD_FLIGHT_MODES:
			ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN-EXPO_ONE_FM_WIDTH, y, event, ed->flightModes, attr);
			break;
			#endif

			case EXPO_FIELD_SWITCH:
			ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->swtch, attr, event);
			break;

			case EXPO_FIELD_SIDE:
			ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
			break;
		}
		y += FH;
	}

	DrawFunction(expoFn);

	int16_t x512 = calibratedStick[ed->chn];
	lcdDrawNumberNAtt(LCD_W-8, 6*FH, calcRESXto100(x512), 0);
	int16_t y512 = expoFn(x512);
	lcdDrawNumberNAtt(LCD_W-8-6*FW, 1*FH, calcRESXto100(y512), 0);

	x512 = X0+x512/(RESXu/WCHART);
	y512 = (LCD_H-1) - (uint16_t)((y512+RESX)/2) * (LCD_H-1) / RESX;

	lcdDrawSolidVerticalLine(x512, y512-3, 3*2+1);
	lcdDrawSolidHorizontalLine(x512-3, y512, 3*2+1);
}

void gvarWeightItem(coord_t x, coord_t y, MixData *md, uint8_t attr, uint8_t event)
{
	u_int8int16_t weight;
	MD_WEIGHT_TO_UNION(md, weight);
	weight.gvword = GVAR_MENU_ITEM(x, y, weight.gvword, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
	MD_UNION_TO_WEIGHT(weight, md);
}

void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
	int16_t offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
	int16_t weight = abs(GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode));
	int16_t barMin = 0;
	int16_t barMax = 0;

	#if defined(OFFSET_ON_INPUT)
	//Offset on input (before weight)
	barMin = (-100 + offset) * weight / 100;
	barMax = (+100 + offset) * weight / 100;

	#else
	//Offset on output (after weight)
	barMin = offset - weight;
	barMax = offset + weight;

	#endif
	if (y > 15) {
		lcdDrawNumberNAtt(x-((barMin >= 0) ? 2 : 3), y-8, barMin, LEFT);
		lcdDrawNumberNAtt(x+GAUGE_WIDTH+1, y-8, barMax);
	}
	if (barMin < -101)
	barMin = -101;
	if (barMax > 101)
	barMax = 101;
	lcdDrawSolidHorizontalLineStip(x-2, y, GAUGE_WIDTH+2, DOTTED);
	lcdDrawSolidHorizontalLineStip(x-2, y+GAUGE_HEIGHT, GAUGE_WIDTH+2, DOTTED);
	lcdDrawSolidVerticalLine(x-2, y+1, GAUGE_HEIGHT-1);
	lcdDrawSolidVerticalLine(x+GAUGE_WIDTH-1, y+1, GAUGE_HEIGHT-1);
	if (barMin <= barMax) {
		int8_t right = (barMax * GAUGE_WIDTH) / 200;
		int8_t left = ((barMin * GAUGE_WIDTH) / 200)-1;
		lcdDrawFilledRect(x+GAUGE_WIDTH/2+left, y+2, right-left, GAUGE_HEIGHT-3);
	}
	lcdDrawSolidVerticalLine(x+GAUGE_WIDTH/2-1, y, GAUGE_HEIGHT+1);
	if (barMin == -101) {
		for (uint8_t i=0; i<3; ++i) {
			lcdDrawPoint(x+i, y+4-i);
			lcdDrawPoint(x+3+i, y+4-i);
		}
	}
	if (barMax == 101) {
		for (uint8_t i=0; i<3; ++i) {
			lcdDrawPoint(x+GAUGE_WIDTH-8+i, y+4-i);
			lcdDrawPoint(x+GAUGE_WIDTH-5+i, y+4-i);
		}
	}
}

void menuModelMixOne(uint8_t event)
{
	TITLE(s_currCh ? STR_INSERTMIX : STR_EDITMIX);
	MixData *md2 = mixAddress(s_currIdx) ;
	putsChn(lcdLastPos+1*FW, 0, md2->destCh+1,0);

	#if defined(ROTARY_ENCODERS)
	#if defined(CURVES)
	if ((menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS) || (menuVerticalPosition == MIX_FIELD_CURVE && md2->curveMode == MODE_CURVE))
	#else
	if (menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS)
	#endif
	SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 0, CASE_CURVES(0) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/})
	else
	SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
	#else
	SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
	#endif

	#if MENU_COLUMNS > 1
	lcdDrawSolidVerticalLine(MENU_COLUMN2_X-4, FH+1, LCD_H-FH-1);
	#endif

	int8_t sub = menuVerticalPosition;
	int8_t editMode = s_editMode;

	for (uint8_t k=0; k<MENU_COLUMNS*(LCD_LINES-1); k++) {

		#if MENU_COLUMNS > 1
		coord_t y;
		coord_t COLUMN_X;
		if (k >= LCD_LINES-1) {
			y = 1 + (k-LCD_LINES+2)*FH;
			COLUMN_X = MENU_COLUMN2_X;
			} else {
			y = 1 + (k+1)*FH;
			COLUMN_X = 0;
		}
		int8_t i = k;
		#else
		coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
		int8_t i = k + menuVerticalOffset;
		#endif

		uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
		switch(i) {

			case MIX_FIELD_SOURCE:
			lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_SOURCE));
			putsMixerSource(COLUMN_X+MIXES_2ND_COLUMN, y, md2->srcRaw, attr);
			if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
			break;
			case MIX_FIELD_WEIGHT:
			lcdDrawTextColumnLeft(COLUMN_X, y, STR_WEIGHT);
			gvarWeightItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
			break;
			case MIX_FIELD_OFFSET: {
				lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_OFFSET));
				u_int8int16_t offset;
				MD_OFFSET_TO_UNION(md2, offset);
				offset.gvword = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN, y, offset.gvword, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
				MD_UNION_TO_OFFSET(offset, md2);
				drawOffsetBar(COLUMN_X+MIXES_2ND_COLUMN+22, y, md2);
				break;
			}
			case MIX_FIELD_TRIM: {
				uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
				int8_t carryTrim = -md2->carryTrim;
				lcdDrawTextColumnLeft(COLUMN_X, y, STR_TRIM);
				lcdDrawTextAtIndex((not_stick ? COLUMN_X+MIXES_2ND_COLUMN : COLUMN_X+6*FW-3), y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
				if (attr && menuHorizontalPosition==0 && (not_stick || editMode>0)) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
				if (!not_stick) {
					lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, STR_DREX);
					menu_lcd_onoff(COLUMN_X+MIXES_2ND_COLUMN+DREX_CHBOX_OFFSET, y, !md2->noExpo, menuHorizontalPosition==1 ? attr : 0);
					if (attr && menuHorizontalPosition==1 && editMode>0) md2->noExpo = !checkIncDecModel(event, !md2->noExpo, 0, 1);
					} else if (attr) {
					REPEAT_LAST_CURSOR_MOVE();
				}
				break;
			}

			#if defined(CURVES)
			case MIX_FIELD_CURVE: {
				lcdDrawTextColumnLeft(COLUMN_X, y, STR_CURVE);
				int8_t curveParam = md2->curveParam;
				if (md2->curveMode == MODE_CURVE) {
					lcdDrawCurveName(COLUMN_X+MIXES_2ND_COLUMN, y, curveParam, attr);
					if (attr) {
						if (event==EVT_KEY_LONG(KEY_ENTER) && (curveParam<0 || curveParam>=CURVE_BASE)) {
							s_curveChan = (curveParam<0 ? -curveParam-1 : curveParam-CURVE_BASE);
							pushMenu(menuModelCurveOne);
							} else {
							CHECK_INCDEC_MODELVAR(event, md2->curveParam, -MAX_CURVES, CURVE_BASE+MAX_CURVES-1);
							if (md2->curveParam == 0)
							md2->curveMode = MODE_DIFFERENTIAL;
						}
					}
					} else {
					lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, PSTR("Diff"), menuHorizontalPosition==0 ? attr : 0);
					md2->curveParam = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN+5*FW, y, curveParam, -100, 100, LEFT|(menuHorizontalPosition==1 ? attr : 0), 0, editMode>0 ? event : 0);
					if (attr && editMode>0 && menuHorizontalPosition==0) {
						int8_t tmp = 0;
						CHECK_INCDEC_MODELVAR(event, tmp, -1, 1);
						if (tmp != 0) {
							md2->curveMode = MODE_CURVE;
							md2->curveParam = tmp;
						}
					}
				}
				break;
			}
			#endif
			#if defined(FLIGHT_MODES)
			case MIX_FIELD_FLIGHT_PHASE:
			md2->flightModes = editFlightModes(COLUMN_X+MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
			break;
			#endif
			case MIX_FIELD_SWITCH:
			md2->swtch = switchMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
			break;
			case MIX_FIELD_WARNING:
			lcdDrawTextColumnLeft(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
			if (md2->mixWarn)
			lcdDrawNumberNAtt(COLUMN_X+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
			else
			lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, STR_OFF, attr);
			if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
			break;
			case MIX_FIELD_MLTPX:
			md2->mltpx = selectMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
			break;
			case MIX_FIELD_DELAY_UP:
			md2->delayUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYUP, md2->delayUp);
			break;
			case MIX_FIELD_DELAY_DOWN:
			md2->delayDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYDOWN, md2->delayDown);
			break;
			case MIX_FIELD_SLOW_UP:
			md2->speedUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWUP, md2->speedUp);
			break;
			case MIX_FIELD_SLOW_DOWN:
			md2->speedDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWDOWN, md2->speedDown);
			break;
		}
	}
}

#if defined(NAVIGATION_MENUS)
void onExpoMixMenu(const char *result)
{
	bool expo = (menuHandlers[menuLevel] == menuModelExposAll);
	uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

	if (result == STR_EDIT) {
		pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
		} else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
		if (!reachExpoMixCountLimit(expo)) {
			s_currCh = chn;
			if (result == STR_INSERT_AFTER) {
				s_currIdx++;
				menuVerticalPosition++;
			}
			insertExpoMix(expo, s_currIdx);
			pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
		}
		} else if (result == STR_COPY || result == STR_MOVE) {
		s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
		s_copySrcIdx = s_currIdx;
		s_copySrcCh = chn;
		s_copySrcRow = menuVerticalPosition;
		} else if (result == STR_DELETE) {
		deleteExpoMix(expo, s_currIdx);
	}
}
#endif

void displayMixInfos(coord_t y, MixData *md)
{
	if (md->curveParam) {
		if (md->curveMode == MODE_CURVE)
		lcdDrawCurveName(MIX_LINE_CURVE_POS, y, md->curveParam);
		else
		displayGVar(MIX_LINE_CURVE_POS+3*FW, y, md->curveParam, -100, 100);
	}

	if (md->swtch) {
		lcdPutsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
	}
}

void displayExpoInfos(coord_t y, ExpoData *ed)
{
	if (ed->curveMode == MODE_CURVE)
	lcdDrawCurveName(EXPO_LINE_EXPO_POS-3*FW, y, ed->curveParam);
	else
	displayGVar(EXPO_LINE_EXPO_POS, y, ed->curveParam, -100, 100);

	lcdPutsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0);
}

void menuModelExpoMix(uint8_t expo, uint8_t event)
{
	uint8_t sub = menuVerticalPosition;

	if (s_editMode > 0)
	s_editMode = 0;

	uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

	switch (event) {
		case EVT_ENTRY:
		case EVT_ENTRY_UP:
		s_copyMode = 0;
		s_copyTgtOfs = 0;
		break;
		case EVT_KEY_LONG(KEY_EXIT):
		if (s_copyMode && s_copyTgtOfs == 0) {
			deleteExpoMix(expo, s_currIdx);
			killEvents(event);
			event = 0;
		}
		// no break
		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_LONG:
		if (s_copyMode) {
			killEvents(event);
		}
		#endif
		case EVT_KEY_BREAK(KEY_EXIT):
		if (s_copyMode) {
			if (s_copyTgtOfs) {
				// cancel the current copy / move operation
				if (s_copyMode == COPY_MODE) {
					deleteExpoMix(expo, s_currIdx);
					} else {
					do {
						swapExpoMix(expo, s_currIdx, s_copyTgtOfs > 0);
						s_copyTgtOfs += (s_copyTgtOfs < 0 ? +1 : -1);
					} while (s_copyTgtOfs != 0);
					eeDirty(EE_MODEL);
				}
				menuVerticalPosition = s_copySrcRow;
				s_copyTgtOfs = 0;
			}
			s_copyMode = 0;
			event = 0;
		}
		break;
		case EVT_KEY_BREAK(KEY_ENTER):
		if (sub != 0 && (!s_currCh || (s_copyMode && !s_copyTgtOfs)) && !READ_ONLY()) {
			s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
			s_copySrcIdx = s_currIdx;
			s_copySrcCh = chn;
			s_copySrcRow = sub;
			break;
		}
		// no break

		CASE_EVT_ROTARY_BREAK
		case EVT_KEY_LONG(KEY_ENTER):
		killEvents(event);
		if (s_copyTgtOfs) {
			s_copyMode = 0;
			s_copyTgtOfs = 0;
			} else if (sub != 0) {
			if (READ_ONLY()) {
				if (!s_currCh) {
					pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
					return;
				}
				} else {
				if (s_copyMode) s_currCh = 0;
				#if defined(NAVIGATION_MENUS)
				if (s_currCh) {
					if (reachExpoMixCountLimit(expo)) break;
					insertExpoMix(expo, s_currIdx);
					pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
					s_copyMode = 0;
					return;
					} else {
					event = 0;
					s_copyMode = 0;
					POPUP_MENU_ADD_ITEM(STR_EDIT);
					POPUP_MENU_ADD_ITEM(STR_INSERT_BEFORE);
					POPUP_MENU_ADD_ITEM(STR_INSERT_AFTER);
					POPUP_MENU_ADD_ITEM(STR_COPY);
					POPUP_MENU_ADD_ITEM(STR_MOVE);
					POPUP_MENU_ADD_ITEM(STR_DELETE);
					popupMenuHandler = onExpoMixMenu;
				}
				#else
				if (s_currCh) {
					if (reachExpoMixCountLimit(expo)) break;
					insertExpoMix(expo, s_currIdx);
				}
				pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
				s_copyMode = 0;
				return;
				#endif
			}
		}
		break;
		case EVT_KEY_LONG(KEY_LEFT):
		case EVT_KEY_LONG(KEY_RIGHT):
		if (s_copyMode && !s_copyTgtOfs) {
			if (reachExpoMixCountLimit(expo)) break;
			s_currCh = chn;
			if (event == EVT_KEY_LONG(KEY_RIGHT)) {
				s_currIdx++;
				menuVerticalPosition++;
			}
			insertExpoMix(expo, s_currIdx);
			pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
			s_copyMode = 0;
			killEvents(event);
			return;
		}
		break;
		#if defined(ROTARY_ENCODER_NAVIGATION)
		case EVT_ROTARY_LEFT:
		case EVT_ROTARY_RIGHT:
		#endif
		case EVT_KEY_FIRST(KEY_MOVE_UP):
		case EVT_KEY_REPT(KEY_MOVE_UP):
		case EVT_KEY_FIRST(KEY_MOVE_DOWN):
		case EVT_KEY_REPT(KEY_MOVE_DOWN):
		if (s_copyMode) {
			uint8_t key = (event & 0x1f);
			uint8_t next_ofs = ((IS_ROTARY_UP(event) || key==KEY_MOVE_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

			if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
				// insert a mix on the same channel (just above / just below)
				if (reachExpoMixCountLimit(expo)) break;
				copyExpoMix(expo, s_currIdx);
				if (IS_ROTARY_DOWN(event) || key==KEY_MOVE_DOWN) s_currIdx++;
				else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
				} else if (next_ofs==0 && s_copyMode==COPY_MODE) {
				// delete the mix
				deleteExpoMix(expo, s_currIdx);
				if (IS_ROTARY_UP(event) || key==KEY_MOVE_UP) s_currIdx--;
				} else {
				// only swap the mix with its neighbor
				if (!swapExpoMix(expo, s_currIdx, IS_ROTARY_UP(event) || key==KEY_MOVE_UP)) break;
				eeDirty(EE_MODEL);
			}

			s_copyTgtOfs = next_ofs;
		}
		break;
	}

	lcdDrawNumberNAtt(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, getExpoMixCount(expo));
	lcdDrawText(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));

	SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? e_InputsAll : e_MixAll, s_maxLines);

	sub = menuVerticalPosition;
	s_currCh = 0;
	uint8_t cur = 1;
	uint8_t i = 0;

	for (uint8_t ch=1; ch<=(expo ? NUM_INPUTS : NUM_CHNOUT); ch++) {
		void *pointer = NULL;
		MixData * &md = (MixData * &)pointer;
		ExpoData * &ed = (ExpoData * &)pointer;
		coord_t y = MENU_HEADER_HEIGHT-FH+1+(cur-menuVerticalOffset)*FH;
		if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
			if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
				if (expo) {
					putsMixerSource(0, y, MIXSRC_Rud+ch-1, 0);
					} else {
					putsChn(0, y, ch, 0); // show CHx
				}
			}
			uint8_t mixCnt = 0;
			do {
				if (s_copyMode) {
					if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < 8 && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
						lcdDrawRect(expo ? 18 : 22, y-1, expo ? LCD_W-18 : LCD_W-22, 9, DOTTED);
						cur++;
						y+=FH;
					}
					if (s_currIdx == i) {
						sub = menuVerticalPosition = cur;
						s_currCh = ch;
					}
					} else if (sub == cur) {
					s_currIdx = i;
				}
				if (menuVerticalOffset < cur && cur-menuVerticalOffset < 8) {
					uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);
					if (expo) {
						ed->weight = GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, event);
						displayExpoInfos(y, ed);
						displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes);
						
						if (ed->mode!=3) {
							lcdDrawChar(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? 126 : 127);
						}
						} else {
						if (mixCnt > 0) lcdDrawTextAtIndex(FW, y, STR_VMLTPX2, md->mltpx, 0);

						putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw, 0);

						gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

						displayMixInfos(y, md);

						char cs = ' ';
						if (md->speedDown || md->speedUp)
						cs = 'S';
						if (md->delayUp || md->delayDown)
						cs = (cs =='S' ? '*' : 'D');
						lcdDrawChar(MIX_LINE_DELAY_POS, y, cs);
					}
					if (s_copyMode) {
						if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
							/* draw a border around the raw on selection mode (copy/move) */
							lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
						}
						if (cur == sub) {
							/* invert the raw when it's the current one */
							lcdDrawFilledRect(expo ? EXPO_LINE_SELECT_POS+1 : 23, y, expo ? (LCD_W-EXPO_LINE_SELECT_POS-2) : (LCD_W-24), 7);
						}
					}
				}
				cur++;
				y+=FH;
				mixCnt++;
				i++;
				if (expo) ed++;
				else md++;
			} while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
			if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
				lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? LCD_W-EXPO_LINE_SELECT_POS : LCD_W-22, 9, DOTTED);
				cur++;
				y+=FH;
			}
			} else {
			uint8_t attr = 0;
			if (sub == cur) {
				s_currIdx = i;
				s_currCh = ch;
				if (!s_copyMode) {
					attr = INVERS;
				}
			}
			if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
				if (expo) {
					putsMixerSource(0, y, MIXSRC_Rud+ch-1, attr);
					} else {
					putsChn(0, y, ch, attr); // show CHx
				}
				if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
					lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, DOTTED);
				}
			}
			cur++;
			y+=FH;
		}
	}
	s_maxLines = cur;
	if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}

void menuModelExposAll(uint8_t event)
{
	return menuModelExpoMix(1, event);
}

void menuModelMixAll(uint8_t event)
{
	return menuModelExpoMix(0, event);
}
