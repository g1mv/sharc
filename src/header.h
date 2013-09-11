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

#ifndef SHARC_FILE_HEADER_H
#define SHARC_FILE_HEADER_H

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>

#include "globals.h"
#include "dictionary.h"
#include "byte_buffer.h"
#include "block.h"

#define SHARC_HEADER_MAGIC_NUMBER      1908011803

typedef enum {
    SHARC_HEADER_ORIGIN_TYPE_STREAM,
    SHARC_HEADER_ORIGIN_TYPE_FILE
} SHARC_HEADER_ORIGIN_TYPE;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint32_t magicNumber;
    sharc_byte version[3];
    sharc_byte blockSignaturesShift;
    sharc_byte resetCycleSizeShift;
    sharc_byte efficiencyCheckSignaturesShift;
    sharc_byte originType;
    sharc_byte compressionMode;
    sharc_byte blockType;
    sharc_byte reserved[3];
} sharc_header_generic;

typedef struct {
    uint64_t originalFileSize;
    uint32_t fileMode;
    uint64_t fileAccessed;
    uint64_t fileModified;
} sharc_header_file_information;

typedef struct {
    sharc_header_generic genericHeader;
    sharc_header_file_information fileInformationHeader;
} sharc_header;
#pragma pack(pop)

sharc_bool sharc_header_checkValidity(sharc_header*);
uint_fast32_t sharc_header_read(sharc_byte_buffer*, sharc_header*);
uint_fast32_t sharc_header_write(sharc_byte_buffer*, const SHARC_HEADER_ORIGIN_TYPE, const SHARC_COMPRESSION_MODE, const SHARC_BLOCK_TYPE, const struct stat*);
sharc_bool sharc_header_restoreFileAttributes(sharc_header_file_information *, const char*);

#endif
