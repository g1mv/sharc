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
 * 01/06/13 20:52
 */

#include "sharc_cipher.h"

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT createEncodingResult(const byte reachableMode, SHARC_BYTE_BUFFER* out) {
    SHARC_ENCODING_RESULT encodingResult;
    encodingResult.reachableMode = reachableMode;
    encodingResult.out = out;
    return encodingResult;
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT createEncodingResultWithPosition(const byte reachableMode, SHARC_BYTE_BUFFER* out, const uint32_t position) {
    out->position = position;
    return createEncodingResult(reachableMode, out);
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT copyMode(SHARC_BYTE_BUFFER* in) {
    return createEncodingResultWithPosition(SHARC_MODE_COPY, in, in->size);
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT sharcEncode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case SHARC_MODE_SINGLE_PASS:
            if(xorHashEncode(in, out))
                return createEncodingResult(mode, out);
            else
                return copyMode(in);
        case SHARC_MODE_DUAL_PASS:
            inter->size = in->size;
            rewindByteBuffer(inter);
            if(directHashEncode(in, inter)) {
                const uint32_t firstPassPosition = inter->position;
                out->size = firstPassPosition;
                inter->size = firstPassPosition;
                rewindByteBuffer(inter);
                if(xorHashEncode(inter, out))
                    return createEncodingResult(mode, out);
                else
                    return createEncodingResultWithPosition(SHARC_MODE_SINGLE_PASS, inter, firstPassPosition);
            }
            return copyMode(in);
        default:
            return copyMode(in);
    }
}

SHARC_FORCE_INLINE bool sharcDecode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case SHARC_MODE_SINGLE_PASS:
            return xorHashDecode(in, out);
        case SHARC_MODE_DUAL_PASS:
            rewindByteBuffer(inter);
            xorHashDecode(in, inter);
            inter->size = inter->position;
            rewindByteBuffer(inter);
            return directHashDecode(inter, out);
        default:
            return SHARC_FALSE;
    }
    
    return SHARC_TRUE;
}
