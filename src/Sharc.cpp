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
 * 11/05/13 18:19
 * @author gpnuma
 */

#include "Sharc.h"

#define HASH_BITS 16

Sharc::Sharc() {
    dictionary = new ENTRY[1 << HASH_BITS];
    reset();
}

Sharc::~Sharc() {
    delete[] dictionary;
}

FORCE_INLINE static void computeHash(unsigned int* hash, const unsigned int* value) {
    *hash = 2166115717;
    *hash ^= *value;
    *hash *= 16777619;
    *hash = (*hash >> (32 - HASH_BITS)) ^ (*hash & 0xFFFF);
}

FORCE_INLINE static void updateEntry(ENTRY* entry, unsigned int* index, unsigned int* outBits) {
    *(unsigned int*)entry = (*index << 1 | 1);
    *outBits += 33;
}

FORCE_INLINE unsigned int Sharc::compress(byte* byteBuffer, unsigned int* byteLength) {
    unsigned int outBits = 0;
    unsigned int hash;
    
    const unsigned int length = *byteLength >> 2;
    const unsigned int* buffer = (const unsigned int*)byteBuffer;
    
    for(unsigned int i = 0; i < length; i ++) {
        computeHash(&hash, &buffer[i]);
        ENTRY* found = &dictionary[hash];
        if(found->exists) {
            if(buffer[i] ^ buffer[found->offset])
                updateEntry(found, &i, &outBits);
            else
                outBits += (HASH_BITS + 1);
        } else {
            updateEntry(found, &i, &outBits);
        }
    }
    
	return outBits;
}

void Sharc::reset() {
    for(unsigned int i = 0; i < (1 << HASH_BITS); i++)
        *(unsigned int*)(dictionary + i) = 0;
}

int main(int argc, char *argv[]) {
	if(argc <= 1)
		exit(0);
    
    Chrono* chrono = new Chrono();
    std::cout << "Chrono initialized" << std::endl;
    
    Sharc* sharc = new Sharc();
    std::cout << "Sharc initialized" << std::endl;
    
    
    const unsigned int readBuffer = 1 << 24;  // Maximum depending on ENTRY.offset length
    std::cout << "Allocating " << readBuffer << " as buffer read" << std::endl;
	byte* readArray = new byte[readBuffer];
    std::cout << "Read array initialized" << std::endl;
    
    for(int i = 1; i < argc; i ++) {
        unsigned int outBits = 0;
        unsigned int totalRead = 0;
        
		std::ifstream file (argv[i], std::ios::in | std::ios::binary);
        chrono->start();
		while(file) {
			file.read ((char*)(readArray), readBuffer);
			unsigned int read = (unsigned int)file.gcount();
			totalRead += read;
            
            outBits += sharc->compress(readArray, &read);
			
			sharc->reset();
		}
		chrono->stop();
		file.close();
        
		std::cout << "--------------------------------------------------------------------" << std::endl;
        
		double outBytes = outBits / 8.0;
		double ratio = outBytes / totalRead;
		std::cout << "File " << argv[i] << ", " << totalRead << " bytes in, " << (unsigned int)outBytes << " bytes out" << std::endl;
        
		double outSpeed = (1000.0 * totalRead) / (chrono->getElapsedMillis() * 1024.0 * 1024.0);
		std::cout  << "Ratio out / in = " << ratio << ", time = " << chrono->getElapsedMillis() << " ms, Speed = " << outSpeed << " MB/s" << std::endl;
        
		std::cout << "COMBINED SCORE = " << outSpeed / ratio << std::endl;
    }
    
	delete readArray;
	delete sharc;
    delete chrono;
}
