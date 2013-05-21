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
 * 12/05/13 02:39
 */

#include "SharcCipher.h"

SharcCipher::SharcCipher() {
    prepareLookupTable();
}

SharcCipher::~SharcCipher() {
}

inline void SharcCipher::prepareLookupTable() {
    for(unsigned int i = 0; i < LOOKUP_TABLE_LENGTH; i ++)
        lookupTable[i] = 0;
    
    byte counter = 0;
    
    lookupTable['e' + (' ' << 7)] = ++counter;
    lookupTable['s' + (' ' << 7)] = ++counter;
    lookupTable['a' + (' ' << 7)] = ++counter;
    lookupTable['o' + (' ' << 7)] = ++counter;
    lookupTable[' ' + ('d' << 7)] = ++counter;
    lookupTable['e' + ('n' << 7)] = ++counter;
    lookupTable['e' + ('r' << 7)] = ++counter;
    lookupTable[' ' + ('a' << 7)] = ++counter;
    lookupTable['n' + (' ' << 7)] = ++counter;
    lookupTable['d' + ('e' << 7)] = ++counter;
    lookupTable['e' + ('s' << 7)] = ++counter;
    lookupTable[',' + (' ' << 7)] = ++counter;
    lookupTable['r' + ('e' << 7)] = ++counter;
    lookupTable[' ' + ('t' << 7)] = ++counter;
    lookupTable['a' + ('n' << 7)] = ++counter;
    lookupTable['i' + ('n' << 7)] = ++counter;
    lookupTable[' ' + ('e' << 7)] = ++counter;
    lookupTable[' ' + ('p' << 7)] = ++counter;
    lookupTable[' ' + ('s' << 7)] = ++counter;
    lookupTable['o' + ('n' << 7)] = ++counter;
    lookupTable['t' + ('e' << 7)] = ++counter;
    lookupTable[' ' + ('c' << 7)] = ++counter;
    lookupTable['r' + (' ' << 7)] = ++counter;
    lookupTable['a' + ('r' << 7)] = ++counter;
    lookupTable['n' + ('t' << 7)] = ++counter;
    lookupTable['r' + ('a' << 7)] = ++counter;
    lookupTable['o' + ('r' << 7)] = ++counter;
    lookupTable['s' + ('t' << 7)] = ++counter;
    lookupTable['t' + (' ' << 7)] = ++counter;
    lookupTable[' ' + ('l' << 7)] = ++counter;
    lookupTable['h' + ('e' << 7)] = ++counter;
    lookupTable['a' + ('l' << 7)] = ++counter;
    lookupTable['a' + ('s' << 7)] = ++counter;
    lookupTable['l' + ('a' << 7)] = ++counter;
    lookupTable['o' + ('s' << 7)] = ++counter;
    lookupTable['c' + ('o' << 7)] = ++counter;
    lookupTable['t' + ('o' << 7)] = ++counter;
    lookupTable['l' + (' ' << 7)] = ++counter;
    lookupTable['t' + ('a' << 7)] = ++counter;
    lookupTable['t' + ('i' << 7)] = ++counter;
    lookupTable['t' + ('h' << 7)] = ++counter;
    lookupTable['l' + ('e' << 7)] = ++counter;
    lookupTable[' ' + ('i' << 7)] = ++counter;
    lookupTable['e' + ('l' << 7)] = ++counter;
    lookupTable['s' + ('e' << 7)] = ++counter;
    lookupTable['n' + ('d' << 7)] = ++counter;
    lookupTable['d' + (' ' << 7)] = ++counter;
    lookupTable[' ' + ('o' << 7)] = ++counter;
    lookupTable['i' + ('s' << 7)] = ++counter;
    lookupTable[' ' + ('m' << 7)] = ++counter;
    lookupTable['a' + ('t' << 7)] = ++counter;
    lookupTable['r' + ('o' << 7)] = ++counter;
    lookupTable['i' + ('e' << 7)] = ++counter;
    lookupTable['r' + ('i' << 7)] = ++counter;
    lookupTable['n' + ('e' << 7)] = ++counter;
    lookupTable['d' + ('o' << 7)] = ++counter;
    lookupTable['i' + ('t' << 7)] = ++counter;
    lookupTable['m' + ('e' << 7)] = ++counter;
    lookupTable['u' + ('e' << 7)] = ++counter;
    lookupTable['i' + ('c' << 7)] = ++counter;
    lookupTable['c' + ('h' << 7)] = ++counter;
    lookupTable['u' + ('n' << 7)] = ++counter;
    lookupTable['n' + ('a' << 7)] = ++counter;
    lookupTable['y' + (' ' << 7)] = ++counter;
    lookupTable['i' + ('o' << 7)] = ++counter;
    lookupTable[' ' + ('f' << 7)] = ++counter;
    lookupTable['c' + ('i' << 7)] = ++counter;
    lookupTable['a' + ('d' << 7)] = ++counter;
    lookupTable['l' + ('i' << 7)] = ++counter;
    lookupTable['d' + ('i' << 7)] = ++counter;
    lookupTable['q' + ('u' << 7)] = ++counter;
    lookupTable[' ' + ('n' << 7)] = ++counter;
    lookupTable['d' + ('a' << 7)] = ++counter;
    lookupTable['c' + ('a' << 7)] = ++counter;
    lookupTable['s' + ('i' << 7)] = ++counter;
    lookupTable['t' + ('r' << 7)] = ++counter;
    lookupTable['m' + ('a' << 7)] = ++counter;
    lookupTable[' ' + ('w' << 7)] = ++counter;
    lookupTable['p' + ('o' << 7)] = ++counter;
    lookupTable[' ' + ('r' << 7)] = ++counter;
    lookupTable['i' + (' ' << 7)] = ++counter;
    lookupTable['v' + ('e' << 7)] = ++counter;
    lookupTable['n' + ('o' << 7)] = ++counter;
    lookupTable['p' + ('r' << 7)] = ++counter;
    lookupTable['n' + ('i' << 7)] = ++counter;
    lookupTable['i' + ('a' << 7)] = ++counter;
    lookupTable['e' + ('d' << 7)] = ++counter;
    lookupTable['e' + ('m' << 7)] = ++counter;
    lookupTable['o' + ('m' << 7)] = ++counter;
    lookupTable['p' + ('a' << 7)] = ++counter;
    lookupTable['h' + ('a' << 7)] = ++counter;
    lookupTable['l' + ('l' << 7)] = ++counter;
    lookupTable[' ' + ('b' << 7)] = ++counter;
    lookupTable[' ' + ('h' << 7)] = ++counter;
    lookupTable['n' + ('g' << 7)] = ++counter;
    lookupTable['l' + ('o' << 7)] = ++counter;
    lookupTable[' ' + ('u' << 7)] = ++counter;
    lookupTable['m' + (' ' << 7)] = ++counter;
    lookupTable['a' + ('c' << 7)] = ++counter;
    lookupTable['u' + ('r' << 7)] = ++counter;
    lookupTable['c' + ('e' << 7)] = ++counter;
    lookupTable['o' + ('u' << 7)] = ++counter;
    lookupTable['p' + ('e' << 7)] = ++counter;
    lookupTable[' ' + ('q' << 7)] = ++counter;
    lookupTable['i' + ('l' << 7)] = ++counter;
    lookupTable['e' + ('c' << 7)] = ++counter;
    lookupTable['s' + ('o' << 7)] = ++counter;
    lookupTable['a' + ('m' << 7)] = ++counter;
    lookupTable['e' + ('t' << 7)] = ++counter;
    lookupTable['m' + ('i' << 7)] = ++counter;
    lookupTable['i' + ('d' << 7)] = ++counter;
    lookupTable['r' + ('t' << 7)] = ++counter;
    lookupTable['n' + ('s' << 7)] = ++counter;
    lookupTable['o' + ('l' << 7)] = ++counter;
    lookupTable['e' + ('i' << 7)] = ++counter;
    lookupTable['m' + ('o' << 7)] = ++counter;
    lookupTable['g' + ('e' << 7)] = ++counter;
    lookupTable['n' + ('c' << 7)] = ++counter;
    lookupTable['s' + ('a' << 7)] = ++counter;
    lookupTable['b' + ('e' << 7)] = ++counter;
    lookupTable['s' + ('s' << 7)] = ++counter;
    lookupTable['u' + ('s' << 7)] = ++counter;
    lookupTable[' ' + ('v' << 7)] = ++counter;
    lookupTable['i' + ('r' << 7)] = ++counter;
    lookupTable[' ' + ('g' << 7)] = ++counter;
    lookupTable['i' + ('m' << 7)] = ++counter;
    lookupTable['e' + ('a' << 7)] = ++counter;
    lookupTable['h' + (' ' << 7)] = ++counter;
    lookupTable['g' + (' ' << 7)] = ++counter;
    lookupTable['r' + ('s' << 7)] = ++counter;
    lookupTable['u' + (' ' << 7)] = ++counter;
    lookupTable['u' + ('t' << 7)] = ++counter;
    lookupTable['v' + ('i' << 7)] = ++counter;
    lookupTable['h' + ('i' << 7)] = ++counter;
    lookupTable['s' + ('u' << 7)] = ++counter;
    lookupTable['a' + ('i' << 7)] = ++counter;
    lookupTable['o' + ('f' << 7)] = ++counter;
    lookupTable['f' + (' ' << 7)] = ++counter;
    lookupTable['h' + ('o' << 7)] = ++counter;
    lookupTable['f' + ('o' << 7)] = ++counter;
    lookupTable['e' + ('g' << 7)] = ++counter;
    lookupTable[' ' + ('S' << 7)] = ++counter;
    lookupTable['o' + ('t' << 7)] = ++counter;
    lookupTable['m' + ('p' << 7)] = ++counter;
    lookupTable['f' + ('i' << 7)] = ++counter;
    lookupTable['t' + ('u' << 7)] = ++counter;
    lookupTable['a' + ('b' << 7)] = ++counter;
    lookupTable[' ' + ('y' << 7)] = ++counter;
    lookupTable['s' + ('.' << 7)] = ++counter;
    lookupTable['s' + (',' << 7)] = ++counter;
    lookupTable['s' + ('c' << 7)] = ++counter;
    lookupTable['a' + ('u' << 7)] = ++counter;
    lookupTable[' ' + ('C' << 7)] = ++counter;
    lookupTable['i' + ('v' << 7)] = ++counter;
    lookupTable['o' + ('d' << 7)] = ++counter;
    lookupTable['i' + ('g' << 7)] = ++counter;
    lookupTable['u' + ('l' << 7)] = ++counter;
    lookupTable[' ' + ('1' << 7)] = ++counter;
    lookupTable['g' + ('a' << 7)] = ++counter;
    lookupTable['c' + ('t' << 7)] = ++counter;
    lookupTable['w' + ('i' << 7)] = ++counter;
    lookupTable['b' + ('a' << 7)] = ++counter;
    lookupTable['o' + ('c' << 7)] = ++counter;
    lookupTable[' ' + ('M' << 7)] = ++counter;
    lookupTable['v' + ('a' << 7)] = ++counter;
    lookupTable[' ' + ('A' << 7)] = ++counter;
    lookupTable['a' + ('g' << 7)] = ++counter;
    lookupTable['u' + ('i' << 7)] = ++counter;
    lookupTable['o' + ('w' << 7)] = ++counter;
    lookupTable['s' + ('p' << 7)] = ++counter;
    lookupTable['w' + ('a' << 7)] = ++counter;
    lookupTable['c' + ('u' << 7)] = ++counter;
    lookupTable['u' + ('a' << 7)] = ++counter;
    lookupTable['u' + ('m' << 7)] = ++counter;
    lookupTable['g' + ('o' << 7)] = ++counter;
    lookupTable[' ' + ('P' << 7)] = ++counter;
    lookupTable['r' + ('d' << 7)] = ++counter;
    lookupTable['p' + ('l' << 7)] = ++counter;
    lookupTable['f' + ('e' << 7)] = ++counter;
    lookupTable['t' + ('t' << 7)] = ++counter;
    lookupTable['o' + ('p' << 7)] = ++counter;
    lookupTable['g' + ('r' << 7)] = ++counter;
    lookupTable['b' + ('i' << 7)] = ++counter;
    lookupTable['e' + (',' << 7)] = ++counter;
    lookupTable['e' + ('v' << 7)] = ++counter;
    lookupTable['r' + ('m' << 7)] = ++counter;
    lookupTable[' ' + ('B' << 7)] = ++counter;
    lookupTable['v' + ('o' << 7)] = ++counter;
    lookupTable['e' + ('.' << 7)] = ++counter;
    lookupTable['a' + ('p' << 7)] = ++counter;
    lookupTable['a' + ('v' << 7)] = ++counter;
    lookupTable['o' + (',' << 7)] = ++counter;
    lookupTable['w' + ('e' << 7)] = ++counter;
    lookupTable['t' + ('s' << 7)] = ++counter;
    lookupTable['r' + ('n' << 7)] = ++counter;
    lookupTable['g' + ('i' << 7)] = ++counter;
    lookupTable['z' + ('a' << 7)] = ++counter;
    lookupTable['d' + ('u' << 7)] = ++counter;
    lookupTable['p' + ('i' << 7)] = ++counter;
    lookupTable[' ' + ('(' << 7)] = ++counter;
    lookupTable['l' + ('t' << 7)] = ++counter;
    lookupTable['g' + ('u' << 7)] = ++counter;
    lookupTable['r' + ('r' << 7)] = ++counter;
    lookupTable['b' + ('l' << 7)] = ++counter;
    lookupTable['l' + ('u' << 7)] = ++counter;
    lookupTable['b' + ('r' << 7)] = ++counter;
    lookupTable['u' + ('c' << 7)] = ++counter;
    lookupTable[' ' + ('2' << 7)] = ++counter;
    lookupTable['a' + (',' << 7)] = ++counter;
    lookupTable['o' + ('b' << 7)] = ++counter;
    lookupTable['r' + ('u' << 7)] = ++counter;
    lookupTable['e' + ('e' << 7)] = ++counter;
    lookupTable['e' + ('u' << 7)] = ++counter;
    lookupTable['f' + ('a' << 7)] = ++counter;
    lookupTable['a' + ('y' << 7)] = ++counter;
    lookupTable['o' + ('v' << 7)] = ++counter;
    lookupTable[' ' + ('z' << 7)] = ++counter;
    lookupTable['z' + ('e' << 7)] = ++counter;
    lookupTable['m' + ('u' << 7)] = ++counter;
    lookupTable['w' + (' ' << 7)] = ++counter;
    lookupTable[' ' + ('j' << 7)] = ++counter;
    lookupTable['n' + ('n' << 7)] = ++counter;
    lookupTable['m' + ('b' << 7)] = ++counter;
    lookupTable['r' + ('c' << 7)] = ++counter;
    lookupTable['o' + ('.' << 7)] = ++counter;
    lookupTable[' ' + ('R' << 7)] = ++counter;
    lookupTable['e' + ('x' << 7)] = ++counter;
    lookupTable['0' + ('0' << 7)] = ++counter;
    lookupTable[' ' + ('T' << 7)] = ++counter;
    lookupTable[' ' + ('L' << 7)] = ++counter;
    lookupTable['p' + ('u' << 7)] = ++counter;
    lookupTable['b' + ('o' << 7)] = ++counter;
    lookupTable[' ' + ('G' << 7)] = ++counter;
    lookupTable['a' + ('.' << 7)] = ++counter;
    lookupTable['c' + ('r' << 7)] = ++counter;
    lookupTable[' ' + ('E' << 7)] = ++counter;
    lookupTable['e' + ('p' << 7)] = ++counter;
    lookupTable['l' + ('y' << 7)] = ++counter;
    lookupTable[' ' + ('F' << 7)] = ++counter;
    lookupTable['k' + ('e' << 7)] = ++counter;
    lookupTable['o' + ('i' << 7)] = ++counter;
    lookupTable[' ' + ('D' << 7)] = ++counter;
    lookupTable['0' + (' ' << 7)] = ++counter;
    lookupTable['r' + ('g' << 7)] = ++counter;
    lookupTable['n' + ('u' << 7)] = ++counter;
    lookupTable['u' + ('d' << 7)] = ++counter;
    lookupTable[' ' + ('I' << 7)] = ++counter;
    lookupTable['T' + ('h' << 7)] = ++counter;
    lookupTable['f' + ('r' << 7)] = ++counter;
    lookupTable['i' + ('f' << 7)] = ++counter;
    lookupTable['z' + ('i' << 7)] = ++counter;
    lookupTable['l' + ('d' << 7)] = ++counter;
    lookupTable[' ' + ('k' << 7)] = ++counter;
    lookupTable['o' + ('g' << 7)] = ++counter;
    lookupTable['k' + (' ' << 7)] = ++counter;
    //lookupTable['e' + ('f' << 7)] = ++counter;
}

