/*
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Centaurean BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Sharc
 * www.centaurean.com
 *
 * 01/06/13 17:27
 */

#ifndef HASH_CIPHER_H
#define HASH_CIPHER_H

#include "cipher.h"

#define HASH_BITS                   16
#define HASH_OFFSET_BASIS           2166115717
#define HASH_PRIME                  16777619

#define MAX_BUFFER_REFERENCES       (1 << 24) // 3 bytes, = ENTRY offset size
#define PREFERRED_BUFFER_SIZE       MAX_BUFFER_REFERENCES >> 2 // Has to be < to MAX_BUFFER_REFERENCES << 2

#pragma pack(push)
#pragma pack(4)
typedef struct {
	byte offset[3];
    byte exists;
} ENTRY;
#pragma pack(pop)

ENTRY dictionary[1 << HASH_BITS];
unsigned int signature;
byte state;
unsigned int chunks[32];

void writeSignature(/*const bool*/);
bool flush();
void reset();
void resetDictionary();
bool checkState();
void computeHash(unsigned int*, const unsigned int);
bool updateEntry(ENTRY*, const unsigned int, const unsigned int);

#endif