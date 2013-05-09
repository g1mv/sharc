/*
 * Copyright (c) 2013, Centaurean
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
 * acceLZW
 *
 * 08/05/13 02:07
 * @author gpnuma
 */

#include "CHash1.h"

#define FNV_OFFSET_BASIS    2166136261
#define FNV_PRIME           16777619
//#define TINY_MASK(x)        (((u_int32_t)1<<(x))-1)

FORCE_INLINE unsigned short rotl32 (unsigned int x, byte r) {
    return (x << r) | (x >> (32 - r));
}

FORCE_INLINE unsigned short rotr32 (unsigned int x, byte r) {
    return (x >> r) | (x << (32 - r));
}

CHash1::CHash1(unsigned int hashSize, unsigned int maxWordLength) : HashFunction(hashSize, maxWordLength) {
}

CHash1::~CHash1() {
}

FORCE_INLINE unsigned short CHash1::hash(byte* buffer, unsigned int offset, unsigned int length) {
    unsigned int hash = FNV_OFFSET_BASIS;
    
    const uint8_t * data8 = (const uint8_t*)(&buffer[offset]);
    const uint32_t * data32 = (const uint32_t *)(data8);
    
    const int nblocks = length >> 2;

    for(unsigned short c = 0; c < nblocks; c++) {
        hash ^= data32[c];
        hash *= FNV_PRIME;
    }
    
    for(unsigned short c = nblocks << 2; c < length; c ++) {
        hash ^= data8[c];
        hash *= FNV_PRIME;
    }
    
    return (hash >> 16) ^ (hash & 0xFFFF);
    //return ((hash >> 12) ^ hash) & 0x0FFF;
    //return (hash >> 20) ^ (hash & 0x0FFF);
    //return (((hash >> 12) ^ hash) & 0x00000FFF);
    //return hash >> 20;
}