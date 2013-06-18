/*
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 18:46
 */

#include "cipher.h"

/*FORCE_INLINE void prepareWorkspace(byte* _inBuffer, uint32_t _inSize, byte* _outBuffer, uint32_t _outSize) {
    inBuffer = _inBuffer;
    inSize = _inSize;
    inPosition = 0;
    
    outBuffer = _outBuffer;
    outSize = _outSize;
    outPosition = 0;
}*/

FORCE_INLINE BYTE_BUFFER createByteBuffer(byte* pointer, uint32_t position, uint32_t size) {
    BYTE_BUFFER byteBuffer;
    byteBuffer.pointer = pointer;
    byteBuffer.position = position;
    byteBuffer.size = size;
    return byteBuffer;
}

FORCE_INLINE void rewindByteBuffer(BYTE_BUFFER* buffer) {
    buffer->position = 0;
}