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
 * 08/09/13 02:05
 */

#include "mode_marker.h"

SHARC_FORCE_INLINE uint_fast32_t sharc_mode_marker_read(sharc_byte_buffer* restrict in, sharc_mode_marker * restrict modeMarker) {
    modeMarker->activeCompressionMode = *(in->pointer + in->position);

    in->position += sizeof(sharc_mode_marker);

    return sizeof(sharc_mode_marker);
}

SHARC_FORCE_INLINE uint_fast32_t sharc_mode_marker_write(sharc_byte_buffer* out, SHARC_COMPRESSION_MODE compressionMode) {
    *(out->pointer + out->position) = (sharc_byte)compressionMode;

    out->position += sizeof(sharc_mode_marker);

    return sizeof(sharc_mode_marker);
}