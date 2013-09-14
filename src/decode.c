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
 * 12/09/13 11:05
 */

#include "decode.h"

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_header(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_header) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_header_read(in, &state->header);

    if (!sharc_header_checkValidity(&state->header))
        return SHARC_DECODE_STATE_ERROR;

    state->process = SHARC_DECODE_PROCESS_READ_BLOCKS;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_footer(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_footer) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_footer_read(in, &state->footer);

    state->process = SHARC_DECODE_PROCESS_FINISHED;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_decode_update_totals(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const uint_fast64_t inPositionBefore, const uint_fast64_t outPositionBefore) {
    state->totalRead += in->position - inPositionBefore;
    state->totalWritten += out->position - outPositionBefore;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_init(sharc_byte_buffer *in, sharc_byte_buffer *workBuffer, const uint_fast64_t workBufferSize, sharc_decode_state *restrict state) {
    SHARC_DECODE_STATE decodeState;

    state->totalRead = 0;
    state->totalWritten = 0;

    if ((decodeState = sharc_decode_read_header(in, state)))
        return decodeState;

    switch (state->header.genericHeader.compressionMode) {
        case SHARC_COMPRESSION_MODE_COPY:
            sharc_block_decode_init(&state->blockDecodeStateA, SHARC_BLOCK_MODE_COPY, (SHARC_BLOCK_TYPE) state->header.genericHeader.blockType, sizeof(sharc_footer), NULL);
            break;

        case SHARC_COMPRESSION_MODE_FASTEST:
            sharc_block_decode_init(&state->blockDecodeStateA, SHARC_BLOCK_MODE_HASH, (SHARC_BLOCK_TYPE) state->header.genericHeader.blockType, sizeof(sharc_footer), sharc_dictionary_resetDirect);
            break;

        case SHARC_COMPRESSION_MODE_DUAL_PASS:
            sharc_block_decode_init(&state->blockDecodeStateA, SHARC_BLOCK_MODE_HASH, (SHARC_BLOCK_TYPE) state->header.genericHeader.blockType, sizeof(sharc_footer), sharc_dictionary_resetCompressed);
            sharc_block_decode_init(&state->blockDecodeStateB, SHARC_BLOCK_MODE_HASH, SHARC_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK, 0, sharc_dictionary_resetDirect);
            break;

        default:
            return SHARC_DECODE_STATE_ERROR;
    }

    state->workBuffer = workBuffer;
    state->workBufferData.memorySize = workBufferSize;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const sharc_bool flush) {
    SHARC_BLOCK_DECODE_STATE blockDecodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;

    while (true) {
        inPositionBefore = in->position;
        outPositionBefore = out->position;

        switch (state->process) {
            case SHARC_DECODE_PROCESS_READ_BLOCKS:
                switch (state->header.genericHeader.compressionMode) {
                    case SHARC_COMPRESSION_MODE_COPY:
                    case SHARC_COMPRESSION_MODE_FASTEST:
                        blockDecodeState = sharc_block_decode_process(in, out, &state->blockDecodeStateA, flush, SHARC_HASH_XOR_MASK_DISPERSION);
                        sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                        switch (blockDecodeState) {
                            case SHARC_BLOCK_DECODE_STATE_READY:
                                state->process = SHARC_DECODE_PROCESS_READ_FOOTER;
                                return SHARC_DECODE_STATE_READY;

                            case SHARC_BLOCK_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

                            case SHARC_BLOCK_DECODE_STATE_STALL_ON_INPUT_BUFFER:
                                return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

                            case SHARC_BLOCK_DECODE_STATE_ERROR:
                                return SHARC_DECODE_STATE_ERROR;
                        }
                        break;

                    case SHARC_COMPRESSION_MODE_DUAL_PASS:
                        state->process = SHARC_DECODE_PROCESS_READ_BLOCKS_IN_TO_WORKBUFFER;
                        break;

                    default:
                        return SHARC_DECODE_STATE_ERROR;
                }
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCKS_IN_TO_WORKBUFFER:
                state->workBuffer->size = state->workBufferData.memorySize;
                blockDecodeState = sharc_block_decode_process(in, state->workBuffer, &state->blockDecodeStateA, flush, SHARC_HASH_XOR_MASK_DIRECT);
                state->totalRead += in->position - inPositionBefore;
                switch (blockDecodeState) {
                    case SHARC_BLOCK_DECODE_STATE_READY:
                    case SHARC_BLOCK_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        break;

                    case SHARC_BLOCK_DECODE_STATE_STALL_ON_INPUT_BUFFER:
                        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

                    default:
                        return SHARC_DECODE_STATE_ERROR;
                }

                state->workBuffer->size = state->workBuffer->position;
                sharc_byte_buffer_rewind(state->workBuffer);

                state->process = SHARC_DECODE_PROCESS_READ_BLOCKS_WORKBUFFER_TO_OUT;
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCKS_WORKBUFFER_TO_OUT:
                blockDecodeState = sharc_block_decode_process(state->workBuffer, out, &state->blockDecodeStateB, flush && in->position == in->size, SHARC_HASH_XOR_MASK_DISPERSION);
                state->totalWritten += out->position - outPositionBefore;
                switch (blockDecodeState) {
                    case SHARC_BLOCK_DECODE_STATE_READY:
                        state->process = SHARC_DECODE_PROCESS_READ_FOOTER;
                        return SHARC_DECODE_STATE_READY;

                    case SHARC_BLOCK_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        return SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

                    case SHARC_BLOCK_DECODE_STATE_STALL_ON_INPUT_BUFFER:
                        break;

                    default:
                        return SHARC_DECODE_STATE_ERROR;
                }
                sharc_byte_buffer_rewind(state->workBuffer);

                state->process = SHARC_DECODE_PROCESS_READ_BLOCKS_IN_TO_WORKBUFFER;
                break;

            default:
                return SHARC_DECODE_STATE_ERROR;
        }
    }
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_finish(sharc_byte_buffer *in, sharc_decode_state *restrict state) {
    if (state->process ^ SHARC_DECODE_PROCESS_READ_FOOTER)
        return SHARC_DECODE_STATE_ERROR;

    sharc_block_decode_finish(&state->blockDecodeStateA);
    if (state->header.genericHeader.compressionMode == SHARC_COMPRESSION_MODE_DUAL_PASS)
        sharc_block_decode_finish(&state->blockDecodeStateB);

    return sharc_decode_read_footer(in, state);
}