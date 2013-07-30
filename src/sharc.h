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
 * 01/06/13 17:55
 */

#ifndef SHARC_H
#define SHARC_H

//#define __USE_LARGEFILE64
//#define _LARGEFILE_SOURCE
//#define _LARGEFILE64_SOURCE

#include <stdlib.h>
#include <stdio.h>

#include "chrono.h"
#include "sharc_cipher.h"
#include "file_header.h"
#include "block_header.h"

void compress(FILE*, FILE*, BYTE_BUFFER*, BYTE_BUFFER*, BYTE_BUFFER*, const byte, const uint32_t, const struct stat64);
FILE_HEADER decompress(FILE*, FILE*, BYTE_BUFFER*, BYTE_BUFFER*, BYTE_BUFFER*);

#endif
