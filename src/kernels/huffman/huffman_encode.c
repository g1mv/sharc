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
 * 27/09/13 19:20
 */

const sharc_huffman_primary_code_lookup hl = {.code = SHARC_HUFFMAN_PRIMARY_HUFFMAN_CODES};

SHARC_FORCE_INLINE void sharc_huffman_encode_prepare_next_output_unit(sharc_byte_buffer *restrict out, sharc_huffman_encode_state *restrict state, const uint_fast64_t data, const uint_fast8_t bitSize) {
    state->shift = (uint8_t) (state->shift - SHARC_HUFFMAN_OUTPUT_UNIT_BIT_SIZE);

    state->output = (sharc_huffman_output_unit *) (out->pointer + out->position);
    *(state->output) = (sharc_huffman_output_unit) (data >> (bitSize - state->shift));
    out->position += sizeof(sharc_huffman_output_unit);
}

SHARC_FORCE_INLINE void sharc_huffman_encode_write_to_output(sharc_huffman_encode_state *state, const uint_fast64_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *(state->output) |= value << state->shift;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    *(state->output) |= value << ((56 - (state->shift & ~0x7)) + (state->shift & 0x7));
#endif
}

SHARC_FORCE_INLINE void sharc_huffman_encode_write(sharc_byte_buffer *restrict out, sharc_huffman_encode_state *restrict state, const uint_fast64_t data, const uint_fast8_t bitSize) {
    sharc_huffman_encode_write_to_output(state, data);

    state->shift += bitSize;
    if (state->shift & ~(SHARC_HUFFMAN_OUTPUT_UNIT_BIT_SIZE - 1))
        sharc_huffman_encode_prepare_next_output_unit(out, state, data, bitSize);
}

SHARC_FORCE_INLINE void sharc_huffman_encode_write_coded(sharc_byte_buffer *restrict out, sharc_huffman_encode_state *restrict state, const sharc_huffman_code *restrict code) {
    sharc_huffman_encode_write(out, state, code->code, code->bitSize);
}

SHARC_FORCE_INLINE void sharc_huffman_encode_process_letter(sharc_byte_buffer *restrict out, sharc_huffman_encode_state *restrict state, const uint8_t *restrict letter) {
    sharc_huffman_dictionary_primary_entry *match = &state->dictionary.primary_entry[*letter];

    const uint8_t rank = match->ranking;
    sharc_huffman_encode_write_coded(out, state, &hl.code[rank]);

    match->durability++;
    if (match->ranking) {
        const uint8_t precedingRank = match->ranking - 1;
        sharc_huffman_dictionary_primary_entry *preceding_match = state->dictionary.ranking.primary[precedingRank];
        if (preceding_match->durability < match->durability) {
            state->dictionary.ranking.primary[precedingRank] = match;
            state->dictionary.ranking.primary[rank] = preceding_match;
            match->ranking -= 1;
            preceding_match->ranking += 1;
        }
    }
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_init(void *s) {
    sharc_huffman_encode_state *state = s;
    state->efficiencyChecked = 0;

    state->process = SHARC_HUFFMAN_ENCODE_PROCESS_PREPARE;

    for (uint16_t i = 0; i < (1 << 8); i++)
        state->dictionary.primary_entry[i].letter = (uint8_t) (i);

    for (uint16_t i = 0; i < SHARC_ARGONAUT_DICTIONARY_PRIMARY_RANKS; i++) {
        state->dictionary.ranking.primary[i] = &state->dictionary.primary_entry[i];
        state->dictionary.primary_entry[i].ranking = (uint8_t) i;
    }

    return SHARC_KERNEL_ENCODE_STATE_READY;
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_process(sharc_byte_buffer *restrict in, sharc_byte_buffer *restrict out, void *restrict s, const sharc_bool flush) {
    sharc_huffman_encode_state *state = s;
    union {
        uint64_t as_uint64_t;
        uint8_t as_letters[8];
    } letters;
    uint_fast64_t inLimit = (in->size & ~0x7llu);
    uint_fast64_t outLimit = (out->position & 0x7) + (out->size & ~0x7llu) - 24 * sizeof(sharc_huffman_output_unit);
    //printf("%llu, %llu, %llu\n", in->size, inLimit, SHARC_PREFERRED_BUFFER_SIZE);
    //printf("%llu, %llu, %llu\n", out->size, outLimit, SHARC_PREFERRED_BUFFER_SIZE);
    //printf("%u\n", flush);

    if (in->size == 0)
        goto exit;

    switch (state->process) {
        case SHARC_HUFFMAN_ENCODE_PROCESS_PREPARE:
            state->shift = 0;
            state->output = (sharc_huffman_output_unit *) (out->pointer + out->position);
            *(state->output) = 0;
            out->position += sizeof(sharc_huffman_output_unit);

        case SHARC_HUFFMAN_ENCODE_PROCESS_CHAR:
            while (true) {
                //printf("%llu, %llu\n", in->position, out->position);
                if (unlikely(out->position == outLimit))
                    return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                if (unlikely(in->position == inLimit)) {
                    while(true) {
                        if(in->position == in->size) {
                            if (flush)
                                goto exit;
                            else
                                return SHARC_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
                        }
                        sharc_huffman_encode_process_letter(out, state, (in->pointer + in->position));
                        in->position ++;
                        if (unlikely(out->position == outLimit))
                            return SHARC_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                    }
                }
                letters.as_uint64_t = *(uint64_t*)(in->pointer + in->position);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[0]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[1]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[2]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[3]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[4]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[5]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[6]);
                sharc_huffman_encode_process_letter(out, state, &letters.as_letters[7]);
                in->position += sizeof(uint64_t);
            }
        exit:
            //for (uint32_t i = 0; i < (1 << 8); i++)
            //    printf("%c\t%lu\t%lu\n", state->dictionary.ranking.primary[i]->letter, state->dictionary.ranking.primary[i]->durability, state->dictionary.ranking.primary[i]->ranking);
            //state->process = SHARC_HUFFMAN_ENCODE_PROCESS_CHAR;
            return SHARC_KERNEL_ENCODE_STATE_FINISHED;

        default:
            return SHARC_KERNEL_ENCODE_STATE_ERROR;
    }
}

SHARC_FORCE_INLINE SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_finish(void *s) {
    //sharc_argonaut_encode_state *state = s;

    return SHARC_KERNEL_ENCODE_STATE_READY;
}
