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

#ifndef SHARC_BLOCK_HEADER_H
#define SHARC_BLOCK_HEADER_H

#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

#include "globals.h"

#define SHARC_BLOCK_HEADER_DICTIONARY_RESET_MASK    0x1

#pragma pack(push)
#pragma pack(4)
typedef struct {
    sharc_byte mode;
    sharc_byte dictionaryFlags;
    sharc_byte reserved[2];
    uint32_t nextBlock;
} SHARC_BLOCK_HEADER;
#pragma pack(pop)

SHARC_BLOCK_HEADER sharc_createBlockHeader(const sharc_byte, const sharc_bool, const uint32_t);
const size_t sharc_readBlockHeaderFromFile(SHARC_BLOCK_HEADER*, FILE*);

#endif
