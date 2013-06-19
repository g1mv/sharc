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

#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

#include "globals.h"

typedef struct {
    byte name[5];
    byte version[3];
    uint64_t originalFileSize;
    uint32_t bufferSize;
    uint16_t fileMode;
    uint64_t fileCreated;
    uint64_t fileAccessed;
    uint64_t fileModified;
} FILE_HEADER;

FILE_HEADER createFileHeader(const uint32_t, struct stat);
bool checkFileType(byte*);
FILE_HEADER readFileHeader(FILE*);

#endif