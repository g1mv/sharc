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
 * 01/06/13 17:31
 */

#include "hash_cipher.h"

SHARC_FORCE_INLINE void writeSignature(uint64_t* signature, const byte* state) {
    *signature |= ((uint64_t)1) << *state;
}

SHARC_FORCE_INLINE void flush(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, const uint64_t* signature, const byte* state, const uint32_t* signaturePointer) {
    *(uint64_t*)(out->pointer + *signaturePointer) = SHARC_LITTLE_ENDIAN_64(*signature);
    in->position += (*state << 2);
}

SHARC_FORCE_INLINE bool reset(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, uint64_t* signature, byte* state, uint32_t* signaturePointer) {
    *signaturePointer = out->position;
    out->position += 8;
    if((out->position + 256) > in->size)
        return SHARC_FALSE;
    *state = 0;
    *signature = 0;
    return SHARC_TRUE;
}

SHARC_FORCE_INLINE void resetDictionary(SHARC_ENTRY* dictionary) {
    for(uint32_t i = 0; i < (1 << SHARC_HASH_BITS); i ++)
        *(uint32_t*)&dictionary[i] = 0;
}

SHARC_FORCE_INLINE bool checkState(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, uint64_t* signature, byte* state, uint32_t* signaturePointer) {
    switch(*state) {
        case 64:
            flush(in, out, signature, state, signaturePointer);
            if(reset(in, out, signature, state, signaturePointer) ^ 0x1)
                return SHARC_FALSE;
            break;
    }
    return SHARC_TRUE;
}

SHARC_FORCE_INLINE void computeHash(uint32_t* hash, const uint32_t value, const uint32_t xorMask) {
    *hash = SHARC_HASH_OFFSET_BASIS;
    *hash ^= (value ^ xorMask);
    *hash *= SHARC_HASH_PRIME;
    *hash = (*hash >> (32 - SHARC_HASH_BITS)) ^ (*hash & 0xFFFF);
}

SHARC_FORCE_INLINE bool updateEntry(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, SHARC_ENTRY* entry, const uint32_t chunk, const uint32_t index, uint64_t* signature, byte* state, uint32_t* signaturePointer) {
    *(uint32_t*)entry = (index & 0xFFFFFF) | SHARC_MAX_BUFFER_REFERENCES;
    *(uint32_t*)(out->pointer + out->position) = chunk;
    out->position += 4;
    *state = *state + 1;
    return checkState(in, out, signature, state, signaturePointer);
}

SHARC_FORCE_INLINE bool kernelEncode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, const uint32_t chunk, const uint32_t xorMask, const uint32_t* buffer, const uint32_t index, SHARC_ENTRY* dictionary, uint32_t* hash, uint64_t* signature, byte* state, uint32_t* signaturePointer) {
    computeHash(hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
    SHARC_ENTRY* found = &dictionary[*hash];
    if((*(uint32_t*)found) & SHARC_MAX_BUFFER_REFERENCES) {
        if(chunk ^ buffer[*(uint32_t*)found & 0xFFFFFF]) {
            if(updateEntry(in, out, found, chunk, index, signature, state, signaturePointer) ^ 0x1)
                return SHARC_FALSE;
        } else {
            writeSignature(signature, state);
            *(uint16_t*)(out->pointer + out->position) = SHARC_LITTLE_ENDIAN_16(*hash);
            out->position += 2;
            *state = *state + 1;
            if(checkState(in, out, signature, state, signaturePointer) ^ 0x1)
                return SHARC_FALSE;
        }
    } else {
        if(updateEntry(in, out, found, chunk, index, signature, state, signaturePointer) ^ 0x1)
            return SHARC_FALSE;
    }
    return SHARC_TRUE;
}

SHARC_FORCE_INLINE bool hashEncode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, const uint32_t xorMask) {
    SHARC_ENTRY dictionary[1 << SHARC_HASH_BITS];
    uint64_t signature;
    uint32_t signaturePointer;
    byte state;
    uint32_t hash;
    
    reset(in, out, &signature, &state, &signaturePointer);
    resetDictionary(dictionary);
    
    const uint32_t* intInBuffer = (const uint32_t*)in->pointer;
    const uint32_t intInSize = in->size >> 2;
    
    for(uint32_t i = 0; i < intInSize; i ++)
        if(kernelEncode(in, out, intInBuffer[i], xorMask, intInBuffer, i, dictionary, &hash, &signature, &state, &signaturePointer) ^ 0x1)
            return SHARC_FALSE;
    
    if(state > 0)
        flush(in, out, &signature, &state, &signaturePointer);
    else
        out->position -= 8;
    
    const uint32_t remaining = in->size - in->position;
    for(uint32_t i = 0; i < remaining; i ++) {
        if(out->position < out->size)
            out->pointer[out->position ++] = in->pointer[in->position ++];
        else
            return SHARC_FALSE;
    }
    
    return SHARC_TRUE;
}

