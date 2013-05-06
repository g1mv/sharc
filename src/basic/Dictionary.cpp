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

#include "Dictionary.h"

Dictionary::Dictionary() {
    used = 0;
	dictionary = new Entry*[DICTIONARY_SIZE];
	for(unsigned int i = 0; i < DICTIONARY_SIZE; i ++)
		dictionary[i] = 0;
	for(byte i = 0; i < (byte)255; i ++) {
		Entry* entry = new Entry(i);
		put(entry);
		//std::cout << i << std::endl;
	}
}

Dictionary::~Dictionary() {
	delete[] dictionary;
}
	
void Dictionary::put(Entry* entry) {
	unsigned int hash = entry->getHashCode();
    if(dictionary[hash] == 0)
        used ++;
	dictionary[hash] = entry;
}

int Dictionary::get(byte* input, unsigned int offset, unsigned int length) {
	if(length > DICTIONARY_MAX_WORD_LENGTH)
		return DICTIONARY_WORD_NOT_FOUND;
	int hash = hashWord(input, offset, length);
	Entry* found = dictionary[hash];
	//std::cout << found << std::endl;
	if(found == 0)
		//std::cout << "Not found !" << std::endl;
		return DICTIONARY_WORD_NOT_FOUND;
	if(!areIdentical(found, input, offset, length))
        return DICTIONARY_WORD_NOT_FOUND;
	return hash;
}

unsigned int Dictionary::getOccupation() {
    return used;
}