inline void SharcCipher::updateSignature(byte* signature, byte* offset, const bool bit) {
    (*signature) |= (bit << (7 - *offset));
}

inline void SharcCipher::processLookup(byte* signature, byte* offset, const unsigned short key, unsigned int* chunks) {
    byte found;
    unsigned short lookupKey;
    
    if((key & 0xFFFF) > 0x7F)
        goto no_encoding;
    
    if((key >> 8) > 0x7F)
        goto no_encoding;
    
    lookupKey = (key & 0xFFFF) + ((key >> 1) & 0xFFFF0000);
    
    found = lookupTable[lookupKey];
    if(found)
        goto encoding;
    else
        goto no_encoding;
    
encoding:
    chunks[*offset] = found;
    updateSignature(signature, offset, true);
    return;
    
    
no_encoding:
    chunks[*offset] = key;
    updateSignature(signature, offset, false);
    return;
}

inline bool SharcCipher::incrementOffset(byte* offset, byte* signature, unsigned int* position, byte* outBuffer, unsigned int* outSize) {
    (*offset) ++;
    if(*offset > 7) {
        if((*position + 1 + 16) > *outSize)
            return false;
        
        outBuffer[*position] = *signature;
        (*position) ++;
        for(byte c = 0; c < *offset; c ++) {
            switch((*signature >> (7 - c)) & 0x1) {
                case 0:
                    *(unsigned short*)(outBuffer + *position) = chunks[c];
                    (*position) += 2;
                    break;
                case 1:
                    *(byte*)(outBuffer + *position) = chunks[c];
                    (*position) ++;
                    break;
            }
        }
        return true;
    }
}

