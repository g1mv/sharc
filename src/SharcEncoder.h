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
 * 18/05/13 18:38
 */

#ifndef SHARC_ENCODER_H
#define SHARC_ENCODER_H

#include <string>
#include <iostream>

#define LOOKUP_TABLE_LENGTH         65536
#define CACHE_SIZE                  16384

#define HASH_BITS                   16
#define HASH_OFFSET_BASIS           2166115717
#define HASH_PRIME                  16777619

typedef unsigned char byte;

#pragma pack(push)
#pragma pack(4)
typedef struct {
	byte offset[3];
    byte exists;
} ENTRY;
#pragma pack(pop)

typedef struct {
    byte size;
    unsigned int* value;
} CACHE_READ;

class SharcEncoder {
private:
    byte lookupTable[LOOKUP_TABLE_LENGTH];
    ENTRY dictionary[1 << HASH_BITS];
    
    CACHE_READ cacheRead;
    
    byte* inBuffer;
    unsigned int inSize;
    unsigned int inPosition;
    
    byte cache[CACHE_SIZE];
    //unsigned int cacheSize;
    unsigned int cachePosition;
    unsigned int cacheMarker;
    bool lastFill;
    
    byte* outBuffer;
    unsigned int outSize;
    unsigned int outPosition;
    
	byte stateLookup;
    byte signatureLookup;
    unsigned int chunksLookup[8];
    
    byte stateHash;
    byte signatureHash;
    unsigned int chunksHash[8];
    
    inline void prepareLookupTable();
    inline void writeLookupSignature(const bool);
	inline void flushCache();
	inline void lookupEncode(const unsigned short);
    inline bool fillCache();
    
    inline bool readFromLookupCache();
    
    inline void computeHash(unsigned int*, const unsigned int*);
    inline bool hashEncode();
    inline bool updateEntry(ENTRY*, const unsigned int*, const unsigned int*);
    inline bool checkHashState();
    inline void writeHashSignature(const bool);
	inline bool flushHash();
    
public:
	SharcEncoder(byte*, const unsigned int, byte*, const unsigned int);
	~SharcEncoder();
    
    
    inline void resetDictionary();
	void resetLookupSignature();
	void resetHashSignature();
	void resetBuffer();
    void resetCache();
    
	inline bool encode();
    
    //CACHE_READ getCacheRead();
};

#endif