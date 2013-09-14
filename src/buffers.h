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
 * 27/08/13 15:01
 */

#ifndef SHARC_BUFFERS_H
#define SHARC_BUFFERS_H

#include "globals.h"
#include "stream.h"
#include "metadata.h"

typedef enum {
    SHARC_BUFFERS_STATE_OK = 0,
    SHARC_BUFFERS_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL,
    SHARC_BUFFERS_STATE_ERROR_INVALID_STATE
} SHARC_BUFFERS_STATE;

SHARC_BUFFERS_STATE sharc_buffers_max_compressed_length(uint_fast64_t *, uint_fast64_t, SHARC_COMPRESSION_MODE);
SHARC_BUFFERS_STATE sharc_buffers_compress(uint_fast64_t*, uint8_t *, uint_fast64_t, uint8_t *, uint_fast64_t, const SHARC_COMPRESSION_MODE, const SHARC_ENCODE_OUTPUT_TYPE, const SHARC_BLOCK_TYPE, const struct stat *fileAttributes, void *(*mem_alloc)(size_t), void (*mem_free)(void *));
SHARC_BUFFERS_STATE sharc_buffers_decompress(uint_fast64_t *, sharc_header*, uint8_t *, uint_fast64_t, uint8_t *, uint_fast64_t, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

#endif