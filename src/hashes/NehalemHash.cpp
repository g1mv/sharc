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
 * 06/05/13 17:45
 * @author gpnuma
 */

#include "NehalemHash.h"

NehalemHash::NehalemHash(unsigned int hashSize) : HashFunction(hashSize, 8) {
}

NehalemHash::~NehalemHash() {
}

unsigned short int NehalemHash::hash(byte* buffer, unsigned int offset, unsigned int length) {
	/*unsigned int hash = 0;
	unsigned int limit = length <= maxWordLength ? length : maxWordLength;
    for(unsigned int i = 0; i < limit; i ++)
        hash += buffer[i + offset] * hashCoeffs[i];
    return hash % hashSize;*/
	/*unsigned int limit = length <= maxWordLength ? length : maxWordLength;
	unsigned __int32 crc32;
	switch(limit) {
	case 1:
		crc32 = _mm_crc32_u8(0, buffer[offset]);
		break;
	case 2:
		crc32 = _mm_crc32_u16(0, (unsigned __int16)buffer[offset]);
		break;
	case 3:
		crc32 = _mm_crc32_u8(0, buffer[offset]);
		crc32 = _mm_crc32_u16(crc32, (unsigned __int16)buffer[offset + 1]);
		break;
	case 4:
		crc32 = _mm_crc32_u32(0, (unsigned __int32)buffer[offset]);
		break;
	case 5:
		crc32 = _mm_crc32_u32(0, (unsigned __int32)buffer[offset]);
		crc32 = _mm_crc32_u8(crc32, buffer[offset + 4]);
		break;
	case 6:
		crc32 = _mm_crc32_u32(0, (unsigned __int32)buffer[offset]);
		crc32 = _mm_crc32_u16(crc32, (unsigned __int16)buffer[offset + 4]);
		break;
	case 7:
		crc32 = _mm_crc32_u32(0, (unsigned __int32)buffer[offset]);
		crc32 = _mm_crc32_u16(crc32, (unsigned __int16)buffer[offset + 4]);
		crc32 = _mm_crc32_u8(crc32, buffer[offset + 6]);
		break;
	case 8:
		crc32 = _mm_crc32_u32(0, (unsigned __int32)buffer[offset]);
		crc32 = _mm_crc32_u32(crc32, (unsigned __int32)buffer[offset + 4]);
		break;
	}
	return (unsigned short int)(crc32 % hashSize);*/
    return 0;
}