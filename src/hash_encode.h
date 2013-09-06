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
 * 28/08/13 18:56
 */

#ifndef SHARC_HASH_ENCODE_H
#define SHARC_HASH_ENCODE_H

#include "byte_buffer.h"
#include "dictionary.h"
#include "hash.h"
#include "block.h"

#define SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD             (sizeof(uint64_t) + 32 * sizeof(uint64_t))

typedef enum {
    SHARC_HASH_ENCODE_STATE_READY = 0,
    SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK,
    SHARC_HASH_ENCODE_STATE_FINISHED,
    SHARC_HASH_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_HASH_ENCODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_HASH_ENCODE_STATE_ERROR
} SHARC_HASH_ENCODE_STATE;

typedef enum {
    SHARC_HASH_ENCODE_PROCESS_CHECK_STATE,
    SHARC_HASH_ENCODE_PROCESS_PREPARE_NEW_BLOCK,
    SHARC_HASH_ENCODE_PROCESS_DATA,
    SHARC_HASH_ENCODE_PROCESS_FINISH
} SHARC_HASH_ENCODE_PROCESS;

typedef uint64_t sharc_hash_encode_signature;

typedef struct {
    SHARC_HASH_ENCODE_PROCESS process;

    uint_fast32_t shift;
    sharc_hash_encode_signature * signature;
    uint_fast32_t signaturesCount;
} sharc_hash_encode_state;

SHARC_HASH_ENCODE_STATE sharc_hash_encode_init(sharc_hash_encode_state*);
SHARC_HASH_ENCODE_STATE sharc_hash_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, const uint32_t, sharc_dictionary *, sharc_hash_encode_state *, const sharc_bool);
SHARC_HASH_ENCODE_STATE sharc_hash_encode_finish(sharc_hash_encode_state*);

#endif