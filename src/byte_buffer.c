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
 * 01/06/13 18:46
 */

#include "byte_buffer.h"

SHARC_FORCE_INLINE void sharc_byte_buffer_encapsulateWithPosition(sharc_byte_buffer * restrict buffer, sharc_byte* restrict pointer, const uint_fast64_t position, const uint_fast64_t size) {
    buffer->pointer = pointer;
    buffer->position = position;
    buffer->size = size;
}

SHARC_FORCE_INLINE void sharc_byte_buffer_encapsulate(sharc_byte_buffer * restrict buffer, sharc_byte* restrict pointer, const uint_fast64_t size) {
    sharc_byte_buffer_encapsulateWithPosition(buffer, pointer, 0, size);
}

SHARC_FORCE_INLINE void sharc_byte_buffer_rewind(sharc_byte_buffer * buffer) {
    buffer->position = 0;
}