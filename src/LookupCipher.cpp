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
 * 21/05/13 02:42
 */

#include "LookupCipher.h"

LookupCipher::LookupCipher() {    
    reset();
}

LookupCipher::~LookupCipher() {
}

void LookupCipher::setLookupTable(byte* lookupTable) {
    this->lookupTable = lookupTable;
}

/*inline void LookupCipher::prepareLookupTable() {
    for(unsigned int i = 0; i < LOOKUP_TABLE_LENGTH; i ++)
        lookupTable[i] = 0;
    
    byte counter = 0;
    
    lookupTable['e' + (' ' << 8)] = ++counter;
    lookupTable['s' + (' ' << 8)] = ++counter;
    lookupTable['a' + (' ' << 8)] = ++counter;
    lookupTable['o' + (' ' << 8)] = ++counter;
    lookupTable[' ' + ('d' << 8)] = ++counter;
    lookupTable['e' + ('n' << 8)] = ++counter;
    lookupTable['e' + ('r' << 8)] = ++counter;
    lookupTable[' ' + ('a' << 8)] = ++counter;
    lookupTable['n' + (' ' << 8)] = ++counter;
    lookupTable['d' + ('e' << 8)] = ++counter;
    lookupTable['e' + ('s' << 8)] = ++counter;
    lookupTable[',' + (' ' << 8)] = ++counter;
    lookupTable['r' + ('e' << 8)] = ++counter;
    lookupTable[' ' + ('t' << 8)] = ++counter;
    lookupTable['a' + ('n' << 8)] = ++counter;
    lookupTable['i' + ('n' << 8)] = ++counter;
    lookupTable[' ' + ('e' << 8)] = ++counter;
    lookupTable[' ' + ('p' << 8)] = ++counter;
    lookupTable[' ' + ('s' << 8)] = ++counter;
    lookupTable['o' + ('n' << 8)] = ++counter;
    lookupTable['t' + ('e' << 8)] = ++counter;
    lookupTable[' ' + ('c' << 8)] = ++counter;
    lookupTable['r' + (' ' << 8)] = ++counter;
    lookupTable['a' + ('r' << 8)] = ++counter;
    lookupTable['n' + ('t' << 8)] = ++counter;
    lookupTable['r' + ('a' << 8)] = ++counter;
    lookupTable['o' + ('r' << 8)] = ++counter;
    lookupTable['s' + ('t' << 8)] = ++counter;
    lookupTable['t' + (' ' << 8)] = ++counter;
    lookupTable[' ' + ('l' << 8)] = ++counter;
    lookupTable['h' + ('e' << 8)] = ++counter;
    lookupTable['a' + ('l' << 8)] = ++counter;
    lookupTable['a' + ('s' << 8)] = ++counter;
    lookupTable['l' + ('a' << 8)] = ++counter;
    lookupTable['o' + ('s' << 8)] = ++counter;
    lookupTable['c' + ('o' << 8)] = ++counter;
    lookupTable['t' + ('o' << 8)] = ++counter;
    lookupTable['l' + (' ' << 8)] = ++counter;
    lookupTable['t' + ('a' << 8)] = ++counter;
    lookupTable['t' + ('i' << 8)] = ++counter;
    lookupTable['t' + ('h' << 8)] = ++counter;
    lookupTable['l' + ('e' << 8)] = ++counter;
    lookupTable[' ' + ('i' << 8)] = ++counter;
    lookupTable['e' + ('l' << 8)] = ++counter;
    lookupTable['s' + ('e' << 8)] = ++counter;
    lookupTable['n' + ('d' << 8)] = ++counter;
    lookupTable['d' + (' ' << 8)] = ++counter;
    lookupTable[' ' + ('o' << 8)] = ++counter;
    lookupTable['i' + ('s' << 8)] = ++counter;
    lookupTable[' ' + ('m' << 8)] = ++counter;
    lookupTable['a' + ('t' << 8)] = ++counter;
    lookupTable['r' + ('o' << 8)] = ++counter;
    lookupTable['i' + ('e' << 8)] = ++counter;
    lookupTable['r' + ('i' << 8)] = ++counter;
    lookupTable['n' + ('e' << 8)] = ++counter;
    lookupTable['d' + ('o' << 8)] = ++counter;
    lookupTable['i' + ('t' << 8)] = ++counter;
    lookupTable['m' + ('e' << 8)] = ++counter;
    lookupTable['u' + ('e' << 8)] = ++counter;
    lookupTable['i' + ('c' << 8)] = ++counter;
    lookupTable['c' + ('h' << 8)] = ++counter;
    lookupTable['u' + ('n' << 8)] = ++counter;
    lookupTable['n' + ('a' << 8)] = ++counter;
    lookupTable['y' + (' ' << 8)] = ++counter;
    lookupTable['i' + ('o' << 8)] = ++counter;
    lookupTable[' ' + ('f' << 8)] = ++counter;
    lookupTable['c' + ('i' << 8)] = ++counter;
    lookupTable['a' + ('d' << 8)] = ++counter;
    lookupTable['l' + ('i' << 8)] = ++counter;
    lookupTable['d' + ('i' << 8)] = ++counter;
    lookupTable['q' + ('u' << 8)] = ++counter;
    lookupTable[' ' + ('n' << 8)] = ++counter;
    lookupTable['d' + ('a' << 8)] = ++counter;
    lookupTable['c' + ('a' << 8)] = ++counter;
    lookupTable['s' + ('i' << 8)] = ++counter;
    lookupTable['t' + ('r' << 8)] = ++counter;
    lookupTable['m' + ('a' << 8)] = ++counter;
    lookupTable[' ' + ('w' << 8)] = ++counter;
    lookupTable['p' + ('o' << 8)] = ++counter;
    lookupTable[' ' + ('r' << 8)] = ++counter;
    lookupTable['i' + (' ' << 8)] = ++counter;
    lookupTable['v' + ('e' << 8)] = ++counter;
    lookupTable['n' + ('o' << 8)] = ++counter;
    lookupTable['p' + ('r' << 8)] = ++counter;
    lookupTable['n' + ('i' << 8)] = ++counter;
    lookupTable['i' + ('a' << 8)] = ++counter;
    lookupTable['e' + ('d' << 8)] = ++counter;
    lookupTable['e' + ('m' << 8)] = ++counter;
    lookupTable['o' + ('m' << 8)] = ++counter;
    lookupTable['p' + ('a' << 8)] = ++counter;
    lookupTable['h' + ('a' << 8)] = ++counter;
    lookupTable['l' + ('l' << 8)] = ++counter;
    lookupTable[' ' + ('b' << 8)] = ++counter;
    lookupTable[' ' + ('h' << 8)] = ++counter;
    lookupTable['n' + ('g' << 8)] = ++counter;
    lookupTable['l' + ('o' << 8)] = ++counter;
    lookupTable[' ' + ('u' << 8)] = ++counter;
    lookupTable['m' + (' ' << 8)] = ++counter;
    lookupTable['a' + ('c' << 8)] = ++counter;
    lookupTable['u' + ('r' << 8)] = ++counter;
    lookupTable['c' + ('e' << 8)] = ++counter;
    lookupTable['o' + ('u' << 8)] = ++counter;
    lookupTable['p' + ('e' << 8)] = ++counter;
    lookupTable[' ' + ('q' << 8)] = ++counter;
    lookupTable['i' + ('l' << 8)] = ++counter;
    lookupTable['e' + ('c' << 8)] = ++counter;
    lookupTable['s' + ('o' << 8)] = ++counter;
    lookupTable['a' + ('m' << 8)] = ++counter;
    lookupTable['e' + ('t' << 8)] = ++counter;
    lookupTable['m' + ('i' << 8)] = ++counter;
    lookupTable['i' + ('d' << 8)] = ++counter;
    lookupTable['r' + ('t' << 8)] = ++counter;
    lookupTable['n' + ('s' << 8)] = ++counter;
    lookupTable['o' + ('l' << 8)] = ++counter;
    lookupTable['e' + ('i' << 8)] = ++counter;
    lookupTable['m' + ('o' << 8)] = ++counter;
    lookupTable['g' + ('e' << 8)] = ++counter;
    lookupTable['n' + ('c' << 8)] = ++counter;
    lookupTable['s' + ('a' << 8)] = ++counter;
    lookupTable['b' + ('e' << 8)] = ++counter;
    lookupTable['s' + ('s' << 8)] = ++counter;
    lookupTable['u' + ('s' << 8)] = ++counter;
    lookupTable[' ' + ('v' << 8)] = ++counter;
    lookupTable['i' + ('r' << 8)] = ++counter;
    lookupTable[' ' + ('g' << 8)] = ++counter;
    lookupTable['i' + ('m' << 8)] = ++counter;
    lookupTable['e' + ('a' << 8)] = ++counter;
    lookupTable['h' + (' ' << 8)] = ++counter;
    lookupTable['g' + (' ' << 8)] = ++counter;
    lookupTable['r' + ('s' << 8)] = ++counter;
    lookupTable['u' + (' ' << 8)] = ++counter;
    lookupTable['u' + ('t' << 8)] = ++counter;
    lookupTable['v' + ('i' << 8)] = ++counter;
    lookupTable['h' + ('i' << 8)] = ++counter;
    lookupTable['s' + ('u' << 8)] = ++counter;
    lookupTable['a' + ('i' << 8)] = ++counter;
    lookupTable['o' + ('f' << 8)] = ++counter;
    lookupTable['f' + (' ' << 8)] = ++counter;
    lookupTable['h' + ('o' << 8)] = ++counter;
    lookupTable['f' + ('o' << 8)] = ++counter;
    lookupTable['e' + ('g' << 8)] = ++counter;
    lookupTable[' ' + ('S' << 8)] = ++counter;
    lookupTable['o' + ('t' << 8)] = ++counter;
    lookupTable['m' + ('p' << 8)] = ++counter;
    lookupTable['f' + ('i' << 8)] = ++counter;
    lookupTable['t' + ('u' << 8)] = ++counter;
    lookupTable['a' + ('b' << 8)] = ++counter;
    lookupTable[' ' + ('y' << 8)] = ++counter;
    lookupTable['s' + ('.' << 8)] = ++counter;
    lookupTable['s' + (',' << 8)] = ++counter;
    lookupTable['s' + ('c' << 8)] = ++counter;
    lookupTable['a' + ('u' << 8)] = ++counter;
    lookupTable[' ' + ('C' << 8)] = ++counter;
    lookupTable['i' + ('v' << 8)] = ++counter;
    lookupTable['o' + ('d' << 8)] = ++counter;
    lookupTable['i' + ('g' << 8)] = ++counter;
    lookupTable['u' + ('l' << 8)] = ++counter;
    lookupTable[' ' + ('1' << 8)] = ++counter;
    lookupTable['g' + ('a' << 8)] = ++counter;
    lookupTable['c' + ('t' << 8)] = ++counter;
    lookupTable['w' + ('i' << 8)] = ++counter;
    lookupTable['b' + ('a' << 8)] = ++counter;
    lookupTable['o' + ('c' << 8)] = ++counter;
    lookupTable[' ' + ('M' << 8)] = ++counter;
    lookupTable['v' + ('a' << 8)] = ++counter;
    lookupTable[' ' + ('A' << 8)] = ++counter;
    lookupTable['a' + ('g' << 8)] = ++counter;
    lookupTable['u' + ('i' << 8)] = ++counter;
    lookupTable['o' + ('w' << 8)] = ++counter;
    lookupTable['s' + ('p' << 8)] = ++counter;
    lookupTable['w' + ('a' << 8)] = ++counter;
    lookupTable['c' + ('u' << 8)] = ++counter;
    lookupTable['u' + ('a' << 8)] = ++counter;
    lookupTable['u' + ('m' << 8)] = ++counter;
    lookupTable['g' + ('o' << 8)] = ++counter;
    lookupTable[' ' + ('P' << 8)] = ++counter;
    lookupTable['r' + ('d' << 8)] = ++counter;
    lookupTable['p' + ('l' << 8)] = ++counter;
    lookupTable['f' + ('e' << 8)] = ++counter;
    lookupTable['t' + ('t' << 8)] = ++counter;
    lookupTable['o' + ('p' << 8)] = ++counter;
    lookupTable['g' + ('r' << 8)] = ++counter;
    lookupTable['b' + ('i' << 8)] = ++counter;
    lookupTable['e' + (',' << 8)] = ++counter;
    lookupTable['e' + ('v' << 8)] = ++counter;
    lookupTable['r' + ('m' << 8)] = ++counter;
    lookupTable[' ' + ('B' << 8)] = ++counter;
    lookupTable['v' + ('o' << 8)] = ++counter;
    lookupTable['e' + ('.' << 8)] = ++counter;
    lookupTable['a' + ('p' << 8)] = ++counter;
    lookupTable['a' + ('v' << 8)] = ++counter;
    lookupTable['o' + (',' << 8)] = ++counter;
    lookupTable['w' + ('e' << 8)] = ++counter;
    lookupTable['t' + ('s' << 8)] = ++counter;
    lookupTable['r' + ('n' << 8)] = ++counter;
    lookupTable['g' + ('i' << 8)] = ++counter;
    lookupTable['z' + ('a' << 8)] = ++counter;
    lookupTable['d' + ('u' << 8)] = ++counter;
    lookupTable['p' + ('i' << 8)] = ++counter;
    lookupTable[' ' + ('(' << 8)] = ++counter;
    lookupTable['l' + ('t' << 8)] = ++counter;
    lookupTable['g' + ('u' << 8)] = ++counter;
    lookupTable['r' + ('r' << 8)] = ++counter;
    lookupTable['b' + ('l' << 8)] = ++counter;
    lookupTable['l' + ('u' << 8)] = ++counter;
    lookupTable['b' + ('r' << 8)] = ++counter;
    lookupTable['u' + ('c' << 8)] = ++counter;
    lookupTable[' ' + ('2' << 8)] = ++counter;
    lookupTable['a' + (',' << 8)] = ++counter;
    lookupTable['o' + ('b' << 8)] = ++counter;
    lookupTable['r' + ('u' << 8)] = ++counter;
    lookupTable['e' + ('e' << 8)] = ++counter;
    lookupTable['e' + ('u' << 8)] = ++counter;
    lookupTable['f' + ('a' << 8)] = ++counter;
    lookupTable['a' + ('y' << 8)] = ++counter;
    lookupTable['o' + ('v' << 8)] = ++counter;
    lookupTable[' ' + ('z' << 8)] = ++counter;
    lookupTable['z' + ('e' << 8)] = ++counter;
    lookupTable['m' + ('u' << 8)] = ++counter;
    lookupTable['w' + (' ' << 8)] = ++counter;
    lookupTable[' ' + ('j' << 8)] = ++counter;
    lookupTable['n' + ('n' << 8)] = ++counter;
    lookupTable['m' + ('b' << 8)] = ++counter;
    lookupTable['r' + ('c' << 8)] = ++counter;
    lookupTable['o' + ('.' << 8)] = ++counter;
    lookupTable[' ' + ('R' << 8)] = ++counter;
    lookupTable['e' + ('x' << 8)] = ++counter;
    lookupTable['0' + ('0' << 8)] = ++counter;
    lookupTable[' ' + ('T' << 8)] = ++counter;
    lookupTable[' ' + ('L' << 8)] = ++counter;
    lookupTable['p' + ('u' << 8)] = ++counter;
    lookupTable['b' + ('o' << 8)] = ++counter;
    lookupTable[' ' + ('G' << 8)] = ++counter;
    lookupTable['a' + ('.' << 8)] = ++counter;
    lookupTable['c' + ('r' << 8)] = ++counter;
    lookupTable[' ' + ('E' << 8)] = ++counter;
    lookupTable['e' + ('p' << 8)] = ++counter;
    lookupTable['l' + ('y' << 8)] = ++counter;
    lookupTable[' ' + ('F' << 8)] = ++counter;
    lookupTable['k' + ('e' << 8)] = ++counter;
    lookupTable['o' + ('i' << 8)] = ++counter;
    lookupTable[' ' + ('D' << 8)] = ++counter;
    lookupTable['0' + (' ' << 8)] = ++counter;
    lookupTable['r' + ('g' << 8)] = ++counter;
    lookupTable['n' + ('u' << 8)] = ++counter;
    lookupTable['u' + ('d' << 8)] = ++counter;
    lookupTable[' ' + ('I' << 8)] = ++counter;
    lookupTable['T' + ('h' << 8)] = ++counter;
    lookupTable['f' + ('r' << 8)] = ++counter;
    lookupTable['i' + ('f' << 8)] = ++counter;
    lookupTable['z' + ('i' << 8)] = ++counter;
    lookupTable['l' + ('d' << 8)] = ++counter;
    lookupTable[' ' + ('k' << 8)] = ++counter;
    lookupTable['o' + ('g' << 8)] = ++counter;
    lookupTable['k' + (' ' << 8)] = ++counter;
    //lookupTable['e' + ('f' << 8)] = ++counter;
}*/

