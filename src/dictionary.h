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
 * 19/08/13 18:48
 */

#ifndef SHARC_DICTIONARY_H
#define SHARC_DICTIONARY_H

#include "globals.h"
#include "hash.h"
#include "dictionary.data"

#include <string.h>

#define SHARC_DICTIONARY_MAX_RESET_CYCLE_SHIFT          32

#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT    7
#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE          (1 << SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT)

#define SHARC_DICTIONARY_VALUE_NOT_SET                  0x76EFE1F1

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint32_t as_uint32_t;
} SHARC_ENTRY;
#pragma pack(pop)

void sharc_resetDictionary(SHARC_ENTRY*);
void sharc_resetDualPassDictionary(SHARC_ENTRY*);

#endif