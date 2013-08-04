/*
 * Centaurean Sharc
 * http://www.centaurean.com/sharc
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation. For the terms of this
 * license, see http://www.gnu.org/licenses/gpl.html
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, see http://www.centaurean.com/sharc for more
 * information.
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
    uint32_t mode;
    uint32_t nextBlock;
} BLOCK_HEADER;

BLOCK_HEADER createBlockHeader(const byte, const uint32_t);
BLOCK_HEADER readBlockHeader(byte*, const uint32_t);
size_t readBlockHeaderFromFile(BLOCK_HEADER*, FILE*);

#endif
