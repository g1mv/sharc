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
 * 01/06/13 13:08
 */

#include "sharc.h"

FORCE_INLINE void compress(FILE* inStream, FILE* outStream, BYTE_BUFFER* in, BYTE_BUFFER* inter, BYTE_BUFFER* out, const byte attemptMode, const uint32_t blockSize, const struct stat64 attributes) {
    ENCODING_RESULT result;
    
    FILE_HEADER fileHeader = createFileHeader(blockSize, attributes);
    fwrite(&fileHeader, sizeof(FILE_HEADER), 1, outStream);
    
    while((in->size = (uint32_t)fread(in->pointer, sizeof(byte), blockSize, inStream)) > 0) {
        result = sharcEncode(in, inter, out, attemptMode);
        
        BLOCK_HEADER blockHeader = createBlockHeader(result.reachableMode, result.out->position);
        fwrite(&blockHeader, sizeof(BLOCK_HEADER), 1, outStream);
        fwrite(result.out->pointer, sizeof(byte), result.out->position, outStream);
        
        rewindByteBuffer(in);
        rewindByteBuffer(out);
    }
} 

FORCE_INLINE FILE_HEADER decompress(FILE* inStream, FILE* outStream, BYTE_BUFFER* in, BYTE_BUFFER* inter, BYTE_BUFFER* out) {
    FILE_HEADER fileHeader = readFileHeader(inStream);
    BLOCK_HEADER blockHeader;
    
    while(readBlockHeaderFromFile(&blockHeader, inStream) > 0) {
        in->size = (uint32_t)fread(in->pointer, sizeof(byte), blockHeader.nextBlock, inStream);
        switch(blockHeader.mode) {
            case MODE_COPY:
                fwrite(in->pointer, sizeof(byte), in->size, outStream);
                break;
            default:
                if(sharcDecode(in, inter, out, blockHeader.mode) ^ 0x1)
                    error("Unable to decompress !");
                fwrite(out->pointer, sizeof(byte), out->position, outStream);
                break;
        }
        
        rewindByteBuffer(in);
        rewindByteBuffer(out);
    }
    
    return fileHeader;
}
