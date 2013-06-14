/*
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Centaurean BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Sharc
 * www.centaurean.com
 *
 * 01/06/13 20:52
 */

#include "sharc_cipher.h"

FORCE_INLINE byte sharcEncode(byte* _inBuffer, uint32_t _inSize, byte* _outBuffer, uint32_t _outSize, byte mode) {
    switch(mode) {
        case MODE_SINGLE_PASS:
            if(xorHashEncode(_inBuffer, _inSize, _outBuffer, _outSize))
                return mode;
            else
                return MODE_COPY;
        case MODE_DUAL_PASS:
            if(directHashEncode(_inBuffer, _inSize, intermediateBuffer, _inSize)) {
                const uint32_t initialOutPosition = outPosition;
                if(xorHashEncode(intermediateBuffer, initialOutPosition, _outBuffer, initialOutPosition)) {
                    return mode;
                } else {
                    outBuffer = intermediateBuffer;
                    outPosition = initialOutPosition;
                    return MODE_SINGLE_PASS;
                }
            }
            return MODE_COPY;
        default:
            return MODE_COPY;
    }
}

FORCE_INLINE bool sharcDecode(byte* _inBuffer, uint32_t _inSize, byte* _outBuffer, uint32_t _outSize) {
    const byte mode = _inBuffer[0];
    
    switch(mode) {
        case MODE_SINGLE_PASS:
            return xorHashDecode(_inBuffer, _inSize, _outBuffer, _outSize);
        case MODE_DUAL_PASS:
            xorHashDecode(_inBuffer, _inSize, intermediateBuffer, _outSize);
            return directHashDecode(intermediateBuffer, _outSize, _outBuffer, _outSize);
        default:
            return FALSE;
    }
    
    return TRUE;
}
