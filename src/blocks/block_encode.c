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
 * 29/08/13 12:55
 */

#include "block_encode.h"

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_write_block_header(sharc_byte_buffer *restrict out, sharc_block_encode_state *restrict state) {
    if (out->position + sizeof(sharc_block_header) > out->size)
        return SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->currentMode = state->targetMode;

    /*if (state->dictionaryData.resetCycle)
        state->dictionaryData.resetCycle--;
    else {
        switch (state->targetMode) {
            case SHARC_BLOCK_MODE_HASH:
                //state->dictionaryData.dictionary_reset(&state->dictionaryData.dictionary);
                break;

            case SHARC_BLOCK_MODE_COPY:
                break;
        }
        state->dictionaryData.resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
    }*/

    state->currentBlockData.inStart = state->totalRead;
    state->currentBlockData.outStart = state->totalWritten;

    state->totalWritten += sharc_block_header_write(out);

    state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_DATA;

    return SHARC_BLOCK_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_write_block_footer(sharc_byte_buffer *restrict out, sharc_block_encode_state *restrict state) {
    if (out->position + sizeof(sharc_block_footer) > out->size)
        return SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->totalWritten += sharc_block_footer_write(out, 0);

    return SHARC_BLOCK_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_write_mode_marker(sharc_byte_buffer *restrict out, sharc_block_encode_state *restrict state) {
    if (out->position + sizeof(sharc_mode_marker) > out->size)
        return SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->totalWritten += sharc_block_mode_marker_write(out, state->targetMode);

    state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_DATA;

    return SHARC_BLOCK_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_block_encode_update_totals(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_block_encode_state *restrict state, const uint_fast64_t inPositionBefore, const uint_fast64_t outPositionBefore) {
    state->totalRead += in->position - inPositionBefore;
    state->totalWritten += out->position - outPositionBefore;
}

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_init(sharc_block_encode_state *restrict state, const SHARC_BLOCK_MODE mode, const SHARC_BLOCK_TYPE blockType, void* kernelState, SHARC_KERNEL_ENCODE_STATE (*kernelInit)(void*), SHARC_KERNEL_ENCODE_STATE (*kernelProcess)(sharc_byte_buffer *, sharc_byte_buffer *, void*, const sharc_bool), SHARC_KERNEL_ENCODE_STATE (*kernelFinish)(void*)) {
    state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
    state->targetMode = mode;
    state->currentMode = mode;
    state->blockType = blockType;

    state->totalRead = 0;
    state->totalWritten = 0;

    //sharc_argonaut_encode_init(&state->hashEncodeState, &state->dictionaryData.dictionary);
    state->kernelEncodeState = kernelState;
    state->kernelEncodeInit = kernelInit;
    state->kernelEncodeProcess = kernelProcess;
    state->kernelEncodeFinish = kernelFinish;

    state->kernelEncodeInit(state->kernelEncodeState);

    //state->dictionaryData.resetCycle = 0;
    //state->dictionaryData.dictionary_reset = dictionary_reset;

    return SHARC_BLOCK_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_block_encode_state *restrict state, const sharc_bool flush) {
    SHARC_BLOCK_ENCODE_STATE encodeState;
    SHARC_KERNEL_ENCODE_STATE hashEncodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;
    /*uint_fast64_t blockRemaining;
    uint_fast64_t inRemaining;
    uint_fast64_t outRemaining;*/

    while (true) {
        switch (state->process) {
            case SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER:
                if ((encodeState = sharc_block_encode_write_block_header(out, state)))
                    return encodeState;
                break;

            case SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER:
                if (state->blockType == SHARC_BLOCK_TYPE_DEFAULT) if ((encodeState = sharc_block_encode_write_block_footer(out, state)))
                    return encodeState;
                state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
                break;

            case SHARC_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER:
                if (state->blockType == SHARC_BLOCK_TYPE_DEFAULT) if ((encodeState = sharc_block_encode_write_block_footer(out, state)))
                    return encodeState;
                state->process = SHARC_BLOCK_ENCODE_PROCESS_FINISHED;
                return SHARC_BLOCK_ENCODE_STATE_READY;

            case SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_MODE_MARKER:
                if ((encodeState = sharc_block_encode_write_mode_marker(out, state)))
                    return encodeState;
                break;

            case SHARC_BLOCK_ENCODE_PROCESS_WRITE_DATA:
                inPositionBefore = in->position;
                outPositionBefore = out->position;

                switch (state->currentMode) {
                    /*case SHARC_BLOCK_MODE_COPY:
                        blockRemaining = SHARC_PREFERRED_BLOCK_SIGNATURES * sizeof(uint32_t) * 8 * sizeof(sharc_hash_signature) - (state->totalRead - state->currentBlockData.inStart);
                        inRemaining = in->size - in->position;
                        outRemaining = out->size - out->position;

                        if (inRemaining <= outRemaining) {
                            if (blockRemaining <= inRemaining)
                                goto copy_until_end_of_block;
                            else {
                                memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) inRemaining);
                                in->position += inRemaining;
                                out->position += inRemaining;
                                sharc_block_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                                if (flush)
                                    state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                                else
                                    return SHARC_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
                            }
                        } else {
                            if (blockRemaining <= outRemaining)
                                goto copy_until_end_of_block;
                            else {
                                memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) outRemaining);
                                in->position += outRemaining;
                                out->position += outRemaining;
                                sharc_block_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                                return SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                            }
                        }
                        goto exit;

                    copy_until_end_of_block:
                        memcpy(out->pointer + out->position, in->pointer + in->position, (size_t) blockRemaining);
                        in->position += blockRemaining;
                        out->position += blockRemaining;
                        sharc_block_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);
                        if (flush && inRemaining == blockRemaining)
                            state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                        else
                            state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER;

                    exit:
                        break;*/

                    case SHARC_BLOCK_MODE_HASH:
                        hashEncodeState = state->kernelEncodeProcess(in, out, state->kernelEncodeState, flush);//sharc_argonaut_encode_process(in, out, xorMask, &state->dictionaryData.dictionary, &state->hashEncodeState, flush); // (false) = c3, (true) = c4
                        sharc_block_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                        switch (hashEncodeState) {
                            case SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                                return SHARC_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER;

                            case SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return SHARC_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

                            case SHARC_KERNEL_ENCODE_STATE_INFO_NEW_BLOCK:
                                state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER;
                                break;

                            case SHARC_KERNEL_ENCODE_STATE_INFO_EFFICIENCY_CHECK:
                                state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_MODE_MARKER;
                                break;

                            case SHARC_KERNEL_ENCODE_STATE_FINISHED:
                                state->process = SHARC_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                                break;

                            case SHARC_KERNEL_ENCODE_STATE_READY:
                                break;

                            default:
                                return SHARC_BLOCK_ENCODE_STATE_ERROR;
                        }
                        break;

                    default:
                        return SHARC_BLOCK_ENCODE_STATE_ERROR;
                }
                break;

            default:
                return SHARC_BLOCK_ENCODE_STATE_ERROR;
        }
    }
}

SHARC_FORCE_INLINE SHARC_BLOCK_ENCODE_STATE sharc_block_encode_finish(sharc_block_encode_state *restrict state) {
    if (state->process ^ SHARC_BLOCK_ENCODE_PROCESS_FINISHED)
        return SHARC_BLOCK_ENCODE_STATE_ERROR;

    //sharc_argonaut_encode_finish(&state->hashEncodeState);
    state->kernelEncodeFinish(state->kernelEncodeState);

    return SHARC_BLOCK_ENCODE_STATE_READY;
}