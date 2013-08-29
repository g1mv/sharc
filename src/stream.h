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

#include "api.h"
#include "hash_encode.h"
#include "encode.h"
#include "byte_buffer.h"

typedef enum {
    SHARC_STREAM_STATE_OK = 0,
    SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_STREAM_STATE_ERROR_INVALID_FILE_ATTRIBUTES,
    SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED,
    SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_4,
    SHARC_STREAM_STATE_ERROR_UNSUPPORTED_COMPRESSION_MODE,
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

SHARC_STREAM_STATE sharc_stream_compressInit(sharc_stream *, SHARC_API_COMPRESSION_MODE, SHARC_API_OUTPUT_TYPE);
SHARC_STREAM_STATE sharc_stream_decompressInit(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_compressEnd(sharc_stream *);
SHARC_STREAM_STATE sharc_stream_decompressEnd(sharc_stream *);

#endif