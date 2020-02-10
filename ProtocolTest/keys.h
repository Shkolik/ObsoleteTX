/*
 * Copyright (C) ObsoleteTX
 *
 * Based on code named
 *   th9x - https://github.com/thus1/th9x
 *   er9x - https://github.com/MikeBland/mbtx
 *   OpenTx - https://github.com/opentx/opentx
 *   OpenAVRc - https://github.com/Ingwie/OpenAVRc_Dev
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef KEYS_H_
#define KEYS_H_

#include "main.h"

#define EVT_KEY_MASK(e)                ((e) & 0x1f)

#define _MSK_KEY_BREAK                 0x20
#define _MSK_KEY_REPT                  0x40
#define _MSK_KEY_FIRST                 0x60
#define _MSK_KEY_LONG                  0x80
#define _MSK_KEY_FLAGS                 0xe0
#define EVT_ENTRY                      0xbf
#define EVT_ENTRY_UP                   0xbe

// normal order of events is: FIRST, LONG, REPEAT, REPEAT, ..., BREAK
#define EVT_KEY_FIRST(key)             ((key)|_MSK_KEY_FIRST)  // fired when key is pressed
#define EVT_KEY_LONG(key)              ((key)|_MSK_KEY_LONG)   // fired when key is held pressed for a while
#define EVT_KEY_REPT(key)              ((key)|_MSK_KEY_REPT)   // fired when key is held pressed long enough, fires multiple times with increasing speed
#define EVT_KEY_BREAK(key)             ((key)|_MSK_KEY_BREAK)  // fired when key is released (short or long), but only if the event was not killed

#define IS_KEY_FIRST(evt)              (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)               (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_LONG)
#define IS_KEY_REPT(evt)               (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_REPT)
#define IS_KEY_BREAK(evt)              (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_BREAK)


typedef uint8_t event_t;
#define IS_NEXT_EVENT(event)         (event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))
#define IS_PREVIOUS_EVENT(event)     (event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))

enum EnumKeys {
	KEY_MENU,
	KEY_BIND,
	KEY_CHANGE,
	KEY_RIGHT,
	KEY_LEFT,

	TRM_BASE,
	TRM_LH_DWN = TRM_BASE,
	TRM_LH_UP,
	TRM_LV_DWN,
	TRM_LV_UP,
	TRM_RV_DWN,
	TRM_RV_UP,
	TRM_RH_DWN,
	TRM_RH_UP,

	NUM_KEYS,
	SW_BASE=NUM_KEYS,
	SW_ID0=SW_BASE,
	SW_ID1,
	SW_ID2,
	SW_THR,
	SW_RUD,
	SW_ELE,
	SW_AIL,
	SW_GEA,
	SW_TRN
};

class Key
{
	private:
	uint8_t m_vals;
	uint8_t m_cnt;
	uint8_t m_state;
	public:
	void input(bool val);
	bool state() const { return m_vals > 0; }
	void pauseEvents();
	void killEvents();
	uint8_t key() const;
};

extern Key keys[NUM_KEYS];
extern event_t s_evt;

#define putEvent(evt) s_evt = evt

void pauseEvents(event_t event);
void killEvents(event_t event);

void clearKeyEvents();
event_t getEvent();
uint8_t keyDown();

struct t_inactivity{
	uint16_t counter;
	uint8_t  sum;
} __attribute__((__packed__));

extern Key keys[NUM_KEYS];

#endif /* KEYS_H_ */