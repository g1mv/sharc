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
 * 06/05/13 17:38
 * @author gpnuma
 */

#include "BernsteinHash.h"

BernsteinHash::BernsteinHash(unsigned int hashSize, unsigned int maxWordLength) : HashFunction(hashSize, maxWordLength) {
    byte* temporary = new byte[3];
    
    /*lookupTableOne = new unsigned short int[256];
    for(unsigned short i = 0; i < 256; i++) {
        temporary[0] = (byte)i;
        lookupTableOne[i] = calculateHash(temporary, 0, 1);
    }*/
    
    lookupTableTwo = new unsigned short int*[256];
    for(unsigned short i = 0; i < 256; i++)
        lookupTableTwo[i] = new unsigned short int[256];
    for(unsigned short i = 0; i < 256; i++)
        for(unsigned short j = 0; j < 256; j++) {
            temporary[0] = (byte)i;
            temporary[1] = (byte)j;
            lookupTableTwo[i][j] = calculateHash(temporary, 0, 2);
        }
    
    /*lookupTableThree = new unsigned short int**[256];
    for(unsigned short i = 0; i < 256; i++)
        lookupTableThree[i] = new unsigned short int*[256];
    for(unsigned short i = 0; i < 256; i++)
        for(unsigned short j = 0; j < 256; j++)
            lookupTableThree[i][j] = new unsigned short int[256];
    for(unsigned short i = 0; i < 256; i++)
        for(unsigned short j = 0; j < 256; j++)
            for(unsigned short k = 0; k < 256; k++) {
                temporary[0] = (byte)i;
                temporary[1] = (byte)j;
                temporary[2] = (byte)k;
                lookupTableThree[i][j][k] = calculateHash(temporary, 0, 3);
            }*/
    
    delete temporary;
}

BernsteinHash::~BernsteinHash() {
    /*for(unsigned short i = 0; i < 256; i++)
        for(unsigned short j = 0; j < 256; j++)
            delete[] lookupTableThree[i][j];
    for(unsigned short i = 0; i < 256; i++)
        delete[] lookupTableThree[i];
    delete[] lookupTableThree;*/
    
    for(unsigned short i = 0; i < 256; i++)
        delete[] lookupTableTwo[i];
    delete[] lookupTableTwo;
    
    //delete[] lookupTableOne;
}

unsigned short int BernsteinHash::calculateHash(byte* buffer, unsigned int offset, unsigned int length) {
    switch(length) {
        case 1:
            return 1445 + buffer[offset];
        case 2:
            return (2629 + 33 * buffer[offset] + buffer[offset + 1]) % 4096;
        /*case 3:
            return (741 + 1089 * buffer[offset] + 33 * buffer[offset + 1] + buffer[offset + 2]) % 4096;
        case 4:
            return (3973 + 35937 * buffer[offset] + 1089 * buffer[offset + 1] + 33 * buffer[offset + 2] + buffer[offset + 3]) % 4096;*/
        default:
            //unsigned int hash = (3973 + 35937 * buffer[offset] + 1089 * buffer[offset + 1] + 33 * buffer[offset + 2] + buffer[offset+ 3]) % 4096;
            unsigned int hash = 2629 + 33 * buffer[offset] + buffer[offset + 1];
            //unsigned int hash = 1445 + buffer[offset];
            //unsigned int hash = 5381;
            for(unsigned char i = 2; i < length; i ++)
                hash = ((hash << 5) + hash) + buffer[i + offset];
            return hash % 4096;
    }
    /*unsigned int hash = 5381 * 33 + buffer[offset];
    //unsigned int limit = length <= maxWordLength ? length : maxWordLength;
    for(unsigned char i = 1; i < length; i ++)
        hash = ((hash << 5) + hash) + buffer[i + offset];
    return hash % hashSize;
    //return (buffer[offset] + (length > 1 ? buffer[offset + 1] << 8 : 0)) % hashSize;*/
}

unsigned short int BernsteinHash::hash(byte* buffer, unsigned int offset, unsigned int length) {
    switch(length) {
        /*case 1:
            return lookupTableOne[buffer[offset]];*/
        case 2:
            return lookupTableTwo[buffer[offset]][buffer[offset + 1]];
        /*case 3:
            return lookupTableThree[buffer[offset]][buffer[offset + 1]][buffer[offset + 2]];*/
        default:
            return calculateHash(buffer, offset, length);
    }
}