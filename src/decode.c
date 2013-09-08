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
 * 31/08/13 13:46
 */

#include "decode.h"

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_header(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_header) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_header_read(in, &state->header);

    if(!sharc_header_checkValidity(&state->header))
        return SHARC_DECODE_STATE_ERROR;

    if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT)
        sharc_hash_decode_set_end_data_size(&state->hashDecodeState, sizeof(sharc_block_footer));
    else
        sharc_hash_decode_set_end_data_size(&state->hashDecodeState, 0);

    state->process = SHARC_DECODE_PROCESS_READ_BLOCK_HEADER;
    state->currentCompressionMode = (SHARC_COMPRESSION_MODE) state->header.genericHeader.compressionMode;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_block_header(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_block_header) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_block_header_read(in, &state->lastBlockHeader);

    state->process = SHARC_DECODE_PROCESS_WRITE_DATA;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_block_footer(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_block_footer) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_block_footer_read(in, &state->lastBlockFooter);

    state->process = SHARC_DECODE_PROCESS_READ_BLOCK_HEADER;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_mode_marker(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_mode_marker) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_mode_marker_read(in, &state->lastModeMarker);

    state->process = SHARC_DECODE_PROCESS_WRITE_DATA;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_decode_update_totals(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const uint_fast64_t inPositionBefore, const uint_fast64_t outPositionBefore) {
    state->totalRead += in->position - inPositionBefore;
    state->totalWritten += out->position - outPositionBefore;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_init(sharc_decode_state *restrict state) {
    state->process = SHARC_DECODE_PROCESS_READ_HEADER;

    state->totalRead = 0;
    state->totalWritten = 0;

    sharc_hash_decode_init(&state->hashDecodeState);

    state->dictionaryData.resetCycle = 0;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const sharc_bool flush) {
    SHARC_DECODE_STATE decodeState;
    SHARC_HASH_DECODE_STATE hashDecodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;

    while (true) {
        switch (state->process) {
            case SHARC_DECODE_PROCESS_READ_HEADER:
                if ((decodeState = sharc_decode_read_header(in, state)))
                    return decodeState;
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCK_HEADER:
                if ((decodeState = sharc_decode_read_block_header(in, state)))
                    return decodeState;

                if (state->dictionaryData.resetCycle)
                    state->dictionaryData.resetCycle--;
                else {
                    switch (state->currentCompressionMode) {
                        default:
                            sharc_dictionary_resetDirect(&state->dictionaryData.dictionary_a);
                            break;
                    }
                    state->dictionaryData.resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
                }
                break;

            case SHARC_DECODE_PROCESS_READ_MODE_MARKER:
                if ((decodeState = sharc_decode_read_mode_marker(in, state)))
                    return decodeState;
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCK_FOOTER:
                if ((decodeState = sharc_decode_read_block_footer(in, state)))
                    return decodeState;
                break;

            case SHARC_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER:
                if ((decodeState = sharc_decode_read_block_footer(in, state)))
                    return decodeState;
                return SHARC_DECODE_STATE_FINISHED;

            case SHARC_DECODE_PROCESS_WRITE_DATA:
                inPositionBefore = in->position;
                outPositionBefore = out->position;

                hashDecodeState = sharc_hash_decode_process(in, out, SHARC_HASH_XOR_MASK_DISPERSION, &state->dictionaryData.dictionary_a, &state->hashDecodeState, flush);
                sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                switch (hashDecodeState) {
                    case SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER:
                        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

                    case SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        return SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

                    case SHARC_HASH_DECODE_STATE_INFO_NEW_BLOCK:
                        if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT)
                            state->process = SHARC_DECODE_PROCESS_READ_BLOCK_FOOTER;
                        else
                            state->process = SHARC_DECODE_PROCESS_READ_BLOCK_HEADER;
                        break;

                    case SHARC_HASH_DECODE_STATE_INFO_EFFICIENCY_CHECK:
                        state->process = SHARC_DECODE_PROCESS_READ_MODE_MARKER;
                        break;

                    case SHARC_HASH_DECODE_STATE_FINISHED:
                        if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT)
                            state->process = SHARC_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER;
                        else
                            return SHARC_DECODE_STATE_FINISHED;
                        break;

                    case SHARC_HASH_DECODE_STATE_READY:
                        break;

                    default:
                        return SHARC_DECODE_STATE_ERROR;
                }
                break;
        }
    }
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_finish(sharc_decode_state *restrict state) {
    if (sharc_hash_decode_finish(&state->hashDecodeState))
        return SHARC_DECODE_STATE_ERROR;

    return SHARC_DECODE_STATE_READY;
}