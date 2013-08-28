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

#ifndef SHARC_API_BUFFERS_H
#define SHARC_API_BUFFERS_H

#include "api.h"
#include "state.h"

#define SHARC_API_BUFFERS_STATE_OK       0
#define SHARC_API_BUFFERS_STATE_ERROR    1

/*
 * Utility function, returns the maximum length that compressed data can occupy
 *
 * Example :
 * uint64_t compressedBufferSize = 65536;
 * uint64_t decompressedBufferSize = sharc_api_buffers_max_compressed_total_length(compressionBufferSize);
 */
uint64_t sharc_api_buffers_max_compressed_total_length(uint64_t);

uint64_t sharc_api_buffers_max_compressed_length_without_header(uint64_t);

uint32_t sharc_api_buffers_compress(uint8_t*, uint64_t, uint8_t*, uint64_t*, uint32_t);
uint32_t sharc_api_buffers_decompress(uint8_t*, uint64_t, uint8_t*, uint64_t*);

#endif