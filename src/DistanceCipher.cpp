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
 * 29/05/13 19:45
 */

#include "DistanceCipher.h"

DistanceCipher::DistanceCipher() {
}

DistanceCipher::~DistanceCipher() {
}

FORCE_INLINE void DistanceCipher::updateFrequency(unsigned short value) {
    frequencyTable[value] ++;
}

FORCE_INLINE byte DistanceCipher::distance(byte a, byte b) {
    //return (a ^ b) ^ 0xFF;
    //return a +< b;
    //return b;
    return b ^ 0xF0;
    /*if(b >= a) {
        return b - a;
    } else {
        return (a - b) ^ 0xFF;
    }*/
}

FORCE_INLINE void DistanceCipher::resetLookupTable() {
    for(unsigned int i = 0; i < FREQUENCY_TABLE_LENGTH; i ++)
        frequencyTable[i] = 0;
}

unsigned int* DistanceCipher::getFrequencyTable() {
    return frequencyTable;
}

inline bool DistanceCipher::processEncoding() {
    resetLookupTable();
    
    outBuffer[outPosition] = inBuffer[inPosition];
    
    while(inPosition < inSize - 1) {
        byte apart = distance(inBuffer[inPosition], inBuffer[++ inPosition]);
        outBuffer[++ outPosition] = apart;
        switch(outPosition & 0x1) {
            case 1:
                updateFrequency(*(unsigned short*)(outBuffer + outPosition - 1));
                break;
        }
    }
    inPosition ++;
    outPosition ++;
    
    return true;
}

inline bool DistanceCipher::processDecoding() {
    return true;
}

