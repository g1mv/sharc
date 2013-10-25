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
 * 27/09/13 19:01
 */

#ifndef SHARC_HUFFMAN_ENCODE_H
#define SHARC_HUFFMAN_ENCODE_H

#include "byte_buffer.h"
#include "block.h"
#include "kernel_encode.h"
#include "argonaut_dictionary.h"
#include "argonaut.h"
#include "encode.h"
#include "huffman_dictionary.h"
#include "huffman_le.data"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>

typedef enum {
    SHARC_HUFFMAN_ENCODE_PROCESS_PREPARE,
    SHARC_HUFFMAN_ENCODE_PROCESS_CHAR
} SHARC_HUFFMAN_ENCODE_PROCESS;

#define SHARC_HUFFMAN_OUTPUT_UNIT_BIT_SIZE                         (sizeof(sharc_huffman_output_unit) << 3)

typedef uint_fast64_t sharc_huffman_output_unit;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    SHARC_HUFFMAN_ENCODE_PROCESS process;

    uint_fast8_t efficiencyChecked;

    sharc_huffman_output_unit* output;
    uint_fast8_t shift;

    sharc_huffman_dictionary dictionary;
} sharc_huffman_encode_state;
#pragma pack(pop)

SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_init(void*);
SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_process(sharc_byte_buffer *, sharc_byte_buffer *, void *, const sharc_bool);
SHARC_KERNEL_ENCODE_STATE sharc_huffman_encode_finish(void*);

#endif