FORCE_INLINE void LookupCipher::writeSignature(const bool bit) {
    signature |= bit << (31 - state);
}

FORCE_INLINE bool LookupCipher::flush() {
    if((outPosition + 4 + 64) > outSize)
        return false;
	*(unsigned int*)(outBuffer + outPosition) = signature;
    outPosition += 4;
    for(byte b = 0; b < state; b ++) {
        switch((signature >> (31 - b)) & 0x1) {
            case 0:
                *(unsigned short*)(outBuffer + outPosition) = chunks[b];
                outPosition += 2;
                break;
            case 1:
                *(byte*)(outBuffer + outPosition) = chunks[b];
                outPosition ++;
                break;
        }
    }
    inPosition += (state << 1);
    return true;
}

FORCE_INLINE void LookupCipher::reset() {
    state = 0;
    signature = 0;
}

FORCE_INLINE bool LookupCipher::checkState() {
    switch(state) {
        case 32:
            if(!flush())
                return false;
            reset();
            break;
    }
    return true;
}

FORCE_INLINE bool LookupCipher::encodeShort(const unsigned short chunk) {
    const byte found = lookupTable[chunk];
    if(found) {
        writeSignature(true);
        chunks[state++] = found;
    } else {
        writeSignature(false);
        chunks[state++] = chunk;
    }
    return checkState();
}

inline bool LookupCipher::processEncoding() {
    reset();
    
    const unsigned int* intInBuffer = (const unsigned int*)inBuffer;
    const unsigned int intInSize = inSize >> 2;
    
    for(unsigned int i = 0; i < intInSize; i ++) {
        const unsigned int chunk = intInBuffer[i];
        if(!encodeShort(chunk & 0xFFFF))
            return false;
        if(!encodeShort(chunk >> 16))
            return false;
    }
    
    flush();
    
    const unsigned int remaining = inSize - inPosition;
    for(unsigned int i = 0; i < remaining; i ++) {
        if(outPosition < outSize)
            outBuffer[outPosition ++] = inBuffer[inPosition ++];
        else
            return false;
    }
    
    return true;
}

inline bool LookupCipher::processDecoding() {
    return true;
}

