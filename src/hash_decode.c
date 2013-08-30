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

SHARC_FORCE_INLINE void sharc_byteCopy(sharc_byte_buffer *in, sharc_byte_buffer *out, const uint32_t number) {
    memcpy(out->pointer + out->position, in->pointer + in->position, number);

    in->position += number;
    out->position += number;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_init(sharc_hash_decode_state *state) {
    state->signaturesCount = 0;

    state->process = SHARC_HASH_DECODE_PROCESS_DATA;

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_kernel(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask, sharc_bool mode) {
    uint_fast32_t hash;
    uint32_t chunk;
    sharc_dictionary_entry *found;
    switch (mode) {
        case SHARC_FALSE:
            chunk = *(uint32_t *) (in->pointer + in->position);
            SHARC_HASH_ALGORITHM(hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
            (&dictionary->entries[hash])->as_uint32_t = chunk;
            *(uint32_t *) (out->pointer + out->position) = chunk;
            in->position += sizeof(uint32_t);
            break;

        case SHARC_TRUE:
            found = &dictionary->entries[SHARC_LITTLE_ENDIAN_16(*(uint16_t *) (in->pointer + in->position))];
            *(uint32_t *) (out->pointer + out->position) = found->as_uint32_t;
            in->position += sizeof(uint16_t);
            break;

        default:
            break;
    }
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_continue(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const uint32_t xorMask, sharc_dictionary * restrict dictionary, sharc_hash_decode_state* state) {
    sharc_byte i;

    if (out->position + SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD > out->size)
        return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

    while(SHARC_TRUE) {
        if(in->position + sizeof(sharc_hash_decode_signature) > in->size) {
            uint32_t remaining = in->size - in->position;
            memcpy(&state->signature, in->pointer + in->position, remaining);
            in->position += remaining;
            state->process = SHARC_HASH_DECODE_PROCESS_READING_SIGNATURE;
            return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
        }

        state->signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
        in->position += 8;
        for (i = 0; i < 0x40; i++)
            sharc_hash_decode_kernel(in, out, dictionary, xorMask, (sharc_bool)((state->signature >> i) & 0x1));

    }

}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_finish(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const uint32_t xorMask, sharc_dictionary *dictionary) {
    uint64_t signature;
    sharc_byte i;

    while (in->position <= in->size - 10 - 8 - 256) {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
        in->position += 8;
        for (i = 0; i < 0x40; i++)
            sharc_hash_decode_kernel(in, out, dictionary, xorMask, (sharc_bool)((signature >> i) & 0x1));
    }

    do {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t *) (in->pointer + in->position));
        in->position += 8;
        i = 0;
        while ((in->size - in->position) & 0xFFFFFFFC && i < 0x40)
            sharc_hash_decode_kernel(in, out, dictionary, xorMask, (sharc_bool)((signature >> (i++)) & 0x1));
    } while (in->position <= in->size - 10);

    const uint32_t remaining = in->size - in->position;
    if (i & 0x40)
        sharc_byteCopy(in, out, remaining);
    else {
        if ((signature >> i) & 0x1) {
            if (remaining & 0x2)
                sharc_hash_decode_kernel(in, out, dictionary, xorMask, SHARC_TRUE);
            else
                sharc_error("Corrupted file !");
            if (remaining == 0x3)
                sharc_byteCopy(in, out, 1);
        } else
            sharc_byteCopy(in, out, remaining);
    }

    return SHARC_TRUE;
}