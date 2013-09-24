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

#define SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
#define SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
//#define SHARC_ARGONAUT_ENCODE_STATS

/*
SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (53,057,920 bytes), Ratio out / in = 53.1%, Time = 0.953 s, Speed = 100 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (51,886,888 bytes), Ratio out / in = 51.9%, Time = 0.964 s, Speed = 99 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (51,573,768 bytes), Ratio out / in = 51.6%, Time = 0.958 s, Speed = 100 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (51,372,968 bytes), Ratio out / in = 51.4%, Time = 0.967 s, Speed = 99 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (51,222,056 bytes), Ratio out / in = 51.2%, Time = 0.957 s, Speed = 100 MB/s

SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (52,479,304 bytes), Ratio out / in = 52.5%, Time = 0.954 s, Speed = 100 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (52,470,400 bytes), Ratio out / in = 52.5%, Time = 0.960 s, Speed = 99 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (52,470,896 bytes), Ratio out / in = 52.5%, Time = 0.964 s, Speed = 99 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (52,470,728 bytes), Ratio out / in = 52.5%, Time = 0.958 s, Speed = 100 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (52,470,552 bytes), Ratio out / in = 52.5%, Time = 0.964 s, Speed = 99 MB/s

SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (46,100,800 bytes), Ratio out / in = 46.1%, Time = 1.126 s, Speed = 85 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (45,038,864 bytes), Ratio out / in = 45.0%, Time = 1.128 s, Speed = 85 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (44,728,248 bytes), Ratio out / in = 44.7%, Time = 1.122 s, Speed = 85 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (44,526,960 bytes), Ratio out / in = 44.5%, Time = 1.130 s, Speed = 84 MB/s
    Compressed enwik8 (99,999,393 bytes) to enwik8.sharc (44,380,760 bytes), Ratio out / in = 44.4%, Time = 1.131 s, Speed = 84 MB/s
 */

typedef enum {
    SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT,
    SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY,
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
    sharc_argonaut_huffman_code lookup [SHARC_ARGONAUT_ENTITY_COUNT];
} ehufflook;

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
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, const uint32_t, sharc_argonaut_dictionary *, sharc_argonaut_encode_state *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_finish(sharc_argonaut_encode_state*);

#endif