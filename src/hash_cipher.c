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
 * 01/06/13 17:31
 */

#include "hash_cipher.h"

FORCE_INLINE void writeSignature(const byte nThread) {
    signature/*[nThread]*/ |= ((uint64_t)1) << state/*[nThread]*/;
}

FORCE_INLINE bool flush(BYTE_BUFFER* in, BYTE_BUFFER* out, const byte nThread) {
    if((out->position + 8 + 256) > out->size)
        return FALSE;
    *(uint64_t*)(out->pointer + out->position) = signature/*[nThread]*/;
    out->position += 8;
#pragma unroll(64)
    for(byte b = 0; b < state/*[nThread]*/; b ++) {
        uint32_t chunk = chunks[b]/*[nThread]*/;
        switch((signature/*[nThread]*/ >> b) & 0x1) {
            case 0:
                *(uint32_t*)(out->pointer + out->position) = chunk;
                out->position += 4;
                break;
            case 1:
                *(uint16_t*)(out->pointer + out->position) = chunk;
                out->position += 2;
                break;
        }
    }
    in->position += (state/*[nThread]*/ << 2);
    return TRUE;
}

FORCE_INLINE void reset(const byte nThread) {
    state/*[nThread]*/ = 0;
    signature/*[nThread]*/ = 0;
}

FORCE_INLINE void resetDictionary(const byte nThread) {
    for(unsigned int i = 0; i < (1 << HASH_BITS); i ++)
        *(uint32_t*)&dictionary[i]/*[nThread]*/ = 0;
}

FORCE_INLINE bool checkState(BYTE_BUFFER* in, BYTE_BUFFER* out, const byte nThread) {
    switch(state/*[nThread]*/) {
        case 64:
            if(!flush(in, out, nThread))
                return FALSE;
            reset(nThread);
            break;
    }
    return TRUE;
}

FORCE_INLINE void computeHash(uint32_t* hash, const uint32_t value, const uint32_t xorMask) {
    *hash = HASH_OFFSET_BASIS;
    *hash ^= (value ^ xorMask);
    *hash *= HASH_PRIME;
    *hash = (*hash >> (32 - HASH_BITS)) ^ (*hash & 0xFFFF);
}

FORCE_INLINE bool updateEntry(BYTE_BUFFER* in, BYTE_BUFFER* out, ENTRY* entry, const uint32_t chunk, const uint32_t index, const byte nThread) {
	*(uint32_t*)entry = (index & 0xFFFFFF) | MAX_BUFFER_REFERENCES;
	chunks[state/*[nThread]*/++]/*[nThread]*/ = chunk;
    return checkState(in, out, nThread);
}

FORCE_INLINE bool kernel(BYTE_BUFFER* in, BYTE_BUFFER* out, const uint32_t chunk, const uint32_t xorMask, const uint32_t* buffer, const uint32_t index, const byte nThread) {
    computeHash(&hash/*[nThread]*/, chunk, xorMask);
    ENTRY* found = &dictionary[hash/*[nThread]*/]/*[nThread]*/;
    if((*(uint32_t*)found) & MAX_BUFFER_REFERENCES) {
        if(chunk ^ buffer[*(uint32_t*)found & 0xFFFFFF]) {
            if(updateEntry(in, out, found, chunk, index, nThread) ^ 0x1)
                return FALSE;
        } else {
            writeSignature(nThread);
            chunks[state/*[nThread]*/++]/*[nThread]*/ = (uint16_t)hash/*[nThread]*/;
            if(checkState(in, out, nThread) ^ 0x1)
                return FALSE;
        }
    } else {
        if(updateEntry(in, out, found, chunk, index, nThread) ^ 0x1)
            return FALSE;
    }
    return TRUE;
}

FORCE_INLINE bool hashEncode(BYTE_BUFFER* in, BYTE_BUFFER* out, const uint32_t xorMask, const byte nThread) {
    reset(nThread);
    resetDictionary(nThread);
    
    const uint32_t* intInBuffer = (const uint32_t*)in->pointer;
    const uint32_t intInSize = in->size >> 2;
    
    for(uint32_t i = 0; i < intInSize; i ++)
        if(kernel(in, out, intInBuffer[i], xorMask, intInBuffer, i, nThread) ^ 0x1)
            return FALSE;
    
    flush(in, out, nThread);
    
    const uint32_t remaining = in->size - in->position;
    for(uint32_t i = 0; i < remaining; i ++) {
        if(out->position < out->size - 1)
            out->pointer[out->position ++] = in->pointer[in->position ++];
        else
            return FALSE;
    }
    
    return TRUE;
}

FORCE_INLINE bool hashDecode(byte* a, uint32_t b, const uint32_t c) {
    /*reset();
    resetDictionary();
    
    uint32_t resultingSize = *(uint32_t*)_inBuffer;
    //if(resultingSize > outSize)
    //    routBuffer = realloc;
    prepareWorkspace(_inBuffer, _inSize, writeBuffer, resultingSize);
    
    
    fread((byte*)finalSize, sizeof(byte), 4, inFile);
    
    uint32_t totalWritten = 0;
    
    while(totalWritten < finalSize) {
        fread((byte*)finalSize, sizeof(byte), 4, inFile);
        uint64_t signature = *(uint64_t*) _inBuffer;
    }
    
    while(ftell(outFile) < limit) {
        uint64_t signature = *(uint64_t*) _inBuffer;
        for (uint32_t i = 0; i < 64; i ++) {
            bool mode = (signature >> i) & 0x1;
            switch(mode) {
                case FALSE:
                    fwrite((_inBuffer + inPosition), sizeof(byte), 4, outFile);
                    //inPosition
                    break;
            }
        }
    }*/
    return TRUE;
}
