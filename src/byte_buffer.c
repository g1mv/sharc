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

SHARC_FORCE_INLINE SHARC_BYTE_BUFFER sharc_createByteBuffer(sharc_byte* pointer, uint32_t position, uint32_t size) {
    SHARC_BYTE_BUFFER byteBuffer;
    byteBuffer.pointer = pointer;
    byteBuffer.position = position;
    byteBuffer.size = size;
    return byteBuffer;
}

SHARC_FORCE_INLINE void sharc_rewindByteBuffer(SHARC_BYTE_BUFFER* buffer) {
    buffer->position = 0;
}