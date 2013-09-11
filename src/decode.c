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

    if (!sharc_header_checkValidity(&state->header))
        return SHARC_DECODE_STATE_ERROR;

    if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT)
        sharc_hash_decode_set_end_data_size(&state->hashDecodeState, sizeof(sharc_block_footer) + sizeof(sharc_footer));
    else
        sharc_hash_decode_set_end_data_size(&state->hashDecodeState, sizeof(sharc_footer));

    state->process = SHARC_DECODE_PROCESS_READ_BLOCK_HEADER;
    state->currentCompressionMode = (SHARC_COMPRESSION_MODE) state->header.genericHeader.compressionMode;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_footer(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_footer) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_footer_read(in, &state->footer);

    state->process = SHARC_DECODE_PROCESS_FINISHED;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_block_header(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_block_header) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->currentBlockData.inStart = state->totalRead;
    state->currentBlockData.outStart = state->totalWritten;

    state->totalRead += sharc_block_header_read(in, &state->lastBlockHeader);

    state->process = SHARC_DECODE_PROCESS_READ_DATA;

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_block_footer(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_block_footer) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_block_footer_read(in, &state->lastBlockFooter);

    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_read_block_mode_marker(sharc_byte_buffer *restrict in, sharc_decode_state *restrict state) {
    if (in->position + sizeof(sharc_mode_marker) > in->size)
        return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

    state->totalRead += sharc_mode_marker_read(in, &state->lastModeMarker);

    state->process = SHARC_DECODE_PROCESS_READ_DATA;

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

    sharc_hash_decode_init(&state->hashDecodeState);
    state->dictionaryData.resetCycle = 0;

    state->workBuffer = workBuffer;

    if((decodeState = sharc_decode_read_header(in, state)))
        return decodeState;

    state->endDataOverhead = (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT ? sizeof(sharc_block_footer) : 0) + sizeof(sharc_footer);
    return SHARC_DECODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_DECODE_STATE sharc_decode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_decode_state *restrict state, const sharc_bool flush) {
    SHARC_DECODE_STATE decodeState;
    SHARC_HASH_DECODE_STATE hashDecodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;
    uint_fast64_t blockRemaining;
    uint_fast64_t inRemaining;
    uint_fast64_t positionIncrement;
    uint_fast64_t outRemaining;

    while (true) {
        switch (state->process) {
            case SHARC_DECODE_PROCESS_READ_BLOCK_HEADER:
                if ((decodeState = sharc_decode_read_block_header(in, state)))
                    return decodeState;

                if (state->dictionaryData.resetCycle)
                    state->dictionaryData.resetCycle--;
                else {
                    switch (state->currentCompressionMode) {
                        case SHARC_COMPRESSION_MODE_FASTEST:
                            sharc_dictionary_resetDirect(&state->dictionaryData.dictionary_a);
                            break;
                        default:
                            break;
                    }
                    state->dictionaryData.resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
                }
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCK_MODE_MARKER:
                if ((decodeState = sharc_decode_read_block_mode_marker(in, state)))
                    return decodeState;
                break;

            case SHARC_DECODE_PROCESS_READ_BLOCK_FOOTER:
                if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT) if ((decodeState = sharc_decode_read_block_footer(in, state)))
                    return decodeState;
                state->process = SHARC_DECODE_PROCESS_READ_BLOCK_HEADER;
                break;

            case SHARC_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER:
                if (state->header.genericHeader.blockType == SHARC_BLOCK_TYPE_DEFAULT) if ((decodeState = sharc_decode_read_block_footer(in, state)))
                    return decodeState;
                state->process = SHARC_DECODE_PROCESS_READ_FOOTER;
                return SHARC_DECODE_STATE_READY;

            case SHARC_DECODE_PROCESS_READ_DATA:
                inPositionBefore = in->position;
                outPositionBefore = out->position;

                switch (state->currentCompressionMode) {
                    case SHARC_COMPRESSION_MODE_COPY:
                        blockRemaining = SHARC_PREFERRED_BLOCK_SIGNATURES * sizeof(uint32_t) * 8 * sizeof(sharc_hash_signature) - (state->totalWritten - state->currentBlockData.outStart);
                        inRemaining = in->size - in->position;
                        outRemaining = out->size - out->position;

                        if (inRemaining <= outRemaining) {
                            if (blockRemaining <= inRemaining)
                                goto copy_until_end_of_block;
                            else {
                                if (flush && inRemaining <= state->endDataOverhead) {
                                    state->process = SHARC_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER;
                                } else if (!inRemaining) {
                                    return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;
                                } else {
                                    positionIncrement = inRemaining - (flush ? state->endDataOverhead : 0);
                                    memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) positionIncrement);
                                    in->position += positionIncrement;
                                    out->position += positionIncrement;
                                    sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                                }
                            }
                        } else {
                            if (blockRemaining <= outRemaining)
                                goto copy_until_end_of_block;
                            else {
                                if (outRemaining) {
                                    memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) outRemaining);
                                    in->position += outRemaining;
                                    out->position += outRemaining;
                                    sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                                } else
                                    return SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;
                            }
                        }
                        goto exit;

                    copy_until_end_of_block:
                        memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) blockRemaining);
                        in->position += blockRemaining;
                        out->position += blockRemaining;
                        sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                        state->process = SHARC_DECODE_PROCESS_READ_BLOCK_FOOTER;

                    exit:
                        break;

                    case SHARC_COMPRESSION_MODE_FASTEST:
                        hashDecodeState = sharc_hash_decode_process(in, out, SHARC_HASH_XOR_MASK_DISPERSION, &state->dictionaryData.dictionary_a, &state->hashDecodeState, flush);
                        sharc_decode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                        switch (hashDecodeState) {
                            case SHARC_HASH_DECODE_STATE_STALL_ON_INPUT_BUFFER:
                                return SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER;

                            case SHARC_HASH_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER;

                            case SHARC_HASH_DECODE_STATE_INFO_NEW_BLOCK:
                                state->process = SHARC_DECODE_PROCESS_READ_BLOCK_FOOTER;
                                break;

                            case SHARC_HASH_DECODE_STATE_INFO_EFFICIENCY_CHECK:
                                state->process = SHARC_DECODE_PROCESS_READ_BLOCK_MODE_MARKER;
                                break;

                            case SHARC_HASH_DECODE_STATE_FINISHED:
                                state->process = SHARC_DECODE_PROCESS_READ_LAST_BLOCK_FOOTER;
                                break;

                            case SHARC_HASH_DECODE_STATE_READY:
                                break;

                            default:
                                return SHARC_DECODE_STATE_ERROR;
                        }
                        break;

                    case SHARC_COMPRESSION_MODE_DUAL_PASS:
                        break;
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

    sharc_hash_decode_finish(&state->hashDecodeState);

    return sharc_decode_read_footer(in, state);
}