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

#include "encode.h"

SHARC_ENCODE_STATE sharc_encode_initialize(sharc_byte_buffer * restrict out, sharc_encode_state * restrict state, SHARC_COMPRESSION_MODE mode, SHARC_ENCODE_TYPE type) {
    state->dictionaryData.resetCycle = 0;

    switch(type) {
        case SHARC_ENCODE_TYPE_WITH_HEADER:
            state->process = SHARC_ENCODE_PROCESS_HEADER;
            break;
        case SHARC_ENCODE_TYPE_WITHOUT_HEADER:
            state->process = SHARC_ENCODE_PROCESS_BLOCK_HEADER;
            break;
    }
    state->mode = mode;

    state->totalRead = 0;
    state->totalWritten = 0;

    state->dictionaryData.resetCycle = 0;

    // todo workbuffer ?

    state->totalWritten += out->position;

    return SHARC_ENCODE_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_kernel(sharc_byte_buffer* restrict in, sharc_byte_buffer* restrict out, sharc_encode_state* restrict state) {
    SHARC_HASH_ENCODE_STATE returnState;

    while (SHARC_TRUE) {
        switch (state->process) {
            case SHARC_ENCODE_PROCESS_HEADER:
                if (out->position + sizeof(sharc_header) > out->size)
                    return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

                state->process = SHARC_ENCODE_PROCESS_BLOCK_HEADER;
                break;

            case SHARC_ENCODE_PROCESS_BLOCK_HEADER:
                if (out->position + sizeof(sharc_block_header) > out->size)
                    return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

                // todo write header
                //out->position ++;
                //printf("HEADER\n");
                //printf("%lu\n", sizeof(sharc_dictionary));

                state->totalWritten += out->position;

                if(state->dictionaryData.resetCycle)
                    state->dictionaryData.resetCycle--;
                else {
                    switch (state->mode) {
                        case SHARC_COMPRESSION_MODE_DUAL_PASS_NO_REVERSION:
                        case SHARC_COMPRESSION_MODE_DUAL_PASS_WITH_REVERSION:
                            sharc_dictionary_resetCompressed(&state->dictionaryData.dictionary_b);

                        case SHARC_COMPRESSION_MODE_FASTEST_NO_REVERSION:
                        case SHARC_COMPRESSION_MODE_FASTEST_WITH_REVERSION:
                            sharc_dictionary_resetDirect(&state->dictionaryData.dictionary_a);
                            break;

                        case SHARC_COMPRESSION_MODE_NO_COMPRESSION:
                            break;
                    }
                    state->dictionaryData.resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
                }

                state->blockData.inStart = state->totalRead;
                state->blockData.outStart = state->totalWritten;
                state->hashEncodeState.signaturesCount = 0;

                // todo write block header ?

                state->process = SHARC_ENCODE_PROCESS_INPUT_DATA;
                break;

            case SHARC_ENCODE_PROCESS_INPUT_DATA:
                returnState = sharc_hash_encode_kernel(in, out, in->size, SHARC_HASH_XOR_MASK_DISPERSION, &state->dictionaryData.dictionary_a, &state->hashEncodeState);

                state->totalRead += in->position;
                state->totalWritten += out->position;

                switch (returnState) {
                    case SHARC_HASH_ENCODE_STATE_STALL_INPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_INPUT_BUFFER;

                    case SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

                    case SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK:
                        state->process = SHARC_ENCODE_PROCESS_BLOCK_HEADER;
                        break;

                    default:
                        return SHARC_ENCODE_STATE_ERROR;
                }
        }
    }
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_finish(sharc_byte_buffer* restrict in, sharc_byte_buffer* restrict out, sharc_encode_state* restrict state) {
    SHARC_HASH_ENCODE_STATE returnState;

    while (SHARC_TRUE) {
        returnState = sharc_hash_encode_finish(in, out, SHARC_HASH_XOR_MASK_DISPERSION, &state->dictionaryData.dictionary_a, &state->hashEncodeState);

        state->totalRead += in->position;
        state->totalWritten += out->position;

        switch (returnState) {
            case SHARC_HASH_ENCODE_STATE_STALL_OUTPUT_BUFFER:
                return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

            case SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK:
                state->process = SHARC_ENCODE_PROCESS_BLOCK_HEADER;
                break;

            case SHARC_HASH_ENCODE_STATE_OK:
                return SHARC_ENCODE_STATE_OK;

            default:
                return SHARC_ENCODE_STATE_ERROR;
        }
    }
}