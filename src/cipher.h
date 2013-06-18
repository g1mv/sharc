/*
 * Copyright (c) 2013, Guillaume Voirin (gvoirin@centaurean.com)
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 17:39
 */

#ifndef CIPHER_H
#define CIPHER_H

#include "globals.h"

typedef struct {
    byte* pointer;
    uint32_t position;
    uint32_t size;
} BYTE_BUFFER;

typedef struct {
    uint32_t* pointer;
    uint32_t position;
    uint32_t size;
} UINT32_BUFFER;

/*byte* inBuffer;
uint32_t inSize;
uint32_t inPosition;

byte* outBuffer;
uint32_t outSize;
uint32_t outPosition;

void prepareWorkspace(byte*, uint32_t, byte*, uint32_t);*/

BYTE_BUFFER createByteBuffer(byte*, uint32_t, uint32_t);
void rewindByteBuffer(BYTE_BUFFER*);

#endif
