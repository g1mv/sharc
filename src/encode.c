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

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_write_header(sharc_byte_buffer *restrict out, sharc_encode_state *restrict state) {
    if (out->position + sizeof(sharc_header) > out->size)
        return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

    if (!state->fileAttributes)
        state->totalWritten += sharc_header_write(out, SHARC_HEADER_ORIGIN_TYPE_STREAM, state->mode, NULL);
    else
        state->totalWritten += sharc_header_write(out, SHARC_HEADER_ORIGIN_TYPE_FILE, state->mode, state->fileAttributes);

    state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER;

    return SHARC_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_write_block_header(sharc_byte_buffer *restrict out, sharc_encode_state *restrict state) {
    if (out->position + sizeof(sharc_block_header) > out->size)
        return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

    sharc_byte dictionaryFlags = 0;
    if (state->dictionaryData.resetCycle)
        state->dictionaryData.resetCycle--;
    else {
        switch (state->mode) {
            case SHARC_COMPRESSION_MODE_DUAL_PASS:
                sharc_dictionary_resetCompressed(&state->dictionaryData.dictionary_b);
                dictionaryFlags |= SHARC_BLOCK_HEADER_COMPRESSED_DICTIONARY_RESET_MASK;

            case SHARC_COMPRESSION_MODE_FASTEST:
                sharc_dictionary_resetDirect(&state->dictionaryData.dictionary_a);
                dictionaryFlags |= SHARC_BLOCK_HEADER_DIRECT_DICTIONARY_RESET_MASK;
                break;

            case SHARC_COMPRESSION_MODE_COPY:
                break;
        }
        state->dictionaryData.resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
    }

    state->blockData.inStart = state->totalRead;
    state->blockData.outStart = state->totalWritten;

    state->totalWritten += sharc_block_header_write(out, dictionaryFlags);

    state->process = SHARC_ENCODE_PROCESS_WRITE_DATA;

    return SHARC_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_encode_update_totals(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_encode_state *restrict state, const uint_fast64_t inPositionBefore, const uint_fast64_t outPositionBefore) {
    state->totalRead += in->position - inPositionBefore;
    state->totalWritten += out->position - outPositionBefore;
}

SHARC_ENCODE_STATE sharc_encode_init_with_file(sharc_encode_state *restrict state, const SHARC_COMPRESSION_MODE mode, const SHARC_ENCODE_TYPE encodeType, const struct stat *fileAttributes) {
    state->dictionaryData.resetCycle = 0;

    switch (encodeType) {
        case SHARC_ENCODE_TYPE_WITH_HEADER:
            state->process = SHARC_ENCODE_PROCESS_WRITE_HEADER;
            break;
        case SHARC_ENCODE_TYPE_WITHOUT_HEADER:
            state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
            break;
    }
    state->mode = mode;
    state->fileAttributes = fileAttributes;

    state->totalRead = 0;
    state->totalWritten = 0;

    sharc_hash_encode_init(&state->hashEncodeState);

    state->dictionaryData.resetCycle = 0;

    // todo workbuffer ?

    return SHARC_ENCODE_STATE_READY;
}

SHARC_ENCODE_STATE sharc_encode_init(sharc_encode_state *restrict state, const SHARC_COMPRESSION_MODE mode, const SHARC_ENCODE_TYPE encodeType) {
    return sharc_encode_init_with_file(state, mode, encodeType, NULL);
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_encode_state *restrict state, const SHARC_BOOL lastIn) {
    SHARC_ENCODE_STATE encodeState;
    SHARC_HASH_ENCODE_STATE hashEncodeState;
    uint_fast64_t inPositionBefore;
    uint_fast64_t outPositionBefore;

    while (SHARC_TRUE) {
        switch (state->process) {
            case SHARC_ENCODE_PROCESS_WRITE_HEADER:
                if ((encodeState = sharc_encode_write_header(out, state)))
                    return encodeState;
                break;

            case SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER:
                if ((encodeState = sharc_encode_write_block_header(out, state)))
                    return encodeState;
                break;

            case SHARC_ENCODE_PROCESS_WRITE_DATA:
                inPositionBefore = in->position;
                outPositionBefore = out->position;

                hashEncodeState = sharc_hash_encode_process(in, out, SHARC_HASH_XOR_MASK_DISPERSION, &state->dictionaryData.dictionary_a, &state->hashEncodeState, lastIn);
                sharc_encode_update_totals(in, out, state, inPositionBefore, outPositionBefore);

                switch (hashEncodeState) {
                    case SHARC_HASH_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_INPUT_BUFFER;

                    case SHARC_HASH_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                        return SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER;

                    case SHARC_HASH_ENCODE_STATE_INFO_NEW_BLOCK:
                        state->process = SHARC_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
                        break;

                    case SHARC_HASH_ENCODE_STATE_FINISHED:
                        return SHARC_ENCODE_STATE_FINISHED;

                    case SHARC_HASH_ENCODE_STATE_READY:
                        break;

                    default:
                        return SHARC_ENCODE_STATE_ERROR;
                }
        }
    }
}

SHARC_FORCE_INLINE SHARC_ENCODE_STATE sharc_encode_finish(sharc_encode_state *restrict state) {
    if(sharc_hash_encode_finish(&state->hashEncodeState))
        return SHARC_ENCODE_STATE_ERROR;

    return SHARC_ENCODE_STATE_READY;
}