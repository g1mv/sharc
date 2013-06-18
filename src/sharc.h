/*
 * Copyright (c) 2013, Guillaume Voirin (gvoirin@centaurean.com)
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 17:55
 */

#ifndef SHARC_H
#define SHARC_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "sharc_cipher.h"
#include "file_header.h"
#include "block_header.h"

#define ACTION_COMPRESS     0
#define ACTION_DECOMPRESS   1

byte readBuffer[MAX_BUFFER_SIZE][MAX_PARALLELISM];
byte writeBuffer[MAX_BUFFER_SIZE][MAX_PARALLELISM];

FILE* checkOpenFile(const char*, const char*);
void compress(const char*, const byte, const uint32_t);
//void decompress(char*);

#endif