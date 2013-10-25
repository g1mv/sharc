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
 * 27/09/13 18:55
 */

#ifndef SHARC_HUFFMAN_DICTIONARY_H
#define SHARC_HUFFMAN_DICTIONARY_H

#include "globals.h"
#include "huffman.h"
#include "huffman_le.data"

#include <string.h>

#define SHARC_HUFFMAN_DICTIONARY_PRIMARY_RANKS                                             (256)

#pragma pack(push)
#pragma pack(4)
typedef struct sharc_huffman_dictionary_primary_entry sharc_huffman_dictionary_primary_entry;
struct sharc_huffman_dictionary_primary_entry {
    uint_fast8_t letter;
    uint_fast32_t durability;
    uint_fast8_t ranking;
};

typedef struct {
    sharc_huffman_code code [SHARC_HUFFMAN_DICTIONARY_PRIMARY_RANKS];
} sharc_huffman_primary_code_lookup;

typedef struct {
    sharc_huffman_dictionary_primary_entry *primary[SHARC_HUFFMAN_DICTIONARY_PRIMARY_RANKS];
} sharc_huffman_dictionary_ranking;

typedef struct {
    sharc_huffman_dictionary_ranking ranking;
    sharc_huffman_dictionary_primary_entry primary_entry[1 << 8];
} sharc_huffman_dictionary;
#pragma pack(pop)

#endif