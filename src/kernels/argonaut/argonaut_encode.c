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

#define BLOCK   512

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
const sharc_argonaut_primary_code_lookup ARGONAUT_NAME(hl) = {.code = SHARC_ARGONAUT_PRIMARY_HUFFMAN_CODES};
//const sharc_argonaut_secondary_code_lookup ARGONAUT_NAME(h2l) = {.code = SHARC_ARGONAUT_PRIMARY_HUFFMAN_CODES_2};
#endif
//const sharc_argonaut_secondary_code_lookup ARGONAUT_NAME(shl) = {.code = SHARC_ARGONAUT_SECONDARY_HUFFMAN_CODES};
//const sharc_argonaut_word_length_code_lookup ARGONAUT_NAME(wlhl) = {.code = SHARC_ARGONAUT_WORD_LENGTH_HUFFMAN_CODES};
//const sharc_argonaut_entity_code_lookup ARGONAUT_NAME(ehl) = {.code = SHARC_ARGONAUT_ENTITY_HUFFMAN_CODES};

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_write_to_signature(sharc_argonaut_encode_state *state, uint_fast8_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *(state->signature) |= ((uint64_t) value) << state->shift;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    *(state->signature) |= ((uint64_t) value) << ((56 - (state->shift & ~0x7)) + (state->shift & 0x7));
#endif
}*/

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_prepare_new_block(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const uint_fast32_t minimumLookahead) {
    out->position += (state->shift >> 3);
    state->shift &= 0x7llu;
    if (out->position + minimumLookahead > out->size)
        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
    state->count = BLOCK;

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
            
            if (state->resetCycle)
                state->resetCycle--;
            else {
                // todo ?
                state->resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;
            }
            
            return SHARC_KERNEL_ENCODE_STATE_INFO_NEW_BLOCK;
        default:
            break;
    }
    state->signaturesCount++;
    
    state->signatureShift = 0;
    state->signature = (sharc_argonaut_signature *) (out->pointer + out->position);
    *state->signature = 0;
    out->position += sizeof(sharc_argonaut_signature);*/

    //tate->output = (sharc_argonaut_signature *) (out->pointer + out->position);
    //state->anchor = out->pointer + out->position;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

/*SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_check_state(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    SHARC_KERNEL_ENCODE_STATE returnState;
    
    switch (state->signatureShift) {
        case 64:
            if ((returnState = sharc_argonaut_encode_prepare_new_block(out, state, SHARC_HASH_ENCODE_MINIMUM_OUTPUT_LOOKAHEAD))) {
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_NEW_BLOCK;
                return returnState;
            }
            break;
        default:
            break;
    }
    
    return SHARC_KERNEL_ENCODE_STATE_READY;
}*/

/*SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_check_next_output_block_memory_available(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    if (out->position + 24 > out->size)
        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    //state->blockShift = 0;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}*/

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_prepare_next_output_unit_even(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    state->shift = 0;

    state->output = (sharc_argonaut_output_unit *) (out->pointer + out->position);
    *(state->output) = 0;
    out->position += sizeof(sharc_argonaut_output_unit);
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_prepare_next_output_unit(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const uint_fast64_t data, const uint_fast8_t bitSize) {
    state->shift = (uint8_t) (state->shift - SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE);

    state->output = (sharc_argonaut_output_unit *) (out->pointer + out->position);
    *(state->output) = (sharc_argonaut_output_unit) (data >> (bitSize - state->shift));
    out->position += sizeof(sharc_argonaut_output_unit);
}*/

SHARC_FORCE_INLINE void sharc_argonaut_encode_write_to_output(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *state, const uint_fast32_t value, const uint_fast8_t bitSize) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *(uint_fast32_t *) (out->pointer + out->position + (state->shift >> 3)) += ((/*(uint_fast64_t)*/ value) << (state->shift & 0x7)); // 18 bits max << 8 = 26
    //*(state->output) |= value << state->shift;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    *(state->output) |= value << ((56 - (state->shift & ~0x7)) + (state->shift & 0x7));
#endif
    state->shift += bitSize;
}

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_write_coded_separator(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    sharc_argonaut_encode_write_to_output(state, ARGONAUT_NAME(ehl).code[SHARC_ARGONAUT_ENTITY_SEPARATOR].code);

    state->shift++;
    if (state->shift == SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE)
        sharc_argonaut_encode_prepare_next_output_unit_even(out, state);
}*/

