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

#include "keys.h"

#define KEY_LONG_DELAY              32  // long key press minimum duration (x10ms), must be less than KEY_REPEAT_DELAY
#define KEY_REPEAT_DELAY            40  // press longer than this enables repeat (but does not fire it yet)
#define KEY_REPEAT_TRIGGER          48  // repeat trigger, used in combination with m_state to produce decreasing times between repeat events
#define KEY_REPEAT_PAUSE_DELAY      64

#define FILTERBITS                4   // defines how many bits are used for debounce

#define KSTATE_OFF                  0
#define KSTATE_RPTDELAY             95
#define KSTATE_START                97
#define KSTATE_PAUSE                98
#define KSTATE_KILLED               99


uint8_t s_evt;
struct t_inactivity inactivity = {0};
Key keys[NUM_KEYS];

uint8_t getEvent()
{
	uint8_t evt = s_evt;
	s_evt = 0;
	return evt;
}

void Key::input(bool val)
{
	// store new value in the bits that hold the key state history (used for debounce)
	uint8_t t_vals = m_vals ;
	t_vals <<= 1 ;
	if (val) t_vals |= 1;
	m_vals = t_vals ;

	m_cnt++;

	if (m_state && m_vals == 0) {
		// key is released
		if (m_state != KSTATE_KILLED) {
			// TRACE("key %d BREAK", key());
			putEvent(EVT_KEY_BREAK(key()));
		}
		m_state = KSTATE_OFF;
		m_cnt = 0;
		return;
	}

	switch (m_state) {
			case KSTATE_OFF:
			if (m_vals == ((1<<FILTERBITS)-1)) {
				m_state = KSTATE_START;
				m_cnt = 0;
			}
			break;
		case KSTATE_START:
			// TRACE("key %d FIRST", key());
			putEvent(EVT_KEY_FIRST(key()));
			inactivity.counter = 0;
			m_state = KSTATE_RPTDELAY;
			m_cnt = 0;
			break;

		case KSTATE_RPTDELAY: // gruvin: delay state before first key repeat
			if (m_cnt == KEY_LONG_DELAY) 
			{
				// generate long key press			
				putEvent(EVT_KEY_LONG(key()));
			}
			if (m_cnt == KEY_REPEAT_DELAY) {
				m_state = 16;
				m_cnt = 0;
			}
			break;

		case 16:
		case 8:
		case 4:
		case 2:
			if (m_cnt >= KEY_REPEAT_TRIGGER) 
			{ //3 6 12 24 48 pulses in every 480ms
				m_state >>= 1;
				m_cnt = 0;
			}
			// no break
		case 1:
			if ((m_cnt & (m_state-1)) == 0) 
			{
			
				putEvent(EVT_KEY_REPT(key()));
			}
			break;

		case KSTATE_PAUSE: //pause repeat events
			if (m_cnt >= KEY_REPEAT_PAUSE_DELAY) {
				m_state = 8;
				m_cnt = 0;
			}
			break;

		case KSTATE_KILLED: //killed
			break;
	}
}

void Key::pauseEvents()
{
	m_state = KSTATE_PAUSE;
	m_cnt = 0;
}

void Key::killEvents()
{
	// TRACE("key %d killed", key());
	m_state = KSTATE_KILLED;
}


uint8_t Key::key() const
{
	return (this - keys);
}

// Introduce a slight delay in the key repeat sequence
void pauseEvents(uint8_t event)
{
	event = EVT_KEY_MASK(event);
	if (event < (int)DIM(keys)) keys[event].pauseEvents();
}

// Disables any further event generation (BREAK and REPEAT) for this key, until the key is released
void killEvents(uint8_t event)
{
	event = EVT_KEY_MASK(event);
	if (event < (int)DIM(keys)) {
		keys[event].killEvents();
	}
}

void clearKeyEvents()
{
	//// loop until all keys are up
	//while (keyDown()) 
	//{
		//wdt_reset();
	//}

	memclear(keys, sizeof(keys));
	putEvent(0);
}
