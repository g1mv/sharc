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
 * 27/08/13 15:01
 */

#ifndef SHARC_API_H
#define SHARC_API_H

#include "stream.h"
#include "buffers.h"

#define SHARC_YES    1
#define SHARC_NO     0

/*
 * If you intend to use SHARC as a library, replace SHARC_NO by SHARC_YES.
 * client.c and client.h content will be masked : there will not be any main() function compiled.
 */
#define SHARC_USE_AS_LIBRARY    SHARC_NO



/***********************************************************************************************************************
 *                                                                                                                     *
 * SHARC byte buffer utilities                                                                                         *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Encapsulate a standard byte buffer into a SHARC byte buffer
 *
 * @param byte_buffer the resulting sharc_byte_buffer
 * @param buffer the byte buffer (type uint8_t*)
 * @param size the size of buffer in bytes
 */
void sharc_byte_buffer_encapsulate(sharc_byte_buffer* byte_buffer, sharc_byte* buffer, uint64_t size);

/*
 * Rewind a SHARC byte buffer
 *
 * @param byte_buffer the SHARC byte buffer to rewind (its position is set to zero)
 */
void sharc_byte_buffer_rewind(sharc_byte_buffer* byte_buffer);



/***********************************************************************************************************************
 *                                                                                                                     *
 * SHARC stream API functions                                                                                          *
 *                                                                                                                     *
 * For a simple example of how to use the stream API, please have a look at client.c                                   *
 *                                                                                                                     *
 * SHARC_STREAM_STATE can have the following values :                                                                  *
 *                                                                                                                     *
 * SHARC_STREAM_STATE_READY, ready to continue                                                                         *
 * SHARC_STREAM_STATE_FINISHED, processing is finished                                                                 *
 * SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER, input buffer has been completely read                                     *
 * SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER, there is not enought space left in the output buffer to continue         *
 * SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32, size of input buffer is no a multiple of 32          *
 * SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL, output buffer size is too small                                   *
 * SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE, error during processing                                            *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Prepare a stream with the encapsulated input/output buffers. This function *must* be called upon changing either buffer pointers / sizes.
 *
 * @param stream the stream
 * @param input_buffer a buffer of bytes
 * @param input_size the size in bytes of input_buffer
 * @param output_buffer a buffer of bytes
 * @param output_size the size of output_buffer, must be at least SHARC_STREAM_MINIMUM_OUT_BUFFER_SIZE
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream *stream, uint8_t* input_buffer, const uint_fast64_t input_size, uint8_t* output_buffer, const uint_fast64_t output_size, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

/*
 * Initialize compression
 *
 * @param stream the stream
 * @param compression_mode the compression mode, can take the following values :
 *      SHARC_COMPRESSION_MODE_COPY
 *      SHARC_COMPRESSION_MODE_FASTEST
 *      SHARC_COMPRESSION_MODE_DUAL_PASS
 * @param output_type the format of data output by encoding.
 *      EXPERTS ONLY ! If unsure, use SHARC_ENCODE_OUTPUT_TYPE_DEFAULT.
 *      Any other option will create output data which is *NOT* directly decompressible by the API. This can be used for parallelizing SHARC.
 * @param block_type the type of data blocks SHARC will generate.
 *      EXPERTS ONLY ! If you're unsure use SHARC_BLOCK_TYPE_DEFAULT.
 *      The option SHARC_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK basically makes the block footer size zero, and removes data integrity checks in the encoded output.
 *      It can be useful in network streaming situations, where data integrity is already checked by the protocol (TCP/IP for example), and the flush option in sharc_stream_compress is often set,
 *      as the absence of block footer will enhance compression ratio.
 * @param file_attributes the file attributes if known, otherwise use NULL.
 */
SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *stream, const SHARC_COMPRESSION_MODE compression_mode, const SHARC_ENCODE_OUTPUT_TYPE output_type, const SHARC_BLOCK_TYPE block_type, const struct stat* file_attributes);

/*
 * Stream decompression initialization
 *
 * @param stream the stream
 */
SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *stream);

/*
 * Stream compression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, last_input_data has to be true. Otherwise it should be false at all times.
 *
 * @param stream the stream
 * @param flush a boolean indicating flush behaviour
 *      If set to true, this will ensure that every byte from the input buffer will have its counterpart in the output buffer.
 *      flush has to be true when the presented data is the last (end of a file for example).
 *      It can also be set to true multiple times to handle network streaming for example. In that case, please also check
 *      the block_type parameter of sharc_stream_compress_init to enable better compression ratio.
 */
SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *stream, const sharc_bool flush);

