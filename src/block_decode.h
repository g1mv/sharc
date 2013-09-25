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

#ifndef SHARC_BLOCK_DECODE_H
#define SHARC_BLOCK_DECODE_H

#include "block_header.h"
#include "block_footer.h"
#include "byte_buffer.h"
#include "chameleon_dictionary.h"
#include "chameleon_decode.h"
#include "header.h"
#include "footer.h"
#include "block_mode_marker.h"
#include "kernel_decode.h"

typedef enum {
    SHARC_BLOCK_DECODE_STATE_READY = 0,
    SHARC_BLOCK_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_BLOCK_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_BLOCK_DECODE_STATE_ERROR
} SHARC_BLOCK_DECODE_STATE;

typedef enum {
    SHARC_BLOCK_DECODE_PROCESS_READ_BLOCK_HEADER,
    SHARC_BLOCK_DECODE_PROCESS_READ_BLOCK_MODE_MARKER,
    SHARC_BLOCK_DECODE_PROCESS_READ_BLOCK_FOOTER,
    SHARC_BLOCK_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER,
    SHARC_BLOCK_DECODE_PROCESS_READ_DATA,
    SHARC_BLOCK_DECODE_PROCESS_FINISHED
} SHARC_BLOCK_DECODE_PROCESS;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint_fast64_t inStart;
    uint_fast64_t outStart;
} sharc_block_decode_current_block_data;

typedef struct {
    sharc_dictionary dictionary;
    uint_fast32_t resetCycle;
    void (*dictionary_reset)(sharc_dictionary *);
} sharc_block_decode_dictionary_data;

typedef struct {
    SHARC_BLOCK_DECODE_PROCESS process;
    SHARC_BLOCK_MODE mode;
    SHARC_BLOCK_TYPE blockType;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;
    uint_fast32_t endDataOverhead;

    sharc_block_header lastBlockHeader;
    sharc_mode_marker lastModeMarker;
    sharc_block_footer lastBlockFooter;

    sharc_hash_decode_state hashDecodeState;
    sharc_block_decode_current_block_data currentBlockData;
    sharc_block_decode_dictionary_data dictionaryData;

    void*kernelDecodeState;
    SHARC_KERNEL_DECODE_STATE (*kernelDecodeInit)(void*, const uint32_t);
    SHARC_KERNEL_DECODE_STATE (*kernelDecodeProcess)(sharc_byte_buffer *, sharc_byte_buffer *, void*, const sharc_bool);
    SHARC_KERNEL_DECODE_STATE (*kernelDecodeFinish)(void*);
} sharc_block_decode_state;
#pragma pack(pop)

SHARC_BLOCK_DECODE_STATE sharc_block_decode_init(sharc_block_decode_state *, const SHARC_BLOCK_MODE, const SHARC_BLOCK_TYPE, const uint32_t, void*, SHARC_KERNEL_DECODE_STATE (*)(void*, const uint32_t), SHARC_KERNEL_DECODE_STATE (*)(sharc_byte_buffer *, sharc_byte_buffer *, void*, const sharc_bool), SHARC_KERNEL_DECODE_STATE (*)(void*));
SHARC_BLOCK_DECODE_STATE sharc_block_decode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_block_decode_state *, const sharc_bool);
SHARC_BLOCK_DECODE_STATE sharc_block_decode_finish(sharc_block_decode_state *);

#endif