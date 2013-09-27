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

#include "chameleon_decode.h"

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE sharc_hash_decode_checkSignaturesCount(sharc_hash_decode_state *restrict state) {
    switch (state->signaturesCount) {
        case SHARC_PREFERRED_EFFICIENCY_CHECK_SIGNATURES:
            if (state->efficiencyChecked ^ 0x1) {
                state->efficiencyChecked = 1;
                return SHARC_KERNEL_DECODE_STATE_INFO_EFFICIENCY_CHECK;
            }
            break;
        case SHARC_PREFERRED_BLOCK_SIGNATURES:
            state->signaturesCount = 0;
            state->efficiencyChecked = 0;

            if (state->resetCycle)
                state->resetCycle--;
            else {
                CHAMELEON_NAME(sharc_dictionary_reset)(&state->dictionary);
                state->resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
            }

            return SHARC_KERNEL_DECODE_STATE_INFO_NEW_BLOCK;
        default:
            break;
    }
    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_signature_fast(sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    state->signature = SHARC_LITTLE_ENDIAN_64(*(sharc_hash_signature *) (in->pointer + in->position));
    in->position += sizeof(sharc_hash_signature);
    state->shift = 0;
    state->signaturesCount++;
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE sharc_hash_decode_read_signature_safe(sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    if (state->signatureBytes) {
        memcpy(&state->partialSignature.as_bytes[state->signatureBytes], in->pointer + in->position, (uint32_t) (sizeof(sharc_hash_signature) - state->signatureBytes));
        state->signature = SHARC_LITTLE_ENDIAN_64(state->partialSignature.as_uint64_t);
        in->position += sizeof(sharc_hash_signature) - state->signatureBytes;
        state->signatureBytes = 0;
        state->shift = 0;
        state->signaturesCount++;
    } else if (in->position + sizeof(sharc_hash_signature) > in->size) {
        state->signatureBytes = in->size - in->position;
        memcpy(&state->partialSignature.as_bytes[0], in->pointer + in->position, (uint32_t) state->signatureBytes);
        in->position = in->size;
        return SHARC_KERNEL_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_signature_fast(in, state);

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_compressed_chunk_fast(uint16_t *chunk, sharc_byte_buffer *restrict in) {
    *chunk = *(uint16_t *) (in->pointer + in->position);
    in->position += sizeof(uint16_t);
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE sharc_hash_decode_read_compressed_chunk_safe(uint16_t *restrict chunk, sharc_byte_buffer *restrict in) {
    if (in->position + sizeof(uint16_t) > in->size)
        return SHARC_KERNEL_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    sharc_hash_decode_read_compressed_chunk_fast(chunk, in);

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_read_uncompressed_chunk_fast(uint32_t *chunk, sharc_byte_buffer *restrict in) {
    *chunk = *(uint32_t *) (in->pointer + in->position);
    in->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE sharc_hash_decode_read_uncompressed_chunk_safe(uint32_t *restrict chunk, sharc_byte_buffer *restrict in, sharc_hash_decode_state *restrict state) {
    if (state->uncompressedChunkBytes) {
        memcpy(&state->partialUncompressedChunk.as_bytes[state->uncompressedChunkBytes], in->pointer + in->position, (uint32_t) (sizeof(uint32_t) - state->uncompressedChunkBytes));
        *chunk = state->partialUncompressedChunk.as_uint32_t;
        in->position += sizeof(uint32_t) - state->uncompressedChunkBytes;
        state->uncompressedChunkBytes = 0;
    } else if (in->position + sizeof(uint32_t) > in->size) {
        state->uncompressedChunkBytes = in->size - in->position;
        memcpy(&state->partialUncompressedChunk.as_bytes[0], in->pointer + in->position, (uint32_t) state->uncompressedChunkBytes);
        in->position = in->size;
        return SHARC_KERNEL_DECODE_STATE_STALL_ON_INPUT_BUFFER;
    } else
        sharc_hash_decode_read_uncompressed_chunk_fast(chunk, in);

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_hash_decode_compressed_chunk(const uint16_t *chunk, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state) {
    *(uint32_t *) (out->pointer + out->position) = (&state->dictionary.entries[SHARC_LITTLE_ENDIAN_16(*chunk)])->as_uint32_t;
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_uncompressed_chunk(const uint32_t *chunk, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state) {
    uint32_t hash;
    SHARC_CHAMELEON_HASH_ALGORITHM(hash, SHARC_LITTLE_ENDIAN_32(*chunk));
    (&state->dictionary.entries[hash])->as_uint32_t = *chunk;
    *(uint32_t *) (out->pointer + out->position) = *chunk;
    out->position += sizeof(uint32_t);
}

SHARC_FORCE_INLINE void sharc_hash_decode_kernel_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const sharc_bool compressed, sharc_hash_decode_state *restrict state) {
    if (compressed) {
        uint16_t chunk;
        sharc_hash_decode_read_compressed_chunk_fast(&chunk, in);
        sharc_hash_decode_compressed_chunk(&chunk, out, state);
    } else {
        uint32_t chunk;
        sharc_hash_decode_read_uncompressed_chunk_fast(&chunk, in);
        sharc_hash_decode_uncompressed_chunk(&chunk, out, state);
    }
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE sharc_hash_decode_kernel_safe(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state, const sharc_bool compressed) {
    SHARC_KERNEL_DECODE_STATE returnState;

    if (out->position + sizeof(uint32_t) > out->size)
        return SHARC_KERNEL_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

    if (compressed) {
        uint16_t chunk;
        if ((returnState = sharc_hash_decode_read_compressed_chunk_safe(&chunk, in)))
            return returnState;
        sharc_hash_decode_compressed_chunk(&chunk, out, state);
    } else {
        uint32_t chunk;
        if ((returnState = sharc_hash_decode_read_uncompressed_chunk_safe(&chunk, in, state)))
            return returnState;
        sharc_hash_decode_uncompressed_chunk(&chunk, out, state);
    }

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE const bool sharc_hash_decode_test_compressed(sharc_hash_decode_state *state) {
    return (sharc_bool const) ((state->signature >> state->shift) & 0x1);
}

SHARC_FORCE_INLINE void sharc_hash_decode_process_data_fast(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state) {
    while (state->shift ^ 64) {
        sharc_hash_decode_kernel_fast(in, out, sharc_hash_decode_test_compressed(state), state);
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

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE CHAMELEON_NAME(sharc_hash_decode_init)(sharc_hash_decode_state *state, const uint_fast32_t endDataOverhead) {
    state->signaturesCount = 0;
    state->efficiencyChecked = 0;
    CHAMELEON_NAME(sharc_dictionary_reset)(&state->dictionary);
    state->resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;

    state->endDataOverhead = endDataOverhead;

    state->signatureBytes = 0;
    state->uncompressedChunkBytes = 0;

    state->process = SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE CHAMELEON_NAME(sharc_hash_decode_process)(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_hash_decode_state *restrict state, const sharc_bool flush) {
    SHARC_KERNEL_DECODE_STATE returnState;
    uint_fast64_t remaining;
    uint_fast64_t limitIn = 0;
    uint_fast64_t limitOut = 0;

    if (in->size > SHARC_HASH_DECODE_MINIMUM_INPUT_LOOKAHEAD + state->endDataOverhead)
        limitIn = in->size - SHARC_HASH_DECODE_MINIMUM_INPUT_LOOKAHEAD - state->endDataOverhead;
    if (out->size > SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD)
        limitOut = out->size - SHARC_HASH_DECODE_MINIMUM_OUTPUT_LOOKAHEAD;

    switch (state->process) {
        case SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST:
            while (in->position < limitIn && out->position < limitOut) {
                if ((returnState = sharc_hash_decode_checkSignaturesCount(state)))
                    return returnState;

                sharc_hash_decode_read_signature_fast(in, state);
                sharc_hash_decode_process_data_fast(in, out, state);
            }
            state->process = SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURE_SAFE;
            break;

        case SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURE_SAFE:
            if (flush && (in->size - in->position < sizeof(sharc_hash_signature) + sizeof(uint16_t) + state->endDataOverhead)) {
                state->process = SHARC_CHAMELEON_DECODE_PROCESS_FINISH;
                return SHARC_KERNEL_DECODE_STATE_READY;
            }

            if ((returnState = sharc_hash_decode_checkSignaturesCount(state)))
                return returnState;

            if ((returnState = sharc_hash_decode_read_signature_safe(in, state)))
                return returnState;

            if (in->position < limitIn && out->position < limitOut) {
                state->process = SHARC_CHAMELEON_DECODE_PROCESS_DATA_FAST;
                return SHARC_KERNEL_DECODE_STATE_READY;
            }
            state->process = SHARC_CHAMELEON_DECODE_PROCESS_DATA_SAFE;
            break;

        case SHARC_CHAMELEON_DECODE_PROCESS_DATA_SAFE:
            while (state->shift ^ 64) {
                if (flush && (in->size - in->position < sizeof(uint16_t) + state->endDataOverhead + (sharc_hash_decode_test_compressed(state) ? 0 : 2))) {
                    state->process = SHARC_CHAMELEON_DECODE_PROCESS_FINISH;
                    return SHARC_KERNEL_DECODE_STATE_READY;
                }
                if ((returnState = sharc_hash_decode_kernel_safe(in, out, state, sharc_hash_decode_test_compressed(state))))
                    return returnState;
                state->shift++;
                if (in->position < limitIn && out->position < limitOut) {
                    state->process = SHARC_CHAMELEON_DECODE_PROCESS_DATA_FAST;
                    return SHARC_KERNEL_DECODE_STATE_READY;
                }
            }
            state->process = SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            break;

        case SHARC_CHAMELEON_DECODE_PROCESS_DATA_FAST:
            sharc_hash_decode_process_data_fast(in, out, state);
            state->process = SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            break;

        case SHARC_CHAMELEON_DECODE_PROCESS_FINISH:
            if (state->uncompressedChunkBytes) {
                if (sharc_hash_decode_attempt_copy(out, state->partialUncompressedChunk.as_bytes, (uint32_t) state->uncompressedChunkBytes))
                    return SHARC_KERNEL_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;
                state->uncompressedChunkBytes = 0;
            }
            remaining = in->size - in->position;
            if (remaining > state->endDataOverhead) {
                if (sharc_hash_decode_attempt_copy(out, in->pointer + in->position, (uint32_t) (remaining - state->endDataOverhead)))
                    return SHARC_KERNEL_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;
                in->position += (remaining - state->endDataOverhead);
            }
            state->process = SHARC_CHAMELEON_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST;
            return SHARC_KERNEL_DECODE_STATE_FINISHED;

        default:
            return SHARC_KERNEL_DECODE_STATE_ERROR;
    }

    return SHARC_KERNEL_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_DECODE_STATE CHAMELEON_NAME(sharc_hash_decode_finish)(sharc_hash_decode_state *state) {
    return SHARC_KERNEL_DECODE_STATE_READY;
}