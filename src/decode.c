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

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_init(sharc_byte_buffer *in, sharc_byte_buffer *workBuffer, sharc_decode_state *restrict state) {
    SHARC_DECODE_STATE decodeState;

    state->totalRead = 0;
    state->totalWritten = 0;

    if((decodeState = sharc_decode_read_header(in, state)))
        return decodeState;

    sharc_block_decode_init(&state->blockDecodeState, state->header.genericHeader.compressionMode ? SHARC_BLOCK_MODE_HASH : SHARC_BLOCK_MODE_COPY, (SHARC_BLOCK_TYPE)state->header.genericHeader.blockType, sizeof(sharc_footer));

    state->workBuffer = workBuffer;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const sharc_bool flush) {
    SHARC_BLOCK_DECODE_STATE blockDecodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;

    while (true) {
        switch (state->process) {
            case SHARC_DECODE_PROCESS_READ_BLOCKS:
                inPositionBefore = in->position;
                outPositionBefore = out->position;

                switch (state->header.genericHeader.compressionMode) {
                    case SHARC_COMPRESSION_MODE_COPY:
                    case SHARC_COMPRESSION_MODE_FASTEST:
                        blockDecodeState = sharc_block_decode_process(in, out, &state->blockDecodeState, flush, SHARC_HASH_XOR_MASK_DISPERSION);
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
                        break;

                    default:
                        return SHARC_DECODE_STATE_ERROR;
                }
                break;

            default:
                return SHARC_DECODE_STATE_ERROR;
        }
    }
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_finish(sharc_byte_buffer *in, sharc_decode_state *restrict state) {
    if (state->process ^ SHARC_DECODE_PROCESS_READ_FOOTER)
        return SHARC_DECODE_STATE_ERROR;

    sharc_block_decode_finish(&state->blockDecodeState);

    return sharc_decode_read_footer(in, state);
}