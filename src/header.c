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

SHARC_FORCE_INLINE void sharc_header_populate(SHARC_HEADER* header, const SHARC_HEADER_ORIGIN_TYPE type, const struct stat64* fileAttributes) {
    SHARC_HEADER_GENERIC genericHeader;
    SHARC_HEADER_FILE_INFORMATION fileInformationHeader;

    genericHeader.magicNumber = SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER);
    genericHeader.version[0] = SHARC_MAJOR_VERSION;
    genericHeader.version[1] = SHARC_MINOR_VERSION;
    genericHeader.version[2] = SHARC_REVISION;
    genericHeader.bufferSizeShift = SHARC_PREFERRED_BLOCK_SIZE_SHIFT;
    genericHeader.resetCycleSizeShift = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT;
    genericHeader.type = type;
    genericHeader.reserved[0] = 0;
    genericHeader.reserved[1] = 0;
    switch(type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            header->fileInformationHeader = fileInformationHeader;
            break;
        default:
            break;
    }

    header->genericHeader = genericHeader;
}

SHARC_FORCE_INLINE SHARC_HEADER sharc_createHeader(const uint32_t bufferSize, const SHARC_HEADER_ORIGIN_TYPE type, const struct stat64* fileAttributes) {
    SHARC_HEADER header;
    SHARC_HEADER_GENERIC genericHeader;
    SHARC_HEADER_FILE_INFORMATION fileInformationHeader;

    genericHeader.magicNumber = SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER);
    genericHeader.version[0] = SHARC_MAJOR_VERSION;
    genericHeader.version[1] = SHARC_MINOR_VERSION;
    genericHeader.version[2] = SHARC_REVISION;
    genericHeader.bufferSizeShift = SHARC_PREFERRED_BLOCK_SIZE_SHIFT;
    genericHeader.resetCycleSizeShift = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT;
    genericHeader.type = type;
    genericHeader.reserved[0] = 0;
    genericHeader.reserved[1] = 0;
    switch(type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            header.fileInformationHeader = fileInformationHeader;
            break;
        default:
            break;
    }

    header.genericHeader = genericHeader;
    return header;
}

SHARC_FORCE_INLINE sharc_bool sharc_checkSource(const uint32_t magic) {
    if(magic == SHARC_HEADER_MAGIC_NUMBER)
        return SHARC_TRUE;
    else
        return SHARC_FALSE;
}

SHARC_FORCE_INLINE SHARC_HEADER sharc_readHeaderFromStream(FILE* inStream) {
    SHARC_HEADER header;
    SHARC_HEADER_GENERIC genericHeader;
    SHARC_HEADER_FILE_INFORMATION fileInformationHeader;

    fread(&genericHeader, sizeof(SHARC_HEADER_GENERIC), 1, inStream);
    if(sharc_checkSource(SHARC_LITTLE_ENDIAN_32(genericHeader.magicNumber)) ^ 0x1)
        sharc_error("Invalid file");
    header.genericHeader = genericHeader;

    switch(genericHeader.type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            fread(&fileInformationHeader, sizeof(SHARC_HEADER_FILE_INFORMATION), 1, inStream);
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

SHARC_FORCE_INLINE uint32_t sharc_writeHeader(sharc_byte* out, const SHARC_HEADER_ORIGIN_TYPE type, const struct stat64* fileAttributes) {
    *(uint32_t*)out = SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER);
    *(out + 4) = SHARC_MAJOR_VERSION;
    *(out + 5) = SHARC_MINOR_VERSION;
    *(out + 6) = SHARC_REVISION;
    *(out + 7) = SHARC_PREFERRED_BLOCK_SIZE_SHIFT;
    *(out + 8) = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT;
    *(out + 9) = type;
    *(out + 10) = 0;
    *(out + 11) = 0;
    switch(type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            *(uint64_t*)(out + 12) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            *(uint32_t*)(out + 20) = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            *(uint64_t*)(out + 24) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            *(uint64_t*)(out + 32) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            return sizeof(SHARC_HEADER_GENERIC) + sizeof(SHARC_HEADER_FILE_INFORMATION);
        default:
            return sizeof(SHARC_HEADER_GENERIC);
    }
}

SHARC_FORCE_INLINE void sharc_writeHeaderToStream(SHARC_HEADER * header, FILE* outStream) {
    fwrite(&header->genericHeader, sizeof(SHARC_HEADER_GENERIC), 1, outStream);
    switch(header->genericHeader.type) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            fwrite(&header->fileInformationHeader, sizeof(SHARC_HEADER_FILE_INFORMATION), 1, outStream);
            break;
        default:
            break;
    }
}

SHARC_FORCE_INLINE void sharc_restoreFileAttributes(SHARC_HEADER_FILE_INFORMATION * fileInformationHeader, const char* fileName) {
    if(chmod(fileName, fileInformationHeader->fileMode))
        sharc_error("Unable to restore original file rights.");
    
    struct utimbuf ubuf;
    ubuf.actime = fileInformationHeader->fileAccessed;
    ubuf.modtime = fileInformationHeader->fileModified;
    if(utime(fileName, &ubuf))
        sharc_error("Unable to restore original file accessed / modified times.");
}
