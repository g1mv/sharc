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
#ifdef DICTIONARY_HEAP
    dictionary = new ENTRY[hashFunction->getHashSize()];
#endif
    /*for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
        dictionary[i] = *new ENTRY();
    for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
        std::cout << i << ", " << &dictionary[i] << std::endl;*/
    keyLengthSpread = new unsigned int[256];
    reset();
}

FastLZW::~FastLZW() {
#ifdef DICTIONARY_HEAP
    delete[] dictionary;
#endif
}

FORCE_INLINE static bool fastEqual(ENTRY* entry, byte* buffer, unsigned int offset, unsigned int length) {
    if(entry->length ^ length)
        return false;
    
    const uint32_t* entry_data32 = (const uint32_t*)(buffer + entry->offset);
    const uint32_t* input_data32 = (const uint32_t*)(buffer + offset);
    
    switch(length) {
        case 1:
            return !((input_data32[0] ^ entry_data32[0]) & 0x000F);
        case 2:
            return !((input_data32[0] ^ entry_data32[0]) & 0x00FF);
        case 3:
            return !((input_data32[0] ^ entry_data32[0]) & 0x0FFF);
        case 4:
            return !(input_data32[0] ^ entry_data32[0]);
        case 5:
            return !((input_data32[0] ^ entry_data32[0]) | ((input_data32[1] ^ entry_data32[1]) & 0x000F));
        case 6:
            return !((input_data32[0] ^ entry_data32[0]) | ((input_data32[1] ^ entry_data32[1]) & 0x00FF));
        case 7:
            return !((input_data32[0] ^ entry_data32[0]) | ((input_data32[1] ^ entry_data32[1]) & 0x0FFF));
        case 8:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]));
        /*case 9:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | ((input_data32[2] ^ entry_data32[2]) & 0x000F));
        case 10:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | ((input_data32[2] ^ entry_data32[2]) & 0x00FF));
        case 11:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | ((input_data32[2] ^ entry_data32[2]) & 0x0FFF));
        case 12:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | (input_data32[2] ^ entry_data32[2]));
        case 13:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | (input_data32[2] ^ entry_data32[2]) | ((input_data32[3] ^ entry_data32[3]) & 0x000F));
        case 14:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | (input_data32[2] ^ entry_data32[2]) | ((input_data32[3] ^ entry_data32[3]) & 0x00FF));
        case 15:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | (input_data32[2] ^ entry_data32[2]) | ((input_data32[3] ^ entry_data32[3]) & 0x0FFF));
        case 16:
            return !((input_data32[0] ^ entry_data32[0]) | (input_data32[1] ^ entry_data32[1]) | (input_data32[2] ^ entry_data32[2]) | (input_data32[3] ^ entry_data32[3]));*/
    }
    
    const unsigned int nblocks = length >> 2;
    
    for(unsigned int i = 0; i < nblocks; i ++)
		if(input_data32[i] ^ entry_data32[i])
			return false;
    
    return !((input_data32[nblocks] ^ entry_data32[nblocks]) & ((1 << ((length - (nblocks << 2)) << 3)) - 1));
}

FORCE_INLINE static void addEntry(ENTRY* found, byte* buffer, unsigned int* indexStart, unsigned int* length, unsigned int* compressedBits, unsigned int* counter, unsigned int* maxKeyLength, unsigned int*keyLengthSpread) {
#ifdef DEBUG
    if(length > *maxKeyLength)
        *maxKeyLength = length;
    keyLengthSpread[length - 1]++;
#endif
    *(unsigned int*)found = (*length << 25 | *indexStart << 1 | 1);
    *compressedBits += HASH_BITS;
    *indexStart = *counter;
}

FORCE_INLINE unsigned int FastLZW::compress(byte* input, unsigned int inputLength, byte* output) {
	unsigned int compressedBits = 0;
	unsigned int indexStart = 256;
	for(unsigned int i = 256; i < inputLength; i++) {
        unsigned int length = i + 1 - indexStart;
        unsigned int hashCode = hashFunction->hash(input, indexStart, length);
        ENTRY* found = &dictionary[hashCode];        
        if(found->exists) {
            if(!fastEqual(found, input, indexStart, length))
                addEntry(found, input, &indexStart, &length, &compressedBits, &i, &maxKeyLength, keyLengthSpread);
            else if(length >= (1 << 7))
                addEntry(found, input, &indexStart, &length, &compressedBits, &i, &maxKeyLength, keyLengthSpread);
        } else {
#ifdef DEBUG
            usedKeys ++;
#endif
            addEntry(found, input, &indexStart, &length, &compressedBits, &i, &maxKeyLength, keyLengthSpread);
        }
	}
#ifdef DEBUG
	std::cout << "Dictionary used keys : " << usedKeys << " / " << hashFunction->getHashSize() << std::endl;
	std::cout << "Dictionary max key length : " << maxKeyLength << " / " << hashFunction->getMaxWordLength() << std::endl;
    std::cout << "Key length spread :" << std::endl;
    for(unsigned int i = 0; i < hashFunction->getMaxWordLength(); i ++)
        std::cout << "Key length = " << i + 1 << " : " << keyLengthSpread[i] << std::endl;
#endif
    return compressedBits;
}

