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
 * 08/09/13 02:04
 */

#ifndef SHARC_BLOCK_MODE_MARKER_H
#define SHARC_BLOCK_MODE_MARKER_H

#include "globals.h"
#include "byte_buffer.h"

#pragma pack(push)
#pragma pack(4)
typedef struct {
    sharc_byte activeCompressionMode;
    sharc_byte reserved;    // To ensure 2-byte-wise length of output data
} sharc_mode_marker;
#pragma pack(pop)

uint_fast32_t sharc_mode_marker_read(sharc_byte_buffer*, sharc_mode_marker *);
uint_fast32_t sharc_mode_marker_write(sharc_byte_buffer*, SHARC_COMPRESSION_MODE);

#endif
