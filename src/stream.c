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

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream *restrict stream, char *restrict in, const uint_fast32_t availableIn, char *restrict out, const uint_fast32_t availableOut) {
    sharc_byte_buffer_encapsulate(&stream->in, (sharc_byte *) in, availableIn);
    sharc_byte_buffer_encapsulate(&stream->out, (sharc_byte *) out, availableOut);

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_check_conformity(sharc_stream *stream) {
    if ((uint64_t) stream->in.pointer & 0x7)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_NOT_PROPERLY_ALIGNED;

    if (stream->out.size < SHARC_STREAM_MINIMUM_OUT_BUFFER_SIZE)
        return SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *stream, const SHARC_COMPRESSION_MODE compressionMode, const struct stat *fileAttributes) {
    if (sharc_encode_init_with_file(&stream->internal_state.internal_encode_state, compressionMode, SHARC_ENCODE_TYPE_WITH_HEADER, fileAttributes))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *stream, const sharc_bool lastIn) {
    SHARC_STREAM_STATE streamState = sharc_stream_check_conformity(stream);
    if (streamState)
        return streamState;

    if (!lastIn) if (stream->out.size & 0x1F)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32;

    SHARC_ENCODE_STATE returnState = sharc_encode_process(&stream->in, &stream->out, &stream->internal_state.internal_encode_state, lastIn);
    stream->in_total_read = stream->internal_state.internal_encode_state.totalRead;
    stream->out_total_written = stream->internal_state.internal_encode_state.totalWritten;

    switch (returnState) {
        case SHARC_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER;

        case SHARC_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

        case SHARC_ENCODE_STATE_FINISHED:
            return SHARC_STREAM_STATE_FINISHED;

        default:
            return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
    }
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress_finish(sharc_stream *stream) {
    if (sharc_encode_finish(&stream->internal_state.internal_encode_state))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *stream) {
    if (sharc_decode_init(&stream->internal_state.internal_decode_state))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *stream, const sharc_bool lastIn) {
    SHARC_STREAM_STATE streamState = sharc_stream_check_conformity(stream);
    if (streamState)
        return streamState;

    SHARC_DECODE_STATE returnState = sharc_decode_process(&stream->in, &stream->out, &stream->internal_state.internal_decode_state, lastIn);
    stream->in_total_read = stream->internal_state.internal_decode_state.totalRead;
    stream->out_total_written = stream->internal_state.internal_decode_state.totalWritten;

    switch (returnState) {
        case SHARC_DECODE_STATE_STALL_ON_INPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER;

        case SHARC_DECODE_STATE_STALL_ON_OUTPUT_BUFFER:
            return SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER;

        case SHARC_DECODE_STATE_FINISHED:
            return SHARC_STREAM_STATE_FINISHED;

        default:
            return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
    }
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_finish(sharc_stream *stream) {
    if (sharc_decode_finish(&stream->internal_state.internal_decode_state))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    return SHARC_STREAM_STATE_READY;
}