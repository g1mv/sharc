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

#include "stream.h"

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compressInit(sharc_stream * stream, SHARC_API_COMPRESSION_MODE mode, SHARC_API_OUTPUT_TYPE type) {
    SHARC_COMPRESSION_MODE correspondingMode;
    SHARC_ENCODE_TYPE correspondingType;

    switch (mode) {
        case SHARC_API_COMPRESSION_MODE_FASTEST:
            correspondingMode = SHARC_COMPRESSION_MODE_FASTEST_NO_REVERSION;
            break;
        default:
            return SHARC_STREAM_STATE_ERROR_UNSUPPORTED_COMPRESSION_MODE;
    }

    switch (type) {
        case SHARC_API_OUTPUT_TYPE_WITH_HEADER:
            correspondingType = SHARC_ENCODE_TYPE_WITH_HEADER;
            break;
        case SHARC_API_OUTPUT_TYPE_WITHOUT_HEADER:
            correspondingType = SHARC_ENCODE_TYPE_WITHOUT_HEADER;
            break;
    }

    if (sharc_encode_initialize(&stream->out, &stream->internal_state.internal_state, correspondingMode, correspondingType))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    return SHARC_STREAM_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompressInit(sharc_stream *);

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *stream) {
    if ((uint32_t) &stream->out & 0x1)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED;

    if (stream->out.size & 0x3)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_4;

    SHARC_ENCODE_STATE returnState = sharc_encode_kernel(&stream->in, &stream->out, &stream->internal_state.internal_state);
    stream->in_total_read = stream->internal_state.internal_state.totalRead;
    stream->out_total_written = stream->internal_state.internal_state.totalWritten;

    switch (returnState) {
        case SHARC_ENCODE_STATE_STALL_INPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER;

        case SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

        case SHARC_ENCODE_STATE_OK:
            return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

        case SHARC_ENCODE_STATE_ERROR:
            return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
    }
}

SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *);

SHARC_STREAM_STATE sharc_stream_compressEnd(sharc_stream * stream) {
    if ((uint32_t) &stream->out & 0x1)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED;

    SHARC_ENCODE_STATE returnState = sharc_encode_finish(&stream->in, &stream->out, &stream->internal_state.internal_state);
    stream->in_total_read = stream->internal_state.internal_state.totalRead;
    stream->out_total_written = stream->internal_state.internal_state.totalWritten;

    switch (returnState) {
        case SHARC_ENCODE_STATE_OK:
            return SHARC_STREAM_STATE_OK;

        case SHARC_ENCODE_STATE_STALL_OUTPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

        default:
            return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
    }
}

SHARC_STREAM_STATE sharc_stream_decompressEnd(sharc_stream *);