SHARC_FORCE_INLINE void kernelDecode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, SHARC_ENTRY* dictionary, const uint32_t xorMask, const bool mode) {
    uint32_t hash;
    uint32_t chunk;
    SHARC_ENTRY* found;
    switch(mode) {
        case SHARC_FALSE:
            chunk = /*SHARC_LITTLE_ENDIAN_32(*/ *(uint32_t*)(in->pointer + in->position)/*)*/;
            computeHash(&hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);
            *(uint32_t*)&dictionary[hash] = ((out->position >> 2) & 0xFFFFFF) | SHARC_MAX_BUFFER_REFERENCES;
            *(uint32_t*)(out->pointer + out->position) = chunk;
            in->position += 4;
            break;
        case SHARC_TRUE:
            found = &dictionary[SHARC_LITTLE_ENDIAN_16(*(uint16_t*)(in->pointer + in->position))];
            *(uint32_t*)(out->pointer + out->position) = /*SHARC_LITTLE_ENDIAN_32(*/ *(uint32_t*)(out->pointer + ((*(uint32_t*)found & 0xFFFFFF) << 2))/*)*/;
            in->position += 2;
            break;
    }
    out->position += 4;
}

SHARC_FORCE_INLINE void byteCopy(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, const uint32_t number) {
    for(byte r = 0; r < number; r++)
        out->pointer[out->position ++] = in->pointer[in->position ++];
}

SHARC_FORCE_INLINE bool hashDecode(SHARC_BYTE_BUFFER* in, SHARC_BYTE_BUFFER* out, const uint32_t xorMask) {
    SHARC_ENTRY dictionary[1 << SHARC_HASH_BITS];
    
    resetDictionary(dictionary);
    
    uint64_t signature;
    byte i;
    
    while(in->position <= in->size - 10 - 8 - 256) {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t*)(in->pointer + in->position));
        in->position += 8;
        for (i = 0; i < 0x40; i ++)
            kernelDecode(in, out, dictionary, xorMask, (signature >> i) & 0x1);
    }

    do {
        signature = SHARC_LITTLE_ENDIAN_64(*(uint64_t*)(in->pointer + in->position));
        in->position += 8;
        i = 0;
        while((in->size - in->position) & 0xFFFFFFFC && i < 0x40)
            kernelDecode(in, out, dictionary, xorMask, (signature >> (i ++)) & 0x1);
    } while(in->position <= in->size - 10);
    
    const uint32_t remaining = in->size - in->position;
    if(i & 0x40)
        byteCopy(in, out, remaining);
    else {
        if((signature >> i) & 0x1) {
            if(remaining & 0x2)
                kernelDecode(in, out, dictionary, xorMask, SHARC_TRUE);
            else
                error("Corrupted file !");
            if(remaining == 0x3)
                byteCopy(in, out, 1);
        } else
            byteCopy(in, out, remaining);
    }
    
    return SHARC_TRUE;
}
