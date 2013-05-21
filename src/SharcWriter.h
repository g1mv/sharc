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
 * Sharc
 *
 * 12/05/13 02:38
 * @author gpnuma
 */

#ifndef SHARC_WRITER_H
#define SHARC_WRITER_H

#include "commons.h"

typedef unsigned char byte;

/*byte bitsSetTable[256];
bitsSetTable[0] = 0;
for (int i = 0; i < 256; i++) {
    bitsSetTable[i] = (i & 1) + BitsSetTable256[i / 2];
}*/

class SharcWriter {
private:
	byte state;
    byte chunksModes;
    unsigned int chunks[8];
    
	unsigned int position;
	byte* buffer;
    unsigned int length;
    unsigned int limit;
    
    bool checkState();
	void writeMode(bool);
    
public:
	SharcWriter(byte*, unsigned int);
	~SharcWriter();
    
	bool writeChunk(unsigned short);
	bool writeChunk(unsigned int);
	void resetModes();
	void resetBuffer();
	bool flush();
    unsigned int getPosition();
    void setLimit(unsigned int);
    unsigned int getLimit();
};

#endif