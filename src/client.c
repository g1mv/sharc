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
    printf("Built for %s (%i bits) using GCC %d.%d.%d, %s\n", PLATFORM, 8 * sizeof(void*), __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, DATE);
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
    printf("  -i, --stdin                       Read from stdin\n");
    printf("  -o, --stdout                      Write to stdout\n");
    printf("  -v, --version                     Display version information\n");
    printf("  -h, --help                        Display this help\n");
    exit(0);
}

FORCE_INLINE void clientCompress(CLIENT_IO* in, CLIENT_IO* out, const byte attemptMode, const uint32_t blockSize, const bool prompting) {
    struct stat attributes;
    char outFileName[strlen(in->name) + 6];
    
    if(in->type == TYPE_FILE) {
        if(out->type == TYPE_FILE) {
            sprintf(outFileName, "%s.sharc", in->name);
            
            out->name = outFileName;
        }
        
        in->stream = checkOpenFile(in->name, "rb", FALSE);
        
        stat(in->name, &attributes);
    } else {
        if(out->type == TYPE_FILE)
            out->name = STDIN_COMPRESSED;
        
        in->stream = stdin;
        in->name = STDIN;
    }
    
    if(out->type == TYPE_FILE)
        out->stream = checkOpenFile(out->name, "wb+", prompting);
    else {
        out->stream = stdout;
        out->name = STDOUT;
    }
    
    BYTE_BUFFER read = createByteBuffer(readBuffer, 0, blockSize);
    BYTE_BUFFER inter = createByteBuffer(interBuffer, 0, blockSize);
    BYTE_BUFFER write = createByteBuffer(writeBuffer, 0, blockSize);
    
    CHRONO chrono;
    chronoStart(&chrono);
    compress(in->stream, out->stream, &read, &inter, &write, attemptMode, blockSize, attributes);
    chronoStop(&chrono);
    
    if(out->type == TYPE_FILE) {
        const double elapsed = chronoElapsed(&chrono);
        
        uint64_t totalWritten = ftell(out->stream);
        fclose(out->stream);
        
        if(in->type == TYPE_FILE) {
            uint64_t totalRead = ftell(in->stream);
            fclose(in->stream);
        
            double ratio = (1.0 * totalWritten) / totalRead;
            double speed = (1.0 * totalRead) / (elapsed * 1024.0 * 1024.0);
            printf("Compressed %s to %s, %lli bytes in, %lli bytes out, ", in->name, out->name, totalRead, totalWritten);
            printf("Ratio out / in = %g, Time = %.3lf s, Speed = %f MB/s\n", ratio, elapsed, speed);
        } else
            printf("Compressed %s to %s, %lli bytes written.\n", in->name, out->name, totalWritten);
    }
}

FORCE_INLINE void clientDecompress(CLIENT_IO* in, CLIENT_IO* out, const bool prompting) {
    const unsigned long originalFileNameLength = strlen(in->name) - 6;
    char outFileName[originalFileNameLength];
    
    if(in->type == TYPE_FILE) {
        if(out->type == TYPE_FILE) {
            outFileName[0] = '\0';
            strncat(outFileName, in->name, originalFileNameLength);
            
            out->name = outFileName;
        }
    
        in->stream = checkOpenFile(in->name, "rb", FALSE);
    } else {
        if(out->type == TYPE_FILE)
            out->name = STDIN;
        
        in->stream = stdin;
        in->name = STDIN;
    }
    
    if(out->type == TYPE_FILE)
        out->stream = checkOpenFile(out->name, "wb+", prompting);
    else {
        out->stream = stdout;
        out->name = STDOUT;
    }
    
    BYTE_BUFFER read = createByteBuffer(readBuffer, 0, 0);
    BYTE_BUFFER inter = createByteBuffer(interBuffer, 0, 0);
    BYTE_BUFFER write = createByteBuffer(writeBuffer, 0, 0);
    
    CHRONO chrono;
    chronoStart(&chrono);
    FILE_HEADER fileHeader = decompress(in->stream, out->stream, &read, &inter, &write);
    chronoStop(&chrono);
    
    if(out->type == TYPE_FILE) {
        const bool notFromStdin = strcmp(out->name, STDIN);
        const double elapsed = chronoElapsed(&chrono);
        
        uint64_t totalWritten = ftell(out->stream);
        fclose(out->stream);
        
        if(notFromStdin)
            restoreFileAttributes(fileHeader, out->name);
        
        if(in->type == TYPE_FILE) {
            uint64_t totalRead = ftell(in->stream);
            fclose(in->stream);
        
            if(notFromStdin)
                if(totalWritten != fileHeader.originalFileSize)
                    error("Input file is corrupt !");
        
            double speed = (1.0 * totalWritten) / (elapsed * 1024.0 * 1024.0);
            printf("Decompressed %s to %s, %lli bytes in, %lli bytes out, ", in->name, out->name, totalRead, totalWritten);
            printf("Time = %.3lf s, Speed = %f MB/s\n", elapsed, speed);
        } else
            printf("Decompressed %s to %s, %lli bytes written.\n", in->name, out->name, totalWritten);
    }
}

int main(int argc, char *argv[]) {
    if(argc <= 1)
        usage();
    
    byte action = ACTION_COMPRESS;
	byte mode = MODE_SINGLE_PASS;
    byte prompting = PROMPTING;
    CLIENT_IO in;
    in.type = TYPE_FILE;
    CLIENT_IO out;
    out.type = TYPE_FILE;
    
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
                    case 'i':
                        in.type = TYPE_STREAM;
                        break;
                    case 'o':
                        out.type = TYPE_STREAM;
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
                            case 's':
                                if(argLength == 7)
                                    in.type = TYPE_STREAM;
                                else if(argLength == 8)
                                    out.type = TYPE_STREAM;
                                else
                                    usage();
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
                in.name = argv[i];
                switch(action) {
                    case ACTION_DECOMPRESS:
                        clientDecompress(&in, &out, prompting);
                        break;
                    default:
                        clientCompress(&in, &out, mode, PREFERRED_BUFFER_SIZE, prompting);
                        break;
                }
                break;
        }
    }
    
    if(in.type == TYPE_STREAM) {
        switch(action) {
            case ACTION_DECOMPRESS:
                clientDecompress(&in, &out, prompting);
                break;
            default:
                clientCompress(&in, &out, mode, PREFERRED_BUFFER_SIZE, prompting);
                break;
        }
    }
}


