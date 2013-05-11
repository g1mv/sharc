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
 * 07/05/13 16:52
 * @author gpnuma
 */

#ifndef FAST_LZW_H
#define FAST_LZW_H

#include "LZW.h"
#include "hashes/BernsteinHash.h"
#include "hashes/MurmurHash3.h"
#include "hashes/DefaultHash.h"
#include "hashes/CHash1.h"
#include "hashes/SdbmHash.h"
#include "commons.h"
#include <fstream>
#include <cstring>
#include <string.h>
#include <stdint.h>
//#include <pmmintrin.h>

//#pragma pack(push)
typedef struct {
    bool exists				:1;
	unsigned int offset		:24;
	unsigned char length	:7;
} ENTRY;
//#pragma pack(pop)

#define DICTIONARY_HEAP
#define READ_ARRAY_HEAP

class FastLZW : public LZW {
private:
    unsigned int usedKeys;
    unsigned int maxKeyLength;
    unsigned int* keyLengthSpread;
#ifdef DICTIONARY_HEAP
    ENTRY* dictionary;
#else
	ENTRY dictionary[1 << HASH_BITS];
#endif
	HashFunction* hashFunction;
    
public:
	FastLZW(HashFunction*);
	~FastLZW();
    unsigned int compress(byte*, unsigned int, byte*);
    unsigned int decompress(byte*, unsigned int, byte*);
	void reset();
};

#endif