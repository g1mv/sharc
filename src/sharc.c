/*
 * Copyright (c) 2013, Guillaume Voirin (gvoirin@centaurean.com)
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 13:08
 */

#include "sharc.h"

FORCE_INLINE void compress(const char* inFileName, const byte attemptMode, const uint32_t blockSize) {
    char outFileName[strlen(inFileName) + 6];
    
    outFileName[0] = '\0';
    strcat(outFileName, inFileName);
    strcat(outFileName, ".sharc");
    
    FILE* inFile = checkOpenFile(inFileName, "rb");    
    FILE* outFile = checkOpenFile(outFileName, "wb+");
    
    byte reachableMode;
    const byte nThread = 0;
    
    time_t chrono = clock();
    
    struct stat attributes;
    stat(inFileName, &attributes);
    
    FILE_HEADER fileHeader = createFileHeader(blockSize, attributes);
    fwrite(&fileHeader, sizeof(FILE_HEADER), 1, outFile);
    
    BYTE_BUFFER in = createByteBuffer(readBuffer[nThread], 0, blockSize);
    //printf("%i,%i\n", sizeof(FILE_HEADER), sizeof(BLOCK_HEADER));
    BYTE_BUFFER out = createByteBuffer(writeBuffer[nThread], 0, blockSize - sizeof(BLOCK_HEADER));
    
    while((in.size = (uint32_t)fread(readBuffer[nThread], sizeof(byte), blockSize, inFile)) > 0) {
        reachableMode = sharcEncode(&in, &out, attemptMode);
        
        BLOCK_HEADER blockHeader;
        if(reachableMode ^ MODE_COPY)
            blockHeader = createBlockHeader(reachableMode, out.position);
        else
            blockHeader = createBlockHeader(reachableMode, in.size);
        
        fwrite(&blockHeader, sizeof(BLOCK_HEADER), 1, outFile);
        
        if(reachableMode ^ MODE_COPY)
            fwrite(writeBuffer, sizeof(byte), sizeof(BLOCK_HEADER) + out.position, outFile);
        else
            fwrite(readBuffer, sizeof(byte), in.size, outFile);
        
        rewindByteBuffer(&in);
        rewindByteBuffer(&out);
    }
    chrono = (1000 * (clock() - chrono)) / CLOCKS_PER_SEC;
    
    uint64_t totalRead = ftell(inFile);
    uint64_t totalWritten = ftell(outFile);
    
    fclose(inFile);
    fclose(outFile);

    double ratio = (1.0 * totalWritten) / totalRead;
    double speed = (1000.0 * totalRead) / (chrono * 1024.0 * 1024.0);
    printf("Compressed file %s, %lli bytes in, %lli bytes out, ", inFileName, totalRead, totalWritten);
    printf("Ratio out / in = %g, Time = %ld ms, Speed = %f MB/s\n", ratio, chrono, speed);
}

FORCE_INLINE void decompress(char* inFileName) {
    char outFileName[strlen(inFileName) - 3];
    
    outFileName[0] = '\0';
    strcat(outFileName, inFileName);
    
    FILE* inFile = checkOpenFile(inFileName, "rb");
    FILE* outFile = checkOpenFile(outFileName, "wb+");
    
    uint32_t bytesRead;
    byte mode;
    
    time_t chrono = clock();
    /*uint64_t decompressedSize = processFileHeader(inFile);
    while(!feof(inFile)) {
        mode = processBlockHeader(inFile);
        if(sharcDecode(inFile, outFile, decompressedSize, mode) ^ 0x1)
            error();
    }*/
    chrono = (1000 * (clock() - chrono)) / CLOCKS_PER_SEC;
    
    uint64_t totalRead = ftell(inFile);
    uint64_t totalWritten = ftell(outFile);
    
    fclose(inFile);
    fclose(outFile);
    
    double speed = (1000.0 * totalWritten) / (chrono * 1024.0 * 1024.0);
    printf("Decompressed file %s, %lli bytes in, %lli bytes out, ", inFileName, totalRead, totalWritten);
    printf("Time = %ld ms, Speed = %f MB/s\n", chrono, speed);
}

int main(int argc, char *argv[]) {
    if(argc <= 1)
		exit(0);

    byte action = ACTION_COMPRESS;
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
                        if(argLength == 2) {
                            mode = MODE_SINGLE_PASS;
                            break;
                        }
                        if(argLength != 3)
                            break;
                        mode = argv[i][2] - '0';
                        break;
                    case 'd':
                        action = ACTION_DECOMPRESS;
                        break;
                }
                break;
            default:
                switch(action) {
                    case ACTION_DECOMPRESS:
                        decompress(argv[i]);
                        break;
                    default:
                        compress(argv[i], mode, PREFERRED_BUFFER_SIZE);
                        break;
                }
                break;
        }
    }
}