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
 * 22/05/13 02:41
 */

#include "HashCipher.h"

HashCipher::HashCipher() {
    reset();
}

HashCipher::~HashCipher() {
}

FORCE_INLINE void HashCipher::writeSignature(const bool bit) {
    signature |= bit << (31 - state);
}

/*FORCE_INLINE void HashCipher::updateFrequency(byte value) {
    frequencyTable[value] ++;
}*/

FORCE_INLINE bool HashCipher::flush() {
    if((outPosition + 4 + 128) > outSize)
        return false;
	*(unsigned int*)(outBuffer + outPosition) = signature;
    //updateFrequency(signature & 0xFF);
    //updateFrequency((signature >> 8) & 0xFF);
    //updateFrequency((signature >> 16) & 0xFF);
    //updateFrequency((signature >> 24) & 0xFF);
    outPosition += 4;
    for(byte b = 0; b < state; b ++) {
        unsigned int chunk = chunks[b];
        switch((signature >> (31 - b)) & 0x1) {
            case 0:
                *(unsigned int*)(outBuffer + outPosition) = chunk;
                //updateFrequency(chunk & 0xFF);
                //updateFrequency((chunk >> 8) & 0xFF);
                //updateFrequency((chunk >> 16) & 0xFF);
                //updateFrequency((chunk >> 24) & 0xFF);
                outPosition += 4;
                break;
            case 1:
                *(unsigned short*)(outBuffer + outPosition) = chunk;
                //updateFrequency(chunk & 0xFF);
                //updateFrequency((chunk >> 8) & 0xFF);
                outPosition += 2;
                break;
        }
    }
    inPosition += (state << 2);
    return true;
}

inline void HashCipher::reset() {
    state = 0;
    signature = 0;
}

inline void HashCipher::resetDictionary() {
    for(unsigned int i = 0; i < (1 << HASH_BITS); i ++)
        dictionary[i].exists = 0;
}

/*inline void HashCipher::resetLookupTable() {
    for(unsigned int i = 0; i < FREQUENCY_TABLE_LENGTH; i ++)
        frequencyTable[i] = 0;
}*/

FORCE_INLINE bool HashCipher::checkState() {
    switch(state) {
        case 32:
            if(!flush())
                return false;
            reset();
            break;
    }
    return true;
}

FORCE_INLINE void HashCipher::computeHash(unsigned int* hash, const unsigned int* value) {
    *hash = HASH_OFFSET_BASIS;
    *hash ^= *value;
    *hash *= HASH_PRIME;
    *hash = (*hash >> (32 - HASH_BITS)) ^ (*hash & 0xFFFF);
}

FORCE_INLINE bool HashCipher::updateEntry(ENTRY* entry, const unsigned int* intInBuffer, const unsigned int* index) {
	*(unsigned int*)entry = (*index & 0xFFFFFF) | (1 << 24);
	writeSignature(false);
    unsigned int value = transform((unsigned int)*(intInBuffer + *index));
    chunks[state++] = value;
    return checkState();
}

inline bool HashCipher::processEncoding() {
    reset();
    resetDictionary();
    //resetLookupTable();
    
    unsigned int hash;
    
    const unsigned int* intInBuffer = (const unsigned int*)inBuffer;
    const unsigned int intInSize = inSize >> 2;
    
    for(unsigned int i = 0; i < intInSize; i ++) {
        const unsigned int chunk = transform(intInBuffer[i])/* ^ 0xF0F0F0F0*//* ^ 0xFFFFFFFF*/;
        computeHash(&hash, &chunk);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
            if(transform(intInBuffer[i]) ^ transform(intInBuffer[*(unsigned int*)found & 0xFFFFFF])) {
                if(!updateEntry(found, intInBuffer, &i))
                    return false;
            } else {
                writeSignature(true);
                unsigned short value = (unsigned short)hash;
                chunks[state++] = value;
                if(!checkState())
                    return false;
            }
        } else {
            if(!updateEntry(found, intInBuffer, &i))
                return false;
        }
    }
    
    flush();
    
    const unsigned int remaining = inSize - inPosition;
    for(unsigned int i = 0; i < remaining; i ++) {
        if(outPosition < outSize - 1)
            outBuffer[outPosition ++] = inBuffer[inPosition ++];
        else
            return false;
    }
    
    return true;
}

inline bool HashCipher::processDecoding() {
    return true;
}

/*unsigned int* HashCipher::getFrequencyTable() {
    return frequencyTable;
}*/

/*int main(int argc, char *argv[]) {
 byte* in = (byte*)"This is a test test test test test test test te";
 byte out[64];
 
 HashCipher* look = new HashCipher();
 look->encode(in, 47, out, 64);
}*/

