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
 * 01/06/13 20:03
 */

#include "xor_hash_cipher.h"

SHARC_FORCE_INLINE sharc_bool sharc_xorHashEncode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, SHARC_ENTRY* dictionary) {
    return sharc_hashEncode(in, out, SHARC_XOR_MASK, dictionary);
}

SHARC_FORCE_INLINE sharc_bool sharc_xorHashDecode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, SHARC_ENTRY* dictionary) {
    return sharc_hashDecode(in, out, SHARC_XOR_MASK, dictionary);
}