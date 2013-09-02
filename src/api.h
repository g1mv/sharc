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

#include "globals.h"
#include "byte_buffer.h"
#include "stream.h"

#define SHARC_YES    1
#define SHARC_NO     0

/*
 * If you intend to use SHARC as a library, replace SHARC_NO by SHARC_YES.
 * client.c and client.h content will be masked : there will not be any main() function compiled.
 */
#define SHARC_USE_AS_LIBRARY    SHARC_NO

/*
 * SHARC byte buffer utilities
 */
void sharc_byte_buffer_encapsulate(sharc_byte_buffer *, sharc_byte*, uint32_t);
void sharc_byte_buffer_rewind(sharc_byte_buffer *);

/*
 * SHARC stream API functions
 *
 * For a simple example of how to use the stream API, please have a look at client.c
 *
 * SHARC_STREAM_STATE can have the following values :
 * SHARC_STREAM_STATE_READY, ready to continue
 * SHARC_STREAM_STATE_FINISHED, processing is finished
 * SHARC_STREAM_STATE_STALL_ON_INPUT_BUFFER, input buffer has been completely read
 * SHARC_STREAM_STATE_STALL_ON_OUTPUT_BUFFER, there is not enought space left in the output buffer to continue
 * SHARC_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32, size of input buffer is no a multiple of 32
 * SHARC_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL, output buffer size is too small
 * SHARC_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE, error during processing
 */

SHARC_STREAM_STATE sharc_stream_prepare(sharc_stream *stream, char* input_buffer, const uint_fast32_t input_size, char* output_buffer, const uint_fast32_t output_size);
/*
 * Prepare a stream with the encapsulated input/output buffers. This function *must* be called upon changing either buffer pointers / sizes.
 */

SHARC_STREAM_STATE sharc_stream_compress_init_with_stat(sharc_stream * stream, const SHARC_COMPRESSION_MODE compression_mode, const struct stat* file_attributes);
/*
 * Initialization function, using a compression mode and a struct stat either instanciated by stat() if processing a file, or NULL.
 */

SHARC_STREAM_STATE sharc_stream_compress_init(sharc_stream *stream, const SHARC_COMPRESSION_MODE compression_mode);
/*
 * Same function as before, assuming file_attributes is NULL
 */

SHARC_STREAM_STATE sharc_stream_decompress_init(sharc_stream *stream);
/*
 * Stream decompression initialization
 */

SHARC_STREAM_STATE sharc_stream_compress(sharc_stream *stream, const sharc_bool last_input_data);
/*
 * Stream compression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, last_input_data has to be true. Otherwise it should be false at all times.
 */

SHARC_STREAM_STATE sharc_stream_decompress(sharc_stream *stream, const sharc_bool last_input_data);
/*
 * Stream decompression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, last_input_data has to be true. Otherwise it should be false at all times.
 */

SHARC_STREAM_STATE sharc_stream_compress_finish(sharc_stream *);
/*
 * Call once processing is finished, to clear up the environment and release eventual memory.
 */

SHARC_STREAM_STATE sharc_stream_decompress_finish(sharc_stream *);
/*
 * Call once processing is finished, to clear up the environment and release eventual memory.
 */


/*
 * SHARC buffers API functions
 *
 * To come
 */

#endif