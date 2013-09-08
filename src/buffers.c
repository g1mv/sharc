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

SHARC_FORCE_INLINE uint_fast64_t sharc_buffers_structure_size(uint_fast64_t initialLength, SHARC_ENCODE_OUTPUT_TYPE outputType, SHARC_BLOCK_TYPE blockType) {
    uint_fast64_t result = 0;
    switch (outputType) {
        case SHARC_ENCODE_OUTPUT_TYPE_DEFAULT:
            result += sizeof(sharc_header) + sizeof(sharc_footer);
            break;
        case SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER:
            result += sizeof(sharc_footer);
            break;
        case SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_FOOTER:
            result += sizeof(sharc_header);
            break;
        case SHARC_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER_NOR_FOOTER:
            break;
    }

    uint64_t blockOverhead = sizeof(sharc_block_header) + sizeof(sharc_mode_marker);
    if (blockType == SHARC_BLOCK_TYPE_DEFAULT)
        blockOverhead += sizeof(sharc_block_footer);

    return result + (1 + initialLength / (SHARC_PREFERRED_BLOCK_SIGNATURES * 32 * sizeof(sharc_hash_signature))) * blockOverhead;
}

SHARC_FORCE_INLINE SHARC_BUFFERS_STATE sharc_buffers_max_compressed_length(uint_fast64_t *result, uint_fast64_t initialLength, SHARC_ENCODE_OUTPUT_TYPE outputType, SHARC_BLOCK_TYPE blockType) {
    *result = initialLength + sharc_buffers_structure_size(initialLength, outputType, blockType);

    return SHARC_BUFFERS_STATE_OK;
}

SHARC_FORCE_INLINE SHARC_BUFFERS_STATE sharc_buffers_compress(uint_fast64_t *written, uint8_t *in, uint_fast64_t inSize, uint8_t *out, uint_fast64_t outSize, const SHARC_COMPRESSION_MODE compressionMode, const SHARC_ENCODE_OUTPUT_TYPE outputType, const SHARC_BLOCK_TYPE blockType, const struct stat *fileAttributes, void *(*mem_alloc)(size_t), void (*mem_free)(void *)) {
    uint_fast64_t minimumOutSize;

    sharc_buffers_max_compressed_length(&minimumOutSize, inSize, outputType, blockType);
    if (outSize < minimumOutSize)
        return SHARC_BUFFERS_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL;

    sharc_stream stream;
    if (sharc_stream_prepare(&stream, in, inSize, out, outSize, mem_alloc, mem_free))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_compress_init(&stream, compressionMode, outputType, blockType, fileAttributes))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_compress(&stream, true))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_compress_finish(&stream))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    *written = stream.out_total_written;

    return SHARC_BUFFERS_STATE_OK;
}

SHARC_BUFFERS_STATE sharc_buffers_decompress(uint_fast64_t *written, uint8_t *in, uint_fast64_t inSize, uint8_t *out, uint_fast64_t outSize, void *(*mem_alloc)(size_t), void (*mem_free)(void *)) {
    sharc_stream stream;
    if (sharc_stream_prepare(&stream, in, inSize, out, outSize, mem_alloc, mem_free))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_decompress_init(&stream))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_decompress(&stream, true))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    if (sharc_stream_decompress_finish(&stream))
        return SHARC_BUFFERS_STATE_ERROR_INVALID_STATE;

    *written = stream.out_total_written;

    return SHARC_BUFFERS_STATE_OK;
}