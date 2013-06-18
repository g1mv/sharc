/*
 * Copyright (c) 2013, Guillaume Voirin (gvoirin@centaurean.com)
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 20:52
 */

#include "sharc_cipher.h"

FORCE_INLINE byte sharcEncode(BYTE_BUFFER* in, BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case MODE_SINGLE_PASS:
            if(xorHashEncode(in, out))
                return mode;
            else
                return MODE_COPY;
        case MODE_DUAL_PASS:
            /*if(directHashEncode(_inBuffer, _inSize, intermediateBuffer, _inSize)) {
                const uint32_t initialOutPosition = outPosition;
                if(xorHashEncode(intermediateBuffer, initialOutPosition, _outBuffer, initialOutPosition)) {
                    return mode;
                } else {
                    outBuffer = intermediateBuffer;
                    outPosition = initialOutPosition;
                    return MODE_SINGLE_PASS;
                }
            }*/
            return MODE_COPY;
        default:
            return MODE_COPY;
    }
}

FORCE_INLINE bool sharcDecode(BYTE_BUFFER* in, BYTE_BUFFER* out, const byte mode) {
    switch(mode) {
        case MODE_SINGLE_PASS:
            return TRUE;//xorHashDecode(inFile, outFile, limit);
        case MODE_DUAL_PASS:
            return FALSE;
        default:
            return FALSE;
    }
    
    return TRUE;
}