/*SHARC_FORCE_INLINE void sharc_argonaut_encode_write(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const uint_fast64_t data, const uint_fast8_t bitSize) {
    sharc_argonaut_encode_write_to_output(state, data);

    state->shift += bitSize;
    //if (state->shift & ~(SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE - 1))
    //    sharc_argonaut_encode_prepare_next_output_unit(out, state, data, bitSize);
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_write_coded(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const sharc_argonaut_huffman_code *restrict code) {
    sharc_argonaut_encode_write(out, state, code->code, code->bitSize);
}*/

SHARC_FORCE_INLINE void sharc_argonaut_encode_process_letter(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, sharc_argonaut_dictionary_word *restrict word, const uint8_t *restrict letter, const uint8_t index, uint_fast32_t *restrict hash, uint8_t *restrict separator) {
    //*hash = *hash ^ *letter;
    //*hash = *hash * SHARC_ARGONAUT_HASH_PRIME;

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    /*sharc_argonaut_dictionary_primary_entry *match = &state->dictionary.primary_entry[*letter];

    const uint8_t rank = match->ranking;
    const uint8_t precedingRank = match->ranking - 1;

    const sharc_argonaut_huffman_code *huffmanCode = &ARGONAUT_NAME(hl).code[rank];
    word->letterCode[index] = huffmanCode;

    match->durability++;
    sharc_argonaut_dictionary_primary_entry *preceding_match = state->dictionary.ranking.primary[precedingRank];
    if (preceding_match->durability < match->durability) {
        state->dictionary.ranking.primary[precedingRank] = match;
        state->dictionary.ranking.primary[rank] = preceding_match;
        match->ranking -= 1;
        if (!match->ranking)
            *separator = *letter;
        preceding_match->ranking += 1;
    }*/
#endif
}

/*SHARC_FORCE_INLINE uint8_t sharc_argonaut_encode_advance_to_separator(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint_fast8_t *restrict letter, uint_fast32_t *restrict hash, sharc_argonaut_dictionary_word *restrict word, uint8_t *separator) {
    *hash = SHARC_ARGONAUT_HASH_OFFSET_BASIS;
    if ((*letter = word->letters[0]) ^ *separator) {
        sharc_argonaut_encode_process_letter(out, state, word, letter, 0, hash, separator);
        if ((*letter = word->letters[1]) ^ *separator) {
            sharc_argonaut_encode_process_letter(out, state, word, letter, 1, hash, separator);
            if ((*letter = word->letters[2]) ^ *separator) {
                sharc_argonaut_encode_process_letter(out, state, word, letter, 2, hash, separator);
                if ((*letter = word->letters[3]) ^ *separator) {
                    sharc_argonaut_encode_process_letter(out, state, word, letter, 3, hash, separator);
                    if ((*letter = word->letters[4]) ^ *separator) {
                        sharc_argonaut_encode_process_letter(out, state, word, letter, 4, hash, separator);
                        if ((*letter = word->letters[5]) ^ *separator) {
                            sharc_argonaut_encode_process_letter(out, state, word, letter, 5, hash, separator);
                            if ((*letter = word->letters[6]) ^ *separator) {
                                sharc_argonaut_encode_process_letter(out, state, word, letter, 6, hash, separator);
                                if ((*letter = word->letters[7]) ^ *separator) {
                                    sharc_argonaut_encode_process_letter(out, state, word, letter, 7, hash, separator);
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

SHARC_FORCE_INLINE uint8_t sharc_argonaut_encode_find_first_separator_position_limited(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint_fast8_t start, uint_fast8_t stop, uint_fast8_t *restrict letter, uint_fast32_t *restrict hash, sharc_argonaut_dictionary_word *restrict word, uint8_t *separator) {
    if (!start)
        *hash = SHARC_ARGONAUT_HASH_OFFSET_BASIS;
    if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
        start++;
        sharc_argonaut_encode_process_letter(out, state, word, letter, 0, hash, separator);
        if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
            start++;
            sharc_argonaut_encode_process_letter(out, state, word, letter, 1, hash, separator);
            if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                start++;
                sharc_argonaut_encode_process_letter(out, state, word, letter, 2, hash, separator);
                if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                    start++;
                    sharc_argonaut_encode_process_letter(out, state, word, letter, 3, hash, separator);
                    if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                        start++;
                        sharc_argonaut_encode_process_letter(out, state, word, letter, 4, hash, separator);
                        if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                            start++;
                            sharc_argonaut_encode_process_letter(out, state, word, letter, 5, hash, separator);
                            if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                                start++;
                                sharc_argonaut_encode_process_letter(out, state, word, letter, 6, hash, separator);
                                if ((start != stop) && (*letter = word->letters[start]) ^ *separator) {
                                    sharc_argonaut_encode_process_letter(out, state, word, letter, 7, hash, separator);
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
}*/

