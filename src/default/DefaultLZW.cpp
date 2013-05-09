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
 * 03/05/13 12:02
 * @author gpnuma
 */

#include "DefaultLZW.h"

DefaultLZW::DefaultLZW(HashFunction* hashFunction) {
	this->hashFunction = hashFunction;
	dictionary = new DefaultDictionary(hashFunction);
}

DefaultLZW::~DefaultLZW() {
	delete dictionary;
}

static bool theSame(byte* input, ENTRY entry, unsigned int offset, unsigned int length) {
    if(entry.length != length)
        return false;
    for(unsigned int i = 0; i < length; i ++)
		if(input[entry.offset + i] != input[offset + i])
			return false;
    return true;
}

unsigned int DefaultLZW::compress(byte* input, unsigned int inputLength, byte* output) {
	unsigned int bytesCompressed = 0;
	unsigned int indexStart = 256;
	//int indexFound = DICTIONARY_WORD_NOT_EXISTING, previousIndexFound;
    //DefaultEntry* test = new DefaultEntry(hashFunction, input, indexStart, 0 + 1 - indexStart);
    
	for(unsigned int i = 256; i < inputLength; i++) {
		//previousIndexFound = indexFound;
        unsigned int length = i + 1 - indexStart;
        unsigned short hashCode = hashFunction->hash(input, indexStart, length);
        ENTRY found = dictionary->get(hashCode);
        if(!found.exists) {
            //dictionary->updateExists(hashCode);
            dictionary->update(hashCode, indexStart, length);
            /*found.offset = indexStart;
            found.length = length;*/
            bytesCompressed++;
            indexStart = i;
        } else if(!theSame(input, found, indexStart, length)) {
            dictionary->update(hashCode, indexStart, length);
            /*found.offset = indexStart;
            found.length = length;*/
            bytesCompressed++;
            indexStart = i;
        }
	}
	std::cout << "Dictionary used keys : " << dictionary->getUsedKeys() << " / " << hashFunction->getHashSize() << std::endl;
	std::cout << "Dictionary max key length : " << dictionary->getMaxKeyLength() << " / " << hashFunction->getMaxWordLength() << std::endl;
	double outBytes = (bytesCompressed * 12.0) / 8;
	std::cout << inputLength << " bytes in, " << (unsigned int)outBytes << " bytes out, ratio out / in = " << outBytes / inputLength << std::endl;

	return bytesCompressed;
}

unsigned int DefaultLZW::decompress(byte* input, unsigned int inputLength, byte* output) {
	return 0;
}

void DefaultLZW::reset() {
	dictionary->reset();
}