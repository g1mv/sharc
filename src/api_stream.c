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
 * 26/08/13 23:18
 */

#include "api_stream.h"

SHARC_FORCE_INLINE sharc_stream* sharc_api_stream_allocate() {
    sharc_stream* created = (sharc_stream*) malloc(sizeof(sharc_stream));
    created->in = sharc_byte_buffer_allocate();
    created->out = sharc_byte_buffer_allocate();
    created->internal_state = sharc_state_allocate();
    return created;
}

void sharc_api_stream_deallocate(sharc_stream* stream) {
    sharc_state_deallocate(stream->internal_state);
    sharc_byte_buffer_deallocate(stream->out);
    sharc_byte_buffer_deallocate(stream->in);
    free(stream);
}

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_pushHeaderWithFileInformation(sharc_stream* stream, struct stat64* fileAttributes) {
    if(stream->out->position + sizeof(SHARC_HEADER) > stream->out->size)
        return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

    if(!fileAttributes)
        return SHARC_API_STREAM_STATE_ERROR_INVALID_FILE_ATTRIBUTES;

    uint32_t bytesWritten = sharc_writeHeader(stream->out->pointer, SHARC_HEADER_ORIGIN_TYPE_FILE, fileAttributes);
    stream->out->position += bytesWritten;
    stream->total_bytes_written += bytesWritten;

    return SHARC_API_STREAM_STATE_READY_TO_CONTINUE;
}

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_pushFooter(sharc_stream* stream) {
    if(stream->out->position + sizeof(SHARC_FOOTER) > stream->out->size)
        return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

    return SHARC_API_STREAM_STATE_READY_TO_CONTINUE;
}

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_compressInit(sharc_stream* stream, SHARC_STATE_COMPRESSION_MODE mode) {
    if(stream->internal_state->status != SHARC_STATE_STATUS_IDLE)
        return SHARC_API_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    stream->internal_state->status = SHARC_STATE_STATUS_NEW_BLOCK_PREPARATION_REQUIRED;
    stream->internal_state->dictionariesResetCycle = 0;

    return SHARC_API_STREAM_STATE_READY_TO_CONTINUE;
}

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_decompressInit(sharc_stream*);

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_newCompressedBlock(sharc_stream* stream) {
    if(stream->internal_state->dictionariesResetCycle) {
        stream->internal_state->dictionariesResetCycle --;
    } else {
        switch (stream->internal_state->mode) {
            case SHARC_STATE_COMPRESSION_MODE_DUAL_PASS_NO_REVERSION:
            case SHARC_STATE_COMPRESSION_MODE_DUAL_PASS_WITH_REVERSION:
                sharc_dictionary_resetCompressed(stream->internal_state->dictionary_b);

            case SHARC_STATE_COMPRESSION_MODE_FASTEST_NO_REVERSION:
            case SHARC_STATE_COMPRESSION_MODE_FASTEST_WITH_REVERSION:
                sharc_dictionary_resetDirect(stream->internal_state->dictionary_a);
                break;

            case SHARC_STATE_COMPRESSION_MODE_NO_COMPRESSION:
                break;
        }
        stream->internal_state->dictionariesResetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
    }

    stream->internal_state->status = SHARC_STATE_STATUS_READY_TO_COMPRESS;
    stream->internal_state->blockSignaturesCount = 0;
    stream->internal_state->totalBytesReadAtLastBlockStart = stream->total_bytes_read;
    stream->internal_state->totalBytesWrittenAtLastBlockStart = stream->total_bytes_written;

    switch(sharc_hash_encode_resetState(stream->out, stream->internal_state)) {
        case SHARC_HASH_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
            return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;
        default:
            return SHARC_API_STREAM_STATE_READY_TO_CONTINUE;
    }
}

SHARC_FORCE_INLINE SHARC_API_STREAM_STATE sharc_api_stream_compress(sharc_stream* stream) {
    switch(stream->internal_state->status) {
        case SHARC_STATE_STATUS_NEW_BLOCK_PREPARATION_REQUIRED:
            switch(sharc_api_stream_newCompressedBlock(stream)) {
                case SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER:
                    return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;
                default:
                    break;
            }
            break;

        case SHARC_STATE_STATUS_READY_TO_COMPRESS:
            if((sharc_byte)stream->out & 0x1)
                return SHARC_API_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED;

            if(stream->out->position + SHARC_HASH_ENCODE_MINIMUM_LOOKAHEAD > stream->out->size)
                return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

            if(stream->in->size & 0x3)
                return SHARC_API_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_4;

            uint64_t inPositionBefore;
            uint64_t outPositionBefore;
            SHARC_HASH_ENCODE_STATE returnState;
            switch (stream->internal_state->mode) {
                case SHARC_STATE_COMPRESSION_MODE_FASTEST_NO_REVERSION:
                        inPositionBefore = stream->in->position;
                        outPositionBefore = stream->out->position;

                        while(SHARC_HASH_ENCODE_STATE_READY_FOR_NEXT == (returnState = sharc_hash_encode_kernel(stream->in, stream->out, SHARC_HASH_XOR_MASK_DISPERSION, stream->internal_state->dictionary_a, stream->internal_state)))

                        switch (returnState) {
                            case SHARC_HASH_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                                return SHARC_API_STREAM_STATE_STALL_ON_INPUT_BUFFER;

                            case SHARC_HASH_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

                            default:
                                stream->total_bytes_read += stream->in->position - inPositionBefore;
                                stream->total_bytes_written += stream->out->position - outPositionBefore;
                                break;
                        }

                        if (stream->internal_state->blockSignaturesCount >= 1024) {
                            switch(sharc_api_stream_newCompressedBlock(stream)) {
                                case SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER:
                                    return SHARC_API_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;
                                default:
                                    break;
                            }
                        }

                        if (stream->in->position == stream->in->size)
                            return SHARC_API_STREAM_STATE_STALL_ON_INPUT_BUFFER;
                default:
                    return SHARC_API_STREAM_STATE_ERROR_INVALID_COMPRESSION_MODE;
            }

        default:
            return SHARC_API_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
    }
}

SHARC_API_STREAM_STATE sharc_api_stream_decompress(sharc_stream*);
SHARC_API_STREAM_STATE sharc_api_stream_compressEnd(sharc_stream*);
SHARC_API_STREAM_STATE sharc_api_stream_decompressEnd(sharc_stream*);