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
 * 01/06/13 17:27
 */

#ifndef SHARC_HASH_CIPHER_H
#define SHARC_HASH_CIPHER_H

#include "byte_buffer.h"

#include <stdio.h>

#define SHARC_HASH_BITS                   16
#define SHARC_HASH_OFFSET_BASIS           2166115717
#define SHARC_HASH_PRIME                  16777619

#define SHARC_PREFERRED_BUFFER_SIZE       1 << 18
#define SHARC_MAX_BUFFER_SIZE             SHARC_PREFERRED_BUFFER_SIZE

#pragma pack(push)
#pragma pack(4)
typedef union {
    struct {
        uint32_t value;
        uint32_t exists;
    } as_struct;
    uint64_t as_uint64_t;
} SHARC_ENTRY;
#pragma pack(pop)

SHARC_ENTRY dictionary_a[1 << SHARC_HASH_BITS];
SHARC_ENTRY dictionary_b[1 << SHARC_HASH_BITS];

void sharc_writeSignature(uint64_t*, const sharc_byte*);
void sharc_flush(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const uint64_t*, const sharc_byte*, const uint32_t*);
sharc_bool sharc_reset(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, uint64_t*, sharc_byte*, uint32_t*);
void sharc_resetDictionary(SHARC_ENTRY*);
sharc_bool sharc_checkState(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, uint64_t*, sharc_byte*, uint32_t*);
void sharc_computeHash(uint32_t*, const uint32_t, const uint32_t);
sharc_bool sharc_updateEntry(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_ENTRY*, const uint32_t, const uint32_t, uint64_t*, sharc_byte*, uint32_t*);
sharc_bool sharc_kernelEncode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const uint32_t, const uint32_t, const uint32_t*, const uint32_t, SHARC_ENTRY*, uint32_t*, uint64_t*, sharc_byte*, uint32_t*);
void sharc_kernelDecode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_ENTRY*, const uint32_t, const sharc_bool);

sharc_bool sharc_hashEncode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const uint32_t, SHARC_ENTRY*);
void sharc_byteCopy(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const uint32_t);
sharc_bool sharc_hashDecode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, const uint32_t, SHARC_ENTRY*);

#endif