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

FORCE_INLINE FILE_HEADER createFileHeader(const uint32_t bufferSize, struct stat64 fileAttributes) {
    FILE_HEADER fileHeader;
    fileHeader.name[0] = 'S';
    fileHeader.name[1] = 'H';
    fileHeader.name[2] = 'A';
    fileHeader.name[3] = 'R';
    fileHeader.name[4] = 'C';
    fileHeader.version[0] = MAJOR_VERSION;
    fileHeader.version[1] = MINOR_VERSION;
    fileHeader.version[2] = REVISION;
    fileHeader.originalFileSize = (uint64_t)SHARC_LITTLE_ENDIAN_64(fileAttributes.st_size);
    fileHeader.bufferSize = (uint32_t)SHARC_LITTLE_ENDIAN_32(bufferSize);
    fileHeader.fileMode = (uint16_t)SHARC_LITTLE_ENDIAN_16(fileAttributes.st_mode);
    fileHeader.fileAccessed = (uint64_t)SHARC_LITTLE_ENDIAN_64(fileAttributes.st_atime);
    fileHeader.fileModified = (uint64_t)SHARC_LITTLE_ENDIAN_64(fileAttributes.st_mtime);
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
    fread(&fileHeader, sizeof(FILE_HEADER), 1, file);
    if(checkFileType((byte*)&fileHeader.name) ^ 0x1)
        error("Invalid file");
    return fileHeader;
}

FORCE_INLINE void restoreFileAttributes(FILE_HEADER fileHeader, const char* fileName) {
    if(chmod(fileName, fileHeader.fileMode))
        error("Unable to restore original file rights.");
    
    struct utimbuf ubuf;
    ubuf.actime = fileHeader.fileAccessed;
    ubuf.modtime = fileHeader.fileModified;
    if(utime(fileName, &ubuf))
        error("Unable to restore original file accessed / modified times.");
}
