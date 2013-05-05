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
	unsigned int bytesCompressed = 0;
	unsigned int indexStart = 0;
	int indexFound = DICTIONARY_WORD_NOT_FOUND, previousIndexFound;

	for(unsigned int i = 0; i < inputLength; i++) {
		previousIndexFound = indexFound;
		indexFound = dictionary->get(input, indexStart, i + 1 - indexStart);
		//std::cout << indexFound << std::endl;
		if(indexFound == DICTIONARY_WORD_NOT_FOUND) {
			dictionary->put(new Entry(input, indexStart, i + 1 - indexStart));
			/*if(i > 0)
				switch(previousIndexFound) {
				case DICTIONARY_WORD_NOT_FOUND:
					std::cout << "Character = " << std::string((const char *)&input[indexStart], i - indexStart) << std::endl;
					break;
				default:
					std::cout << "Code = " << previousIndexFound << std::endl;
					break;
				}*/
			bytesCompressed++;
			indexStart = i;			
		}
	}
	/*if(indexFound == DICTIONARY_WORD_NOT_FOUND)
		std::cout << std::string((const char *)&input[indexStart], inputLength - indexStart) << std::endl;
	else
		std::cout << "Code = " << indexFound << std::endl;*/
	std::cout << inputLength << " bytes in, " << (bytesCompressed * 12) / 8 << " bytes out, ratio = " << (bytesCompressed * 12.0f) / (8 * inputLength) << std::endl;

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
	
	srand ((unsigned int)time(NULL));
	char* test = "The scenario described by Welch's 1984 paper[1] encodes sequences of 8-bit data as fixed-length 12-bit codes. The codes from 0 to 255 represent 1-character sequences consisting of the corresponding 8-bit character, and the codes 256 through 4095 are created in a dictionary for sequences encountered in the data as it is encoded. At each stage in compression, input bytes are gathered into a sequence until the next character would make a sequence for which there is no code yet in the dictionary. The code for the sequence (without that character) is added to the output, and a new code (for the sequence with that character) is added to the dictionary. The idea was quickly adapted to other situations. In an image based on a color table, for example, the natural character alphabet is the set of color table indexes, and in the 1980s, many images had small color tables (on the order of 16 colors). For such a reduced alphabet, the full 12-bit codes yielded poor compression unless the image was large, so the idea of a variable-width code was introduced: codes typically start one bit wider than the symbols being encoded, and as each code size is used up, the code width increases by 1 bit, up to some prescribed maximum (typically 12 bits). Further refinements include reserving a code to indicate that the code table should be cleared (a clear code, typically the first value immediately after the values for the individual alphabet characters), and a code to indicate the end of data (a stop code, typically one greater than the clear code). The clear code allows the table to be reinitialized after it fills up, which lets the encoding adapt to changing patterns in the input data. Smart encoders can monitor the compression efficiency and clear the table whenever the existing table no longer matches the input well. Since the codes are added in a manner determined by the data, the decoder mimics building the table as it sees the resulting codes. It is critical that the encoder and decoder agree on which variety of LZW is being used: the size of the alphabet, the maximum code width, whether variable-width encoding is being used, the initial code size, whether to use the clear and stop codes (and what values they have). Most formats that employ LZW build this information into the format specification or provide explicit fields for them in a compression header for the data.The scenario described by Welch's 1984 paper[1] encodes sequences of 8-bit data as fixed-length 12-bit codes. The codes from 0 to 255 represent 1-character sequences consisting of the corresponding 8-bit character, and the codes 256 through 4095 are created in a dictionary for sequences encountered in the data as it is encoded. At each stage in compression, input bytes are gathered into a sequence until the next character would make a sequence for which there is no code yet in the dictionary. The code for the sequence (without that character) is added to the output, and a new code (for the sequence with that character) is added to the dictionary. The idea was quickly adapted to other situations. In an image based on a color table, for example, the natural character alphabet is the set of color table indexes, and in the 1980s, many images had small color tables (on the order of 16 colors). For such a reduced alphabet, the full 12-bit codes yielded poor compression unless the image was large, so the idea of a variable-width code was introduced: codes typically start one bit wider than the symbols being encoded, and as each code size is used up, the code width increases by 1 bit, up to some prescribed maximum (typically 12 bits). Further refinements include reserving a code to indicate that the code table should be cleared (a clear code, typically the first value immediately after the values for the individual alphabet characters), and a code to indicate the end of data (a stop code, typically one greater than the clear code). The clear code allows the table to be reinitialized after it fills up, which lets the encoding adapt to changing patterns in the input data. Smart encoders can monitor the compression efficiency and clear the table whenever the existing table no longer matches the input well. Since the codes are added in a manner determined by the data, the decoder mimics building the table as it sees the resulting codes. It is critical that the encoder and decoder agree on which variety of LZW is being used: the size of the alphabet, the maximum code width, whether variable-width encoding is being used, the initial code size, whether to use the clear and stop codes (and what values they have). Most formats that employ LZW build this information into the format specification or provide explicit fields for them in a compression header for the data.The scenario described by Welch's 1984 paper[1] encodes sequences of 8-bit data as fixed-length 12-bit codes. The codes from 0 to 255 represent 1-character sequences consisting of the corresponding 8-bit character, and the codes 256 through 4095 are created in a dictionary for sequences encountered in the data as it is encoded. At each stage in compression, input bytes are gathered into a sequence until the next character would make a sequence for which there is no code yet in the dictionary. The code for the sequence (without that character) is added to the output, and a new code (for the sequence with that character) is added to the dictionary. The idea was quickly adapted to other situations. In an image based on a color table, for example, the natural character alphabet is the set of color table indexes, and in the 1980s, many images had small color tables (on the order of 16 colors). For such a reduced alphabet, the full 12-bit codes yielded poor compression unless the image was large, so the idea of a variable-width code was introduced: codes typically start one bit wider than the symbols being encoded, and as each code size is used up, the code width increases by 1 bit, up to some prescribed maximum (typically 12 bits). Further refinements include reserving a code to indicate that the code table should be cleared (a clear code, typically the first value immediately after the values for the individual alphabet characters), and a code to indicate the end of data (a stop code, typically one greater than the clear code). The clear code allows the table to be reinitialized after it fills up, which lets the encoding adapt to changing patterns in the input data. Smart encoders can monitor the compression efficiency and clear the table whenever the existing table no longer matches the input well. Since the codes are added in a manner determined by the data, the decoder mimics building the table as it sees the resulting codes. It is critical that the encoder and decoder agree on which variety of LZW is being used: the size of the alphabet, the maximum code width, whether variable-width encoding is being used, the initial code size, whether to use the clear and stop codes (and what values they have). Most formats that employ LZW build this information into the format specification or provide explicit fields for them in a compression header for the data.The scenario described by Welch's 1984 paper[1] encodes sequences of 8-bit data as fixed-length 12-bit codes. The codes from 0 to 255 represent 1-character sequences consisting of the corresponding 8-bit character, and the codes 256 through 4095 are created in a dictionary for sequences encountered in the data as it is encoded. At each stage in compression, input bytes are gathered into a sequence until the next character would make a sequence for which there is no code yet in the dictionary. The code for the sequence (without that character) is added to the output, and a new code (for the sequence with that character) is added to the dictionary. The idea was quickly adapted to other situations. In an image based on a color table, for example, the natural character alphabet is the set of color table indexes, and in the 1980s, many images had small color tables (on the order of 16 colors). For such a reduced alphabet, the full 12-bit codes yielded poor compression unless the image was large, so the idea of a variable-width code was introduced: codes typically start one bit wider than the symbols being encoded, and as each code size is used up, the code width increases by 1 bit, up to some prescribed maximum (typically 12 bits). Further refinements include reserving a code to indicate that the code table should be cleared (a clear code, typically the first value immediately after the values for the individual alphabet characters), and a code to indicate the end of data (a stop code, typically one greater than the clear code). The clear code allows the table to be reinitialized after it fills up, which lets the encoding adapt to changing patterns in the input data. Smart encoders can monitor the compression efficiency and clear the table whenever the existing table no longer matches the input well. Since the codes are added in a manner determined by the data, the decoder mimics building the table as it sees the resulting codes. It is critical that the encoder and decoder agree on which variety of LZW is being used: the size of the alphabet, the maximum code width, whether variable-width encoding is being used, the initial code size, whether to use the clear and stop codes (and what values they have). Most formats that employ LZW build this information into the format specification or provide explicit fields for them in a compression header for the data.";
	//char* test = "banana_bandana";
	unsigned int size = 1024*1024;/*14;*//*2393*4;*/
	byte* testArray = new byte[size];
	for(unsigned int i = 0; i < size; i++)
		testArray[i] = rand() % 16;/*(byte)(test[i]);*/

	chrono->start();
	unsigned int compressedSize = lzw->compress(testArray, size, testArray);
	unsigned int decompressedSize = lzw->decompress(testArray, compressedSize, testArray);
	chrono->stop();

    std::cout << "Time = " << chrono->getElapsedMillis() << "ms" << std::endl;

	delete testArray;
	delete lzw;
    delete chrono;
}
