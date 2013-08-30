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

uint64_t sharc_api_buffers_max_compressed_length_without_header(uint64_t uncompressedLength) {
    return sizeof(sharc_block_header) * (1 + uncompressedLength / SHARC_PREFERRED_BLOCK_SIGNATURES) /*+ sizeof(sharc_hash_encode_signature) * (1 + uncompressedLength / 32)*/ + uncompressedLength;
}

uint64_t sharc_api_buffers_max_compressed_total_length(uint64_t uncompressedLength) {
    return sizeof(sharc_header) + sharc_api_buffers_max_compressed_length_without_header(uncompressedLength);
}
