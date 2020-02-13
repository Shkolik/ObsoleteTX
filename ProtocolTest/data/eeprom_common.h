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


#ifndef EEPROM_COMMON_H_
#define EEPROM_COMMON_H_

#include "../main.h"

/////////////////////////
// checkIncDec flags
#define EE_NO           0x00
#define EE_GENERAL      0x01
#define EE_MODEL        0x02
#define NO_INCDEC_MARKS 0x04
#define INCDEC_SWITCH   0x08
#define INCDEC_SOURCE   0x10
#define INCDEC_REP10    0x40
#define NO_DBLKEYS      0x80

#define WRITE_DELAY_10MS 200

#define TIME_TO_WRITE() (s_eeDirtyMsk && (uint16_t)(g_tmr10ms - s_eeDirtyTime10ms) >= (uint16_t)WRITE_DELAY_10MS)

extern uint8_t		s_eeDirtyMsk;
extern uint16_t		s_eeDirtyTime10ms;

extern void eeDirty(uint8_t msk);
extern void eeCheck(bool immediately);
extern void eeReadAll();
extern bool eeModelExists(uint8_t id);
extern void eeLoadModel(uint8_t id);
extern uint8_t eeConvert();
extern void eeErase(bool warn);
extern uint8_t eeFindEmptyModel(uint8_t id, bool down);
extern void selectModel(uint8_t sub);


#endif /* EEPROM_COMMON_H_ */