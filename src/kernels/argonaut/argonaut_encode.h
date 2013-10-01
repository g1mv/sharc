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
#include "block.h"
#include "kernel_encode.h"
#include "argonaut_dictionary.h"
#include "argonaut.h"
#include "encode.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>

//#define SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD             (sizeof(sharc_hash_signature) + sizeof(uint32_t) * 8 * sizeof(sharc_hash_signature))

#define SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE                         (sizeof(sharc_argonaut_output_unit) << 3)
//#define SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD_UNITS        8
//#define SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_BYTES_LOOKAHEAD        (SHARC_ARGONAUT_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD_UNITS * SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE)

#define SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
//#define SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
//#define SHARC_ARGONAUT_ENCODE_STATS

#define sharc_argonaut_contains_zero(search64) (((search64) - 0x0101010101010101llu) & ~(search64) & 0x8080808080808080llu)
#define sharc_argonaut_contains_value(search64, value8) (sharc_argonaut_contains_zero((search64) ^ (~0llu / 255 * (value8))))

typedef enum {
    //SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT,
            SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_OUTPUT_MEMORY,
    //SHARC_ARGONAUT_ENCODE_PROCESS_ALLOCATE_ANCHOR,
    //SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY,
    SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD,
    SHARC_ARGONAUT_ENCODE_PROCESS_WORD,
    SHARC_ARGONAUT_ENCODE_PROCESS_FINISH
} SHARC_ARGONAUT_ENCODE_PROCESS;

typedef enum {
    SHARC_ARGONAUT_ENTITY_SEPARATOR = 0,
    SHARC_ARGONAUT_ENTITY_RANKED_KEY = 1,
    SHARC_ARGONAUT_ENTITY_WORD = 2,
    SHARC_ARGONAUT_ENTITY_KEY = 3,
    SHARC_ARGONAUT_ENTITY_COUNT
} SHARC_ARGONAUT_ENTITY;

typedef struct {
    sharc_argonaut_huffman_code code[SHARC_ARGONAUT_ENTITY_COUNT];
} sharc_argonaut_entity_code_lookup;

typedef uint_fast64_t sharc_argonaut_signature;
typedef uint_fast64_t sharc_argonaut_output_unit;

typedef struct {
    sharc_argonaut_huffman_code code [SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
} sharc_argonaut_word_length_code_lookup;

typedef struct {
    union {
        uint64_t as_uint64_t;
        //uint16_t as_uint16_t;
        uint8_t letters[SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
    };
    uint_fast8_t length; // todo
    //const sharc_argonaut_huffman_code* letterCode[SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS];
} sharc_argonaut_encode_word;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    SHARC_ARGONAUT_ENCODE_PROCESS process;
    
    //uint_fast64_t resetCycle;

    //uint_fast8_t efficiencyChecked;
    //sharc_argonaut_output_unit* output;
    //sharc_byte* anchor;
    uint_fast32_t shift;
    uint_fast16_t count;
    
    //uint_fast64_t bitCount;

    sharc_argonaut_encode_word word;
    
    //uint_fast32_t signatureShift;
    //sharc_argonaut_signature * signature;
    //uint_fast32_t signaturesCount;
    
    uint_fast64_t buffer;
    uint_fast8_t bufferBits;

    sharc_argonaut_dictionary dictionary;
} sharc_argonaut_encode_state;
#pragma pack(pop)

SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_init_default(void*);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process_default(sharc_byte_buffer *, sharc_byte_buffer *, void *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_finish_default(void*);

SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_init_post_processing(void*);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process_post_processing(sharc_byte_buffer *, sharc_byte_buffer *, void *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_finish_post_processing(void*);

#endif