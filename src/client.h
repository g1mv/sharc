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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "../modules/libssc/src/ssc_api.h"
#include "chrono.h"
#include "header.h"
#include "globals.h"

#define SHARC_ACTION_COMPRESS         0
#define SHARC_ACTION_DECOMPRESS       1

#define SHARC_NO_PROMPTING            false
#define SHARC_PROMPTING               true

#define SHARC_STDIN                   "stdin"
#define SHARC_STDOUT                  "stdout"
#define SHARC_STDIN_COMPRESSED        "stdin.sharc"

#define SHARC_OUTPUT_PATH_MAX_SIZE    256
#define SHARC_FILE_OUTPUT_PATH        false
#define SHARC_FIXED_OUTPUT_PATH       true

#define SHARC_PREFERRED_BUFFER_SIZE   1 << 19

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SHARC_ENDIAN_STRING           "Little"
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SHARC_ENDIAN_STRING           "Big"
#endif

#if defined(_WIN64) || defined(_WIN32)
#define SHARC_PATH_SEPARATOR          '\\'
#else
#define SHARC_PATH_SEPARATOR          '/'
#define SHARC_ALLOW_ANSI_ESCAPE_SEQUENCES
#endif

#if defined(_WIN64) || defined(_WIN32)
#define SHARC_PLATFORM_STRING         "Microsoft Windows"
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#include "globals.h"
#if TARGET_IPHONE_SIMULATOR
#define SHARC_PLATFORM_STRING         "iOS Simulator"
#elif TARGET_OS_IPHONE
#define SHARC_PLATFORM_STRING         "iOS"
#elif TARGET_OS_MAC
#define SHARC_PLATFORM_STRING         "Mac OS/X"
#else
#define SHARC_PLATFORM_STRING         "an unknown Apple platform"
#endif
#elif defined(__FreeBSD__)
#define SHARC_PLATFORM_STRING         "FreeBSD"
#elif defined(__linux__)
#define SHARC_PLATFORM_STRING         "GNU/Linux"
#elif defined(__unix__)
#define SHARC_PLATFORM_STRING         "Unix"
#elif defined(__posix__)
#define SHARC_PLATFORM_STRING         "Posix"
#else
#define SHARC_PLATFORM_STRING         "an unknown platform"
#endif

typedef struct {
    const char* name;
    FILE* stream;
    SHARC_HEADER_ORIGIN_TYPE origin_type;
} sharc_client_io;

uint8_t input_buffer[SHARC_PREFERRED_BUFFER_SIZE];
uint8_t output_buffer[SHARC_PREFERRED_BUFFER_SIZE];

FILE* sharc_client_checkOpenFile(const char*, const char*, const sharc_bool);
void sharc_client_version();
void sharc_client_usage();
void sharc_client_compress(sharc_client_io *, sharc_client_io *, const SSC_COMPRESSION_MODE, const sharc_bool, const char*, const char*);
void sharc_client_decompress(sharc_client_io *, sharc_client_io *, const sharc_bool, const char*, const char*);

#endif
