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
#include "argonaut.h"
#include "argonaut_le.data"

#include <string.h>

#define SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS                                             (256)
#define SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS                                           (2048)
#define SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS                                          8

#pragma pack(push)
#pragma pack(4)
typedef struct sharc_argonaut_dictionary_primary_entry sharc_argonaut_dictionary_primary_entry;
struct sharc_argonaut_dictionary_primary_entry {
    uint_fast8_t letter;
    uint_fast32_t durability;
    uint_fast8_t ranking;
};

typedef struct {
    union {
        uint64_t as_uint64_t;
        uint16_t as_uint16_t;
        uint8_t letters[SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
    };
    uint_fast8_t length;
    const sharc_argonaut_huffman_code* letterCode[SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
} sharc_argonaut_dictionary_word;

typedef struct {
    sharc_argonaut_huffman_code code [SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS];
} sharc_argonaut_primary_code_lookup;

typedef struct {
    sharc_argonaut_huffman_code code [SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS];
} sharc_argonaut_secondary_code_lookup;

typedef struct {
    sharc_argonaut_dictionary_word word;
    uint_fast32_t durability;
    uint_fast16_t ranking;
    uint_fast8_t ranked;
} sharc_argonaut_dictionary_secondary_entry;

typedef struct {
    sharc_argonaut_dictionary_primary_entry *primary[SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS];
    sharc_argonaut_dictionary_secondary_entry *secondary[SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS];
} sharc_argonaut_dictionary_ranking;

typedef struct {
    sharc_argonaut_dictionary_ranking ranking;
    sharc_argonaut_dictionary_primary_entry primary_entry[1 << 8];
    sharc_argonaut_dictionary_secondary_entry secondary_entry[1 << 16];
} sharc_argonaut_dictionary;
#pragma pack(pop)

#endif