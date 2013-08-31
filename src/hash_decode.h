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
 * 28/08/13 22:28
 */

#ifndef SHARC_HASH_DECODE_H
#define SHARC_HASH_DECODE_H

#include <stdint.h>
#include "byte_buffer.h"
#include "dictionary.h"
#include "hash.h"
#include "block_header.h"

#define SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD              (sizeof(uint32_t))

typedef enum {
    SHARC_HASH_DECODE_STATE_READY = 0,
    SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_HASH_DECODE_STATE_ERROR
} SHARC_HASH_DECODE_STATE;

typedef enum {
    SHARC_HASH_DECODE_PROCESS_READING_SIGNATURE,
    SHARC_HASH_DECODE_PROCESS_READING_CHUNK,
    SHARC_HASH_DECODE_PROCESS_DATA,
    SHARC_HASH_DECODE_PROCESS_FINISH
} SHARC_HASH_DECODE_PROCESS;

typedef uint64_t sharc_hash_decode_signature;

typedef struct {
    SHARC_HASH_DECODE_PROCESS process;
    uint_fast64_t signature;
    uint_fast8_t signatureBytes;
    uint_fast32_t signaturesCount;
} sharc_hash_decode_state;

#endif