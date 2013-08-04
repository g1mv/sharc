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


#include "file_header.h"

SHARC_FORCE_INLINE SHARC_HEADER sharc_createHeader(const uint32_t bufferSize, const sharc_byte type, struct stat64 fileAttributes) {
    SHARC_HEADER header;
    SHARC_GENERIC_HEADER genericHeader;
    SHARC_FILE_INFORMATION_HEADER fileInformationHeader;

    genericHeader.name[0] = 'S';
    genericHeader.name[1] = 'H';
    genericHeader.name[2] = 'A';
    genericHeader.name[3] = 'R';
    genericHeader.name[4] = 'C';
    genericHeader.version[0] = SHARC_MAJOR_VERSION;
    genericHeader.version[1] = SHARC_MINOR_VERSION;
    genericHeader.version[2] = SHARC_REVISION;
    genericHeader.type = type;
    switch(type) {
        case SHARC_TYPE_FILE:
            fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_16(fileAttributes.st_mode);
            fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_size);
            fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_atime);
            fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_mtime);
            header.fileInformationHeader = fileInformationHeader;
            break;
    }

    header.genericHeader = genericHeader;
    return header;
}

SHARC_FORCE_INLINE sharc_bool sharc_checkSourceType(sharc_byte* fileHeader) {
    if(fileHeader[0] != 'S')
        return SHARC_FALSE;
    if(fileHeader[1] != 'H')
        return SHARC_FALSE;
    if(fileHeader[2] != 'A')
        return SHARC_FALSE;
    if(fileHeader[3] != 'R')
        return SHARC_FALSE;
    if(fileHeader[4] != 'C')
        return SHARC_FALSE;
    return SHARC_TRUE;
}

SHARC_FORCE_INLINE SHARC_HEADER sharc_readHeader(FILE* inStream) {
    SHARC_HEADER header;
    SHARC_GENERIC_HEADER genericHeader;
    SHARC_FILE_INFORMATION_HEADER fileInformationHeader;

    fread(&genericHeader, sizeof(SHARC_GENERIC_HEADER), 1, inStream);
    if(sharc_checkSourceType((sharc_byte*)&genericHeader.name) ^ 0x1)
            sharc_error("Invalid file");
    header.genericHeader = genericHeader;

    switch(genericHeader.type) {
        case SHARC_TYPE_FILE:
            fread(&fileInformationHeader, sizeof(SHARC_FILE_INFORMATION_HEADER), 1, inStream);
        fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_16(fileInformationHeader.fileMode);
        fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.originalFileSize);
        fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.fileAccessed);
        fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileInformationHeader.fileModified);
        header.fileInformationHeader = fileInformationHeader;
    }

    return header;
}

SHARC_FORCE_INLINE void sharc_writeHeader(SHARC_HEADER* header, FILE* outStream) {
    fwrite(&header->genericHeader, sizeof(SHARC_GENERIC_HEADER), 1, outStream);
    switch(header->genericHeader.type) {
        case SHARC_TYPE_FILE:
            fwrite(&header->fileInformationHeader, sizeof(SHARC_FILE_INFORMATION_HEADER), 1, outStream);
            break;
    }
}

SHARC_FORCE_INLINE void sharc_restoreFileAttributes(SHARC_FILE_INFORMATION_HEADER fileInformationHeader, const char* fileName) {
    if(chmod(fileName, fileInformationHeader.fileMode))
        sharc_error("Unable to restore original file rights.");
    
    struct utimbuf ubuf;
    ubuf.actime = fileInformationHeader.fileAccessed;
    ubuf.modtime = fileInformationHeader.fileModified;
    if(utime(fileName, &ubuf))
        sharc_error("Unable to restore original file accessed / modified times.");
}
