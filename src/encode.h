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

#include "block_footer.h"
#include "block_header.h"
#include "dictionary.h"
#include "hash_encode.h"
#include "header.h"
#include "footer.h"
#include "block_mode_marker.h"

typedef enum {
    SHARC_ENCODE_STATE_READY = 0,
    SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_ENCODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_ENCODE_STATE_ERROR
} SHARC_ENCODE_STATE;

typedef enum {
    SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER,
    SHARC_ENCODE_PROCESS_WRITE_BLOCK_MODE_MARKER,
    SHARC_ENCODE_PROCESS_WRITE_BLOCK_FOOTER,
    SHARC_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER,
    SHARC_ENCODE_PROCESS_WRITE_DATA,
    SHARC_ENCODE_PROCESS_WRITE_FOOTER,
    SHARC_ENCODE_PROCESS_FINISHED
} SHARC_ENCODE_PROCESS;

typedef enum {
    SHARC_ENCODE_OUTPUT_TYPE_DEFAULT = 0,
    SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER = 1,
    SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_FOOTER = 2,
    SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER_NOR_FOOTER = 3
} SHARC_ENCODE_OUTPUT_TYPE;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint_fast64_t inStart;
    uint_fast64_t outStart;
} sharc_encode_current_block_data;

typedef struct {
    sharc_dictionary dictionary_a;
    sharc_dictionary dictionary_b;
    uint_fast32_t resetCycle;
} sharc_encode_dictionary_data;

typedef struct {
    SHARC_ENCODE_PROCESS process;
    SHARC_COMPRESSION_MODE targetCompressionMode;
    SHARC_COMPRESSION_MODE currentCompressionMode;
    SHARC_BLOCK_TYPE blockType;
    const struct stat* fileAttributes;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    sharc_hash_encode_state hashEncodeState;
    sharc_encode_current_block_data currentBlockData;
    sharc_encode_dictionary_data dictionaryData;

    sharc_byte_buffer* workBuffer;
} sharc_encode_state;
#pragma pack(pop)

SHARC_ENCODE_STATE sharc_encode_init(sharc_byte_buffer *, sharc_byte_buffer *, sharc_encode_state *, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_OUTPUT_TYPE, const SHARC_BLOCK_TYPE, const struct stat*);
SHARC_ENCODE_STATE sharc_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_encode_state *, const sharc_bool);
SHARC_ENCODE_STATE sharc_encode_finish(sharc_byte_buffer *, sharc_encode_state*);

#endif