inline unsigned int SharcCipher::encode(const byte* inBuffer, const unsigned int inLength, const byte* outBuffer, const unsigned int outLength) {
    const unsigned int* intInBuffer = (const unsigned int*)inBuffer;
    const unsigned int intInLength = inLength >> 2;
    
    unsigned int cachePosition = 0;
    byte signature = 0;
    byte offset = 0;
    unsigned int chunks[8];
    
    for(unsigned int i = 0; i < intInLength; i ++) {
        unsigned int intRead = intInBuffer[i];
        
        processLookup(&signature, &offset, intRead >> 16, chunks);
        processLookup(&signature, &offset, intRead & 0xFFFF, chunks);
        
    }
    
    return SHARC_CIPHER_ERROR;
}

FORCE_INLINE bool SharcCipher::checkState() {
    switch(state) {
        case 8:
            if(!flush())
                return false;
            resetModes();
    }
    return true;
}

inline unsigned int SharcCipher::decode(byte*, unsigned int, byte*, unsigned int) {
    
}

FORCE_INLINE bool SharcCipher::writeChunk(unsigned short value) {
    //writeMode(true);
    //std::cout << "s = " << value << std::endl;
    chunks[state++] = value;
    //std::cout << "st = " << *(unsigned short*)(chunks + state - 1) << std::endl;
    return checkState();
}

