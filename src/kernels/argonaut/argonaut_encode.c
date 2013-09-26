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

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
const sharc_argonaut_primary_code_lookup NAME(hl) = {.code = SHARC_ARGONAUT_PRIMARY_HUFFMAN_CODES};
#endif
const sharc_argonaut_secondary_code_lookup NAME(shl) = {.code = SHARC_ARGONAUT_SECONDARY_HUFFMAN_CODES};
const sharc_argonaut_word_length_code_lookup NAME(wlhl) = {.code = SHARC_ARGONAUT_WORD_LENGTH_HUFFMAN_CODES};
const sharc_argonaut_entity_code_lookup NAME(ehl) = {.code = SHARC_ARGONAUT_ENTITY_HUFFMAN_CODES};

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_check_next_output_block_memory_available(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    if (out->position + 64 > out->size)
        return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    //state->blockShift = 0;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_prepare_next_output_unit_even(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
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
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_write_to_output(sharc_argonaut_encode_state *state, const uint_fast64_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *(state->output) |= value << state->shift;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    *(state->output) |= value << ((56 - (state->shift & ~0x7)) + (state->shift & 0x7));
#endif
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_write_coded_separator(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state) {
    sharc_argonaut_encode_write_to_output(state, NAME(ehl).code[SHARC_ARGONAUT_ENTITY_SEPARATOR].code);

    state->shift++;
    if (state->shift == SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE)
        sharc_argonaut_encode_prepare_next_output_unit_even(out, state);
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_write(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const uint_fast64_t data, const uint_fast8_t bitSize) {
    sharc_argonaut_encode_write_to_output(state, data);

    state->shift += bitSize;
    if (state->shift & ~(SHARC_ARGONAUT_OUTPUT_UNIT_BIT_SIZE - 1))
        sharc_argonaut_encode_prepare_next_output_unit(out, state, data, bitSize);
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_write_coded(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, const sharc_argonaut_huffman_code *restrict code) {
    sharc_argonaut_encode_write(out, state, code->code, code->bitSize);
}

SHARC_FORCE_INLINE void sharc_argonaut_encode_process_letter(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, sharc_argonaut_dictionary_word *restrict word, const uint8_t *restrict letter, const uint8_t index, uint_fast32_t *restrict hash, uint8_t *restrict separator) {
    *hash = *hash ^ *letter;
    *hash = *hash * SHARC_ARGONAUT_HASH_PRIME;

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    sharc_argonaut_dictionary_primary_entry *match = &state->dictionary.primary_entry[*letter];

    const uint8_t rank = match->ranking;
    const uint8_t precedingRank = match->ranking - 1;

    const sharc_argonaut_huffman_code *huffmanCode = &NAME(hl).code[rank];
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
    }
#endif
}

SHARC_FORCE_INLINE uint8_t sharc_argonaut_encode_advance_to_separator(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint_fast8_t *restrict letter, uint_fast32_t *restrict hash, sharc_argonaut_dictionary_word *restrict word, uint8_t *separator) {
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

SHARC_FORCE_INLINE uint8_t sharc_argonaut_encode_advance_to_separator_limited(sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint_fast8_t start, uint_fast8_t stop, uint_fast8_t *restrict letter, uint_fast32_t *restrict hash, sharc_argonaut_dictionary_word *restrict word, uint8_t *separator) {
    if (!start)
        *hash = SHARC_ARGONAUT_HASH_OFFSET_BASIS;
    if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
        start++;
        sharc_argonaut_encode_process_letter(out, state, word, letter, 0, hash, separator);
        if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
            start++;
            sharc_argonaut_encode_process_letter(out, state, word, letter, 1, hash, separator);
            if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
                start++;
                sharc_argonaut_encode_process_letter(out, state, word, letter, 2, hash, separator);
                if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
                    start++;
                    sharc_argonaut_encode_process_letter(out, state, word, letter, 3, hash, separator);
                    if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
                        start++;
                        sharc_argonaut_encode_process_letter(out, state, word, letter, 4, hash, separator);
                        if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
                            start++;
                            sharc_argonaut_encode_process_letter(out, state, word, letter, 5, hash, separator);
                            if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
                                start++;
                                sharc_argonaut_encode_process_letter(out, state, word, letter, 6, hash, separator);
                                if (start ^ stop && (*letter = word->letters[start]) ^ *separator) {
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

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_goto_next_word(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, sharc_argonaut_encode_state *restrict state, uint8_t *separator) {
    //state->count[7]++;
    uint_fast64_t start = in->position;
    while (*(in->pointer + in->position) == *separator) {
        if (unlikely(out->position == out->size))
            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
        sharc_argonaut_encode_write_coded_separator(out, state);
        in->position++;
        if (unlikely(in->position == in->size))
            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
    }
    uint_fast64_t separatorCount = in->position - start;
    state->dictionary.ranking.primary[0]->durability += separatorCount;
#ifdef SHARC_ARGONAUT_ENCODE_STATS
    state->count[1] += separatorCount;
#endif

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_argonaut_encode_process_word(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, uint_fast8_t *letter, uint_fast32_t *restrict hash, sharc_argonaut_encode_state *restrict state, uint8_t *separator) {
    if (unlikely(state->word.length)) {
        state->word.as_uint64_t |= ((*(uint64_t *) (in->pointer + in->position)) >> state->word.length);
        const uint8_t addedLength = sharc_argonaut_encode_advance_to_separator_limited(out, state, state->word.length, SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS, letter, hash, &state->word, separator);
        state->word.length += addedLength;
        in->position += addedLength;
    } else {
        state->word.as_uint64_t = *(uint64_t *) (in->pointer + in->position);
        if (likely(in->size - in->position > SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS)) {
            state->word.length = sharc_argonaut_encode_advance_to_separator(out, state, letter, hash, &state->word, separator);
        } else {
            state->word.length = sharc_argonaut_encode_advance_to_separator_limited(out, state, 0, (uint8_t) (in->size - in->position), letter, hash, &state->word, separator);
            state->word.as_uint64_t &= ((((uint64_t) 1) << (state->word.length << 3)) - 1);
            if (state->word.length == in->size - in->position)
                return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
        }
        in->position += state->word.length;
    }

    if (state->word.length ^ SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS)
        state->word.as_uint64_t &= ((((uint64_t) 1) << (state->word.length << 3)) - 1);

    const uint8_t bits = 16;
    uint_fast32_t shash = (uint_fast32_t) ((((*hash >> bits) ^ *hash) & ((1 << bits) - 1)));
    sharc_argonaut_dictionary_secondary_entry *match = &state->dictionary.secondary_entry[shash];
    if (state->word.as_uint64_t ^ match->word.as_uint64_t) {
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
        if (match->durability)
            match->durability--;
        else {
#endif
            match->word.as_uint64_t = state->word.as_uint64_t;
            match->word.length = state->word.length;
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
        }
#endif
        const uint8_t wordLength = state->word.length;
        // todo word code (3)
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
        sharc_argonaut_encode_write_coded(out, state, &NAME(ehl).code[SHARC_ARGONAUT_ENTITY_WORD]); // todo move
#else
        sharc_argonaut_encode_write(out, state, 0, 2);
#endif
        sharc_argonaut_encode_write_coded(out, state, &NAME(wlhl).code[wordLength - 1]);
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
        for (uint_fast8_t i = 0; i ^ wordLength; i++)
            sharc_argonaut_encode_write_coded(out, state, state->word.letterCode[i]);
#else
        for (uint_fast8_t i = 0; i ^ wordLength; i++)
            sharc_argonaut_encode_write(out, state, state->word.letters[i], 8);
#endif
#ifdef SHARC_ARGONAUT_ENCODE_STATS
        state->count[2]++;
        state->length[match->word.length - 1]++;
#endif
    } else {
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
        if (match->ranked) {
            // todo dict code (3)
            sharc_argonaut_encode_write_coded(out, state, &NAME(ehl).code[SHARC_ARGONAUT_ENTITY_RANKED_KEY]);
            //sharc_argonaut_encode_write(out, state, 0, 2);
            sharc_argonaut_encode_write_coded(out, state, &NAME(shl).code[match->ranking]);
#ifdef SHARC_ARGONAUT_ENCODE_STATS
            state->count[3]++;
#endif
        } else {
            // todo dict code (2)
            sharc_argonaut_encode_write_coded(out, state, &NAME(ehl).code[SHARC_ARGONAUT_ENTITY_KEY]);
#else
            sharc_argonaut_encode_write(out, state, 0, 2);
#endif
            //sharc_argonaut_encode_write(out, state, 0, encodeRanks ? 3 : 1);
            sharc_argonaut_encode_write(out, state, shash, 16);
#ifdef SHARC_ARGONAUT_ENCODE_STATS
            state->count[4]++;
#endif
#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_RANKS
        }
        match->durability++;
        if (match->ranked) {
            if (match->ranking) {
                const uint16_t preceding = match->ranking - 1;
                sharc_argonaut_dictionary_secondary_entry *preceding_match = state->dictionary.ranking.secondary[preceding];
                if (preceding_match->durability < match->durability) {
                    state->dictionary.ranking.secondary[preceding] = match;
                    state->dictionary.ranking.secondary[match->ranking] = preceding_match;
                    match->ranking -= 1;
                    preceding_match->ranking += 1;
                }
            }
        } else {
            const uint16_t preceding = SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS - 1;
            sharc_argonaut_dictionary_secondary_entry *preceding_match = state->dictionary.ranking.secondary[preceding];
            if (preceding_match->durability < match->durability) {
                state->dictionary.ranking.secondary[preceding] = match;
                match->ranking = preceding;
                match->ranked = true;
                preceding_match->ranking = 0;
                preceding_match->ranked = false;
            }
        }
#endif
    }

    state->word.length = 0;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE NAME(sharc_argonaut_encode_init)(void *s) {
    sharc_argonaut_encode_state *state = s;
    state->efficiencyChecked = 0;

    state->process = SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT;
    state->word.length = 0;

    for (uint16_t i = 0; i < SHARC_ARGONAUT_DICTIONARY_SECONDARY_RANKS; i++) {
        state->dictionary.ranking.secondary[i] = &state->dictionary.secondary_entry[i];
        state->dictionary.secondary_entry[i].ranking = i;
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

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE NAME(sharc_argonaut_encode_process)(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, void *restrict s, const sharc_bool flush) {
    sharc_argonaut_encode_state *state = s;
    SHARC_KERNEL_ENCODE_STATE returnState;
    uint_fast8_t letter;
    uint_fast32_t hash;

#ifdef SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
    uint8_t separator = state->dictionary.ranking.primary[0]->letter;
#else
    uint8_t separator = 0x20;
#endif

    if (in->size == 0)
        goto exit;

    switch (state->process) {
        case SHARC_ARGONAUT_ENCODE_PROCESS_PREPARE_OUTPUT:
            sharc_argonaut_encode_prepare_next_output_unit_even(out, state);
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;

        case SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD:
        goto_next_word:
            returnState = sharc_argonaut_encode_goto_next_word(in, out, state, &separator);
            if (returnState) {
                if (flush) {
                    if (returnState == SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER) {
                        state->process = SHARC_ARGONAUT_ENCODE_PROCESS_FINISH;
                        return SHARC_KERNEL_ENCODE_STATE_READY;
                    }
                } else
                    return returnState;
            }
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY;

        case SHARC_ARGONAUT_ENCODE_PROCESS_CHECK_AVAILABLE_MEMORY:
            if ((returnState = sharc_argonaut_encode_check_next_output_block_memory_available(out, state)))
                return returnState;
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_WORD;

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
            }
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            goto goto_next_word;

        case SHARC_ARGONAUT_ENCODE_PROCESS_FINISH:
#ifdef SHARC_ARGONAUT_ENCODE_STATS
            for (uint8_t i = 0; i < 8; i++)
                printf("%llu, ", state->count[i]);
            printf("\n");
#endif
            /*for (uint8_t i = 0; i < SHARC_ARGONAUT_DICTIONARY_MAX_WORD_LETTERS; i++)
                printf("%llu, ", state->length[i]);
            printf("\n");*/
        exit:
            state->process = SHARC_ARGONAUT_ENCODE_PROCESS_GOTO_NEXT_WORD;
            return SHARC_KERNEL_ENCODE_STATE_FINISHED;

        default:
            return SHARC_KERNEL_ENCODE_STATE_ERROR;
    }
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE NAME(sharc_argonaut_encode_finish)(void *s) {
    sharc_argonaut_encode_state *state = s;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}
