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

#include "LZWBasic.h"

LZWBasic::LZWBasic() {
	dictionary = new Dictionary();
}

LZWBasic::~LZWBasic() {
	delete dictionary;
}

unsigned int LZWBasic::compress(byte* input, unsigned int inputLength, byte* output) {
	unsigned int indexStart = 0;
	int indexFound = DICTIONARY_WORD_NOT_FOUND, previousIndexFound;

	for(unsigned int i = 0; i < inputLength; i++) {
		previousIndexFound = indexFound;
		indexFound = dictionary->get(input, indexStart, i + 1 - indexStart);
		//std::cout << indexFound << std::endl;
		if(indexFound == DICTIONARY_WORD_NOT_FOUND) {
			dictionary->put(new Entry(input, indexStart, i + 1 - indexStart));
			switch(previousIndexFound) {
			case DICTIONARY_WORD_NOT_FOUND:
				std::cout << std::string((const char *)&input[indexStart], i - indexStart) << std::endl;
				break;
			default:
				std::cout << previousIndexFound << std::endl;
				break;
			}
			indexStart = i;			
		}
	}
	std::cout << std::string((const char *)&input[indexStart], inputLength - indexStart) << std::endl;

	return 0;
}

unsigned int LZWBasic::decompress(byte* input, unsigned int inputLength, byte* output) {
	return 0;
}

int main(int argc, char *argv[]) {
    Chrono* chrono = new Chrono();
    std::cout << "Chrono initialized" << std::endl;

	LZW* lzw = new LZWBasic();
    std::cout << "LZW initialized" << std::endl;

	char* test = "TOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOTTOBEORNOTTOBEORTOBEORNOT";
	unsigned int size = 24*20;
	byte* testArray = new byte[size];
	for(unsigned int i = 0; i < size; i++)
		testArray[i] = (byte)(test[i]);

	chrono->start();
	unsigned int compressedSize = lzw->compress(testArray, size, testArray);
	unsigned int decompressedSize = lzw->decompress(testArray, compressedSize, testArray);
	chrono->stop();

    std::cout << size << " bytes in " << chrono->getElapsedMillis() << "ms" << std::endl;

	delete testArray;
	delete lzw;
    delete chrono;
}
