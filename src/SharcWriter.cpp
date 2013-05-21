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
 * Sharc
 *
 * 12/05/13 02:39
 * @author gpnuma
 */

#include "SharcWriter.h"

SharcWriter::SharcWriter(byte* buffer, unsigned int length) {
    this->buffer = buffer;
    this->length = length;
    resetBuffer();
}

SharcWriter::~SharcWriter() {
}

FORCE_INLINE void SharcWriter::writeMode(bool bit) {
    chunksModes |= bit << (7 - state);
}

FORCE_INLINE bool SharcWriter::checkState() {
    switch(state) {
    case 8:
        if(!flush())
            return false;
        resetModes();
    }
    return true;
}

FORCE_INLINE bool SharcWriter::writeChunk(unsigned short value) {
    writeMode(true);
    //std::cout << "s = " << value << std::endl;
    chunks[state++] = value;
    //std::cout << "st = " << *(unsigned short*)(chunks + state - 1) << std::endl;
    return checkState();
}

FORCE_INLINE bool SharcWriter::writeChunk(unsigned int value) {
    writeMode(false);
    //std::cout << "i = " << value << std::endl;
    chunks[state++] = value;
    //std::cout << "it = " << *(unsigned int*)(chunks + state - 1) << std::endl;
    return checkState();
}

FORCE_INLINE void SharcWriter::resetModes() {
    state = 0;
    chunksModes = 0;
}

FORCE_INLINE void SharcWriter::resetBuffer() {
	position = 0;
}

FORCE_INLINE bool SharcWriter::flush() {
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

FORCE_INLINE unsigned int SharcWriter::getPosition() {
    return position;
}

FORCE_INLINE void SharcWriter::setLimit(unsigned int limit) {
    this->limit = limit;
}

FORCE_INLINE unsigned int SharcWriter::getLimit() {
    return limit;
}
