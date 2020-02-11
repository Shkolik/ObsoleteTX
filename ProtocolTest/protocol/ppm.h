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

#ifndef PPM_H_
#define PPM_H_

#include "../main.h"

// PPM Def
#define PPMFRAMELENGTH  rfOptionValue1 // 0=22.5ms  (10ms-30ms) 0.5ms increments
#define PPMDELAY        rfOptionValue2
#define PPMNCH          rfSubType
#define PULSEPOL        rfOptionBool1

extern const void *PPM_Cmds(enum ProtoCmds);

#endif /* PPM_H_ */