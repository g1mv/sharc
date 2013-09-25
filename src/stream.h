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
 * 26/08/13 22:03
 */

#ifndef SHARC_API_STREAM_H
#define SHARC_API_STREAM_H

#include <stdint.h>

#include "chameleon_encode.h"
#include "byte_buffer.h"
#include "header.h"
#include "block.h"
#include "metadata.h"
#include "globals.h"
#include "encode.h"
#include "decode.h"

#define SHARC_STREAM_MINIMUM_OUT_BUFFER_SIZE                        (1 << 9)

typedef enum {
    SHARC_STREAM_STATE_READY = 0,
    SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32,
    SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL,
    SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE
} SHARC_STREAM_STATE;

typedef enum {
    SHARC_STREAM_PROCESS_PREPARED,
    SHARC_STREAM_PROCESS_COMPRESSION_INITED,
    SHARC_STREAM_PROCESS_COMPRESSION_DATA_FINISHED,
    SHARC_STREAM_PROCESS_COMPRESSION_FINISHED,
    SHARC_STREAM_PROCESS_DECOMPRESSION_INITED,
    SHARC_STREAM_PROCESS_DECOMPRESSION_DATA_FINISHED,
    SHARC_STREAM_PROCESS_DECOMPRESSION_FINISHED,
} SHARC_STREAM_PROCESS;

typedef enum {
    SHARC_STREAM_ORIGIN_TYPE_STREAM,
    SHARC_STREAM_ORIGIN_TYPE_FILE
} SHARC_STREAM_ORIGIN_TYPE;

typedef struct {
    SHARC_STREAM_PROCESS process;

    sharc_byte_buffer workBuffer;

    void *(*mem_alloc)(size_t);
    void (*mem_free)(void *);

    sharc_encode_state internal_encode_state;
    sharc_decode_state internal_decode_state;
} sharc_stream_state;

typedef struct {
    sharc_byte_buffer in;
    uint_fast64_t* in_total_read;

    sharc_byte_buffer out;
    uint_fast64_t* out_total_written;

    sharc_stream_state internal_state;
} sharc_stream;

SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream *, uint8_t*, const uint_fast64_t, uint8_t*, const uint_fast64_t, void *(*)(size_t), void (*)(void *));

SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_OUTPUT_TYPE, const SHARC_BLOCK_TYPE, const struct stat*);
SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *, const sharc_bool);
SHARC_STREAM_STATE sharc_stream_compress_finish(sharc_stream *);

SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *, const sharc_bool);
SHARC_STREAM_STATE sharc_stream_decompress_finish(sharc_stream *);

SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_header(sharc_stream*, sharc_header*);

#endif