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

#include <sys/stat.h>

#if defined(__FreeBSD__) || defined(__APPLE__)
#define stat64 stat
#endif

#include "hash_encode.h"
#include "encode.h"
#include "byte_buffer.h"

#define SHARC_STREAM_MINIMUM_OUT_BUFFER_SIZE            (1 << 11)

typedef enum {
    SHARC_STREAM_STATE_OK = 0,
    SHARC_STREAM_STATE_FINISHED,
    SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_STREAM_STATE_ERROR_INVALID_FILE_ATTRIBUTES,
    SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED,
    SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_4,
    SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL,
    SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_NOT_PROPERLY_ALIGNED,
    SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE
} SHARC_STREAM_STATE;

typedef struct {
    sharc_encode_state internal_state;
} sharc_stream_state;

typedef struct {
    sharc_byte_buffer in;
    uint64_t in_total_read;

    sharc_byte_buffer out;
    uint64_t out_total_written;

    sharc_stream_state internal_state;
} sharc_stream;

SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream * stream, char*, const uint32_t, char*, const uint32_t);
SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *, SHARC_COMPRESSION_MODE);
SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_compress_continue(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_decompress_continue(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_compress_finish(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_decompress_finish(sharc_stream *);

#endif