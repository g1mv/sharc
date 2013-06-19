/*
 * Copyright (c) 2013, Guillaume Voirin
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
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 17/06/13 18:14
 */


#include "file_header.h"

FORCE_INLINE FILE_HEADER createFileHeader(const uint32_t bufferSize, struct stat fileAttributes) {
    FILE_HEADER fileHeader;
    fileHeader.name[0] = 'S';
    fileHeader.name[1] = 'H';
    fileHeader.name[2] = 'A';
    fileHeader.name[3] = 'R';
    fileHeader.name[4] = 'C';
    fileHeader.version[0] = MAJOR_VERSION;
    fileHeader.version[1] = MINOR_VERSION;
    fileHeader.version[2] = REVISION;
    fileHeader.originalFileSize = (uint64_t)fileAttributes.st_size;
    fileHeader.bufferSize = (uint32_t)bufferSize;
    fileHeader.fileMode = (uint16_t)fileAttributes.st_mode;
    fileHeader.fileCreated = (uint64_t)gmtime(&(fileAttributes.st_ctime));
    fileHeader.fileAccessed = (uint64_t)gmtime(&(fileAttributes.st_atime));
    fileHeader.fileModified = (uint64_t)gmtime(&(fileAttributes.st_mtime));
    return fileHeader;
}

FORCE_INLINE bool checkFileType(byte* fileHeader) {
    if(fileHeader[0] != 'S')
        return FALSE;
    if(fileHeader[1] != 'H')
        return FALSE;
    if(fileHeader[2] != 'A')
        return FALSE;
    if(fileHeader[3] != 'R')
        return FALSE;
    if(fileHeader[4] != 'C')
        return FALSE;
    return TRUE;
}

FORCE_INLINE FILE_HEADER readFileHeader(FILE* file) {
    FILE_HEADER fileHeader;
    fread(&fileHeader, sizeof(fileHeader), 1, file);
    if(checkFileType((byte*)&fileHeader.name) ^ 0x1)
        error("Invalid file");
    return fileHeader;
}
