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
 * 17/06/13 18:14
 */


#include "header.h"

SHARC_FORCE_INLINE uint32_t sharc_header_write(sharc_byte_buffer* restrict out, const SHARC_HEADER_ORIGIN_TYPE originType, const SHARC_COMPRESSION_MODE compressionMode, const struct stat64* restrict fileAttributes) {
    uint32_t written;
    sharc_byte* pointer = out->pointer + out->position;
    *(uint32_t*) pointer = SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER);
    *(pointer + 4) = SHARC_MAJOR_VERSION;
    *(pointer + 5) = SHARC_MINOR_VERSION;
    *(pointer + 6) = SHARC_REVISION;
    *(pointer + 7) = SHARC_PREFERRED_BLOCK_SIGNATURES_SHIFT;
    *(pointer + 8) = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT;
    *(pointer + 9) = originType;
    *(pointer + 10) = compressionMode;
    *(pointer + 11) = 0;
    switch(originType) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            *(uint64_t*)(pointer + 12) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            *(uint32_t*)(pointer + 20) = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            *(uint64_t*)(pointer + 24) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            *(uint64_t*)(pointer + 32) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            written = sizeof(sharc_header_generic) + sizeof(sharc_header_file_information);
            break;

        default:
            written = sizeof(sharc_header_generic);
            break;
    }
    out->position += written;
    return written;
}

SHARC_FORCE_INLINE sharc_bool sharc_checkSource(const uint32_t magic) {
    if(magic == SHARC_HEADER_MAGIC_NUMBER)
        return SHARC_TRUE;
    else
        return SHARC_FALSE;
}

SHARC_FORCE_INLINE sharc_header sharc_readHeaderFromStream(FILE* inStream) {
    sharc_header header;
    sharc_header_generic genericHeader;
    sharc_header_file_information fileInformationHeader;

    fread(&genericHeader, sizeof(sharc_header_generic), 1, inStream);
    if(sharc_checkSource(SHARC_LITTLE_ENDIAN_32(genericHeader.magicNumber)) ^ 0x1)
        sharc_error("Invalid file");
    header.genericHeader = genericHeader;

    switch(genericHeader.type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            fread(&fileInformationHeader, sizeof(sharc_header_file_information), 1, inStream);
            fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.originalFileSize);
            fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_32(fileInformationHeader.fileMode);
            fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.fileAccessed);
            fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.fileModified);
            header.fileInformationHeader = fileInformationHeader;
        default:
            break;
    }

    return header;
}

SHARC_FORCE_INLINE void sharc_restoreFileAttributes(sharc_header_file_information * fileInformationHeader, const char* fileName) {
    if(chmod(fileName, fileInformationHeader->fileMode))
        sharc_error("Unable to restore original file rights.");
    
    struct utimbuf ubuf;
    ubuf.actime = fileInformationHeader->fileAccessed;
    ubuf.modtime = fileInformationHeader->fileModified;
    if(utime(fileName, &ubuf))
        sharc_error("Unable to restore original file accessed / modified times.");
}
