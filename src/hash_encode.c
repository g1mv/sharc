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
 * 28/08/13 18:57
 */

#include "encode.h"

SHARC_FORCE_INLINE void sharc_hash_encode_writeSignature(sharc_hash_encode_state *state) {
    *(state->signature) |= ((uint64_t)1) << state->shift;
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_resetState(sharc_byte_buffer* restrict out, sharc_hash_encode_state* restrict state) {
    if(out->position + SHARC_HASH_ENCODE_MINIMUM_LOOKAHEAD > out->size)
        return SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER;

    state->signaturesCount ++;
    if (state->signaturesCount == 8)
        return SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK;

    state->signature = (sharc_hash_encode_signature *)(out->pointer + out->position);
    *state->signature = 0;
    out->position += sizeof(sharc_hash_encode_signature);
    state->shift = 0;

    return SHARC_HASH_ENCODE_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_checkState(sharc_byte_buffer* restrict out, sharc_hash_encode_state* restrict state) {
    SHARC_HASH_ENCODE_STATE returnState;

    switch(state->shift) {
        case 64:
            returnState = sharc_hash_encode_resetState(out, state);
            if(returnState)
                return returnState;
            break;
        default:
            break;
    }
    return SHARC_HASH_ENCODE_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_kernel(sharc_byte_buffer* restrict in, sharc_byte_buffer* restrict out, const uint32_t inLimit, const uint32_t xorMask, sharc_dictionary* restrict dictionary, sharc_hash_encode_state* restrict state) {
    SHARC_HASH_ENCODE_STATE returnState;
    uint32_t hash;

    sharc_hash_encode_resetState(out, state);

    while(SHARC_TRUE) {
        const uint32_t chunk = *(uint32_t*)(in->pointer + in->position);
        SHARC_HASH_ALGORITHM(hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
        sharc_dictionary_entry * found = &dictionary->entries[hash];

        if(chunk ^ found->as_uint32_t) {
            found->as_uint32_t = chunk;
            *(uint32_t*)(out->pointer + out->position) = chunk;
            out->position += sizeof(uint32_t);
        } else {
            sharc_hash_encode_writeSignature(state);
            *(uint16_t*)(out->pointer + out->position) = SHARC_LITTLE_ENDIAN_16(hash);
            out->position += 2;
        }
        state->shift ++;
        returnState  = sharc_hash_encode_checkState(out, state);
        if(returnState)
            return returnState;

        in->position += sizeof(uint32_t);
        if(in->position == inLimit)
            return SHARC_HASH_ENCODE_STATE_STALL_INPUT_BUFFER;
    }
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_finish(sharc_byte_buffer* restrict in, sharc_byte_buffer* restrict out, const uint32_t xorMask, sharc_dictionary* restrict dictionary, sharc_hash_encode_state* restrict state) {
    const uint32_t remaining = in->size & 0x3;

    if(in->size > remaining)
        switch(sharc_hash_encode_kernel(in, out, in->size - remaining, xorMask, dictionary, state)) {
            case SHARC_HASH_ENCODE_STATE_STALL_INPUT_BUFFER:
                break;
            case SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER:
                return SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER;
            case SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK:
                return SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK;
            default:
                return SHARC_HASH_ENCODE_STATE_ERROR;
        }

    if(state->shift == 0)
        out->position -= sizeof(sharc_hash_encode_signature);

    if(out->position + remaining <= out->size)
        memcpy(out->pointer + out->position, in->pointer + in->position, remaining);
    else
        return SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER;

    return SHARC_HASH_ENCODE_STATE_OK;
}
