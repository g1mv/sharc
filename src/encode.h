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
 * 29/08/13 12:32
 */

#ifndef SHARC_ENCODE_H
#define SHARC_ENCODE_H

#include "block_header.h"
#include "byte_buffer.h"
#include "dictionary.h"
#include "hash_encode.h"
#include "header.h"

typedef enum {
    SHARC_ENCODE_STATE_READY = 0,
    SHARC_ENCODE_STATE_FINISHED,
    SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER,
    SHARC_ENCODE_STATE_STALL_INPUT_BUFFER,
    SHARC_ENCODE_STATE_ERROR
} SHARC_ENCODE_STATE;

typedef enum {
    SHARC_ENCODE_PROCESS_WRITE_HEADER,
    SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER,
    SHARC_ENCODE_PROCESS_WRITE_DATA
} SHARC_ENCODE_PROCESS;

typedef enum {
    SHARC_ENCODE_TYPE_WITH_HEADER,
    SHARC_ENCODE_TYPE_WITHOUT_HEADER
} SHARC_ENCODE_TYPE;

typedef struct {
    uint_fast64_t inStart;
    uint_fast64_t outStart;
} sharc_encode_block_data;

typedef struct {
    sharc_dictionary dictionary_a;
    sharc_dictionary dictionary_b;
    uint_fast32_t resetCycle;
} sharc_encode_dictionary_data;

typedef struct {
    SHARC_ENCODE_PROCESS process;
    SHARC_COMPRESSION_MODE mode;
    const struct stat* fileAttributes;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    sharc_hash_encode_state hashEncodeState;
    sharc_encode_block_data blockData;
    sharc_encode_dictionary_data dictionaryData;

    sharc_byte_buffer workBuffer;
} sharc_encode_state;

SHARC_ENCODE_STATE sharc_encode_init(sharc_encode_state *, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_TYPE);
SHARC_ENCODE_STATE sharc_encode_init_with_file(sharc_encode_state *, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_TYPE, const struct stat*);
SHARC_ENCODE_STATE sharc_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_encode_state *, const SHARC_BOOL);
SHARC_ENCODE_STATE sharc_encode_finish(sharc_encode_state*);

#endif