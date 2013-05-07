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
 * 07/05/13 16:52
 * @author gpnuma
 */

#include "FastLZW.h"

FastLZW::FastLZW(HashFunction* hashFunction) {
	this->hashFunction = hashFunction;
    dictionary = new ENTRY[hashFunction->getHashSize()];
    /*for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
        dictionary[i] = *new ENTRY();
    for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
        std::cout << i << ", " << &dictionary[i] << std::endl;*/
    keyLengthSpread = new unsigned int[hashFunction->getMaxWordLength()];
    reset();
}

FastLZW::~FastLZW() {
    delete[] dictionary;
}

static bool theSame(byte* input, ENTRY* entry, unsigned int offset, unsigned int length) {
    if(entry->length != length)
        return false;
    if(input[entry->offset] != input[offset])
        return false;
    for(unsigned int i = 1; i < length; i ++)
		if(input[entry->offset + i] != input[offset + i])
			return false;
    return true;
}

unsigned int FastLZW::compress(byte* input, unsigned int inputLength, byte* output) {
	unsigned int compressedBits = 0;
	unsigned int indexStart = 256;
	for(unsigned int i = 256; i < inputLength; i++) {
        unsigned int length = i + 1 - indexStart;
        unsigned short hashCode = hashFunction->hash(input, indexStart, length);
        ENTRY* found = &dictionary[hashCode];
        if(!found->exists) {
            //std::cout << hashCode << ", " << &dictionary[hashCode] << std::endl;
            found->exists = true;
            usedKeys ++;
            found->offset = indexStart;
            if(length > maxKeyLength)
                maxKeyLength = length;
            found->length = length;
            compressedBits += 12;
            indexStart = i;
        } else if(!theSame(input, found, indexStart, length)) {
            if(length > maxKeyLength)
                maxKeyLength = length;
            keyLengthSpread[length - 1]++;
            found->offset = indexStart;
            found->length = length;
            compressedBits += 12;
            indexStart = i;
        }
	}
	std::cout << "Dictionary used keys : " << usedKeys << " / " << hashFunction->getHashSize() << std::endl;
	std::cout << "Dictionary max key length : " << maxKeyLength << " / " << hashFunction->getMaxWordLength() << std::endl;
    std::cout << "Key length spread :" << std::endl;
    for(unsigned int i = 0; i < hashFunction->getMaxWordLength(); i ++)
        std::cout << "Key length = " << i + 1 << " : " << keyLengthSpread[i] << std::endl;
	double outBytes = compressedBits / 8.0;
	std::cout << inputLength - 256 << " bytes in, " << (unsigned int)outBytes << " bytes out, ratio out / in = " << outBytes / inputLength << std::endl;
    
	return compressedBits;
}

unsigned int FastLZW::decompress(byte* input, unsigned int inputLength, byte* output) {
	return 0;
}

void FastLZW::reset() {
    byte* temporary = new byte[1];
    for(unsigned short i = 0; i < 256; i ++) {
        temporary[0] = (byte)i;
        unsigned short hash = hashFunction->hash(temporary, 0, 1);
        ENTRY* entry = &dictionary[hash];
        entry->exists = true;
        entry->offset = i;
        entry->length = 1;
    }
    usedKeys = 256;
    maxKeyLength = 1;
    keyLengthSpread[0] = 256;
}

int main(int argc, char *argv[]) {
	if(argc <= 1)
		exit(0);
    
    Chrono* chrono = new Chrono();
    std::cout << "Chrono initialized" << std::endl;
    
    CpuInfo* info = new CpuInfo();
    std::cout << "CpuInfo requested" << std::endl;
    
	HashFunction* hashFunction;
    chrono->start();
	/*if(info->getSse42())
     hashFunction = new NehalemHash(4096);
     else*/
    hashFunction = new BernsteinHash(4096, 64);
    chrono->stop();
    std::cout << "Hash algorithm prepared in " << chrono->getElapsedMillis() << " ms" << std::endl;
    
    LZW* lzw = new FastLZW(hashFunction);
    std::cout << "LZW initialized" << std::endl;
	
	unsigned int size = 1024*1024*128;
	byte* testArray = new byte[size];
	unsigned int readBuffer = 16384;
    
	for(int i = 1; i < argc; i ++) {
        chrono->start();
        for(unsigned int i = 0; i < 256; i ++)
            testArray[i] = (byte)i;
		unsigned int index = 256;
		std::ifstream file (argv[i], std::ios::in | std::ios::binary);
		while(file) {
			file.read ((char*)(testArray + index), readBuffer);
			index += readBuffer;
		}
		index += (unsigned int)file.gcount() - readBuffer;
		file.close();
        chrono->stop();
		std::cout << "--------------------------------------------------------------------" << std::endl << "Loaded file " << argv[i] << " in " << chrono->getElapsedMillis() << " ms" << std::endl;
        
		chrono->start();
		unsigned int compressedSize = lzw->compress(testArray, index, testArray);
		chrono->stop();
        
		std::cout  << "File " << argv[i] << ", time = " << chrono->getElapsedMillis() << " ms, Speed = " << (1000.0 * index) / (chrono->getElapsedMillis() * 1024.0 * 1024.0) << " MB/s" << std::endl;
        
		lzw->reset();
    }
    
	delete testArray;
	delete lzw;
    delete chrono;
}
