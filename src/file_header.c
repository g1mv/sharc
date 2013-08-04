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

SHARC_FORCE_INLINE SHARC_FILE_HEADER sharc_createFileHeader(const uint32_t bufferSize, struct stat64 fileAttributes) {
    SHARC_FILE_HEADER fileHeader;
    fileHeader.name[0] = 'S';
    fileHeader.name[1] = 'H';
    fileHeader.name[2] = 'A';
    fileHeader.name[3] = 'R';
    fileHeader.name[4] = 'C';
    fileHeader.version[0] = SHARC_MAJOR_VERSION;
    fileHeader.version[1] = SHARC_MINOR_VERSION;
    fileHeader.version[2] = SHARC_REVISION;
    fileHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_size);
    fileHeader.bufferSize = SHARC_LITTLE_ENDIAN_32(bufferSize);
    fileHeader.fileMode = SHARC_LITTLE_ENDIAN_32(fileAttributes.st_mode);
    fileHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_atime);
    fileHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileAttributes.st_mtime);
    return fileHeader;
}

SHARC_FORCE_INLINE sharc_bool sharc_checkFileType(sharc_byte* fileHeader) {
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

SHARC_FORCE_INLINE SHARC_FILE_HEADER sharc_readFileHeader(FILE* file) {
    SHARC_FILE_HEADER fileHeader;
    fread(&fileHeader, sizeof(SHARC_FILE_HEADER), 1, file);
    fileHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(fileHeader.originalFileSize);
    fileHeader.bufferSize = SHARC_LITTLE_ENDIAN_32(fileHeader.bufferSize);
    fileHeader.fileMode = SHARC_LITTLE_ENDIAN_32(fileHeader.fileMode);
    fileHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(fileHeader.fileAccessed);
    fileHeader.fileModified = SHARC_LITTLE_ENDIAN_64(fileHeader.fileModified);
    if(sharc_checkFileType((sharc_byte*)&fileHeader.name) ^ 0x1)
        sharc_error("Invalid file");
    return fileHeader;
}

SHARC_FORCE_INLINE void sharc_restoreFileAttributes(SHARC_FILE_HEADER fileHeader, const char* fileName) {
    if(chmod(fileName, fileHeader.fileMode))
        sharc_error("Unable to restore original file rights.");
    
    struct utimbuf ubuf;
    ubuf.actime = fileHeader.fileAccessed;
    ubuf.modtime = fileHeader.fileModified;
    if(utime(fileName, &ubuf))
        sharc_error("Unable to restore original file accessed / modified times.");
}
