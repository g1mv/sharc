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
 * 18/09/13 14:48
 */

#ifndef SHARC_ARGONAUT_DICTIONARY_H
#define SHARC_ARGONAUT_DICTIONARY_H

#include "globals.h"
#include "hash.h"

#include <string.h>

//#define SHARC_DICTIONARY_MAX_RESET_CYCLE_SHIFT          32

//#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT    16//todo
//#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE          (1 << SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT)

//#define SHARC_DICTIONARY_VALUE_NOT_SET                  0x76EFE1F1

#define PRIM_RANKS                                          (1 << 7)

#pragma pack(push)
#pragma pack(4)
typedef struct sharc_dictionary_primary_entry sharc_dictionary_primary_entry;
struct sharc_dictionary_primary_entry {
    uint8_t letter;
    uint_fast32_t durability;
    uint_fast8_t isRanked;
    uint_fast8_t ranking;
    sharc_dictionary_primary_entry* preceding;
};

typedef struct {
    union {
        uint64_t as_uint64_t;
        uint16_t as_uint16_t[4];
        uint8_t letters[8];
    };
    uint8_t length;
    uint8_t compressedLength;
} word;

typedef struct {
    word aword;
    uint32_t durability;
    uint16_t ranking;
    uint8_t ranked;
} sharc_dictionary_qentry;

typedef struct {
    /*sharc_dictionary_entry* first16[1 << 4];
    uint32_t first16Limit;

    sharc_dictionary_entry* first256[1 << 8];
    uint32_t first256Limit;*/
    sharc_dictionary_primary_entry* lastPrimRanked;
    sharc_dictionary_qentry* top[(1 << 4) + (1 << 8)];
    //sharc_dictionary_primary_entry* topPrim[PRIM_RANKS];
} sharc_dictionary_ranking;

typedef struct {
    sharc_dictionary_ranking ranking;
    sharc_dictionary_primary_entry prim[1 << 8];
    //sharc_dictionary_entry sec[1 << 16];
    //sharc_dictionary_64_entry ter[1 << 16];
    sharc_dictionary_qentry quad[1 << 16];
    //uint8_t hits[1 << 16];
} sharc_argonaut_dictionary;
#pragma pack(pop)

void sharc_argonaut_dictionary_resetDirect(sharc_argonaut_dictionary *);
void sharc_argonaut_dictionary_resetCompressed(sharc_argonaut_dictionary *);

#endif