/*
 * Stream decompression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, last_input_data has to be true. Otherwise it should be false at all times.
 *
 * @param stream the stream
 * @param flush a boolean indicating flush behaviour
 *      If set to true, this will ensure that every byte from the input buffer will have its counterpart in the output buffer.
 *      flush has to be true when the presented data is the last (end of a file for example)
 *      It can also be set to true multiple times to handle network streaming for example.
 */
SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *stream, const sharc_bool flush);

/*
 * Call once processing is finished, to clear up the environment and release eventual allocated memory.
 *
 * @param stream the stream
 */
SHARC_STREAM_STATE sharc_stream_compress_finish(sharc_stream *stream);

/*
 * Call once processing is finished, to clear up the environment and release eventual allocated memory.
 *
 * @param stream the stream
 */
SHARC_STREAM_STATE sharc_stream_decompress_finish(sharc_stream *stream);

/*
 * Returns the file header struct, or SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE if it has not been read
 *
 * @param stream the stream
 * @param header the file header struct if read
 */
SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_header(sharc_stream *stream, sharc_header *header);

/*
 * Returns the origin type (file, stream) of the decoded data
 *
 * @param stream the stream
 * @param origin_type a SHARC_STREAM_ORIGIN_TYPE where the result will be stored
 */
SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_origin_type(sharc_stream* stream, SHARC_STREAM_ORIGIN_TYPE * origin_type);

/*
 * Returns the original file size of the decoded file, if known. Otherwise returns SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE
 *
 * @param stream the stream
 * @param origin_type an uint_fast64_t where the result will be stored
 */
SHARC_STREAM_STATE sharc_stream_decompress_utilities_get_original_file_size(sharc_stream* stream, uint_fast64_t* original_file_size);

/*
 * Restores the decoded file's attributes (if any available) to the file named file_name.
 * If that is not possible (file attributes were not known at compression time for example), returns SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE
 *
 * @param stream the stream
 * @param file_name the name of the file whose attributes will be restored (usually the name of the decompressed file)
 */
SHARC_STREAM_STATE sharc_stream_decompress_utilities_restore_file_attributes(sharc_stream* stream, const char* file_name);



/***********************************************************************************************************************
 *                                                                                                                     *
 * SHARC buffers API functions                                                                                         *
 *                                                                                                                     *
 * Here are the different values for SHARC_BUFFERS_STATE :                                                             *
 *                                                                                                                     *
 * SHARC_BUFFERS_STATE_OK                                                                                              *
 * SHARC_BUFFERS_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL, the provided output buffer is too small                          *
 * SHARC_BUFFERS_STATE_ERROR_INVALID_STATE, an error occurred during processing                                        *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Returns the max compressed length possible (with incompressible data)
 *
 * @param result the resulting length
 * @param in_length, the length of the input data to compress
 * @param output_type, the encode output type to be used
 * @param block_type the block type to be used
 */
SHARC_BUFFERS_STATE sharc_buffers_max_compressed_length(uint_fast64_t * result, uint_fast64_t in_length, SHARC_ENCODE_OUTPUT_TYPE output_type, SHARC_BLOCK_TYPE block_type);

/*
 * Buffers compression function
 *
 * @param total_written the total bytes written returned
 * @param in the input buffer to compress
 * @param in_size the size of the input buffer in bytes
 * @param out the output buffer
 * @param out_size the size of the output buffer in bytes
 * @param compression_mode the compression mode to use
 * @param output_type the output type to use (if unsure, SHARC_ENCODE_OUTPUT_TYPE_DEFAULT), see the sharc_stream_compress documentation
 * @param block_type the block type to use (if unsure, SHARC_BLOCK_TYPE_DEFAULT), see the sharc_stream_compress documentation
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
SHARC_BUFFERS_STATE sharc_buffers_compress(uint_fast64_t* total_written, uint8_t *in, uint_fast64_t in_size, uint8_t *out, uint_fast64_t out_size, const SHARC_COMPRESSION_MODE compression_mode, const SHARC_ENCODE_OUTPUT_TYPE output_type, const SHARC_BLOCK_TYPE block_type, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

/*
 * Buffers decompression function
 *
 * @param total_written the total bytes written returned
 * @param in the input buffer to decompress
 * @param in_size the size of the input buffer in bytes
 * @param out the output buffer
 * @param out_size the size of the output buffer in bytes
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
SHARC_BUFFERS_STATE sharc_buffers_decompress(uint_fast64_t * total_written, uint8_t *in, uint_fast64_t in_size, uint8_t *out, uint_fast64_t out_size, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

#endif