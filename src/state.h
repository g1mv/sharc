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
 * 26/08/13 23:19
 */

#ifndef SHARC_INTERNAL_STATE_H
#define SHARC_INTERNAL_STATE_H

#include "globals.h"
#include "dictionary.h"
#include "byte_buffer.h"
#include "api.h"

#define SHARC_INTERNAL_STATE_PREFERRED_WORK_BUFFER_SIZE    4096

typedef enum {
    SHARC_STATE_STATUS_IDLE,
    SHARC_STATE_STATUS_READY_TO_COMPRESS,
    SHARC_STATE_STATUS_NEW_BLOCK_PREPARATION_REQUIRED
} SHARC_STATE_STATUS;

typedef enum {
    SHARC_STATE_COMPRESSION_MODE_NO_COMPRESSION,
    SHARC_STATE_COMPRESSION_MODE_FASTEST_NO_REVERSION,
    SHARC_STATE_COMPRESSION_MODE_FASTEST_WITH_REVERSION,
    SHARC_STATE_COMPRESSION_MODE_DUAL_PASS_NO_REVERSION,
    SHARC_STATE_COMPRESSION_MODE_DUAL_PASS_WITH_REVERSION,
} SHARC_STATE_COMPRESSION_MODE;

typedef struct {
    uint64_t as_uint64_t;
} sharc_signature;

typedef struct {
    SHARC_STATE_STATUS status;
    SHARC_STATE_COMPRESSION_MODE mode;
    uint32_t blockSignaturesCount;
    uint64_t totalBytesReadAtLastBlockStart;
    uint64_t totalBytesWrittenAtLastBlockStart;
    uint32_t hash;
    uint32_t shift;
    sharc_signature * signature;
    sharc_byte_buffer * workBuffer;
    sharc_dictionary * dictionary_a;
    sharc_dictionary * dictionary_b;
    uint32_t dictionariesResetCycle;
} sharc_state;

sharc_state *sharc_state_allocate();
void sharc_state_deallocate(sharc_state *);

#endif