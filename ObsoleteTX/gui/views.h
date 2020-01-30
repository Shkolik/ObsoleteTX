/*
 * views.h
 *
 * Created: 1/29/2020 4:25:10 PM
 *  Author: andrew.shkolik
 */ 


#ifndef VIEWS_H_
#define VIEWS_H_

#include "../ObsoleteTX.h"
#include "../timers.h"
#include "../protocol/misc.h"
#include "widgets.h"
#include "menu_general.h"
#include "menus.h"

//main view
#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#define EVT_KEY_CONTEXT_MENU EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_SHUTDOWN     EVT_KEY_LONG(KEY_EXIT)

#define LBOX_CENTERX  (LCD_W/4 + 10)
#define RBOX_CENTERX  (3*LCD_W/4 - 10)
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW-1)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (LCD_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)

#define TRIM_LEN 27

#ifdef TELEMETRY

#define STATUS_BAR_Y     (7*FH+1)
#define TELEM_2ND_COLUMN (10*FW)
#define BAR_LEFT    25
#define BAR_WIDTH   100

#endif


#endif /* VIEWS_H_ */