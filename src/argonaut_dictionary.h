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
#include "argonaut_le.data"

#include <string.h>

#define SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS                                             (256)
#define SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS                                           (2048)
#define SHARC_ARGONAUT_DICTIONARY_WORD_MAX_LETTERS                                          10

#pragma pack(push)
#pragma pack(4)
typedef struct sharc_argonaut_dictionary_primary_entry sharc_argonaut_dictionary_primary_entry;
struct sharc_argonaut_dictionary_primary_entry {
    uint_fast8_t letter;
    uint_fast32_t durability;
    uint_fast8_t ranking;
};

typedef struct {
    uint_fast64_t code;
    uint_fast8_t bitSize;
} sharc_argonaut_huffman_code;

typedef struct {
    union {
        uint64_t as_uint64_t[2];
        uint16_t as_uint16_t[SHARC_ARGONAUT_DICTIONARY_WORD_MAX_LETTERS >> 1];
        uint8_t letters[SHARC_ARGONAUT_DICTIONARY_WORD_MAX_LETTERS];
    };
    uint_fast8_t length;
    const sharc_argonaut_huffman_code* letterCode[SHARC_ARGONAUT_DICTIONARY_WORD_MAX_LETTERS];
    //uint8_t compressedBitLength;
} sharc_argonaut_dictionary_word;

typedef struct {
    sharc_argonaut_huffman_code lookup [SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS];
} primhufflook;

typedef struct {
    sharc_argonaut_huffman_code lookup [SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS];
} sechufflook;

typedef struct {
    sharc_argonaut_dictionary_word word;
    uint_fast32_t durability;
    uint_fast16_t ranking;
    uint_fast8_t ranked;
} sharc_dictionary_qentry;

typedef struct {
    /*sharc_dictionary_entry* first16[1 << 4];
    uint32_t first16Limit;

    sharc_dictionary_entry* first256[1 << 8];
    uint32_t first256Limit;*/
    //sharc_dictionary_primary_entry* lastPrimRanked;
    sharc_dictionary_qentry* top[SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS];
    sharc_argonaut_dictionary_primary_entry *primary[SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS];
} sharc_dictionary_ranking;

typedef struct {
    sharc_dictionary_ranking ranking;
    sharc_argonaut_dictionary_primary_entry prim[1 << 8];
    //sharc_dictionary_entry sec[1 << 16];
    //sharc_dictionary_64_entry ter[1 << 16];
    sharc_dictionary_qentry quad[1 << 16];
    //uint8_t hits[1 << 16];
} sharc_argonaut_dictionary;
#pragma pack(pop)

void sharc_argonaut_dictionary_resetDirect(sharc_argonaut_dictionary *);
void sharc_argonaut_dictionary_resetCompressed(sharc_argonaut_dictionary *);

#endif