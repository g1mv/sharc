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
 * 09/09/13 12:12
 */

#ifndef SHARC_METADATA_H
#define SHARC_METADATA_H

#include "globals.h"
#include "header.h"
#include "footer.h"
#include "block_header.h"
#include "block_mode_marker.h"
#include "block_footer.h"

uint_fast64_t sharc_metadata_structure_overhead();
uint_fast64_t sharc_metadata_block_structure_overhead(const uint_fast64_t);
uint_fast64_t sharc_metadata_max_compressed_length(const uint_fast64_t, const SHARC_COMPRESSION_MODE, const bool);
uint_fast64_t sharc_metadata_max_decompressed_length(const uint_fast64_t, const SHARC_COMPRESSION_MODE, const bool);

#endif