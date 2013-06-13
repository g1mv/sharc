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

FORCE_INLINE void writeFileHeader(FILE* inFile, char* inFileName, FILE* outFile) {
    fputs("SHARC", outFile);
    fputc(MAJOR_VERSION, outFile);
    fputc(MINOR_VERSION, outFile);
    fputc(SUB_MINOR_VERSION, outFile);
    
    struct stat attributes;
    stat(inFileName, &attributes);
    
    const uint64_t size = (uint64_t)attributes.st_size;
    const uint16_t mode = (uint16_t)attributes.st_mode;
    const uint64_t created = (uint64_t)gmtime(&(attributes.st_ctime));
    const uint64_t accessed = (uint64_t)gmtime(&(attributes.st_atime));
    const uint64_t modified = (uint64_t)gmtime(&(attributes.st_mtime));
    
    fwrite(&size, 8, 1, outFile);
    fwrite(&mode, 2, 1, outFile);
    fwrite(&created, 8, 1, outFile);
    fwrite(&accessed, 8, 1, outFile);
    fwrite(&modified, 8, 1, outFile);
}

FORCE_INLINE void writeBlockHeader(const byte mode, byte* writeBuffer) {
    *writeBuffer = mode;
}

FORCE_INLINE void compress(char* inFileName, byte mode, byte* readBuffer, byte* writeBuffer, uint32_t size) {
    char outFileName[strlen(inFileName) + 6];
    
    outFileName[0] = '\0';
    strcat(outFileName, inFileName);
    strcat(outFileName, ".sharc");
    
    FILE* inFile = fopen(inFileName, "rb");
    if(inFile == NULL) {
        printf("Unable to open file : %s\n", inFileName);
        exit(0);
    }
    
    FILE* outFile = fopen(outFileName, "wb+");
    
    uint32_t bytesRead;
    byte reachableMode;
    
    time_t chrono = clock();
    writeFileHeader(inFile, inFileName, outFile);
    while((bytesRead = (uint32_t)fread(readBuffer, sizeof(byte), size, inFile)) > 0) {
        reachableMode = sharcEncode(readBuffer, bytesRead, writeBuffer + 1, bytesRead - 1, mode);
        writeBlockHeader(reachableMode, writeBuffer);
        if(reachableMode ^ MODE_COPY)
            fwrite(writeBuffer, sizeof(byte), outPosition, outFile);
        else
            fwrite(readBuffer, sizeof(byte), bytesRead, outFile);
    }
    chrono = (1000 * (clock() - chrono)) / CLOCKS_PER_SEC;
    
    uint64_t totalRead = ftell(inFile);
    uint64_t totalWritten = ftell(outFile);
    
    fclose(inFile);
    fclose(outFile);

    double ratio = (1.0 * totalWritten) / totalRead;
    double speed = (1000.0 * totalRead) / (chrono * 1024.0 * 1024.0);
    printf("File %s, %lli bytes in, %lli bytes out, ", inFileName, totalRead, totalWritten);
    printf("Ratio out / in = %g, Time = %ld ms, Speed = %g MB/s\n", ratio, chrono, speed);
}

int main(int argc, char *argv[]) {
    if(argc <= 1)
		exit(0);

	byte mode = MODE_SINGLE_PASS;
    
    size_t argLength;
    for(int i = 1; i < argc; i ++) {
        switch(argv[i][0]) {
            case '-':
                argLength = strlen(argv[i]);
                if(argLength < 2)
                    break;
                switch(argv[i][1]) {
                    case 'c':
                        if(argLength != 3)
                            break;
                        mode = argv[i][2] - '0';
                        break;
                }
                break;
            default:
                compress(argv[i], mode, readBuffer, writeBuffer, PREFERRED_BUFFER_SIZE);
                break;
        }
    }
}