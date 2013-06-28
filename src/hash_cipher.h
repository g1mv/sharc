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

#ifndef HASH_CIPHER_H
#define HASH_CIPHER_H

#include "byte_buffer.h"

#include <stdio.h>

#define HASH_BITS                   16
#define HASH_OFFSET_BASIS           2166115717	//14695981039346656037//
#define HASH_PRIME                  16777619	//1099511628211//

#define MAX_BUFFER_REFERENCES       (1 << 24) // 3 bytes, = ENTRY offset size
#define PREFERRED_BUFFER_SIZE       MAX_BUFFER_REFERENCES >> 2 // Has to be < to MAX_BUFFER_REFERENCES << 2
#define MAX_BUFFER_SIZE             PREFERRED_BUFFER_SIZE

#pragma pack(push)
#pragma pack(4)
typedef struct {
	byte offset[3];
    byte exists;
} ENTRY;
#pragma pack(pop)

void writeSignature(uint64_t*, const byte*);
void flush(BYTE_BUFFER*, BYTE_BUFFER*, const uint64_t*, const byte*, const uint32_t*);
bool reset(BYTE_BUFFER*, BYTE_BUFFER*, uint64_t*, byte*, uint32_t*);
void resetDictionary(ENTRY*);
bool checkState(BYTE_BUFFER*, BYTE_BUFFER*, uint64_t*, byte*, uint32_t*);
void computeHash(uint32_t*, const uint32_t, const uint32_t);
bool updateEntry(BYTE_BUFFER*, BYTE_BUFFER*, ENTRY*, const uint32_t, const uint32_t, uint64_t*, byte*, uint32_t*);
bool kernelEncode(BYTE_BUFFER*, BYTE_BUFFER*, const uint32_t, const uint32_t, const uint32_t*, const uint32_t, ENTRY*, uint32_t*, uint64_t*, byte*, uint32_t*);
void kernelDecode(BYTE_BUFFER*, BYTE_BUFFER*, ENTRY*, const uint32_t, const bool);

bool hashEncode(BYTE_BUFFER*, BYTE_BUFFER*, const uint32_t);
void byteCopy(BYTE_BUFFER*, BYTE_BUFFER*, const uint32_t);
bool hashDecode(BYTE_BUFFER*, BYTE_BUFFER*, const uint32_t);

#endif