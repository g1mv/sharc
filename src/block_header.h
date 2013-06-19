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
 * 17/06/13 19:16
 */

#ifndef BLOCK_HEADER_H
#define BLOCK_HEADER_H

#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

#include "globals.h"

typedef struct {
    byte mode;
    uint32_t nextBlock;
} BLOCK_HEADER;

BLOCK_HEADER createBlockHeader(const byte, const uint32_t);
BLOCK_HEADER readBlockHeader(byte*, const uint32_t);
BLOCK_HEADER readBlockHeaderFromFile(FILE* inFile);

#endif