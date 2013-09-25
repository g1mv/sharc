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
 * 12/09/13 11:04
 */

#ifndef SHARC_DECODE_H
#define SHARC_DECODE_H

#include "block_header.h"
#include "block_footer.h"
#include "byte_buffer.h"
#include "chameleon_dictionary.h"
#include "chameleon_decode.h"
#include "header.h"
#include "footer.h"
#include "block_mode_marker.h"
#include "block_decode.h"

typedef enum {
    SHARC_DECODE_STATE_READY = 0,
    SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_DECODE_STATE_ERROR
} SHARC_DECODE_STATE;

typedef enum {
    SHARC_DECODE_PROCESS_READ_BLOCKS,
    SHARC_DECODE_PROCESS_READ_BLOCKS_IN_TO_WORKBUFFER,
    SHARC_DECODE_PROCESS_READ_BLOCKS_WORKBUFFER_TO_OUT,
    SHARC_DECODE_PROCESS_READ_FOOTER,
    SHARC_DECODE_PROCESS_FINISHED
} SHARC_DECODE_PROCESS;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint_fast64_t memorySize;
} sharc_decode_work_buffer_data;

typedef struct {
    SHARC_DECODE_PROCESS process;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    sharc_header header;
    sharc_footer footer;

    sharc_block_decode_state blockDecodeStateA;
    sharc_block_decode_state blockDecodeStateB;

    sharc_byte_buffer* workBuffer;
    sharc_decode_work_buffer_data workBufferData;
} sharc_decode_state;
#pragma pack(pop)

SHARC_DECODE_STATE sharc_decode_init(sharc_byte_buffer*, sharc_byte_buffer*, const uint_fast64_t, sharc_decode_state *);
SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_decode_state *, const sharc_bool);
SHARC_DECODE_STATE sharc_decode_finish(sharc_byte_buffer *, sharc_decode_state*);

#endif