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
 * 09/09/13 12:11
 */

#include "metadata.h"

SHARC_FORCE_INLINE uint_fast64_t sharc_metadata_structure_overhead() {
    return sizeof(sharc_header) + sizeof(sharc_footer);
}

SHARC_FORCE_INLINE uint_fast64_t sharc_metadata_block_structure_overhead(const uint_fast64_t length) {
    return (1 + length / (SHARC_PREFERRED_BLOCK_SIGNATURES * sizeof(uint16_t) * 8 * 8/*sizeof(sharc_hash_signature)*/)) * (sizeof(sharc_block_header) + sizeof(sharc_mode_marker) + sizeof(sharc_block_footer));
}

SHARC_FORCE_INLINE uint_fast64_t sharc_metadata_max_compressed_length(const uint_fast64_t length, const SHARC_COMPRESSION_MODE mode, const bool includeStructure) {
    uint_fast64_t headerFooterLength = (includeStructure ? sharc_metadata_structure_overhead() : 0);
    switch(mode) {
        default:
            return headerFooterLength + length;

        case SHARC_COMPRESSION_MODE_CHAMELEON:
            return headerFooterLength + sharc_metadata_block_structure_overhead(length) + length;

        case SHARC_COMPRESSION_MODE_CHAMELEON_DUAL_PASS:
            return headerFooterLength + sharc_metadata_block_structure_overhead(sharc_metadata_block_structure_overhead(length) + length) + length;
    }
}

SHARC_FORCE_INLINE uint_fast64_t sharc_metadata_max_decompressed_length(const uint_fast64_t length, const SHARC_COMPRESSION_MODE mode, const bool includeStructure) {
    uint_fast64_t headerFooterLength = (includeStructure ? sharc_metadata_structure_overhead() : 0);
    uint_fast64_t intermediate;
    switch(mode) {
        default:
            return length - headerFooterLength;

        case SHARC_COMPRESSION_MODE_CHAMELEON:
            return (length - sharc_metadata_block_structure_overhead(length)) << (1 - headerFooterLength);

        case SHARC_COMPRESSION_MODE_CHAMELEON_DUAL_PASS:
            intermediate = (length - sharc_metadata_block_structure_overhead(length)) << 1;
            return (intermediate - sharc_metadata_block_structure_overhead(intermediate)) << (1 - headerFooterLength);
    }
}

