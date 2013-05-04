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
 * 04/05/13 13:49
 * @author gpnuma
 */

#ifndef ENTRY_H
#define ENTRY_H

#include "../Types.h"
#include <string.h>

#define HASH_SIZE		1024
#define HASH_MAX_LENGTH 8

class Entry {
private:
	byte* entry;
	unsigned int length;
	int hashCode;

public:
	Entry(byte);
	Entry(byte*, unsigned int, unsigned int);
	~Entry();

	/*bool operator == (Entry* e) {
		if(this == 0) {
			if(e == 0)
				return true;
			else
				return false;
		} else {
			if(e == 0)
				return false;
		}
		int i = this->getLength();
		if(this->getLength() != e->getLength())
			return false;
		for(unsigned int i = 0; i < this->getLength(); i ++)
			if(this->getValue()[i] != e->getValue()[i])
				return false;
        return true;
    }*/

	byte* getValue();
	unsigned int getLength();
	int getHashCode();
};

static unsigned int hashCoeffs[HASH_MAX_LENGTH] = {2, 3, 5, 7, 11, 13, 17, 19};

static int hashWord(byte* buffer, unsigned int offset, unsigned int length) {
	int hash = 0;
	for(unsigned int i = 0; i < length; i ++)
		hash += buffer[i + offset] * hashCoeffs[i];
	return hash % HASH_SIZE;
}

static bool areIdentical(Entry* entry, byte* buffer, unsigned int offset, unsigned int length) {
	if(entry->getLength() != length)
		return false;
	for(unsigned int i = 0; i < entry->getLength(); i ++)
		if(entry->getValue()[i] != buffer[i + offset])
			return false;
    return true;
}

#endif