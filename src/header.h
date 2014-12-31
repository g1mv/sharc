/*
 * Centaurean Sharc
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <stdint.h>

#include "globals.h"

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
    sharc_byte originType;
    sharc_byte reserved[4];
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

sharc_bool sharc_header_check_validity(sharc_header*);
uint_fast32_t sharc_header_read(FILE*, sharc_header*);
uint_fast32_t sharc_header_write(FILE*, const SHARC_HEADER_ORIGIN_TYPE, const struct stat*);
sharc_bool sharc_header_restore_file_attributes(sharc_header *, const char*);

#endif
