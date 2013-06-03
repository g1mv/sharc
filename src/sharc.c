/*
 * Copyright (c) 2013, Guillaume Voirin
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
 * www.centaurean.com
 *
 * 01/06/13 13:08
 */

#include "sharc.h"

int main(int argc, char *argv[]) {
    if(argc <= 1)
		exit(0);
    
    const uint32_t readBufferSize = PREFERRED_BUFFER_SIZE;  // Maximum depending on ENTRY.offset byte length
    //byte readArray[PREFERRED_BUFFER_SIZE];
    byte* readArray = (byte*)malloc(readBufferSize * sizeof(byte));
	byte writeArray[readBufferSize];
    //byte* writeArray = (byte*)malloc(readBufferSize * sizeof(byte));
    
    for(int i = 1; i < argc; i ++) {
        uint64_t totalRead = 0;
        uint64_t totalWritten = 0;
        
		char* inFileName = argv[i];
		char* outFileName = (char*)malloc((strlen(inFileName) + 6) * sizeof(char));
        
        outFileName[0] = '\0';
        strcat(outFileName, inFileName);
        strcat(outFileName, ".sharc");
		
        FILE* inFile = fopen(inFileName, "rb");
        FILE* outFile = fopen(outFileName, "wb+");
        
        uint32_t bytesRead;
        
        time_t chrono = clock();
		while((bytesRead = (uint32_t)fread(readArray, sizeof(byte), readBufferSize, inFile)) > 0) {
			totalRead += bytesRead;
            
            if(sharcEncode(readArray, bytesRead, writeArray, bytesRead, MODE_DUAL_PASS_XOR))
                totalWritten += fwrite(writeArray, sizeof(byte), outPosition, outFile);
            else
                totalWritten += fwrite(readArray, sizeof(byte), bytesRead, outFile);
		}
        chrono = (1000 * (clock() - chrono)) / CLOCKS_PER_SEC;
        
        double outBytes = totalWritten;
		double ratio = outBytes / totalRead;
        double speed = (1000.0 * totalRead) / (chrono * 1024.0 * 1024.0);
        printf("File %s, %lli bytes in, %lli bytes out, ", argv[i], totalRead, totalWritten);
        printf("Ratio out / in = %g, Time = %ld ms, Speed = %g MB/s\n", ratio, chrono, speed);
		
        free(outFileName);
        
        fclose(inFile);
        fclose(outFile);
    }
    
	free(readArray);
    //free(writeArray);
}