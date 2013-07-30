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

#ifndef CLIENT_H
#define CLIENT_H

#define __USE_LARGEFILE64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <string.h>

#include "file_header.h"
#include "sharc.h"
#include "chrono.h"

#define ACTION_COMPRESS         0
#define ACTION_DECOMPRESS       1

#define NO_PROMPTING            FALSE
#define PROMPTING               TRUE

#define TYPE_FILE               0
#define TYPE_STREAM             1

#define STDIN                   "stdin"
#define STDOUT                  "stdout"
#define STDIN_COMPRESSED        "stdin.sharc"

typedef struct {
    const char* name;
    FILE* stream;
    byte type;
} CLIENT_IO;

byte readBuffer[MAX_BUFFER_SIZE];
byte interBuffer[MAX_BUFFER_SIZE];
byte writeBuffer[MAX_BUFFER_SIZE];

FILE* checkOpenFile(const char*, const char*, const bool);
void version();
void usage();
void clientCompress(CLIENT_IO*, CLIENT_IO*, const byte, const uint32_t, const bool);
void clientDecompress(CLIENT_IO*, CLIENT_IO*, const bool);
int main(int, char **);

#endif
