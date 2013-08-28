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
 * 28/08/13 19:02
 */

#include "hash.h"

SHARC_FORCE_INLINE void sharc_hash_compute(uint32_t* hash, const uint32_t value, const uint32_t xorMask) {
    *hash = SHARC_HASH_OFFSET_BASIS;
    *hash ^= (value ^ xorMask);
    *hash *= SHARC_HASH_PRIME;
    *hash = (*hash >> (32 - SHARC_HASH_BITS)) ^ (*hash & ((1 << SHARC_HASH_BITS) - 1));
}