/*
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 20:51
 */

#ifndef SHARC_CIPHER_H
#define SHARC_CIPHER_H

#include "direct_hash_cipher.h"
#include "xor_hash_cipher.h"
#include "block_header.h"

#define MODE_SINGLE_PASS        0
#define MODE_DUAL_PASS          1
#define MODE_COPY               255

byte intermediateBuffer[MAX_BUFFER_SIZE][MAX_PARALLELISM];

byte sharcEncode(BYTE_BUFFER*, BYTE_BUFFER*, const byte);
bool sharcDecode(BYTE_BUFFER*, BYTE_BUFFER*, const byte);

#endif