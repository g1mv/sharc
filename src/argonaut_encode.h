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
 * 18/09/13 14:56
 */

#ifndef SHARC_ARGONAUT_ENCODE_H
#define SHARC_ARGONAUT_ENCODE_H

#include "byte_buffer.h"
#include "hash.h"
#include "block.h"
#include "kernel_encode.h"
#include "argonaut_dictionary.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>

//#define SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD             (sizeof(sharc_hash_signature) + sizeof(uint32_t) * 8 * sizeof(sharc_hash_signature))

#define SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE                         (sizeof(sharc_argonaut_output_unit) << 3)
//#define SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD_UNITS        8
//#define SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_BYTES_LOOKAHEAD        (SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD_UNITS * SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE)

typedef enum {
    SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT,
    SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY,
    SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD,
    SHARC_ARGONAUT_ENCODE_PROCESS_WORD,
    SHARC_ARGONAUT_ENCODE_PROCESS_FINISH
} SHARC_ARGONAUT_ENCODE_PROCESS;

typedef uint_fast64_t sharc_argonaut_output_unit;

typedef struct {
    sharc_argonaut_huffman_code lookup [SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
} wlhufflook;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    SHARC_ARGONAUT_ENCODE_PROCESS process;

    //uint_fast64_t bitCount;
    int_fast64_t count[8];
    int_fast64_t length[SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
    //uint32_t offset;
    uint_fast8_t efficiencyChecked;
    //sharc_argonaut_dictionary_word partialWord;
    sharc_argonaut_dictionary_word word;

    sharc_argonaut_huffman_code code;
    uint_fast8_t shift;
    uint_fast8_t preliminaryShift;
    //uint_fast8_t blockShift;
    sharc_argonaut_output_unit* output;
} sharc_argonaut_encode_state;
#pragma pack(pop)

SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_init(sharc_argonaut_encode_state*, sharc_argonaut_dictionary *);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, const uint32_t, sharc_argonaut_dictionary *, sharc_argonaut_encode_state *, const sharc_bool, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_finish(sharc_argonaut_encode_state*);

#endif