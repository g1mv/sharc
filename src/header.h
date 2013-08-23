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

#define SHARC_MAGIC_NUMBER      1908011803

#define SHARC_TYPE_STREAM       0
#define SHARC_TYPE_FILE         1

#pragma pack(push)
#pragma pack(4)
typedef struct {
    uint32_t magicNumber;
    sharc_byte version[3];
    sharc_byte bufferSizeShift;
    sharc_byte resetCycleSizeShift;
    sharc_byte type;
    sharc_byte reserved[2];
} SHARC_GENERIC_HEADER;

typedef struct {
    uint64_t originalFileSize;
    uint32_t fileMode;
    uint64_t fileAccessed;
    uint64_t fileModified;
} SHARC_FILE_INFORMATION_HEADER;

typedef struct {
    SHARC_GENERIC_HEADER genericHeader;
    SHARC_FILE_INFORMATION_HEADER fileInformationHeader;
} SHARC_HEADER;
#pragma pack(pop)

SHARC_HEADER sharc_createHeader(const uint32_t, const sharc_byte, struct stat64);
sharc_bool sharc_checkSource(const uint32_t);
SHARC_HEADER sharc_readHeader(FILE*);
void sharc_writeHeader(SHARC_HEADER*, FILE*);
void sharc_restoreFileAttributes(SHARC_FILE_INFORMATION_HEADER*, const char*);

#endif
