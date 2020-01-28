/*
 * menu_model.h
 *
 * Created: 1/27/2020 7:57:43 PM
 *  Author: Andrew
 */ 


#ifndef MENU_MODEL_H_
#define MENU_MODEL_H_


void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelXany(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuModelTelemetry(uint8_t event);
void menuModelTemplates(uint8_t event);
void menuModelExpoOne(uint8_t event);
uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay);
extern uint8_t s_curveChan;


// TODO elsewhere!
#define WCHART (LCD_H/2)
#define X0     (LCD_W-WCHART-2)
#define Y0     (LCD_H/2)
#define MIXES_2ND_COLUMN  (12*FW)

#define FlightModesType uint8_t

#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)


#define INCDEC_DECLARE_VARS(f)  uint8_t incdecFlag = (f)
#define INCDEC_SET_FLAG(f)      incdecFlag = (f)
#define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag)


extern uint8_t s_currIdx;
#define COPY_MODE 1
#define MOVE_MODE 2
extern uint8_t s_copyMode;
extern int8_t s_copySrcRow;
extern int8_t s_copyTgtOfs;


enum EnumTabModel {
	e_ModelSelect,
	e_ModelSetup,
	CASE_HELI(e_Heli)
	CASE_FLIGHT_MODES(e_FlightModesAll)
	CASE_X_ANY(e_Xany)
	e_InputsAll,
	e_MixAll,
	e_Limits,
	CASE_CURVES(e_CurvesAll)
	e_LogicalSwitches,
	e_CustomFunctions,
	CASE_FRSKY(e_Telemetry)
	CASE_TEMPLATES(e_Templates)
};

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
	menuModelSelect,
	menuModelSetup,
	CASE_HELI(menuModelHeli)
	CASE_FLIGHT_MODES(menuModelFlightModesAll)
	CASE_X_ANY(menuModelXany)
	menuModelExposAll,
	menuModelMixAll,
	menuModelLimits,
	CASE_CURVES(menuModelCurvesAll)
	menuModelLogicalSwitches,
	menuModelCustomFunctions,
	CASE_FRSKY(menuModelTelemetry)
	CASE_TEMPLATES(menuModelTemplates)
};


#endif /* MENU_MODEL_H_ */