unsigned int FastLZW::decompress(byte* input, unsigned int inputLength, byte* output) {
	return 0;
}

void FastLZW::reset() {
#ifdef DEBUG
    for(unsigned int i = 0; i < 256; i ++)
        keyLengthSpread[i] = 0;
#endif
    for(unsigned int i = 0; i < hashFunction->getHashSize(); i++)
        dictionary[i].exists = false;
    byte* temporary = new byte[1];
    for(unsigned short i = 0; i < 256; i ++) {
        temporary[0] = (byte)i;
        unsigned short hash = hashFunction->hash(temporary, 0, 1);
        ENTRY* entry = &dictionary[hash];
        entry->exists = true;
        entry->offset = i;
        entry->length = 1;
    }
#ifdef DEBUG
    usedKeys = 256;
    maxKeyLength = 1;
    keyLengthSpread[0] = 256;
#endif
}

int mainOld(int argc, char *argv[]) {
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
    hashFunction = new CHash1(1 << HASH_BITS, 128);
    chrono->stop();
    std::cout << "Hash algorithm prepared in " << chrono->getElapsedMillis() << " ms" << std::endl;
    
    /*byte* test = (byte*)"testanouveautesttestanouveautest";
    chrono->start();
    for(unsigned int i = 0; i < 1000000000; i++)
        hashFunction->hash(test, 0, i % 32);
    chrono->stop();
    std::cout << "Performed 1000000000 1 to 32-byte hashes in " << chrono->getElapsedMillis() << " ms, Speed = " << (16.0 * 1000 * 1000000000) / (1024.0 * 1024 * chrono->getElapsedMillis()) << " MB/s" << std::endl;*/
    
    /*ENTRY entry;
    entry.offset = 0;
    entry.length = 2;
    unsigned int total = 0;
    chrono->start();
    for(unsigned int i = 0; i < 1000000000; i++)
        total += theSame(&entry, test, 0, 2 + i % 2);
    chrono->stop();
    std::cout << "Total = " << total << ", performed " << 1000000000 << " 20-byte compares in " << chrono->getElapsedMillis() << " ms, Speed = " << (20.0 * 1000 * 1000000000) / (1024.0 * 1024 * chrono->getElapsedMillis()) << " MB/s" << std::endl;
    total = 0;
    chrono->start();
    for(unsigned int i = 0; i < 1000000000; i++)
        total += fastEqual(&entry, test, 0, 2 + i % 2);
    chrono->stop();
    std::cout << "Total = " << total << ", performed " << 1000000000 << " 20-byte compares in " << chrono->getElapsedMillis() << " ms, Speed = " << (20.0 * 1000 * 1000000000) / (1024.0 * 1024 * chrono->getElapsedMillis()) << " MB/s" << std::endl;*/
    
    
    LZW* lzw = new FastLZW(hashFunction);
    std::cout << "LZW initialized" << std::endl;
	
	//const unsigned int size = 1024*1024*128;
    const unsigned int readBuffer = 1 << 24;//1048576*8;  // Maximum due to ENTRY.offset length
#ifdef READ_ARRAY_HEAP
	byte* readArray = new byte[readBuffer + 256];
#else
	byte readArray[readBuffer + 256];
#endif
    for(unsigned int j = 0; j < 256; j ++)
        readArray[j] = (byte)j;
    
	for(int i = 1; i < argc; i ++) {
        unsigned int compressedBits = 0;
        unsigned int totalRead = 0;
        
		std::ifstream file (argv[i], std::ios::in | std::ios::binary);
        chrono->start();
		while(file) {
			file.read ((char*)(readArray + 256), readBuffer);
			unsigned int read = (unsigned int)file.gcount();
			totalRead += read;

			//index += (unsigned int)file.gcount() - readBuffer;
			//std::cout << index << ", " << file.gcount() << std::endl;
        
			//chrono->start();
			compressedBits += lzw->compress(readArray, read + 256, readArray);
			//chrono->stop();
        
			lzw->reset();
		}
		chrono->stop();
		file.close();

		std::cout << "--------------------------------------------------------------------" << std::endl/* << "Loaded file " << argv[i] << " in " << chrono->getElapsedMillis() << " ms" << std::endl*/;
        
		double outBytes = compressedBits / 8.0;
		double ratio = outBytes / totalRead;
		std::cout << totalRead << " bytes in, " << (unsigned int)outBytes << " bytes out, ratio out / in = " << ratio << std::endl;
        
		double outSpeed = (1000.0 * totalRead) / (chrono->getElapsedMillis() * 1024.0 * 1024.0);
		std::cout  << "File " << argv[i] << ", time = " << chrono->getElapsedMillis() << " ms, Speed = " << outSpeed << " MB/s" << std::endl;
        
		std::cout << "COMBINED = " << outSpeed / ratio << std::endl;
    }
    
#ifdef READ_ARRAY_HEAP
	delete readArray;
#endif
	delete lzw;
    delete chrono;
}
