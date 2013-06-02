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
 * 01/06/13 13:10
 */

#include "XORHashCipher.h"

XORHashCipher::XORHashCipher() {
}

XORHashCipher::~XORHashCipher() {
}

FORCE_INLINE bool XORHashCipher::traverse(const unsigned int* intInBuffer, unsigned int intInSize) {
    unsigned int chunk;
    unsigned int xorChunk;
    unsigned int hash;
    
    for(unsigned int i = 0; i < intInSize; i ++) {
        chunk = intInBuffer[i];
        xorChunk = chunk ^ 0xFF0000FF;
        computeHash(&hash, &xorChunk);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
            if(xorChunk ^ intInBuffer[*(unsigned int*)found & 0xFFFFFF]) {
                if(!updateEntry(found, xorChunk, i))
                    return false;
            } else {
                writeSignature(true);
                unsigned short value = (unsigned short)hash;
                chunks[state++] = value;
                if(!checkState())
                    return false;
            }
        } else {
            if(!updateEntry(found, xorChunk, i))
                return false;
        }
    }
    return true;
}