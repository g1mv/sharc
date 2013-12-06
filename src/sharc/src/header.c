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

#include <inttypes.h>
#include "header.h"

SHARC_FORCE_INLINE sharc_bool sharc_header_check_validity(sharc_header *restrict header) {
    return (header->genericHeader.magicNumber == SHARC_HEADER_MAGIC_NUMBER);
}

SHARC_FORCE_INLINE uint_fast32_t sharc_header_read(FILE *restrict in, sharc_header *restrict header) {
    uint_fast32_t read = (uint_fast32_t)fread(&header->genericHeader.magicNumber, sizeof(sharc_byte), sizeof(uint32_t), in);
    header->genericHeader.magicNumber = SHARC_LITTLE_ENDIAN_32(header->genericHeader.magicNumber);
    header->genericHeader.version[0] = (sharc_byte)fgetc(in);
    header->genericHeader.version[1] = (sharc_byte)fgetc(in);
    header->genericHeader.version[2] = (sharc_byte)fgetc(in);
    header->genericHeader.originType = (sharc_byte)fgetc(in);
    read += 4;
    switch (header->genericHeader.originType) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            read += fread(&header->fileInformationHeader.originalFileSize, sizeof(sharc_byte), sizeof(uint64_t), in);
            header->fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(header->fileInformationHeader.originalFileSize);
            read += fread(&header->fileInformationHeader.fileMode, sizeof(sharc_byte), sizeof(uint32_t), in);
            header->fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_32(header->fileInformationHeader.fileMode);
            read += fread(&header->fileInformationHeader.fileAccessed, sizeof(sharc_byte), sizeof(uint64_t), in);
            header->fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(header->fileInformationHeader.fileAccessed);
            read += fread(&header->fileInformationHeader.fileModified, sizeof(sharc_byte), sizeof(uint64_t), in);
            header->fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(header->fileInformationHeader.fileModified);
            break;

        default:
            break;
    }
    return read;
}

SHARC_FORCE_INLINE uint_fast32_t sharc_header_write(FILE *restrict out, const SHARC_HEADER_ORIGIN_TYPE originType, const struct stat *restrict fileAttributes) {
    uint32_t temp32;
    uint64_t temp64;

    temp32 = SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER);
    uint_fast32_t written = (uint_fast32_t)fwrite(&temp32, sizeof(sharc_byte), sizeof(uint32_t), out);
    fputc(SHARC_MAJOR_VERSION, out);
    fputc(SHARC_MINOR_VERSION, out);
    fputc(SHARC_REVISION, out);
    fputc(originType, out);
    written += 4;
    switch (originType) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            temp64 = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            written += fwrite(&temp64, sizeof(sharc_byte), sizeof(uint64_t), out);
            temp32 = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            written += fwrite(&temp32, sizeof(sharc_byte), sizeof(uint32_t), out);
            temp64 = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            written += fwrite(&temp64, sizeof(sharc_byte), sizeof(uint64_t), out);
            temp64 = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            written += fwrite(&temp64, sizeof(sharc_byte), sizeof(uint64_t), out);
            break;

        default:
            break;
    }
    return written;
}

SHARC_FORCE_INLINE sharc_bool sharc_header_restore_file_attributes(sharc_header *restrict header, const char *restrict fileName) {
    if (header->genericHeader.originType == SHARC_HEADER_ORIGIN_TYPE_FILE) {
        struct utimbuf ubuf;
        ubuf.actime = (time_t) header->fileInformationHeader.fileAccessed;
        ubuf.modtime = (time_t) header->fileInformationHeader.fileModified;
        if (utime(fileName, &ubuf))
            return false;

        if (chmod(fileName, (mode_t) header->fileInformationHeader.fileMode))
            return false;

        return true;
    } else
        return false;
}
