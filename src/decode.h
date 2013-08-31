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
 * 31/08/13 13:45
 */

#ifndef SHARC_DECODE_H
#define SHARC_DECODE_H

#include "block_header.h"
#include "byte_buffer.h"
#include "dictionary.h"
#include "hash_decode.h"
#include "header.h"

typedef enum {
    SHARC_DECODE_STATE_READY = 0,
    SHARC_DECODE_STATE_FINISHED,
    SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_DECODE_STATE_ERROR
} SHARC_DECODE_STATE;

typedef enum {
    SHARC_DECODE_PROCESS_READ_HEADER,
    SHARC_DECODE_PROCESS_READ_BLOCK_HEADER,
    SHARC_DECODE_PROCESS_WRITE_DATA
} SHARC_DECODE_PROCESS;

typedef struct {
    sharc_dictionary dictionary_a;
    sharc_dictionary dictionary_b;
    uint_fast32_t resetCycle;
} sharc_decode_dictionary_data;

typedef struct {
    SHARC_DECODE_PROCESS process;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    sharc_header header;
    sharc_block_header lastBlockHeader;

    sharc_hash_decode_state hashDecodeState;
    sharc_decode_dictionary_data dictionaryData;
} sharc_decode_state;

SHARC_DECODE_STATE sharc_decode_init(sharc_decode_state *);
SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_decode_state *, const sharc_bool);
SHARC_DECODE_STATE sharc_decode_finish(sharc_decode_state*);

#endif