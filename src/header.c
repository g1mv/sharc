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

SHARC_FORCE_INLINE sharc_bool sharc_header_check_validity(sharc_header *restrict header) {
    return (header->genericHeader.magicNumber == SHARC_HEADER_MAGIC_NUMBER);
}

SHARC_FORCE_INLINE uint_fast32_t sharc_header_read(FILE *restrict in, sharc_header *restrict header) {
    /*header->genericHeader.magicNumber = SHARC_LITTLE_ENDIAN_32(*(uint32_t *) (in->pointer + in->position));
    in->position += sizeof(uint32_t);
    header->genericHeader.version[0] = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.version[1] = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.version[2] = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.blockSignaturesShift = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.resetCycleSizeShift = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.efficiencyCheckSignaturesShift = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.originType = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.compressionMode = *(in->pointer + in->position);
    in->position += sizeof(sharc_byte);
    header->genericHeader.blockType = *(in->pointer + in->position);
    in->position += 4 * sizeof(sharc_byte);

    switch (header->genericHeader.originType) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            header->fileInformationHeader.originalFileSize = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
            in->position += sizeof(uint64_t);
            header->fileInformationHeader.fileMode = SHARC_LITTLE_ENDIAN_32(*(uint32_t *) (in->pointer + in->position));
            in->position += sizeof(uint32_t);
            header->fileInformationHeader.fileAccessed = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
            in->position += sizeof(uint64_t);
            header->fileInformationHeader.fileModified = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
            in->position += sizeof(uint64_t);
            return sizeof(sharc_header_generic) + sizeof(sharc_header_file_information);
        default:
            return sizeof(sharc_header_generic);
    }*/
    return 0;
}

SHARC_FORCE_INLINE uint_fast32_t sharc_header_write(FILE *restrict out, const SHARC_HEADER_ORIGIN_TYPE originType, const struct stat *restrict fileAttributes) {
    /*uint_fast32_t written;
    fwrite((uint32_t *)SHARC_LITTLE_ENDIAN_32(SHARC_HEADER_MAGIC_NUMBER), sizeof(sharc_byte), sizeof(SHARC_HEADER_MAGIC_NUMBER), out);
    *(pointer + 4) = SHARC_MAJOR_VERSION;
    *(pointer + 5) = SHARC_MINOR_VERSION;
    *(pointer + 6) = SHARC_REVISION;
    switch (originType) {
        case SHARC_HEADER_ORIGIN_TYPE_FILE:
            *(uint64_t *) (pointer + 16) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_size);
            *(uint32_t *) (pointer + 24) = SHARC_LITTLE_ENDIAN_32(fileAttributes->st_mode);
            *(uint64_t *) (pointer + 28) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_atime);
            *(uint64_t *) (pointer + 36) = SHARC_LITTLE_ENDIAN_64(fileAttributes->st_mtime);
            written = sizeof(sharc_header_generic) + sizeof(sharc_header_file_information);
            break;

        default:
            written = sizeof(sharc_header_generic);
            break;
    }
    out->position += written;
    return written; */
    return 0;
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
