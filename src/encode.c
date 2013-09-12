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
 * 12/09/13 09:56
 */

#include "encode.h"

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_write_header(sharc_byte_buffer *restrict out, sharc_encode_state *restrict state) {
    if (out->position + sizeof(sharc_header) > out->size)
        return SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    if (!state->fileAttributes)
        state->totalWritten += sharc_header_write(out, SHARC_HEADER_ORIGIN_TYPE_STREAM, state->compressionMode, state->blockType, NULL);
    else
        state->totalWritten += sharc_header_write(out, SHARC_HEADER_ORIGIN_TYPE_FILE, state->compressionMode, state->blockType, state->fileAttributes);

    state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCKS;

    return SHARC_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_write_footer(sharc_byte_buffer *restrict out, sharc_encode_state *restrict state) {
    if (out->position + sizeof(sharc_footer) > out->size)
        return SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->totalWritten += sharc_footer_write(out);

    state->process = SHARC_ENCODE_PROCESS_FINISHED;

    return SHARC_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_encode_update_totals(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_encode_state *restrict state, const uint_fast64_t inPositionBefore, const uint_fast64_t outPositionBefore) {
    state->totalRead += in->position - inPositionBefore;
    state->totalWritten += out->position - outPositionBefore;
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_init(sharc_byte_buffer *restrict out, sharc_byte_buffer *restrict workBuffer, const uint_fast64_t workBufferSize, sharc_encode_state *restrict state, const SHARC_COMPRESSION_MODE mode, const SHARC_ENCODE_OUTPUT_TYPE encodeType, const SHARC_BLOCK_TYPE blockType, const struct stat *restrict fileAttributes) {
    state->compressionMode = mode;
    state->blockType = blockType;
    state->fileAttributes = fileAttributes;

    state->totalRead = 0;
    state->totalWritten = 0;

    switch(mode) {
        case SHARC_COMPRESSION_MODE_COPY:
            sharc_block_encode_init(&state->blockEncodeStateA, SHARC_BLOCK_MODE_COPY, blockType, NULL);
            break;

        case SHARC_COMPRESSION_MODE_FASTEST:
            sharc_block_encode_init(&state->blockEncodeStateA, mode ? SHARC_BLOCK_MODE_HASH : SHARC_BLOCK_MODE_COPY, blockType, sharc_dictionary_resetDirect);
            break;

        case SHARC_COMPRESSION_MODE_DUAL_PASS:
            sharc_block_encode_init(&state->blockEncodeStateA, SHARC_BLOCK_MODE_HASH, SHARC_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK, sharc_dictionary_resetDirect);
            sharc_block_encode_init(&state->blockEncodeStateB, SHARC_BLOCK_MODE_HASH, blockType, sharc_dictionary_resetCompressed);
            break;
    }

    state->workBuffer = workBuffer;
    state->workBufferData.memorySize = workBufferSize;

    switch (encodeType) {
        case SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER:
        case SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER_NOR_FOOTER:
            state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCKS;
            return SHARC_ENCODE_STATE_READY;
        default:
            return sharc_encode_write_header(out, state);
    }
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_encode_state *restrict state, const sharc_bool flush) {
    SHARC_BLOCK_ENCODE_STATE blockEncodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;

    while (true) {
        inPositionBefore = in->position;
        outPositionBefore = out->position;

        switch (state->process) {
            case SHARC_ENCODE_PROCESS_WRITE_BLOCKS:
                switch (state->compressionMode) {
                    case SHARC_COMPRESSION_MODE_COPY:
                    case SHARC_COMPRESSION_MODE_FASTEST:
                        blockEncodeState = sharc_block_encode_process(in, out, &state->blockEncodeStateA, flush, SHARC_HASH_XOR_MASK_DISPERSION);
                        sharc_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                        switch (blockEncodeState) {
                            case SHARC_BLOCK_ENCODE_STATE_READY:
                                state->process = SHARC_ENCODE_PROCESS_WRITE_FOOTER;
                                return SHARC_ENCODE_STATE_READY;

                            case SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

                            case SHARC_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                                return SHARC_ENCODE_STATE_STALL_ON_INPUT_BUFFER;

                            case SHARC_BLOCK_ENCODE_STATE_ERROR:
                                return SHARC_ENCODE_STATE_ERROR;
                        }
                        break;

                    case SHARC_COMPRESSION_MODE_DUAL_PASS:
                        state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCKS_IN_TO_WORKBUFFER;
                        break;
                }
                break;

            case SHARC_ENCODE_PROCESS_WRITE_BLOCKS_IN_TO_WORKBUFFER:
                state->workBuffer->size = state->workBufferData.memorySize;
                blockEncodeState = sharc_block_encode_process(in, state->workBuffer, &state->blockEncodeStateA, flush, SHARC_HASH_XOR_MASK_DISPERSION);
                state->totalRead += in->position - inPositionBefore;
                switch (blockEncodeState) {
                    case SHARC_BLOCK_ENCODE_STATE_READY:
                    case SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        break;

                    case SHARC_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_ON_INPUT_BUFFER;

                    default:
                        return SHARC_ENCODE_STATE_ERROR;
                }

                state->workBufferData.outstandingBytes = state->workBuffer->position & 0x1F;
                state->workBuffer->size = flush ? state->workBuffer->position : state->workBuffer->position & ~0x1F;
                sharc_byte_buffer_rewind(state->workBuffer);

                state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCKS_WORKBUFFER_TO_OUT;
                break;

            case SHARC_ENCODE_PROCESS_WRITE_BLOCKS_WORKBUFFER_TO_OUT:
                blockEncodeState = sharc_block_encode_process(state->workBuffer, out, &state->blockEncodeStateB, flush && in->position == in->size, SHARC_HASH_XOR_MASK_DIRECT);
                state->totalWritten += out->position - outPositionBefore;
                switch (blockEncodeState) {
                    case SHARC_BLOCK_ENCODE_STATE_READY:
                        state->process = SHARC_ENCODE_PROCESS_WRITE_FOOTER;
                        return SHARC_ENCODE_STATE_READY;

                    case SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

                    case SHARC_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                        break;

                    default:
                        return SHARC_ENCODE_STATE_ERROR;
                }

                if (state->workBuffer->size & ~0x1F)
                    memcpy(state->workBuffer->pointer, state->workBuffer->pointer + (state->workBuffer->size & ~0x1F), (size_t) state->workBufferData.outstandingBytes);
                state->workBuffer->position = state->workBufferData.outstandingBytes;

                state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCKS_IN_TO_WORKBUFFER;
                break;

            default:
                return SHARC_ENCODE_STATE_ERROR;
        }
    }
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_finish(sharc_byte_buffer *restrict out, sharc_encode_state *restrict state) {
    if (state->process ^ SHARC_ENCODE_PROCESS_WRITE_FOOTER)
        return SHARC_ENCODE_STATE_ERROR;

    sharc_block_encode_finish(&state->blockEncodeStateA);

    return sharc_encode_write_footer(out, state);
}