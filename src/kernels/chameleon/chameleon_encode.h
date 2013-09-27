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
#include "chameleon_dictionary.h"
#include "chameleon.h"
#include "block.h"
#include "kernel_encode.h"
#include "block_encode.h"

#define SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD             (sizeof(sharc_hash_signature) + sizeof(uint32_t) * 8 * sizeof(sharc_hash_signature))

typedef enum {
    SHARC_HASH_ENCODE_PROCESS_CHECK_STATE,
    SHARC_HASH_ENCODE_PROCESS_PREPARE_NEW_BLOCK,
    SHARC_HASH_ENCODE_PROCESS_DATA,
    SHARC_HASH_ENCODE_PROCESS_FINISH
} SHARC_HASH_ENCODE_PROCESS;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    SHARC_HASH_ENCODE_PROCESS process;

    uint_fast64_t resetCycle;

    uint_fast32_t shift;
    sharc_hash_signature * signature;
    uint_fast32_t signaturesCount;
    uint_fast8_t efficiencyChecked;

    sharc_dictionary dictionary;
} sharc_chameleon_encode_state;
#pragma pack(pop)

SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_init_default(sharc_chameleon_encode_state *);
SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_process_default(sharc_byte_buffer *, sharc_byte_buffer *, sharc_chameleon_encode_state *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_finish_default(sharc_chameleon_encode_state *);

SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_init_dispersion(sharc_chameleon_encode_state *);
SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_process_dispersion(sharc_byte_buffer *, sharc_byte_buffer *, sharc_chameleon_encode_state *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_chameleon_encode_finish_dispersion(sharc_chameleon_encode_state *);

#endif