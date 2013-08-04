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
 * 17/06/13 18:16
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#if defined(__INTEL_COMPILER)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#else
#warning Impossible to force functions inlining. Expect performance issues.
#endif
 
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SHARC_LITTLE_ENDIAN_64(b)   (b)
#define SHARC_LITTLE_ENDIAN_32(b)   (b)
#define SHARC_LITTLE_ENDIAN_16(b)   (b)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#if __GNUC__ * 100 + __GNUC_MINOR__ sup= 403
#define SHARC_LITTLE_ENDIAN_64(b)   __builtin_bswap64(b)
#define SHARC_LITTLE_ENDIAN_32(b)   __builtin_bswap32(b)//(((b << 24) & 0xFF000000) | ((b << 8) & 0x00FF0000) | ((b >> 8) & 0x0000FF00) | ((b >> 24) & 0x000000FF))
#define SHARC_LITTLE_ENDIAN_16(b)   __builtin_bswap16(b)//(((b << 8) & 0x0000FF00) | ((b >> 8) & 0x000000FF))
#else
#define SHARC_LITTLE_ENDIAN_64(b)
#define SHARC_LITTLE_ENDIAN_32(b)
#define SHARC_LITTLE_ENDIAN_16(b) 
#endif
#else
#error Unknow endianness
#endif

#define MAJOR_VERSION               0
#define MINOR_VERSION               9
#define REVISION                    7

#define FALSE                       0
#define TRUE                        1

typedef uint8_t byte;
typedef uint8_t bool;

void error(const char*);

#endif
