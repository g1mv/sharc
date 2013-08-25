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

#include "sharc_transform.h"

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT sharc_createEncodingResult(const sharc_byte reachableMode, SHARC_BYTE_BUFFER* out) {
    SHARC_ENCODING_RESULT encodingResult;
    encodingResult.reachableMode = reachableMode;
    encodingResult.out = out;
    return encodingResult;
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT sharc_createEncodingResultWithPosition(const sharc_byte reachableMode, SHARC_BYTE_BUFFER* out, const uint32_t position) {
    out->position = position;
    return sharc_createEncodingResult(reachableMode, out);
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT sharc_copyMode(SHARC_BYTE_BUFFER* in) {
    return sharc_createEncodingResultWithPosition(SHARC_MODE_COPY, in, in->size);
}

SHARC_FORCE_INLINE SHARC_ENCODING_RESULT sharc_sharcEncode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out, const sharc_byte mode, SHARC_ENTRY* dictionary_a, SHARC_ENTRY* dictionary_b) {
    switch(mode) {
        case SHARC_MODE_SINGLE_PASS:
            if(sharc_xorHashEncode(in, out, dictionary_a))
                return sharc_createEncodingResult(mode, out);
            else
                return sharc_copyMode(in);
        case SHARC_MODE_DUAL_PASS:
            inter->size = in->size;
            sharc_rewindByteBuffer(inter);
            if(sharc_directHashEncode(in, inter, dictionary_a)) {
                const uint32_t firstPassPosition = inter->position;
                out->size = firstPassPosition;
                inter->size = firstPassPosition;
                sharc_rewindByteBuffer(inter);
                if(sharc_xorHashEncode(inter, out, dictionary_b))
                    return sharc_createEncodingResult(mode, out);
                else
                    return sharc_createEncodingResultWithPosition(SHARC_MODE_SINGLE_PASS, inter, firstPassPosition);
            }
            return sharc_copyMode(in);
        default:
            return sharc_copyMode(in);
    }
}

SHARC_FORCE_INLINE sharc_bool sharc_sharcDecode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* inter, SHARC_BYTE_BUFFER* out, const sharc_byte mode, SHARC_ENTRY* dictionary_a, SHARC_ENTRY* dictionary_b) {
    switch(mode) {
        case SHARC_MODE_SINGLE_PASS:
            return sharc_xorHashDecode(in, out, dictionary_a);
        case SHARC_MODE_DUAL_PASS:
            sharc_rewindByteBuffer(inter);
            sharc_xorHashDecode(in, inter, dictionary_b);
            inter->size = inter->position;
            sharc_rewindByteBuffer(inter);
            return sharc_directHashDecode(inter, out, dictionary_a);
        default:
            return SHARC_FALSE;
    }
    
    return SHARC_TRUE;
}
