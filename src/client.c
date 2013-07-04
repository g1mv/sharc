/*
 * Centaurean Sharc
 * http://www.centaurean.com/sharc
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation. For the terms of this
 * license, see http://www.gnu.org/licenses/gpl.html
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, see http://www.centaurean.com/sharc for more
 * information.
 *
 * 03/07/13 14:55
 */

#include "client.h"

FORCE_INLINE FILE* checkOpenFile(const char* fileName, const char* options, const bool checkOverwrite) {
    if(checkOverwrite && access(fileName, F_OK) != -1) {
        printf("File %s already exists. Do you want to overwrite it (y/N) ? ", fileName);
        switch(getchar()) {
            case 'y':
                break;
            default:
                exit(0);
        }
    }
    FILE* file = fopen(fileName, options);
    if(file == NULL) {
        printf("Unable to open file : %s\n", fileName);
        exit(0);
    }
    return file;
}

FORCE_INLINE void version() {
    printf("Centaurean Sharc %i.%i.%i\n", MAJOR_VERSION, MINOR_VERSION, REVISION);
}

FORCE_INLINE void usage() {
    version();
    printf("Copyright (C) 2013 Guillaume Voirin\n");
    printf("Usage : sharc [OPTIONS]... [FILES]...\n")   ;
    printf("Superfast archiving of files.\n\n");
    printf("Available options :\n");
    printf("  -c[LEVEL], --compress[=LEVEL]     Compress files using LEVEL if specified (default)\n");
	printf("                                    LEVEL can have the following values :\n");
	printf("                                    0 = Fastest compression algorithm (default)\n");
	printf("                                    1 = Better compression (dual pass), slightly slower\n");
	printf("  -d, --decompress                  Decompress files\n");
    printf("  -n, --no-prompt                   Overwrite without prompting\n");
    printf("  -v, --version                     Display version information\n");
    printf("  -h, --help                        Display this help\n");
    exit(0);
}

FORCE_INLINE void compressStream(FILE* inStream, FILE* outStream, const byte attemptMode, const uint32_t blockSize, const struct stat attributes) {
    BYTE_BUFFER in = createByteBuffer(readBuffer, 0, blockSize);
    BYTE_BUFFER inter = createByteBuffer(interBuffer, 0, blockSize);
    BYTE_BUFFER out = createByteBuffer(writeBuffer, 0, blockSize);
    
    compress(inStream, outStream, &in, &inter, &out, attemptMode, blockSize, attributes);
}

FORCE_INLINE FILE_HEADER decompressStream(FILE* inStream, FILE* outStream) {
    BYTE_BUFFER in = createByteBuffer(readBuffer, 0, 0);
    BYTE_BUFFER inter = createByteBuffer(interBuffer, 0, 0);
    BYTE_BUFFER out = createByteBuffer(writeBuffer, 0, 0);
    
    return decompress(inStream, outStream, &in, &inter, &out);
}

FORCE_INLINE void compressFile(const char* inFileName, const byte attemptMode, const uint32_t blockSize, const bool prompting) {
    char outFileName[strlen(inFileName) + 6];
    
    outFileName[0] = '\0';
    strcat(outFileName, inFileName);
    strcat(outFileName, ".sharc");
    
    FILE* inFile = checkOpenFile(inFileName, "rb", FALSE);
    FILE* outFile = checkOpenFile(outFileName, "wb+", prompting);
    
    struct stat attributes;
    stat(inFileName, &attributes);
    
    CHRONO chrono;
    chronoStart(&chrono);
    
    compressStream(inFile, outFile, attemptMode, blockSize, attributes);
    
    chronoStop(&chrono);
    const double elapsed = chronoElapsed(&chrono);
    
	uint64_t totalRead = ftell(inFile);
    uint64_t totalWritten = ftell(outFile);
    
    fclose(inFile);
    fclose(outFile);
    
    double ratio = (1.0 * totalWritten) / totalRead;
    double speed = (1.0 * totalRead) / (elapsed * 1024.0 * 1024.0);
    printf("Compressed file %s, %lli bytes in, %lli bytes out, ", inFileName, totalRead, totalWritten);
    printf("Ratio out / in = %g, Time = %.3lf s, Speed = %f MB/s\n", ratio, elapsed, speed);
}

