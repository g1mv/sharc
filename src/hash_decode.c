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
 * 28/08/13 22:30
 */

#include "hash_decode.h"

SHARC_FORCE_INLINE void sharc_kernelDecode(sharc_byte_buffer *in, sharc_byte_buffer *out, sharc_dictionary_entry *dictionary, const uint32_t xorMask, const sharc_bool mode) {
    uint32_t hash;
    uint32_t chunk;
    sharc_dictionary_entry *found;
    switch (mode) {
        case SHARC_FALSE:
            chunk = *(uint32_t *) (in->pointer + in->position);
            sharc_hash_compute(&hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
            (&dictionary[hash])->as_uint32_t = chunk;
            *(uint32_t *) (out->pointer + out->position) = chunk;
            in->position += 4;
            break;
        case SHARC_TRUE:
            found = &dictionary[SHARC_LITTLE_ENDIAN_16(*(uint16_t *) (in->pointer + in->position))];
            *(uint32_t *) (out->pointer + out->position) = found->as_uint32_t;
            in->position += 2;
            break;
    }
    out->position += 4;
}

SHARC_FORCE_INLINE void sharc_byteCopy(sharc_byte_buffer *in, sharc_byte_buffer *out, const uint32_t number) {
    for (sharc_byte r = 0; r < number; r++)
        out->pointer[out->position++] = in->pointer[in->position++];
}

SHARC_FORCE_INLINE sharc_bool sharc_hashDecode(sharc_byte_buffer *in, sharc_byte_buffer *out, const uint32_t xorMask, sharc_dictionary_entry *dictionary) {
    uint64_t signature;
    sharc_byte i;

    while (in->position <= in->size - 10 - 8 - 256) {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
        in->position += 8;
        for (i = 0; i < 0x40; i++)
            sharc_kernelDecode(in, out, dictionary, xorMask, (signature >> i) & 0x1);
    }

    do {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
        in->position += 8;
        i = 0;
        while ((in->size - in->position) & 0xFFFFFFFC && i < 0x40)
            sharc_kernelDecode(in, out, dictionary, xorMask, (signature >> (i++)) & 0x1);
    } while (in->position <= in->size - 10);

    const uint32_t remaining = in->size - in->position;
    if (i & 0x40)
        sharc_byteCopy(in, out, remaining);
    else {
        if ((signature >> i) & 0x1) {
            if (remaining & 0x2)
                sharc_kernelDecode(in, out, dictionary, xorMask, SHARC_TRUE);
            else
                sharc_error("Corrupted file !");
            if (remaining == 0x3)
                sharc_byteCopy(in, out, 1);
        } else
            sharc_byteCopy(in, out, remaining);
    }

    return SHARC_TRUE;
}