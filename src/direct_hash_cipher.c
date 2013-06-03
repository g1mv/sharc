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
 * 01/06/13 13:08
 */

#include "direct_hash_cipher.h"

FORCE_INLINE bool directHashEncode(byte* _inBuffer, uint32_t _inSize, byte* _outBuffer, uint32_t _outSize) {
    prepareWorkspace(_inBuffer, _inSize, _outBuffer, _outSize);
    
    reset();
    resetDictionary();
    
    const uint32_t* intInBuffer = (const uint32_t*)inBuffer;
    const uint32_t intInSize = inSize >> 2;
    
    uint32_t chunk;
    uint32_t hash;
    
    for(uint32_t i = 0; i < intInSize; i ++) {
        chunk = intInBuffer[i];
        computeHash(&hash, chunk);
        ENTRY* found = &dictionary[hash];        
        if((*(uint32_t*)found) & MAX_BUFFER_REFERENCES) {
            if(chunk ^ intInBuffer[*(uint32_t*)found & 0xFFFFFF]) {
                if(updateEntry(found, chunk, i) ^ 0x1)
                    return FALSE;
            } else {
                writeSignature();
                chunks[state++] = (uint16_t)hash;
                if(checkState() ^ 0x1)
                    return FALSE;
            }
        } else {
            if(updateEntry(found, chunk, i) ^ 0x1)
                return FALSE;
        }
    }
    
    flush();
    
    const uint32_t remaining = inSize - inPosition;
    for(uint32_t i = 0; i < remaining; i ++) {
        if(outPosition < outSize - 1)
            outBuffer[outPosition ++] = inBuffer[inPosition ++];
        else
            return FALSE;
    }
    
    return TRUE;
}

FORCE_INLINE bool directHashDecode(byte* inBuffer, uint32_t inSize, byte* outBuffer, uint32_t outSize) {
    return TRUE;
}
