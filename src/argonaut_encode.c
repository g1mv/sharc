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
 * 18/09/13 14:57
 */

#include "argonaut_encode.h"

const primhufflook hl = {.lookup = HUFFMAN_PRIMARY_ENCODE};
const sechufflook shl = {.lookup = HUFFMAN_SECONDARY_ENCODE};

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_writeToSignature(sharc_argonaut_encode_state *state) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *(state->signature) |= ((uint64_t) 1) << state->shift;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    *(state->signature) |= ((uint64_t) 1) << ((56 - (state->shift & ~0x7)) + (state->shift & 0x7));
#endif
}*/

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_prepareNewBlock(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const uint_fast32_t minimumLookahead) {
    if (out->position + minimumLookahead > out->size)
        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    /*switch (state->signaturesCount) {
        case SHARC_PREFERRED_EFFICIENCY_CHECK_SIGNATURES:
            if (state->efficiencyChecked ^ 0x1) {
                state->efficiencyChecked = 1;
                return SHARC_KERNEL_ENCODE_STATE_INFO_EFFICIENCY_CHECK;
            }
            break;
        case SHARC_PREFERRED_BLOCK_SIGNATURES:
            state->signaturesCount = 0;
            state->efficiencyChecked = 0;
            return SHARC_KERNEL_ENCODE_STATE_INFO_NEW_BLOCK;
        default:
            break;
    }
    state->signaturesCount++;

    state->shift = 0;
    state->signature = (sharc_hash_signature *) (out->pointer + out->position);
    *state->signature = 0;*/

    state->block_read = 0;
    state->block_written = 0;
    //state->count ++;
    //out->position += sizeof(sharc_hash_signature) << 1;// + 4;//todo

    state->shift = 0;
    state->output = (uint64_t *) (out->pointer + out->position);
    state->partialOutput = 0;
    state->partialOutputBits = 0;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_checkState(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    SHARC_KERNEL_ENCODE_STATE returnState;

    switch (state->shift) {
        default:
            if ((returnState = sharc_argonaut_encode_prepareNewBlock(out, state, 8/*SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD*/))) {
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_NEW_BLOCK;
                return returnState;
            }
            break;
        case 0:
            break;
    }

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

/*SHARC_FORCE_INLINE bool compare(word *a, word *b) {
    if (a->length ^ b->length)
        return false;
    for (uint8_t compared = 0; compared < a->length; compared++)
        if (a->letters[compared] ^ b->letters[compared])
            return false;
    return true;
}*/

/*SHARC_FORCE_INLINE sharc_dictionary_primary_entry *swap(sharc_dictionary_primary_entry *a, sharc_dictionary_primary_entry *b) {
    if (a->previous)
        a->previous->next = b;
    b->previous = a->previous;
    if (b->next)
        b->next->previous = a;
    a->next = b->next;
    b->next = a;
    a->previous = b;
    b->rank -= 1;
    a->rank += 1;
    return a;
}*/

SHARC_FORCE_INLINE void one_round(const uint8_t *restrict letter, uint_fast32_t *restrict hash, sharc_argonaut_dictionary *restrict dictionary, uint_fast32_t *offset, uint8_t* restrict separator) {
    *hash = *hash ^ *letter;
    *hash = *hash * SHARC_HASH_PRIME;
    sharc_dictionary_primary_entry *match = &dictionary->prim[*letter];
    match->durability++;
    //if (match->isRanked) {
    *offset +=  hl.lookup[match->ranking].size; //size(match->rank);//
    //if (match->ranking) {
        uint16_t preceding = match->ranking - 1;
        sharc_dictionary_primary_entry *pointerPreceding3 = dictionary->ranking.topPrim[preceding];
        if (pointerPreceding3->durability < match->durability) {
            dictionary->ranking.topPrim[preceding] = match;
            dictionary->ranking.topPrim[match->ranking] = pointerPreceding3;
            match->ranking -= 1;
            if(!match->ranking)
                *separator = *letter;
            pointerPreceding3->ranking += 1;
        }
    //}

    /*if (match->previous) {
        if (match->durability > match->previous->durability) {
            //if (match == dictionary->ranking.lastPrimRanked)
            //    dictionary->ranking.lastPrimRanked = match->previous;
            swap(match->previous, match);
        }
    }*/

    /*} else {
        *offset += 16;
        sharc_dictionary_primary_entry *lastRanked = dictionary->ranking.lastPrimRanked;// dictionary->ranking.topPrim[preceding];

        if (match->durability > lastRanked->durability) {

            lastRanked->isRanked = false;
            lastRanked->previous->next = match;
            match->isRanked = true;
            match->previous = lastRanked->previous;
            match->next = NULL;
            match->rank = PRIM_RANKS - 1;
            dictionary->ranking.lastPrimRanked = match;
        }
    }*/
}

SHARC_FORCE_INLINE uint8_t byteSearch(uint_fast8_t *restrict letter, uint_fast32_t *restrict hash, word *restrict aword, const uint32_t xorMask, uint8_t* separator, sharc_argonaut_dictionary *restrict dictionary, uint_fast32_t *offset) {
    *hash = SHARC_HASH_OFFSET_BASIS ^ xorMask;
    if ((*letter = aword->letters[0]) ^ *separator) {
        one_round(letter, hash, dictionary, offset, separator);
        if ((*letter = aword->letters[1]) ^ *separator) {
            one_round(letter, hash, dictionary, offset, separator);
            if ((*letter = aword->letters[2]) ^ *separator) {
                one_round(letter, hash, dictionary, offset, separator);
                if ((*letter = aword->letters[3]) ^ *separator) {
                    one_round(letter, hash, dictionary, offset, separator);
                    if ((*letter = aword->letters[4]) ^ *separator) {
                        one_round(letter, hash, dictionary, offset, separator);
                        if ((*letter = aword->letters[5]) ^ *separator) {
                            one_round(letter, hash, dictionary, offset, separator);
                            if ((*letter = aword->letters[6]) ^ *separator) {
                                one_round(letter, hash, dictionary, offset, separator);
                                if ((*letter = aword->letters[7]) ^ *separator) {
                                    one_round(letter, hash, dictionary, offset, separator);
                                    return 8;
                                } else
                                    return 7;
                            } else
                                return 6;
                        } else
                            return 5;
                    } else
                        return 4;
                } else
                    return 3;
            } else
                return 2;
        } else
            return 1;
    } else
        return 0;
}

/*SHARC_FORCE_INLINE uint8_t sharc_transform(sharc_byte_buffer *restrict in, sharc_argonaut_dictionary *restrict dictionary, sharc_argonaut_encode_state *restrict state) {
    uint_fast64_t offset = 0;
    for (uint_fast64_t index = in->position;
    index < in->size;
    index++) {
        uint8_t read = *(in->pointer + index);
        sharc_dictionary_primary_entry *primHit = &dictionary->prim[read];

        if (primHit->ranked) {
            if (!(primHit->ranking & ~0x7)) {
                //aword.letters[majorOffset] |= (0x8 | (primHit->ranking - 1)) << minorOffset;
                offset += 1 + 3;
            } else if (!(primHit->ranking & ~0xF)) {
                //aword.letters[majorOffset] |= (0x10 | (primHit->ranking - 1)) << minorOffset;
                offset += 2 + 3;
            } else if (!(primHit->ranking & ~0x1F)) {
                //aword.letters[majorOffset] |= (0x20 | (primHit->ranking - 1)) << minorOffset;
                offset += 3 + 4;
            } else if (!(primHit->ranking & ~0x3F)) {
                //aword.letters[majorOffset] |= (0x40 | (primHit->ranking - 1)) << minorOffset;
                offset += 4 + 5;
            } else if (!(primHit->ranking & ~0x7F)) {
                //aword.letters[majorOffset] |= (0x80 | (primHit->ranking - 1)) << minorOffset;
                offset += 5 + 6;
            }
        } else {
            //aword.letters[offset >> 3] |= ((primHit->ranking - 1)) << minorOffset;
            offset += 5 + 7;
        }

        primHit->durability++;
        if (primHit->ranked) {
            if (primHit->ranking) {
                const uint8_t preceding = primHit->ranking - 1;
                sharc_dictionary_primary_entry *pointerPreceding1 = dictionary->ranking.topPrim[preceding];
                if (pointerPreceding1->durability < primHit->durability) {
                    dictionary->ranking.topPrim[preceding] = primHit;
                    dictionary->ranking.topPrim[primHit->ranking] = pointerPreceding1;
                    primHit->ranking -= 1;
                    pointerPreceding1->ranking += 1;
                }
            }
        } else {
            const uint8_t preceding = (1 << 7) - 1;
            sharc_dictionary_primary_entry *pointerPreceding2 = dictionary->ranking.topPrim[preceding];
            if (pointerPreceding2->durability < primHit->durability) {
                dictionary->ranking.topPrim[preceding] = primHit;
                primHit->ranking = preceding;
                primHit->ranked = true;
                pointerPreceding2->ranking = 0;
                pointerPreceding2->ranked = false;
            }
        }
        //uint16_t majorOffset = offset >> 3;
        //uint16_t minorOffset = offset ^ 0x7;
    }
    return dictionary->ranking.topPrim[0]->letter;
}*/

SHARC_FORCE_INLINE void sharc_argonaut_encode_kernel(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, uint_fast8_t *letter, uint_fast32_t *restrict hash, const uint32_t xorMask, sharc_argonaut_dictionary *restrict dictionary, sharc_argonaut_encode_state *restrict state, uint8_t* separator) {
    state->count[7]++;
    //const uint8_t separator = dictionary->ranking.topPrim[0]->letter;
    uint_fast64_t start = in->position;
    while (*(in->pointer + in->position) == *separator)
        in->position++;

    uint_fast64_t totl = in->position - start;
    dictionary->ranking.topPrim[0]->durability += totl;
    state->bitCount += 1 * totl;
    state->count[0] += totl;

    /*state->count[7] ++;    // bubble sort on huff
    if(state->count[7] & 0xFF) {
        for(uint8_t i = 0; i < 4; i ++) {
            if(state->countRank[i] < state->countRank[i + 1]) {
                int_fast64_t * temp = state->countRank[i];
                state->countRank[i] = state->countRank[i + 1];
                state->countRank[i + 1] = temp;
            }
        }
    }    */

    //if (!state->limit)
    //    state->limit = 32;

    //totl = 0;
    //uint32_t chunk = 0;
    //uint8_t read;
    //hash = offset_basis
    //for each octet_of_data to be hashed
    //hash = hash xor octet_of_data
    //hash = hash * FNV_prime
    //return hash

    word aword;// = {};
    aword.as_uint64_t = *(uint64_t *) (in->pointer + in->position);

    //while(aword.letters[totl] ^ 0x20 && totl < 8)
    //    totl ++;

    uint_fast32_t offset = 0;
    aword.length = byteSearch(letter, hash, &aword, xorMask, separator, dictionary, &offset);
    in->position += aword.length;
    if (aword.length ^ 0x8)
        aword.as_uint64_t &= ((((uint64_t) 1) << (aword.length << 3)) - 1);

    //SHARC_HASH_ALGORITHM_64(*hash, aword.as_uint64_t);

    /*for(uint8_t i = 0; i < aword.length; i ++) {
        *hash = *hash ^ aword.letters[i];
        *hash = *hash * SHARC_HASH_PRIME;
    }*/

    /**hash = SHARC_HASH_OFFSET_BASIS ^ xorMask;
    while ((read = *(in->pointer + in->position)) ^ 0x20 && totl < 10 && in->position < in->size) {  // best 10
        *hash = *hash ^ read;
        *hash = *hash * SHARC_HASH_PRIME;
        aword.letters[totl] = read;
        in->position++;
        totl++;
    }
    aword.length = totl;*/


    /*uint16_t offset = 0;
    //if (!(state->count[7] & 0x0)) {
        const uint8_t limit = aword.length - 1;
        for (uint8_t index = 0; index ^ limit; index++) {
            //__builtin_prefetch(&aword.letters[index + 4]);
            uint8_t read = aword.letters[index];
            sharc_dictionary_primary_entry *primHit = &dictionary->prim[read];

            primHit->durability++;

            if (primHit->isRanked) {
                //if (!(primHit->ranking & ~0x7)) {
                //aword.letters[majorOffset] |= (0x8 | (primHit->ranking - 1)) << minorOffset;
                //    offset += 1 + 3;
                //} else if (!(primHit->ranking & ~0xF)) {
                //aword.letters[majorOffset] |= (0x10 | (primHit->ranking - 1)) << minorOffset;
                //    offset += 2 + 3;
                //} else if (!(primHit->ranking & ~0x1F)) {
                //aword.letters[majorOffset] |= (0x20 | (primHit->ranking - 1)) << minorOffset;
                //    offset += 3 + 4;
                //} else if (!(primHit->ranking & ~0x3F)) {
                //aword.letters[majorOffset] |= (0x40 | (primHit->ranking - 1)) << minorOffset;
                //    offset += 4 + 5;
                //} else if (!(primHit->ranking & ~0x7F)) {
                //aword.letters[majorOffset] |= (0x80 | (primHit->ranking - 1)) << minorOffset;
                //    offset += 5 + 6;
                //}
                offset += 6;//(0x80 - primHit->ranking) / 32 + 3;

                if (primHit->ranking) {
                    sharc_dictionary_primary_entry *pointerPreceding1 = primHit->preceding;// dictionary->ranking.topPrim[preceding];
                    if (pointerPreceding1->durability < primHit->durability) {
                        //const uint8_t preceding = primHit->ranking - 1;
                        //dictionary->ranking.topPrim[preceding] = primHit;
                        //dictionary->ranking.topPrim[primHit->ranking] = pointerPreceding1;
                        //const uint8_t r = primHit->ranking;
                        primHit->ranking = pointerPreceding1->ranking;
                        pointerPreceding1->ranking += 1;
                        primHit->preceding = pointerPreceding1->preceding;// dictionary->ranking.topPrim[preceding - 1];
                        pointerPreceding1->preceding = primHit;
                        if(primHit == dictionary->ranking.lastPrimRanked)
                            dictionary->ranking.lastPrimRanked = pointerPreceding1;
                    }
                }
            } else {
                //aword.letters[offset >> 3] |= ((primHit->ranking - 1)) << minorOffset;
                offset += 5 + 7;
                //const uint8_t preceding = PRIM_RANKS - 1;
                sharc_dictionary_primary_entry *pointerPreceding2 = dictionary->ranking.lastPrimRanked;// dictionary->ranking.topPrim[preceding];
                if (pointerPreceding2->durability < primHit->durability) {
                    //dictionary->ranking.topPrim[preceding] = primHit;
                    primHit->ranking = pointerPreceding2->ranking;// preceding;
                    primHit->isRanked = true;
                    primHit->preceding = pointerPreceding2->preceding;// dictionary->ranking.topPrim[preceding - 1];
                    //pointerPreceding2->ranking = 0;
                    pointerPreceding2->isRanked = false;
                    //pointerPreceding2->preceding = NULL;
                }
            }

            //uint16_t majorOffset = offset >> 3;
            //uint16_t minorOffset = offset ^ 0x7;
        }*/
    aword.compressedLength = (uint8_t) (round((float) offset / 8.0));//aword.length;// aword.length;//
    //} else
    //    aword.compressedLength = aword.length;
    //printf("[%s], %u, %u, %llu\n", aword.letters, aword.length, aword.compressedLength, *hash);
    //fflush(stdout);

    /*if(totl == state->limit) {
        state->overflows ++;
        if(state->overflows == 8)
            state->limit = 4;
    }*/
    //uint32_t chunk = *(uint32_t*)(in->pointer + in->position);
    //in->position += 4;


    //printf("[%s]\n", (char*)&chunk);
    //fflush(stdout);

    //SHARC_HASH_ALGORITHM(*hash, SHARC_LITTLE_ENDIAN_32(chunk), xorMask);

    const uint8_t bits = 16;
    uint_fast32_t shash = (uint_fast32_t) ((((*hash >> bits) ^ *hash) & ((1 << bits) - 1)));
    sharc_dictionary_qentry *secondaryHit = &dictionary->quad[shash];
    if (aword.as_uint64_t ^ secondaryHit->aword.as_uint64_t /*!compare(&aword, &secondaryHit->aword)*/) {
        if (secondaryHit->durability)
            secondaryHit->durability--;
        else {
            //memcpy(&secondaryHit->aword.as_uint64_t, &aword, aword.length);
            secondaryHit->aword.as_uint64_t = aword.as_uint64_t;
            secondaryHit->aword.length = aword.length;
            secondaryHit->aword.compressedLength = aword.compressedLength;
        }
        *(uint64_t *) (out->pointer + out->position) = 0;//0x80000000 | (chunk >> 2);
        out->position += sizeof(uint64_t);
        //if(aword.compressedLength < aword.length)
        state->bitCount += (aword.compressedLength << 3) + 3/* + 1*/;
        //else
        //    state->bitCount += (aword.length << 3) + 3 + 1;
        state->count[1]++;
        goto exit;
    } else {
        if (secondaryHit->ranked) {
            /*if (secondaryHit->ranking <= 16) {
                state->bitCount += 4 + 4;
                state->count[2]++;
            } else {
                state->bitCount += 8 + 4;
                state->count[3]++;
            }*/
            state->bitCount += shl.lookup[secondaryHit->ranking - 1].size;
        } else {
            state->bitCount += bits + 2;
            state->count[4]++;
        }

        secondaryHit->durability++;
        if (secondaryHit->ranked) {
            if (secondaryHit->ranking) {
                uint16_t preceding = secondaryHit->ranking - 1;
                sharc_dictionary_qentry *pointerPreceding3 = dictionary->ranking.top[preceding];
                if (pointerPreceding3->durability < secondaryHit->durability) {
                    dictionary->ranking.top[preceding] = secondaryHit;
                    dictionary->ranking.top[secondaryHit->ranking] = pointerPreceding3;
                    secondaryHit->ranking -= 1;
                    pointerPreceding3->ranking += 1;
                }
            }
        } else {
            const uint16_t preceding = SEC_RANKS - 1;
            sharc_dictionary_qentry *pointerPreceding4 = dictionary->ranking.top[preceding];
            if (pointerPreceding4->durability < secondaryHit->durability) {
                dictionary->ranking.top[preceding] = secondaryHit;
                secondaryHit->ranking = preceding;
                secondaryHit->ranked = true;
                pointerPreceding4->ranking = 0;
                pointerPreceding4->ranked = false;
            }
        }

        //sharc_argonaut_encode_writeToSignature(state);
        *(uint16_t *) (out->pointer + out->position) = SHARC_LITTLE_ENDIAN_16(shash);// SHARC_LITTLE_ENDIAN_16(0x40000000 | shash);
        out->position += sizeof(uint16_t);
        goto exit;
    }

    exit:
    //state->previous_chunk = chunk;
    state->shift++;
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_process_chunk(uint64_t *chunk, sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, uint32_t *restrict hash, const uint32_t xorMask, sharc_argonaut_dictionary *restrict dictionary, sharc_argonaut_encode_state *restrict state) {
    *chunk = *(uint64_t *) (in->pointer + in->position);
    /*sharc_hash_encode_kernel(out, hash, (uint16_t) (*chunk & 0xFFFF), xorMask, dictionary, state);
    sharc_hash_encode_kernel(out, hash, (uint16_t) ((*chunk & 0xFFFF0000) >> 16), xorMask, dictionary, state);
    sharc_hash_encode_kernel(out, hash, (uint16_t) ((*chunk & 0xFFFF00000000) >> 32), xorMask, dictionary, state);
    sharc_hash_encode_kernel(out, hash, (uint16_t) (*chunk >> 48), xorMask, dictionary, state);*/
/*#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    sharc_hash_encode_kernel(in, out, hash, (uint32_t) (*chunk & 0xFFFFFFFF), xorMask, dictionary, state);
#endif
    sharc_hash_encode_kernel(in, out, hash, (uint32_t) (*chunk >> 32), xorMask, dictionary, state);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    sharc_hash_encode_kernel(in, out, hash, (uint32_t) (*chunk & 0xFFFFFFFF), xorMask, dictionary, state);
#endif*/
    //sharc_hash_encode_kernel(out, hash, *chunk, xorMask, dictionary, state);
    //in->position += sizeof(uint64_t);
}

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_process_span(uint64_t *chunk, sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, uint_fast32_t *restrict hash, const uint32_t xorMask, sharc_argonaut_dictionary *restrict dictionary, sharc_argonaut_encode_state *restrict state, const uint8_t separator) {
    //sharc_hash_encode_process_chunk(chunk, in, out, hash, xorMask, dictionary, state);
    //sharc_hash_encode_process_chunk(chunk, in, out, hash, xorMask, dictionary, state);
    //sharc_hash_encode_process_chunk(chunk, in, out, hash, xorMask, dictionary, state);
    //sharc_hash_encode_process_chunk(chunk, in, out, hash, xorMask, dictionary, state);
    sharc_argonaut_encode_kernel(in, out, hash, xorMask, dictionary, state, separator);
}*/

SHARC_FORCE_INLINE sharc_bool sharc_argonaut_encode_attempt_copy(sharc_byte_buffer *restrict out, sharc_byte *restrict origin, const uint_fast32_t count) {
    if (out->position + count <= out->size) {
        memcpy(out->pointer + out->position, origin, count);
        out->position += count;
        return false;
    }
    return true;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_init(sharc_argonaut_encode_state *state, sharc_argonaut_dictionary *restrict dictionary) {
    state->signaturesCount = 0;
    state->efficiencyChecked = 0;

    state->process = SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_NEW_BLOCK;

    /*for(uint8_t i = 0; i < 8; i ++)
        state->countRank[i] = &state->count[i];

    state->huff[0] = 1;
    state->huff[1] = 2;
    state->huff[2] = 3;
    state->huff[3] = 4;
    state->huff[4] = 4;*/

    for (uint16_t i = 0; i < SEC_RANKS; i++) {
        dictionary->ranking.top[i] = &dictionary->quad[i];
        dictionary->quad[i].ranking = i;
        dictionary->quad[i].ranked = true;
    }
    for (uint16_t i = 0; i < (1 << 8); i++) {
        dictionary->prim[i].letter = (uint8_t) (i);
        //dictionary->prim[i].isRanked = false;
        //dictionary->prim[i].next = NULL;
        //dictionary->prim[i].previous = NULL;
    }
    for (uint16_t i = 0; i < PRIM_RANKS; i++) {
        dictionary->ranking.topPrim[i] = &dictionary->prim[i];
        //dictionary->ranking.topPrim[i] = &dictionary->prim[i];
        //dictionary->prim[i].ranking = (uint8_t) (i);
        //dictionary->prim[i].isRanked = true;
        dictionary->prim[i].ranking = (uint8_t) i;
        /*if (i)
            dictionary->prim[i].previous = &dictionary->prim[i - 1];
        if (i < PRIM_RANKS - 1)
            dictionary->prim[i].next = &dictionary->prim[i + 1];*/
    }
    //dictionary->ranking.lastPrimRanked = &dictionary->prim[PRIM_RANKS - 1];

    //for(uint_fast16_t i = 0; i < (1<<8); i ++)
    //    printf("%u => %c, %u, %u, -> %c\n", i, dictionary->prim[i].letter, dictionary->prim[i].durability, dictionary->prim[i].ranking, dictionary->prim[i].ranking ? dictionary->prim[i].preceding->letter : 0);

    uint32_t workSize = ((SHARC_PREFERRED_BUFFER_SIZE * 3) >> 1);
    sharc_byte_buffer_encapsulate(&state->workBuffer, malloc((size_t) workSize), workSize);

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, const uint32_t xorMask, sharc_argonaut_dictionary *restrict dictionary, sharc_argonaut_encode_state *restrict state, const sharc_bool flush) {
    SHARC_KERNEL_ENCODE_STATE returnState;
    uint_fast8_t letter;
    uint_fast32_t hash;
    uint_fast64_t remaining;

    uint8_t separator = dictionary->ranking.topPrim[0]->letter;

    if (in->size == 0)
        goto exit;

    const uint_fast64_t limit = in->size & ~0x1F;

    switch (state->process) {
        case SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_STATE:
            if ((returnState = sharc_argonaut_encode_checkState(out, state)))
                return returnState;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_DATA;
            break;

        case SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_NEW_BLOCK:
            if ((returnState = sharc_argonaut_encode_prepareNewBlock(out, state, 8/*SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD*/)))
                return returnState;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_DATA;
            break;

        case SHARC_ARGONAUT_ENCODE_PROCESS_DATA:
            //if (in->size - in->position < 4 * sizeof(uint64_t))
            //    goto finish;
            //separator = sharc_transform(in, dictionary, state);
            while (true) {
                sharc_argonaut_encode_kernel(in, out, &letter, &hash, xorMask, dictionary, state, &separator);
                if (in->position >= limit - 4) {
                    if (flush) {
                        state->process = SHARC_ARGONAUT_ENCODE_PROCESS_FINISH;
                        return SHARC_KERNEL_ENCODE_STATE_READY;
                    } else {
                        state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_STATE;
                        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
                    }
                }

                if ((returnState = sharc_argonaut_encode_checkState(out, state)))
                    return returnState;
            }

        case SHARC_ARGONAUT_ENCODE_PROCESS_FINISH:
            while (true) {
                while (state->shift ^ 64) {
                    if (in->size - in->position < sizeof(uint32_t))
                        goto finish;
                    else {
                        if (out->size - out->position < sizeof(uint32_t))
                            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                        sharc_argonaut_encode_kernel(in, out, &letter, &hash, xorMask, dictionary, state, &separator);
                        //in->position += sizeof(uint32_t);
                    }
                }
                if (in->size - in->position < sizeof(uint32_t))
                    goto finish;
                else if ((returnState = sharc_argonaut_encode_prepareNewBlock(out, state, sizeof(sharc_hash_signature) << 1)))//todo
                    return returnState;
            }
        finish:
            /*for(uint32_t i = 0; i < (1 << 19); i ++)
                if(dictionary->sec[i].durability > 10000)
                    printf("%u,", dictionary->sec[i].durability);*/
            //for (uint_fast16_t i = 0; i < (1 << 8); i++)
            //    printf("%u => %c, %u, %u\n", i, dictionary->prim[i].letter, dictionary->prim[i].durability, dictionary->prim[i].rank);

            printf("%llu\n", state->bitCount / 8);
            for (uint8_t i = 0; i < 8; i++)
                printf("%llu, ", state->count[i]);
            printf("\n");

            remaining = in->size - in->position;
            if (remaining > 0) {
                if (sharc_argonaut_encode_attempt_copy(out, in->pointer + in->position, (uint32_t) remaining))
                    return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                in->position += remaining;
            }
        exit:
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_NEW_BLOCK;
            return SHARC_KERNEL_ENCODE_STATE_FINISHED;

        default:
            return SHARC_KERNEL_ENCODE_STATE_ERROR;
    }

    return
            SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_finish(sharc_argonaut_encode_state *state) {
    return SHARC_KERNEL_ENCODE_STATE_READY;
}
