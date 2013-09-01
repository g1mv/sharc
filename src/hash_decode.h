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

#define SHARC_HASH_DECODE_MINIMUM_INPUT_LOOKAHEAD               (/*sizeof(sharc_hash_decode_signature) - 2) + */sizeof(sharc_hash_decode_signature) + 32 * sizeof(sharc_hash_decode_signature))
#define SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD              (32 * sizeof(uint64_t))

typedef enum {
    SHARC_HASH_DECODE_STATE_READY = 0,
    SHARC_HASH_DECODE_STATE_INFO_NEW_BLOCK,
    SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_HASH_DECODE_STATE_FINISHED,
    SHARC_HASH_DECODE_STATE_ERROR
} SHARC_HASH_DECODE_STATE;

typedef enum {
    SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST,
    SHARC_HASH_DECODE_PROCESS_SIGNATURE_SAFE,
    SHARC_HASH_DECODE_PROCESS_DATA_FAST,
    SHARC_HASH_DECODE_PROCESS_DATA_SAFE,
    SHARC_HASH_DECODE_PROCESS_FINISH
} SHARC_HASH_DECODE_PROCESS;

typedef uint64_t sharc_hash_decode_signature;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    SHARC_HASH_DECODE_PROCESS process;

    sharc_hash_decode_signature signature;
    uint_fast32_t shift;
    uint_fast32_t signaturesCount;

    sharc_byte partialSignature[8];
    sharc_byte partialChunk[4];
    uint_fast32_t chunkBytes;
    uint_fast32_t signatureBytes;
} sharc_hash_decode_state;
#pragma pack(pop)

SHARC_HASH_DECODE_STATE sharc_hash_decode_init(sharc_hash_decode_state*);
SHARC_HASH_DECODE_STATE sharc_hash_decode_process(sharc_byte_buffer *, sharc_byte_buffer *, const uint_fast32_t, sharc_dictionary *, sharc_hash_decode_state *, const sharc_bool);
SHARC_HASH_DECODE_STATE sharc_hash_decode_finish(sharc_hash_decode_state*);

#endif