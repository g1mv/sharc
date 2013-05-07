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
 * 04/05/13 02:17
 * @author gpnuma
 */

#include "DefaultDictionary.h"

DefaultDictionary::DefaultDictionary(HashFunction* hashFunction) {
    this->hashFunction = hashFunction;
	dictionary = new ENTRY[hashFunction->getHashSize()];
    for(unsigned short i = 0; i < hashFunction->getHashSize(); i++) {
        ENTRY entry;
        entry.exists = false;
        //entry.hashCode = i;
        entry.offset = 0;
        entry.length = 0;
        dictionary[i] = entry;
    }
    reset();
}

DefaultDictionary::~DefaultDictionary() {
	/*for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
		delete dictionary[i];*/
	delete[] dictionary;
}

void DefaultDictionary::reset() {
    usedKeys = 0;
    maxKeyLength = 0;
	for(unsigned int i = 0; i < hashFunction->getHashSize(); i ++)
		dictionary[i].exists = false;
    byte* temporary = new byte[1];
	for(unsigned short i = 0; i < 256; i ++) {
        temporary[0] = (byte)i;
        unsigned short hash = hashFunction->hash(temporary, 0, 1);
        //std::cout << hash << std::endl;
        updateExists(hash);
		update(hash, i, 1);
	}
}

void DefaultDictionary::update(unsigned short hashCode, unsigned int offset, unsigned int length) {
    ENTRY entry = dictionary[hashCode];
    if(length > maxKeyLength)
        maxKeyLength = length;
    //entry.exists = true;
    entry.offset = offset;
    entry.length = length;    
}

void DefaultDictionary::updateExists(unsigned short hashCode) {
    ENTRY entry = dictionary[hashCode];
    if(!entry.exists)
        usedKeys ++;
    entry.exists = true;
}

/*void DefaultDictionary::put(unsigned short hashCode, ENTRY entry) {
    if(!dictionary[hashCode].exists)
        usedKeys ++;
    if(entry.length > maxKeyLength)
        maxKeyLength = entry.length;
	//dictionary[entry.hashCode] = entry;
}*/

ENTRY DefaultDictionary::get(unsigned short hashCode) {
    return dictionary[hashCode];
}

/*int DefaultDictionary::get(byte* input, unsigned int offset, unsigned int length) {
	if(length > hashFunction->getMaxWordLength())
		return DICTIONARY_WORD_NOT_EXISTING;
	unsigned short hash = hashFunction->hash(input, offset, length);
	ENTRY found = dictionary[hash];
	if(!found.exists)
		return -1 - hash;    
	if(!theSame(found, offset, length))
        return -1 - hash;
	return hash;
}*/

unsigned int DefaultDictionary::getUsedKeys() {
    return usedKeys;
}

unsigned int DefaultDictionary::getMaxKeyLength() {
    return maxKeyLength;
}
