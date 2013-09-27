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

#ifndef SHARC_GLOBALS_H
#define SHARC_GLOBALS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(__INTEL_COMPILER)
#define SHARC_FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define SHARC_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define SHARC_FORCE_INLINE __forceinline
#else
#warning Impossible to force functions inlining. Expect performance issues.
#endif
 
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SHARC_LITTLE_ENDIAN_64(b)   ((uint64_t)b)
#define SHARC_LITTLE_ENDIAN_32(b)   ((uint32_t)b)
#define SHARC_LITTLE_ENDIAN_16(b)   ((uint16_t)b)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#if __GNUC__ * 100 + __GNUC_MINOR__ >= 403
#define SHARC_LITTLE_ENDIAN_64(b)   __builtin_bswap64(b)
#define SHARC_LITTLE_ENDIAN_32(b)   __builtin_bswap32(b)
#define SHARC_LITTLE_ENDIAN_16(b)   __builtin_bswap16(b)
#else
#warning Using bulk byte swap routines. Expect performance issues.
#define SHARC_LITTLE_ENDIAN_64(b)   ((((b) & 0xFF00000000000000ull) >> 56) | (((b) & 0x00FF000000000000ull) >> 40) | (((b) & 0x0000FF0000000000ull) >> 24) | (((b) & 0x000000FF00000000ull) >> 8) | (((b) & 0x00000000FF000000ull) << 8) | (((b) & 0x0000000000FF0000ull) << 24ull) | (((b) & 0x000000000000FF00ull) << 40) | (((b) & 0x00000000000000FFull) << 56))
#define SHARC_LITTLE_ENDIAN_32(b)   ((((b) & 0xFF000000) >> 24) | (((b) & 0x00FF0000) >> 8) | (((b) & 0x0000FF00) << 8) | (((b) & 0x000000FF) << 24))
#define SHARC_LITTLE_ENDIAN_16(b)   ((((b) & 0xFF00) >> 8) | (((b) & 0x00FF) << 8))
#endif
#else
#error Unknow endianness
#endif

#define SHARC_DICTIONARY_MAX_RESET_CYCLE_SHIFT          32
#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT    6
#define SHARC_DICTIONARY_PREFERRED_RESET_CYCLE          (1 << SHARC_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT)

#define likely(x)                         __builtin_expect(!!(x), 1)
#define unlikely(x)                       __builtin_expect(!!(x), 0)

#define SHARC_MAJOR_VERSION               0
#define SHARC_MINOR_VERSION               9
#define SHARC_REVISION                    11

#define SHARC_ESCAPE_CHARACTER   ((char)27)

typedef uint8_t sharc_byte;
typedef bool sharc_bool;

typedef enum {
    SHARC_COMPRESSION_MODE_COPY = 0,
    SHARC_COMPRESSION_MODE_CHAMELEON = 1,
    SHARC_COMPRESSION_MODE_CHAMELEON_DUAL_PASS = 2,
    SHARC_COMPRESSION_MODE_ARGONAUT = 3,
    SHARC_COMPRESSION_MODE_ARGONAUT_POST_PROCESSING = 4
} SHARC_COMPRESSION_MODE;

#endif
