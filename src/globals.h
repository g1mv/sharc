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
 * 17/06/13 18:16
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN

#elif __BYTE_ORDER == __BIG_ENDIAN
#error Big endian systems not yet supported
#else
#error Unknow endianness
#endif

#if defined(__INTEL_COMPILER)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#else
#warning Impossible to force functions inlining. Expect performance issues.
#endif

#define MAX_PARALLELISM             4

#define MAJOR_VERSION               0
#define MINOR_VERSION               9
#define REVISION                    1

#define FALSE                       0
#define TRUE                        1

typedef uint8_t byte;
typedef uint8_t bool;

void error(const char*);
FILE* checkOpenFile(const char*, const char*, bool);

#endif
