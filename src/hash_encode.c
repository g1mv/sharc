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

#include "hash_encode.h"

SHARC_FORCE_INLINE void sharc_hash_encode_writeSignature(sharc_state * internalState) {
    internalState->signature->as_uint64_t |= ((uint64_t)1) << internalState->shift;
}

SHARC_FORCE_INLINE void sharc_hash_encode_flush(sharc_byte_buffer * out, sharc_state * internalState) {
    *(uint64_t*)(out->pointer + *(sharc_byte*)internalState->signature) = SHARC_LITTLE_ENDIAN_64(internalState->signature->as_uint64_t);
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_resetState(sharc_byte_buffer* out, sharc_state * internalState) {
    if((out->position + SHARC_HASH_ENCODE_MINIMUM_LOOKAHEAD) > out->size)
        return SHARC_HASH_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    internalState->signature = (sharc_signature*)(out->pointer + out->position);
    internalState->signature->as_uint64_t = 0;
    out->position += 8;
    internalState->shift = 0;
    internalState->blockSignaturesCount ++;
    return SHARC_HASH_ENCODE_STATE_READY_FOR_NEXT;
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_checkState(sharc_byte_buffer * in, sharc_byte_buffer * out, sharc_state * internalState) {
    switch(internalState->shift) {
        case 64:
            sharc_hash_encode_flush(out, internalState);
            SHARC_HASH_ENCODE_STATE returnedState = sharc_hash_encode_resetState(out, internalState);
            if(returnedState)
                return returnedState;
            break;
        default:
            break;
    }
    return SHARC_HASH_ENCODE_STATE_READY_FOR_NEXT;
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_updateDictionaryEntry(sharc_byte_buffer * in, sharc_byte_buffer * out, sharc_dictionary_entry * entry, const uint32_t chunk, sharc_state * internalState) {
    entry->as_uint32_t = chunk;
    *(uint32_t*)(out->pointer + out->position) = chunk;
    out->position += 4;
    internalState->shift ++;
    return sharc_hash_encode_checkState(in, out, internalState);
}

SHARC_FORCE_INLINE SHARC_HASH_ENCODE_STATE sharc_hash_encode_kernel(sharc_byte_buffer * in, sharc_byte_buffer * out, const uint32_t xorMask, sharc_dictionary * dictionary, sharc_state * internalState) {
    const uint32_t chunk = *(uint32_t*)(in->pointer + in->position);
    sharc_hash_compute(&(internalState->hash), SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
    sharc_dictionary_entry * found = &dictionary->entries[internalState->hash];
    if(chunk ^ found->as_uint32_t) {
        SHARC_HASH_ENCODE_STATE returnedState = sharc_hash_encode_updateDictionaryEntry(in, out, found, chunk, internalState);
        if(returnedState)
            return returnedState;
    } else {
        sharc_hash_encode_writeSignature(internalState);
        *(uint16_t*)(out->pointer + out->position) = SHARC_LITTLE_ENDIAN_16(internalState->hash);
        out->position += 2;
        internalState->shift ++;
        SHARC_HASH_ENCODE_STATE returnedState = sharc_hash_encode_checkState(in, out, internalState);
        if(returnedState)
            return returnedState;
    }

    in->position += 4;

    if(in->position == in->size)
        return SHARC_HASH_ENCODE_STATE_STALL_ON_INPUT_BUFFER;

    return SHARC_HASH_ENCODE_STATE_READY_FOR_NEXT;
}

SHARC_FORCE_INLINE sharc_bool sharc_hash_encode_finish(sharc_byte_buffer * in, sharc_byte_buffer * out, sharc_state * internalState) {
    if(internalState->shift > 0)
        sharc_hash_encode_flush(out, internalState);
    else
        out->position -= 8;

    const uint32_t remaining = in->size - in->position;
    for(uint32_t i = 0; i < remaining; i ++) {
        if(out->position < out->size)
            out->pointer[out->position ++] = in->pointer[in->position ++];
        else
            return SHARC_FALSE;
    }

    return SHARC_TRUE;
}
