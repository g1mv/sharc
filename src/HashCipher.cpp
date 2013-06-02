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

FORCE_INLINE bool HashCipher::flush() {
    if((outPosition + 4 + 128) > outSize)
        return false;
	*(unsigned int*)(outBuffer + outPosition) = signature;
    outPosition += 4;
    for(byte b = 0; b < state; b ++) {
        unsigned int chunk = chunks[b];
        switch((signature >> (31 - b)) & 0x1) {
            case 0:
                *(unsigned int*)(outBuffer + outPosition) = chunk;
                outPosition += 4;
                break;
            case 1:
                *(unsigned short*)(outBuffer + outPosition) = chunk;
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

FORCE_INLINE bool HashCipher::updateEntry(ENTRY* entry, const unsigned int chunk, const unsigned int index) {
	*(unsigned int*)entry = (index & 0xFFFFFF) | (1 << 24);
	writeSignature(false);
    chunks[state++] = chunk;
    return checkState();
}

FORCE_INLINE bool HashCipher::traverse(const unsigned int* intInBuffer, unsigned int intInSize) {
    unsigned int chunk;
    unsigned int hash;
    
    for(unsigned int i = 0; i < intInSize; i ++) {
        chunk = intInBuffer[i];
        computeHash(&hash, &chunk);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
            if(chunk ^ intInBuffer[*(unsigned int*)found & 0xFFFFFF]) {
                if(!updateEntry(found, chunk, i))
                    return false;
            } else {
                writeSignature(true);
                unsigned short value = (unsigned short)hash;
                chunks[state++] = value;
                if(!checkState())
                    return false;
            }
        } else {
            if(!updateEntry(found, chunk, i))
                return false;
        }
    }
    return true;
}

inline bool HashCipher::processEncoding() {
    reset();
    resetDictionary();
    
    const unsigned int* intInBuffer = (const unsigned int*)inBuffer;
    const unsigned int intInSize = inSize >> 2;
    
    /*if(!traverse(intInBuffer, intInSize))
        return false;*/
    unsigned int chunk;
    unsigned int hash;
    
    for(unsigned int i = 0; i < intInSize; i ++) {
        chunk = intInBuffer[i];
        computeHash(&hash, &chunk);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
            if(chunk ^ intInBuffer[*(unsigned int*)found & 0xFFFFFF]) {
                if(!updateEntry(found, chunk, i))
                    return false;
            } else {
                writeSignature(true);
                unsigned short value = (unsigned short)hash;
                chunks[state++] = value;
                if(!checkState())
                    return false;
            }
        } else {
            if(!updateEntry(found, chunk, i))
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