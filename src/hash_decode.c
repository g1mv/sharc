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

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_checkSignaturesCount(sharc_hash_decode_state *restrict state) {
    if (state->signaturesCount == (SHARC_PREFERRED_BLOCK_SIGNATURES - 1)) {
        state->signaturesCount = 0;
        return SHARC_HASH_DECODE_STATE_INFO_NEW_BLOCK;
    }
    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_signature_fast(sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    state->signature = SHARC_LITTLE_ENDIAN_64(*(sharc_hash_decode_signature *) (in->pointer + in->position));
    in->position += sizeof(sharc_hash_decode_signature);
    state->shift = 0;
    state->signaturesCount++;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_signature_safe(sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    if (state->signatureBytes) {
        memcpy(&state->partialSignature.as_bytes[state->signatureBytes], in->pointer + in->position, sizeof(sharc_hash_decode_signature) - state->signatureBytes);
        state->signature = SHARC_LITTLE_ENDIAN_64(state->partialSignature.as_uint64_t);
        in->position += sizeof(sharc_hash_decode_signature) - state->signatureBytes;
        state->signatureBytes = 0;
        state->shift = 0;
        state->signaturesCount++;
    } else if (in->position + sizeof(sharc_hash_decode_signature) > in->size) {
        state->signatureBytes = in->size - in->position;
        memcpy(&state->partialSignature.as_bytes[0], in->pointer + in->position, state->signatureBytes);
        in->position = in->size;
        return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_signature_fast(in, state);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_compressed_chunk_fast(uint16_t *chunk, sharc_byte_buffer *restrict in) {
    *chunk = SHARC_LITTLE_ENDIAN_16(*(uint16_t *) (in->pointer + in->position));
    in->position += sizeof(uint16_t);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_compressed_chunk_safe(uint16_t *restrict chunk, sharc_byte_buffer *restrict in) {
    if (in->position + sizeof(uint16_t) > in->size)
        return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    sharc_hash_decode_read_compressed_chunk_fast(chunk, in);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_uncompressed_chunk_fast(uint32_t *chunk, sharc_byte_buffer *restrict in) {
    *chunk = SHARC_LITTLE_ENDIAN_32(*(uint32_t *) (in->pointer + in->position));
    in->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_read_uncompressed_chunk_safe(uint32_t *restrict chunk, sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    if (state->chunkBytes) {
        memcpy(&state->partialChunk.as_bytes[state->chunkBytes], in->pointer + in->position, sizeof(uint32_t) - state->chunkBytes);
        *chunk = SHARC_LITTLE_ENDIAN_32(state->partialChunk.as_uint32_t);
        in->position += sizeof(uint32_t) - state->chunkBytes;
        state->chunkBytes = 0;
    } else if (in->position + sizeof(uint32_t) > in->size) {
        state->chunkBytes = in->size - in->position;
        memcpy(&state->partialChunk.as_bytes[0], in->pointer + in->position, state->chunkBytes);
        in->position = in->size;
        return SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_uncompressed_chunk_fast(chunk, in);

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_compressed_chunk(const uint16_t *chunk, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary) {
    *(uint32_t *) (out->pointer + out->position) = (&dictionary->entries[SHARC_LITTLE_ENDIAN_16(*chunk)])->as_uint32_t;
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_uncompressed_chunk(const uint32_t *chunk, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask) {
    uint32_t hash;
    SHARC_HASH_ALGORITHM(hash, SHARC_LITTLE_ENDIAN_32(*chunk), xorMask);
    (&dictionary->entries[hash])->as_uint32_t = *chunk;
    *(uint32_t *) (out->pointer + out->position) = *chunk;
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_kernel_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask, const sharc_bool compressed) {
    if (compressed) {
        uint16_t chunk;
        sharc_hash_decode_read_compressed_chunk_fast(&chunk, in);
        sharc_hash_decode_compressed_chunk(&chunk, out, dictionary);
    } else {
        uint32_t chunk;
        sharc_hash_decode_read_uncompressed_chunk_fast(&chunk, in);
        sharc_hash_decode_uncompressed_chunk(&chunk, out, dictionary, xorMask);
    }
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_kernel_safe(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_dictionary *restrict dictionary, const uint32_t xorMask, sharc_hash_decode_state *restrict state, const sharc_bool compressed) {
    SHARC_HASH_DECODE_STATE returnState;

    if (out->position + sizeof(uint32_t) > out->size)
        return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

    if (compressed) {
        uint16_t chunk;
        if ((returnState = sharc_hash_decode_read_compressed_chunk_safe(&chunk, in)))
            return returnState;
        sharc_hash_decode_compressed_chunk(&chunk, out, dictionary);
    } else {
        uint32_t chunk;
        if ((returnState = sharc_hash_decode_read_uncompressed_chunk_safe(&chunk, in, state)))
            return returnState;
        sharc_hash_decode_uncompressed_chunk(&chunk, out, dictionary, xorMask);
    }

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE const bool sharc_hash_decode_test_compressed(sharc_hash_decode_state *state) {
    return (sharc_bool const) ((state->signature >> state->shift) & 0x1);
}

SHARC_FORCE_INLINE void sharc_hash_decode_process_data_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const uint32_t xorMask, sharc_dictionary *restrict dictionary, sharc_hash_decode_state *restrict state) {
    while (state->shift ^ 64) {
        sharc_hash_decode_kernel_fast(in, out, dictionary, xorMask, sharc_hash_decode_test_compressed(state));
        state->shift++;
    }
}

SHARC_FORCE_INLINE sharc_bool sharc_hash_decode_attempt_copy(sharc_byte_buffer *restrict out, sharc_byte *restrict origin, const uint_fast32_t count) {
    if (out->position + count <= out->size) {
        memcpy(out->pointer + out->position, origin, count);
        out->position += count;
        return false;
    }
    return true;
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
    uint_fast32_t limitIn = 0;
    uint_fast32_t limitOut = 0;

    if (in->size > SHARC_HASH_DECODE_MINIMUM_INPUT_LOOKAHEAD)
        limitIn = in->size - SHARC_HASH_DECODE_MINIMUM_INPUT_LOOKAHEAD;
    if (out->size > SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD)
        limitOut = out->size - SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD;

    switch (state->process) {
        case SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST:
            while (in->position < limitIn && out->position < limitOut) {
                if ((returnState = sharc_hash_decode_checkSignaturesCount(state)))
                    return returnState;

                sharc_hash_decode_read_signature_fast(in, state);
                sharc_hash_decode_process_data_fast(in, out, xorMask, dictionary, state);
            }
            state->process = SHARC_HASH_DECODE_PROCESS_SIGNATURE_SAFE;
            break;

        case SHARC_HASH_DECODE_PROCESS_SIGNATURE_SAFE:
            if (lastIn && (in->size - in->position < sizeof(sharc_hash_decode_signature) + sizeof(uint16_t))) {
                state->process = SHARC_HASH_DECODE_PROCESS_FINISH;
                return SHARC_HASH_DECODE_STATE_READY;
            }

            if ((returnState = sharc_hash_decode_checkSignaturesCount(state)))
                return returnState;

            if ((returnState = sharc_hash_decode_read_signature_safe(in, state)))
                return returnState;

            if (in->position < limitIn && out->position < limitOut) {
                state->process = SHARC_HASH_DECODE_PROCESS_DATA_FAST;
                return SHARC_HASH_DECODE_STATE_READY;
            }
            state->process = SHARC_HASH_DECODE_PROCESS_DATA_SAFE;
            break;

        case SHARC_HASH_DECODE_PROCESS_DATA_SAFE:
            while (state->shift ^ 64) {
                if (lastIn && (in->size - in->position < sizeof(uint16_t))) {
                    state->process = SHARC_HASH_DECODE_PROCESS_FINISH;
                    return SHARC_HASH_DECODE_STATE_READY;
                }
                if ((returnState = sharc_hash_decode_kernel_safe(in, out, dictionary, xorMask, state, sharc_hash_decode_test_compressed(state))))
                    return returnState;
                state->shift++;
                if (in->position < limitIn && out->position < limitOut) {
                    state->process = SHARC_HASH_DECODE_PROCESS_DATA_FAST;
                    return SHARC_HASH_DECODE_STATE_READY;
                }
            }
            state->process = SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            break;

        case SHARC_HASH_DECODE_PROCESS_DATA_FAST:
            sharc_hash_decode_process_data_fast(in, out, xorMask, dictionary, state);
            state->process = SHARC_HASH_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            break;

        case SHARC_HASH_DECODE_PROCESS_FINISH:
            if (state->chunkBytes) {
                if (sharc_hash_decode_attempt_copy(out, state->partialChunk.as_bytes, state->chunkBytes))
                    return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;
                state->chunkBytes = 0;
            }
            remaining = in->size - in->position;
            if (sharc_hash_decode_attempt_copy(out, in->pointer + in->position, remaining))
                return SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;
            in->position += remaining;
            return SHARC_HASH_DECODE_STATE_FINISHED;

        default:
            return SHARC_HASH_DECODE_STATE_ERROR;
    }

    return SHARC_HASH_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_HASH_DECODE_STATE sharc_hash_decode_finish(sharc_hash_decode_state *state) {
    return SHARC_HASH_DECODE_STATE_READY;
}