FORCE_INLINE void decompressFile(const char* inFileName, const bool prompting) {
    const unsigned long originalFileNameLength = strlen(inFileName) - 6;
    char outFileName[originalFileNameLength];
    
    outFileName[0] = '\0';
    strncat(outFileName, inFileName, originalFileNameLength);
    
    FILE* inFile = checkOpenFile(inFileName, "rb", FALSE);
    FILE* outFile = checkOpenFile(outFileName, "wb+", prompting);
    
    CHRONO chrono;
    chronoStart(&chrono);

    FILE_HEADER fileHeader = decompressStream(inFile, outFile);
    
    chronoStop(&chrono);
    const double elapsed = chronoElapsed(&chrono);
    
    uint64_t totalRead = ftell(inFile);
    uint64_t totalWritten = ftell(outFile);
    
    fclose(inFile);
    fclose(outFile);
    
    if(totalWritten != fileHeader.originalFileSize)
        error("Input file is corrupt !");
    
    restoreFileAttributes(fileHeader, outFileName);
    
    double speed = (1.0 * totalWritten) / (elapsed * 1024.0 * 1024.0);
    printf("Decompressed file %s, %lli bytes in, %lli bytes out, ", inFileName, totalRead, totalWritten);
    printf("Time = %.3lf s, Speed = %f MB/s\n", elapsed, speed);
}

int main(int argc, char *argv[]) {
    bool file = FALSE;
    byte action = ACTION_COMPRESS;
	byte mode = MODE_SINGLE_PASS;
    byte prompting = PROMPTING;
    
    size_t argLength;
    for(int i = 1; i < argc; i ++) {
        switch(argv[i][0]) {
            case '-':
                argLength = strlen(argv[i]);
                if(argLength < 2)
                    usage();
                switch(argv[i][1]) {
                    case 'c':
                        if(argLength == 2) {
                            mode = MODE_SINGLE_PASS;
                            break;
                        }
                        if(argLength != 3)
                            usage();
                        mode = argv[i][2] - '0';
                        break;
                    case 'd':
                        action = ACTION_DECOMPRESS;
                        break;
                    case 'n':
                        prompting = NO_PROMPTING;
                        break;
                    case 'v':
                        version();
                        exit(0);
                        break;
                    case 'h':
                        usage();
                        break;
					case '-':
						if(argLength < 3)
							usage();
						switch(argv[i][2]) {
							case 'c':
                                if(argLength == 10)
                                    break;
								if(argLength != 12)
									usage();
								mode = argv[i][11] - '0';
								break;
							case 'd':
                                if(argLength != 12)
                                    usage();
								action = ACTION_DECOMPRESS;
								break;
                            case 'n':
                                if(argLength != 11)
                                    usage();
                                prompting = NO_PROMPTING;
                                break;
                            case 'v':
                                if(argLength != 9)
                                    usage();
                                version();
                                exit(0);
                                break;
                            case 'h':
                                usage();
                                break;
							default:
								usage();
						}
						break;
                }
                break;
            default:
                file = TRUE;
                switch(action) {
                    case ACTION_DECOMPRESS:
                        decompressFile(argv[i], prompting);
                        break;
                    default:
                        compressFile(argv[i], mode, PREFERRED_BUFFER_SIZE, prompting);
                        break;
                }
                break;
        }
    }
    
    if(!file) {        
        struct stat attributes;
        switch(action) {
            case ACTION_DECOMPRESS:
                decompressStream(stdin, stdout);
                break;
            default:
                compressStream(stdin, stdout, mode, PREFERRED_BUFFER_SIZE, attributes);
                break;
        }
    }
}


