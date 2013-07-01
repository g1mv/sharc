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

FORCE_INLINE ENCODING_RESULT createEncodingResult(const byte reachableMode, BYTE_BUFFER* out) {
    ENCODING_RESULT encodingResult;
    encodingResult.reachableMode = reachableMode;
    encodingResult.out = out;
    return encodingResult;
}

FORCE_INLINE ENCODING_RESULT createEncodingResultWithPosition(const byte reachableMode, BYTE_BUFFER* out, const uint32_t position) {
    out->position = position;
    return createEncodingResult(reachableMode, out);
}

FORCE_INLINE ENCODING_RESULT copyMode(BYTE_BUFFER* in) {
    return createEncodingResultWithPosition(MODE_COPY, in, in->size);
}

FORCE_INLINE ENCODING_RESULT sharcEncode(BYTE_BUFFER* in, BYTE_BUFFER* inter, BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case MODE_SINGLE_PASS:
            if(xorHashEncode(in, out))
                return createEncodingResult(mode, out);
            else
                return copyMode(in);
        case MODE_DUAL_PASS:
            rewindByteBuffer(inter);
            if(directHashEncode(in, inter)) {
                const uint32_t firstPassPosition = inter->position;
                inter->size = inter->position;
                out->size = inter->position;
                rewindByteBuffer(inter);
                if(xorHashEncode(inter, out))
                    return createEncodingResult(mode, out);
                else
                    return createEncodingResultWithPosition(MODE_SINGLE_PASS, inter, firstPassPosition);
            }
            return copyMode(in);
        default:
            return copyMode(in);
    }
}

FORCE_INLINE bool sharcDecode(BYTE_BUFFER* in, BYTE_BUFFER* inter, BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case MODE_SINGLE_PASS:
            return xorHashDecode(in, out);
        case MODE_DUAL_PASS:
            rewindByteBuffer(inter);
            xorHashDecode(in, inter);
            inter->size = inter->position;
            rewindByteBuffer(inter);
            return directHashDecode(inter, out);
        default:
            return FALSE;
    }
    
    return TRUE;
}
