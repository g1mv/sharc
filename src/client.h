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
 * 03/07/13 14:54
 */

#ifndef SHARC_CLIENT_H
#define SHARC_CLIENT_H

#ifdef _WIN32
#include <fcntl.h>
#endif

#include <string.h>

#include "file_header.h"
#include "sharc.h"
#include "chrono.h"

#define SHARC_ACTION_COMPRESS         0
#define SHARC_ACTION_DECOMPRESS       1

#define SHARC_NO_PROMPTING            SHARC_FALSE
#define SHARC_PROMPTING               SHARC_TRUE

#define SHARC_TYPE_FILE               0
#define SHARC_TYPE_STREAM             1

#define SHARC_STDIN                   "stdin"
#define SHARC_STDOUT                  "stdout"
#define SHARC_STDIN_COMPRESSED        "stdin.sharc"

#if defined(_WIN64) || defined(_WIN32)
#define SHARC_PLATFORM_STRING         "Microsoft Windows"
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
#define SHARC_PLATFORM_STRING         "iOS Simulator"
#elif TARGET_OS_IPHONE
#define SHARC_PLATFORM_STRING         "iOS"
#elif TARGET_OS_MAC
#define SHARC_PLATFORM_STRING         "Mac OS/X"
#else
#define SHARC_PLATFORM_STRING         "an unknown Apple platform"
#endif
#elif __FreeBSD__
#define SHARC_PLATFORM_STRING         "FreeBSD"
#elif __linux
#define SHARC_PLATFORM_STRING         "GNU/Linux"
#elif __unix
#define SHARC_PLATFORM_STRING         "Unix"
#elif __posix
#define SHARC_PLATFORM_STRING         "Posix"
#else
#define SHARC_PLATFORM_STRING         "an unknown platform"
#endif

typedef struct {
    const char* name;
    FILE* stream;
    byte type;
} SHARC_CLIENT_IO;

byte readBuffer[SHARC_MAX_BUFFER_SIZE];
byte interBuffer[SHARC_MAX_BUFFER_SIZE];
byte writeBuffer[SHARC_MAX_BUFFER_SIZE];

FILE* checkOpenFile(const char*, const char*, const bool);
void version();
void usage();
void clientCompress(SHARC_CLIENT_IO*, SHARC_CLIENT_IO*, const byte, const uint32_t, const bool);
void clientDecompress(SHARC_CLIENT_IO*, SHARC_CLIENT_IO*, const bool);
int main(int, char **);

#endif
