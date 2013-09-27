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
 * 12/09/13 09:55
 */

#ifndef SHARC_ENCODE_H
#define SHARC_ENCODE_H

#include "block_footer.h"
#include "block_header.h"
#include "chameleon_dictionary.h"
#include "chameleon_encode.h"
#include "header.h"
#include "footer.h"
#include "block_mode_marker.h"
#include "block_encode.h"
#include "argonaut_encode.h"
#include "argonaut_dictionary.h"

typedef enum {
    SHARC_ENCODE_STATE_READY = 0,
    SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_ENCODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_ENCODE_STATE_ERROR
} SHARC_ENCODE_STATE;

typedef enum {
    SHARC_ENCODE_PROCESS_WRITE_BLOCKS,
    SHARC_ENCODE_PROCESS_WRITE_BLOCKS_IN_TO_WORKBUFFER,
    SHARC_ENCODE_PROCESS_WRITE_BLOCKS_WORKBUFFER_TO_OUT,
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
    uint_fast64_t memorySize;
    uint_fast64_t outstandingBytes;
} sharc_encode_work_buffer_data;

typedef struct {
    SHARC_ENCODE_PROCESS process;
    SHARC_COMPRESSION_MODE compressionMode;
    SHARC_BLOCK_TYPE blockType;
    const struct stat* fileAttributes;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    sharc_block_encode_state blockEncodeStateA;
    sharc_block_encode_state blockEncodeStateB;

    sharc_byte_buffer* workBuffer;
    sharc_encode_work_buffer_data workBufferData;
} sharc_encode_state;
#pragma pack(pop)

SHARC_ENCODE_STATE sharc_encode_init(sharc_byte_buffer *, sharc_byte_buffer *, const uint_fast64_t, sharc_encode_state *, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_OUTPUT_TYPE, const SHARC_BLOCK_TYPE, const struct stat *);
SHARC_ENCODE_STATE sharc_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, sharc_encode_state *, const sharc_bool);
SHARC_ENCODE_STATE sharc_encode_finish(sharc_byte_buffer *, sharc_encode_state *);

#endif