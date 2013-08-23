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
 * 01/06/13 20:51
 */

#ifndef SHARC_SHARC_TRANSFORM_H
#define SHARC_SHARC_TRANSFORM_H

#include "direct_hash_transform.h"
#include "xor_hash_transform.h"
#include "block_header.h"

#define SHARC_MODE_SINGLE_PASS        0
#define SHARC_MODE_DUAL_PASS          1
#define SHARC_MODE_COPY               255

typedef struct {
    sharc_byte reachableMode;
    SHARC_BYTE_BUFFER* out;
} SHARC_ENCODING_RESULT;

SHARC_ENCODING_RESULT sharc_createEncodingResult(const sharc_byte, SHARC_BYTE_BUFFER*);
SHARC_ENCODING_RESULT sharc_createEncodingResultWithPosition(const sharc_byte, SHARC_BYTE_BUFFER*, const uint32_t);
SHARC_ENCODING_RESULT sharc_copyMode(SHARC_BYTE_BUFFER*);
SHARC_ENCODING_RESULT sharc_sharcEncode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const sharc_byte, SHARC_ENTRY*, SHARC_ENTRY*);
sharc_bool sharc_sharcDecode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const sharc_byte, SHARC_ENTRY*, SHARC_ENTRY*);

#endif