FORCE_INLINE bool SharcCipher::writeChunk(unsigned int value) {
    //writeMode(false);
    //std::cout << "i = " << value << std::endl;
    chunks[state++] = value;
    //std::cout << "it = " << *(unsigned int*)(chunks + state - 1) << std::endl;
    return checkState();
}

FORCE_INLINE void SharcCipher::resetModes() {
    state = 0;
    chunksModes = 0;
}

FORCE_INLINE void SharcCipher::resetBuffer() {
	position = 0;
}

FORCE_INLINE bool SharcCipher::flush() {
    //std::cout << position << ", " << length << std::endl;
    if((position + 1 + 32) > limit)
        return false;
    //std::cout << position << ", " << chunksModes << std::endl;
	buffer[position] = chunksModes;
    position ++;
    for(byte c = 0; c < state; c ++) {
        switch((chunksModes >> (7 - c)) & 0x1) {
            case 0:
                *(unsigned int*)(buffer + position) = chunks[c];
                position += 4;
                break;
            case 1:
                *(unsigned short*)(buffer + position) = chunks[c];
                position += 2;
                break;
        }
        //std::cout << "f = " << chunks[c] << std::endl;
    }
    //resetBuffer();
	//return &currentByte;
    return true;
}

FORCE_INLINE unsigned int SharcCipher::getPosition() {
    return position;
}

FORCE_INLINE void SharcCipher::setLimit(unsigned int limit) {
    this->limit = limit;
}

FORCE_INLINE unsigned int SharcCipher::getLimit() {
    return limit;
}
