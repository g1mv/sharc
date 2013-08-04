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

SHARC_FORCE_INLINE void sharc_compress(FILE* inStream, FILE* outStream, SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out, const sharc_byte attemptMode, const uint32_t blockSize, const struct stat64 attributes) {
    SHARC_ENCODING_RESULT result;
    
    SHARC_FILE_HEADER fileHeader = sharc_createFileHeader(blockSize, attributes);
    fwrite(&fileHeader, sizeof(SHARC_FILE_HEADER), 1, outStream);
    
    while((in->size = (uint32_t)fread(in->pointer, sizeof(sharc_byte), blockSize, inStream)) > 0) {
        result = sharc_sharcEncode(in, inter, out, attemptMode);
        
        SHARC_BLOCK_HEADER blockHeader = sharc_createBlockHeader(result.reachableMode, result.out->position);
        fwrite(&blockHeader, sizeof(SHARC_BLOCK_HEADER), 1, outStream);
        fwrite(result.out->pointer, sizeof(sharc_byte), result.out->position, outStream);
        
        sharc_rewindByteBuffer(in);
        sharc_rewindByteBuffer(out);
    }
} 

SHARC_FORCE_INLINE SHARC_FILE_HEADER sharc_decompress(FILE* inStream, FILE* outStream, SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out) {
    SHARC_FILE_HEADER fileHeader = sharc_readFileHeader(inStream);
    SHARC_BLOCK_HEADER blockHeader;
    
    while(sharc_readBlockHeaderFromFile(&blockHeader, inStream) > 0) {
        in->size = (uint32_t)fread(in->pointer, sizeof(sharc_byte), blockHeader.nextBlock, inStream);
        switch(blockHeader.mode) {
            case SHARC_MODE_COPY:
                fwrite(in->pointer, sizeof(sharc_byte), in->size, outStream);
                break;
            default:
                if(sharc_sharcDecode(in, inter, out, (const sharc_byte)blockHeader.mode) ^ 0x1)
                    sharc_error("Unable to decompress !");
                fwrite(out->pointer, sizeof(sharc_byte), out->position, outStream);
                break;
        }
        
        sharc_rewindByteBuffer(in);
        sharc_rewindByteBuffer(out);
    }
    
    return fileHeader;
}
