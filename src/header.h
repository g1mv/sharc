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
#include <stdio.h>
#include <time.h>
#include <utime.h>

#include "globals.h"
#include "dictionary.h"
#include "byte_buffer.h"

#if defined(__FreeBSD__) || defined(__APPLE__)
#define stat64 stat
#endif

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
    sharc_byte bufferSizeShift;
    sharc_byte resetCycleSizeShift;
    SHARC_HEADER_ORIGIN_TYPE type;
    sharc_byte reserved[2];
} SHARC_HEADER_GENERIC;

typedef struct {
    uint64_t originalFileSize;
    uint32_t fileMode;
    uint64_t fileAccessed;
    uint64_t fileModified;
} SHARC_HEADER_FILE_INFORMATION;

typedef struct {
    SHARC_HEADER_GENERIC genericHeader;
    SHARC_HEADER_FILE_INFORMATION fileInformationHeader;
} sharc_header;
#pragma pack(pop)

void sharc_header_populate(sharc_header *, const SHARC_HEADER_ORIGIN_TYPE, const struct stat64*);
sharc_header sharc_createHeader(const uint32_t, const SHARC_HEADER_ORIGIN_TYPE, const struct stat64*);
sharc_bool sharc_checkSource(const uint32_t);
sharc_header sharc_readHeaderFromStream(FILE*);
uint32_t sharc_writeHeader(sharc_byte*, const SHARC_HEADER_ORIGIN_TYPE, const struct stat64*);
void sharc_writeHeaderToStream(sharc_header *, FILE*);
void sharc_restoreFileAttributes(SHARC_HEADER_FILE_INFORMATION*, const char*);

#endif
