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

SHARC_FORCE_INLINE uint_fast32_t sharc_hash_decode_byteTransfer32(sharc_byte_buffer *restrict in, uint32_t *destination, int_fast32_t count, const uint_fast32_t offset) {
    const int_fast32_t initialCount = count;
    while (--count >= 0) {
        *destination |= (*(in->pointer + in->position) << ((count + offset) << 3));
        in->position++;
    }
    return (uint32_t) initialCount;
}

SHARC_FORCE_INLINE uint_fast32_t sharc_hash_decode_byteTransfer64(sharc_byte_buffer *restrict in, uint64_t *destination, int_fast32_t count, const uint_fast32_t offset) {
    const int_fast32_t initialCount = count;
    while (--count >= 0) {
        *destination |= (*(in->pointer + in->position) << ((count + offset) << 3));
        in->position++;
    }
    return (uint32_t) initialCount;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_signatureCountIncrement(sharc_hash_decode_state *restrict state) {
    state->signaturesCount++;
    if (state->signaturesCount == SHARC_PREFERRED_BLOCK_SIGNATURES) {
        state->signaturesCount = 0;
        return SHARC_HASH_DECODE_STATE_INFO_NEW_BLOCK;
    }

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_signature_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state) {
    SHARC_HASH_DECODE_STATE returnState;

    if (out->position + SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD > out->size)
        return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

    if ((returnState = sharc_hash_decode_signatureCountIncrement(state)))
        return returnState;

    state->signature = SHARC_LITTLE_ENDIAN_64(*(sharc_hash_decode_signature *) (in->pointer + in->position));
    in->position += sizeof(sharc_hash_decode_signature);
    state->shift = 0;

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_signature_safe(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state) {
    SHARC_HASH_DECODE_STATE returnState;

    if (out->position + SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD > out->size)
        return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

    if ((returnState = sharc_hash_decode_signatureCountIncrement(state)))
        return returnState;

    if (state->signatureBytes) {
        sharc_hash_decode_byteTransfer64(in, &state->signature, sizeof(sharc_hash_decode_signature) - state->signatureBytes, 0);
        state->signature = __builtin_bswap64(state->signature);
        state->signatureBytes = 0;
        state->shift = 0;
        state->process = SHARC_HASH_DECODE_PROCESS_DATA_SAFE;
    } else if (in->position + sizeof(sharc_hash_decode_signature) > in->size) {
        const int_fast32_t available = in->size - in->position;
        state->signatureBytes = sharc_hash_decode_byteTransfer64(in, &state->signature, available, sizeof(sharc_hash_decode_signature) - available);
        return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_signature_fast(in, out, state);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_chunk_fast(uint32_t *chunk, sharc_byte_buffer *restrict in) {
    *chunk = SHARC_LITTLE_ENDIAN_32(*(uint32_t *) (in->pointer + in->position));
    in->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_chunk_safe(sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    if (state->chunkBytes) {
        sharc_hash_decode_byteTransfer32(in, &state->chunk, sizeof(uint32_t) - state->chunkBytes, 0);
        state->chunk = __builtin_bswap32(state->chunk);
        state->chunkBytes = 0;
        state->process = SHARC_HASH_DECODE_PROCESS_DATA_SAFE;
    } else if (in->position + sizeof(uint32_t) > in->size) {
        const int_fast32_t available = in->size - in->position;
        state->chunkBytes = sharc_hash_decode_byteTransfer32(in, &state->chunk, available, sizeof(uint32_t) - available);
        return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_chunk_fast(&state->chunk, in);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_process_chunk_compressed(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary) {
    sharc_dictionary_entry *found = &dictionary->entries[SHARC_LITTLE_ENDIAN_16(*(uint16_t *) (in->pointer + in->position))];
    *(uint32_t *) (out->pointer + out->position) = found->as_uint32_t;
    in->position += sizeof(uint16_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_process_chunk_uncompressed(const uint32_t *chunk, uint_fast32_t *hash, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint_fast32_t xorMask) {
    SHARC_HASH_ALGORITHM(*hash, *chunk, xorMask);
    (&dictionary->entries[*hash])->as_uint32_t = *chunk;
    *(uint32_t *) (out->pointer + out->position) = *chunk;
    //in->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_kernel_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask, sharc_hash_decode_state *restrict state, const sharc_bool compressed) {
    uint_fast32_t hash;

    if (compressed)
        sharc_hash_decode_process_chunk_compressed(in, out, dictionary);
    else {
        sharc_hash_decode_read_chunk_fast(&state->chunk, in);
        sharc_hash_decode_process_chunk_uncompressed(&state->chunk, &hash, out, dictionary, xorMask);
    }
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_kernel_safe(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask, sharc_hash_decode_state *restrict state, const sharc_bool compressed) {
    SHARC_HASH_DECODE_STATE returnState;
    uint_fast32_t hash;

    if (compressed) {
        if (in->position + sizeof(uint16_t) > in->size)
            return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
        sharc_hash_decode_process_chunk_compressed(in, out, dictionary);
    } else {
        if ((returnState = sharc_hash_decode_read_chunk_safe(in, state)))
            return returnState;
        sharc_hash_decode_process_chunk_uncompressed(&state->chunk, &hash, out, dictionary, xorMask);
    }
    out->position += sizeof(uint32_t);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE const bool sharc_hash_decode_test_compressed(sharc_hash_decode_state *state) {
    return (sharc_bool const) ((state->signature >> state->shift) & 0x1);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_init(sharc_hash_decode_state *state) {
    state->signaturesCount = 0;
    state->signatureBytes = 0;
    state->chunkBytes = 0;

    state->process = SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const uint32_t xorMask, sharc_dictionary *restrict dictionary, sharc_hash_decode_state *restrict state, const sharc_bool lastIn) {
    SHARC_HASH_DECODE_STATE returnState;
    uint_fast32_t remaining;
    uint_fast32_t limit = 0;

    const uint_fast32_t minimumFastLookahead = (sizeof(sharc_hash_decode_signature) - 2) + sizeof(sharc_hash_decode_signature) + 32 * sizeof(sharc_hash_decode_signature);
    if (in->size > minimumFastLookahead)
        limit = in->size - minimumFastLookahead;

    switch (state->process) {
        case SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST:
            while (in->position < limit) {
                if ((returnState = sharc_hash_decode_read_signature_fast(in, out, state)))
                    return returnState;

                while (state->shift ^ 64) {
                    sharc_hash_decode_kernel_fast(in, out, dictionary, xorMask, state, sharc_hash_decode_test_compressed(state));
                    state->shift++;
                }
            }
            if (lastIn)
                state->process = SHARC_HASH_DECODE_PROCESS_FINISH;
            else
                state->process = SHARC_HASH_DECODE_PROCESS_READ_SIGNATURE_SAFE;
            break;

        case SHARC_HASH_DECODE_PROCESS_READ_SIGNATURE_SAFE:
            if ((returnState = sharc_hash_decode_read_signature_safe(in, out, state)))
                return returnState;

            state->process = SHARC_HASH_DECODE_PROCESS_DATA_SAFE;
            break;

        case SHARC_HASH_DECODE_PROCESS_DATA_SAFE:
            while (state->shift ^ 64) {
                if ((returnState = sharc_hash_decode_kernel_safe(in, out, dictionary, xorMask, state, sharc_hash_decode_test_compressed(state)))) {
                    state->shift--;
                    return returnState;
                }
                state->shift++;
            }
            if (in->position < limit)
                state->process = SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            else
                state->process = SHARC_HASH_DECODE_PROCESS_READ_SIGNATURE_SAFE;
            break;

        case SHARC_HASH_DECODE_PROCESS_FINISH:
            remaining = in->size & 0x1F;
            if (out->position + remaining <= out->size) {
                memcpy(out->pointer + out->position, in->pointer + in->position, remaining);

                in->position += remaining;
                out->position += remaining;

                return SHARC_HASH_DECODE_STATE_FINISHED;
            } else
                return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

        default:
            return SHARC_HASH_DECODE_STATE_ERROR;
    }

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_finish(sharc_hash_decode_state *state) {
    return SHARC_HASH_DECODE_STATE_READY;
}