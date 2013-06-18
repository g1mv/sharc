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
 * 01/06/13 13:08
 */

#include "direct_hash_cipher.h"

FORCE_INLINE bool directHashEncode(byte* _inBuffer, uint32_t _inSize, byte* _outBuffer, uint32_t _outSize) {
    return TRUE;//return hashEncode(_inBuffer, _inSize, _outBuffer, _outSize, DIRECT_XOR_MASK);
}

FORCE_INLINE bool directHashDecode(FILE* inFile, FILE* outFile, const uint64_t limit) {
    return TRUE;//hashDecode(inFile, outFile, DIRECT_XOR_MASK, limit);
}