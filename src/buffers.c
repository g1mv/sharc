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
 * 27/08/13 15:02
 */

#include "buffers.h"

SHARC_FORCE_INLINE SHARC_BUFFERS_STATE sharc_buffers_translate_state(SHARC_STREAM_STATE state) {
    switch(state) {
        case SHARC_STREAM_STATE_READY:
            return SHARC_BUFFERS_STATE_OK;
        case SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER:
            return SHARC_BUFFERS_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL;
        default:
            return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;
    }
}

SHARC_FORCE_INLINE SHARC_BUFFERS_STATE sharc_buffers_max_compressed_length(uint_fast64_t *result, uint_fast64_t initialLength, const SHARC_COMPRESSION_MODE compressionMode) {
    *result = sharc_metadata_max_compressed_length(initialLength, compressionMode, true);

    return SHARC_BUFFERS_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_BUFFERS_STATE sharc_buffers_compress(uint_fast64_t *written, uint8_t *in, uint_fast64_t inSize, uint8_t *out, uint_fast64_t outSize, const SHARC_COMPRESSION_MODE compressionMode, const SHARC_ENCODE_OUTPUT_TYPE outputType, const SHARC_BLOCK_TYPE blockType, const struct stat *fileAttributes, void *(*mem_alloc)(size_t), void (*mem_free)(void *)) {
    SHARC_STREAM_STATE returnState;

    sharc_stream stream = {};
    if ((returnState = sharc_stream_prepare(&stream, in, inSize, out, outSize, mem_alloc, mem_free)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_compress_init(&stream, compressionMode, outputType, blockType, fileAttributes)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_compress(&stream, true)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_compress_finish(&stream)))
        return sharc_buffers_translate_state(returnState);

    *written = *stream.out_total_written;

    return SHARC_BUFFERS_STATE_OK;
}

SHARC_BUFFERS_STATE sharc_buffers_decompress(uint_fast64_t *written, uint8_t *in, uint_fast64_t inSize, uint8_t *out, uint_fast64_t outSize, void *(*mem_alloc)(size_t), void (*mem_free)(void *)) {
    SHARC_STREAM_STATE returnState;

    sharc_stream stream = {};
    if ((returnState = sharc_stream_prepare(&stream, in, inSize, out, outSize, mem_alloc, mem_free)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_decompress_init(&stream)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_decompress(&stream, true)))
        return sharc_buffers_translate_state(returnState);

    if ((returnState = sharc_stream_decompress_finish(&stream)))
        return sharc_buffers_translate_state(returnState);

    *written = *stream.out_total_written;

    return SHARC_BUFFERS_STATE_OK;
}