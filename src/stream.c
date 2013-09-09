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

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream *restrict stream, uint8_t *restrict in, const uint_fast64_t availableIn, uint8_t *restrict out, const uint_fast64_t availableOut, void *(*mem_alloc)(size_t), void (*mem_free)(void *)) {
    sharc_byte_buffer_encapsulate(&stream->in, in, availableIn);
    sharc_byte_buffer_encapsulate(&stream->out, out, availableOut);

    if (mem_alloc == NULL)
        stream->internal_state.mem_alloc = malloc;
    else
        stream->internal_state.mem_alloc = mem_alloc;

    if (mem_free == NULL)
        stream->internal_state.mem_free = free;
    else
        stream->internal_state.mem_free = mem_free;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_check_conformity(sharc_stream *stream) {
    if (stream->out.size < SHARC_STREAM_MINIMUM_OUT_BUFFER_SIZE)
        return SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *stream, const SHARC_COMPRESSION_MODE compressionMode, const SHARC_ENCODE_OUTPUT_TYPE outputType, const SHARC_BLOCK_TYPE blockType, const struct stat *fileAttributes) {
    stream->internal_state.process = SHARC_STREAM_PROCESS_ENCODING_INIT;

    if (sharc_encode_init(&stream->internal_state.workBuffer, &stream->internal_state.internal_encode_state, compressionMode, outputType, blockType, fileAttributes))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    uint_fast64_t workSize = stream->in.size + (stream->in.size >> 1);          // Max output size for compression
    if (stream->internal_state.workBuffer.size != workSize) {
        stream->internal_state.mem_free(stream->internal_state.workBuffer.pointer);
        sharc_byte_buffer_encapsulate(&stream->internal_state.workBuffer, stream->internal_state.mem_alloc((size_t) workSize), workSize);
    }

    stream->internal_state.process = SHARC_STREAM_PROCESS_ENCODING;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *stream, const sharc_bool flush) {
    SHARC_STREAM_STATE streamState = sharc_stream_check_conformity(stream);
    if (streamState)
        return streamState;

    if (!flush) if (stream->out.size & 0x1F)
        return SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32;

    SHARC_ENCODE_STATE returnState = sharc_encode_process(&stream->in, &stream->out, &stream->internal_state.internal_encode_state, flush);
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

    stream->internal_state.mem_free(stream->internal_state.workBuffer.pointer);
    stream->internal_state.workBuffer.pointer = NULL;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *stream) {
    stream->internal_state.process = SHARC_STREAM_PROCESS_DECODING_INIT;

    uint_fast64_t workSize = stream->in.size << 1;                              // Max output size for decompression
    if (stream->internal_state.workBuffer.size != workSize) {
        stream->internal_state.mem_free(stream->internal_state.workBuffer.pointer);
        sharc_byte_buffer_encapsulate(&stream->internal_state.workBuffer, stream->internal_state.mem_alloc((size_t) workSize), workSize);
    }

    if (sharc_decode_init(&stream->internal_state.workBuffer, &stream->internal_state.internal_decode_state))
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;

    stream->internal_state.process = SHARC_STREAM_PROCESS_DECODING;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *stream, const sharc_bool flush) {
    SHARC_STREAM_STATE streamState = sharc_stream_check_conformity(stream);
    if (streamState)
        return streamState;

    SHARC_DECODE_STATE returnState = sharc_decode_process(&stream->in, &stream->out, &stream->internal_state.internal_decode_state, flush);
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

    stream->internal_state.mem_free(stream->internal_state.workBuffer.pointer);
    stream->internal_state.workBuffer.pointer = NULL;

    return SHARC_STREAM_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_origin_type(sharc_stream *stream, SHARC_STREAM_ORIGIN_TYPE *originType) {
    if (stream->internal_state.process == SHARC_STREAM_PROCESS_DECODING) {
        switch (stream->internal_state.internal_decode_state.header.genericHeader.originType) {
            case SHARC_HEADER_ORIGIN_TYPE_FILE:
                *originType = SHARC_STREAM_ORIGIN_TYPE_FILE;
                break;
            default:
                *originType = SHARC_STREAM_ORIGIN_TYPE_STREAM;
                break;
        }
        return SHARC_STREAM_STATE_READY;
    } else
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_original_file_size(sharc_stream *stream, uint_fast64_t *size) {
    SHARC_STREAM_ORIGIN_TYPE originType;
    SHARC_STREAM_STATE returnState;

    if ((returnState = sharc_stream_decompress_utilities_get_origin_type(stream, &originType)))
        return returnState;

    if (originType == SHARC_STREAM_ORIGIN_TYPE_FILE) {
        *size = stream->internal_state.internal_decode_state.header.fileInformationHeader.originalFileSize;
        return SHARC_STREAM_STATE_READY;
    } else
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
}

SHARC_FORCE_INLINE SHARC_STREAM_STATE sharc_stream_decompress_utilities_restore_file_attributes(sharc_stream *stream, const char *fileName) {
    SHARC_STREAM_ORIGIN_TYPE originType;
    SHARC_STREAM_STATE returnState;

    if ((returnState = sharc_stream_decompress_utilities_get_origin_type(stream, &originType)))
        return returnState;

    if (originType == SHARC_STREAM_ORIGIN_TYPE_FILE) {
        sharc_header_restoreFileAttributes(&stream->internal_state.internal_decode_state.header.fileInformationHeader, fileName);
        return SHARC_STREAM_STATE_READY;
    } else
        return SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE;
}