SHARC_FORCE_INLINE uint_fast8_t sharc_argonaut_encode_find_position(uint64_t chain) {
    if (chain) {
        uint_fast32_t word_beginning = (uint_fast32_t) (chain & 0xFFFFFFFF);
        if (word_beginning)
            return __builtin_ctz(word_beginning) >> 3;
        else {
            uint_fast32_t word_end = (uint_fast32_t) (chain >> 32);
            return 4 + (__builtin_ctz(word_end) >> 3);
        }
    } else
        return (uint_fast8_t) sizeof(uint64_t);
}

SHARC_FORCE_INLINE uint_fast8_t sharc_argonaut_encode_find_first_separator_position(uint64_t *restrict chain, uint8_t *restrict separator) {
    return sharc_argonaut_encode_find_position(sharc_argonaut_contains_value(*chain, *separator));
}

/*SHARC_FORCE_INLINE uint_fast8_t sharc_argonaut_find_first_non_separator_position(uint64_t *restrict chain, uint8_t *restrict separator) {
    return sharc_argonaut_encode_find_position(sharc_argonaut_contains_value(*chain, *separator) ^ 0x8080808080808080llu);
}*/

SHARC_FORCE_INLINE uint_fast8_t sharc_argonaut_encode_add_letters_until_separator_limited(sharc_byte_buffer *restrict in, uint8_t *restrict separator, uint_fast64_t limit) {
    uint_fast64_t start = in->position;
    while (limit--) {
        if (*(in->pointer + in->position) == *separator)
            return (uint_fast8_t) (in->position - start);
        in->position++;
    }
    return (uint_fast8_t) (in->position - start);
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_find_next_word(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint8_t *separator) {
    while (*(in->pointer + in->position) == *separator) {
        /*if (unlikely(out->position + (state->shift >> 3) == out->size - 1))
            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;*/
        state->shift += 2;
        state->dictionary.ranking.primary[0]->durability++;
        in->position++;
        if (unlikely(in->position == in->size))
            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
        /*if(unlikely(1 == state->count --))
            return SHARC_KERNEL_ENCODE_STATE_READY;*/
    }
    //if (unlikely(out->size - out->position < 24))
    //    return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process_word(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, uint_fast8_t *letter, uint_fast32_t *restrict hash, sharc_argonaut_encode_state *restrict state, uint8_t *separator) {
    /*if (unlikely(out->position + (state->shift >> 3) > out->size - 24))
        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;*/
    if (unlikely(state->word.length)) {
        state->word.as_uint64_t |= ((*(uint64_t *) (in->pointer + in->position)) << state->word.length);
        const uint8_t addedLength = sharc_argonaut_encode_add_letters_until_separator_limited(in, separator, (uint_fast64_t) (SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS - state->word.length));
        state->word.length += addedLength;
    } else {
        state->word.as_uint64_t = *(uint64_t *) (in->pointer + in->position);
        if (likely(in->size - in->position > SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS)) {
            state->word.length = sharc_argonaut_encode_find_first_separator_position(&state->word.as_uint64_t, separator);
            in->position += state->word.length;
        } else {
            const uint8_t remaining = (uint8_t) (in->size - in->position);
            state->word.length = sharc_argonaut_encode_add_letters_until_separator_limited(in, separator, remaining/*in->size*/);
            if (state->word.length == remaining) {
                state->word.as_uint64_t &= ((((uint64_t) 1) << (state->word.length << 3)) - 1);
                return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
            }
        }
    }

    const uint_fast8_t wordLength = state->word.length;
    if (wordLength ^ SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS)
        state->word.as_uint64_t &= ((((uint64_t) 1) << (wordLength << 3)) - 1);

    uint_fast64_t h = 14695981039346656037llu;
    h ^= state->word.as_uint64_t;
    h *= 1099511628211llu;
    uint_fast32_t xorfold = (uint_fast32_t) ((h >> 32) ^ h);
    uint_fast16_t shash = (uint_fast16_t) ((xorfold >> 16) ^ xorfold);

    sharc_argonaut_dictionary_secondary_entry *match = &state->dictionary.secondary_entry[shash];
    if (state->word.as_uint64_t != match->word.as_uint64_t) {
        if (match->durability)
            match->durability--;
        else {
            match->word.as_uint64_t = state->word.as_uint64_t;
        }
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
        sharc_argonaut_encode_write_to_output(out, state, 0x1, 2);
        sharc_argonaut_encode_write_to_output(out, state, wordLength, 3);
        /*for (uint8_t i = 0; i < wordLength - 1; i += 2) {
            uint16_t diword = state->word.letters[i] + (((uint16_t)state->word.letters[i + 1]) << 8);
            sharc_argonaut_dictionary_tertiary_entry *m = &state->dictionary.tertiary_entry[diword];

            const uint16_t r = m->ranking;

            //const sharc_argonaut_huffman_code *huffmanCode = &ARGONAUT_NAME(hl).code[r];
            sharc_argonaut_encode_write_to_output(out, state, 0x12345678, 9);

            m->durability++;
            if (r) {
                const uint16_t pr = m->ranking - 1;
                sharc_argonaut_dictionary_tertiary_entry *pm = state->dictionary.ranking.tertiary[pr];
                if (unlikely(pm->durability < m->durability)) { // todo unlikely after a while
                    state->dictionary.ranking.tertiary[pr] = m;
                    state->dictionary.ranking.tertiary[r] = pm;
                    m->ranking -= 1;
                    pm->ranking += 1;
                }
            }
        }*/
        //uint_fast8_t rk = 0;
        //uint_fast8_t prev_rk = 0;
        //uint_fast8_t i;
        //const uint8_t limit = wordLength - 1;
        for (uint_fast8_t i = 0; i != wordLength; i ++) {
            *letter = state->word.letters[i];
            sharc_argonaut_dictionary_primary_entry *m = &state->dictionary.primary_entry[*letter];

            const uint_fast8_t rk = m->ranking;

            const sharc_argonaut_huffman_code *huffmanCode = &ARGONAUT_NAME(hl).code[rk];
            sharc_argonaut_encode_write_to_output(out, state, /*(uint32_t)*/huffmanCode->code, huffmanCode->bitSize);

            /*state->buffer += (huffmanCode->code << state->bufferBits);
            state->bufferBits += huffmanCode->bitSize;
            if(unlikely(state->bufferBits & ~0x1F)) {
                sharc_argonaut_encode_write_to_output(out, state, state->buffer, state->bufferBits);
                state->buffer = 0;
                state->bufferBits = 0;
            }*/

            /*if(i & 0x1) {
                const sharc_argonaut_huffman_code *huffmanCode = &ARGONAUT_NAME(h2l).code[(prev_rk << 8) + rk];
                sharc_argonaut_encode_write_to_output(out, state, huffmanCode->code, huffmanCode->bitSize);
            }*/
            
            m->durability++;
            if (likely(rk)) {
                const uint8_t pr = m->ranking - 1;
                sharc_argonaut_dictionary_primary_entry *pm = state->dictionary.ranking.primary[pr];
                if (unlikely(pm->durability < m->durability)) { // todo unlikely after a while
                    state->dictionary.ranking.primary[pr] = m;
                    state->dictionary.ranking.primary[rk] = pm;
                    m->ranking -= 1;
                    pm->ranking += 1;
                }
            }
            //prev_rk = rk;
        }
        /*if(!(i & 0x1)) {
            const sharc_argonaut_huffman_code *huffmanCode = &ARGONAUT_NAME(hl).code[rk];
            sharc_argonaut_encode_write_to_output(out, state, huffmanCode->code, huffmanCode->bitSize);
        }*/
        *separator = state->dictionary.ranking.primary[0]->letter;
#else
        sharc_argonaut_encode_write_to_output(out, state, 0x1, 2);
        sharc_argonaut_encode_write_to_output(out, state, wordLength, 3);
        /*for (uint_fast8_t i = 0; i != wordLength; i ++) {
            *letter = state->word.letters[i];
            *(out->pointer + (out->position ++)) = *letter;
        }*/
        *(uint64_t*)(out->pointer + out->position) = state->word.as_uint64_t;
        out->position += wordLength;
#endif
    } else {
        //match->durability++;
        if (match->ranked) {
            const uint_fast8_t rk = match->ranking;
            //state->bitCount += 2 + 8;
            sharc_argonaut_encode_write_to_output(out, state, 0x2, 2);
            sharc_argonaut_encode_write_to_output(out, state, rk, 8);
            // todo dict code (3)
            //sharc_argonaut_encode_write_coded(out, state, &ARGONAUT_NAME(ehl).code[SHARC_ARGONAUT_ENTITY_RANKED_KEY]);
            //sharc_argonaut_encode_write(out, state, 0, 2);
            //sharc_argonaut_encode_write_coded(out, state, &ARGONAUT_NAME(shl).code[match->ranking]);
            match->durability++;
            if (rk) {
                const uint16_t preceding = rk - 1;
                sharc_argonaut_dictionary_secondary_entry *preceding_match = state->dictionary.ranking.secondary[preceding];
                if (preceding_match->durability < match->durability) {
                    state->dictionary.ranking.secondary[preceding] = match;
                    state->dictionary.ranking.secondary[rk] = preceding_match;
                    match->ranking -= 1;
                    preceding_match->ranking += 1;
                }
            }

        } else {
            //state->bitCount += 2 + 16;
            sharc_argonaut_encode_write_to_output(out, state, 0x3, 2);
            sharc_argonaut_encode_write_to_output(out, state, shash, 16);
            // todo dict code (2)
            //sharc_argonaut_encode_write_coded(out, state, &ARGONAUT_NAME(ehl).code[SHARC_ARGONAUT_ENTITY_KEY]);
            //sharc_argonaut_encode_write(out, state, 0, encodeRanks ? 3 : 1);
            //sharc_argonaut_encode_write(out, state, shash, 16);
            match->durability++;
            //if (wordLength & ~0x1) {
            const uint16_t preceding = SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS - 1;
            sharc_argonaut_dictionary_secondary_entry *preceding_match = state->dictionary.ranking.secondary[preceding];
            if (preceding_match->durability < match->durability) {
                state->dictionary.ranking.secondary[preceding] = match;
                match->ranking = preceding;
                match->ranked = true;
                preceding_match->ranking = 0;
                preceding_match->ranked = false;
            }
            //}
        }
    }

    state->word.length = 0;
    //state->count --;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE ARGONAUT_NAME(sharc_argonaut_encode_init)(void *s) {
    sharc_argonaut_encode_state *state = s;
    //state->efficiencyChecked = 0;

    state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_OUTPUT_MEMORY;
    state->word.length = 0;
    //state->resetCycle = SHARC_DICTIONARY_PREFERRED_RESET_CYCLE - 1;

    for (uint16_t i = 0; i < SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS; i++) {
        state->dictionary.ranking.secondary[i] = &state->dictionary.secondary_entry[i];
        state->dictionary.secondary_entry[i].ranking = (uint_fast8_t) i;
        state->dictionary.secondary_entry[i].ranked = true;
    }

    for (uint16_t i = 0; i < (1 << 8); i++)
        state->dictionary.primary_entry[i].letter = (uint8_t) (i);

    for (uint16_t i = 0; i < SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS; i++) {
        state->dictionary.ranking.primary[i] = &state->dictionary.primary_entry[i];
        state->dictionary.primary_entry[i].ranking = (uint8_t) i;
    }

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE ARGONAUT_NAME(sharc_argonaut_encode_process)(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, void *restrict s, const sharc_bool flush) {
    sharc_argonaut_encode_state *state = s;
    SHARC_KERNEL_ENCODE_STATE returnState;
    uint_fast8_t letter;
    uint_fast32_t hash = 0;

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    uint8_t separator = state->dictionary.ranking.primary[0]->letter;
#else
    uint8_t separator = 0x20;
#endif

    if (in->size == 0)
        goto exit;

    switch (state->process) {
        /*case SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_STATE:
            if ((returnState = sharc_argonaut_encode_check_state(out, state)))
                return returnState;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            break;*/

        case SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_OUTPUT_MEMORY:
        check_mem:
            if ((returnState = sharc_argonaut_encode_prepare_new_block(out, state, BLOCK * 32)))
                return returnState;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            //break;
            /*case SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT:
                sharc_argonaut_encode_prepare_next_output_unit_even(out, state);
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;*/

        case SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD:
        next_word:
            returnState = sharc_argonaut_encode_find_next_word(in, out, state, &separator);
            if (returnState) {
                if (flush) {
                    if (returnState == SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER) {
                        state->process = SHARC_ARGONAUT_ENCODE_PROCESS_FINISH;
                        return SHARC_KERNEL_ENCODE_STATE_READY;
                    }
                } else
                    return returnState;
            }/* else if(unlikely(!state->count)) {
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_OUTPUT_MEMORY;
                goto check_mem;
            }*/
            //state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_WORD;
            //break;

            /*case SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY:
                if ((returnState = sharc_argonaut_encode_check_next_output_block_memory_available(out, state)))
                    return returnState;
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_WORD;*/

        case SHARC_ARGONAUT_ENCODE_PROCESS_WORD:
            returnState = sharc_argonaut_encode_process_word(in, out, &letter, &hash, state, &separator);
            if (returnState) {
                if (flush) {
                    if (returnState == SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER) {
                        state->process = SHARC_ARGONAUT_ENCODE_PROCESS_FINISH;
                        return SHARC_KERNEL_ENCODE_STATE_READY;
                    }
                } else
                    return returnState;
            }/* else if(unlikely(!state->count)) {
                state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_OUTPUT_MEMORY;
                goto check_mem;
            }*/
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            if (likely(--state->count))
                goto next_word;
            else
                goto check_mem;

        case SHARC_ARGONAUT_ENCODE_PROCESS_FINISH:
            //printf("%llu\n", state->bitCount >> 3);
            //for (uint32_t i = 0; i < (1 << 8); i++)
            //    printf("%c\t%u\t%u\n", state->dictionary.ranking.primary[i]->letter, state->dictionary.ranking.primary[i]->durability, state->dictionary.ranking.primary[i]->ranking);
        exit:
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            return SHARC_KERNEL_ENCODE_STATE_FINISHED;

        default:
            return SHARC_KERNEL_ENCODE_STATE_ERROR;
    }
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE ARGONAUT_NAME(sharc_argonaut_encode_finish)(void *s) {
    //sharc_argonaut_encode_state *state = s;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}
