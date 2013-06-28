/*
 * Copyright (c) 2013, Guillaume Voirin
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
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 17:39
 */

#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include "globals.h"

typedef struct {
    byte* pointer;
    uint32_t position;
    uint32_t size;
} BYTE_BUFFER;

BYTE_BUFFER createByteBuffer(byte*, uint32_t, uint32_t);
void rewindByteBuffer(BYTE_BUFFER*);

#endif
