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
 * 25/09/13 15:38
 */

#ifndef SHARC_ARGONAUT_H
#define SHARC_ARGONAUT_H

//#ifdef SHARC_ARGONAUT_POST_PROCESSING
#define SHARC_ARGONAUT_SUFFIX                                      post_processing
#define SHARC_ARGONAUT_ENCODE_PROCESS_LETTERS
//#else
//#define SHARC_ARGONAUT_SUFFIX                                      default
//#endif

#define PASTER(x,y) x ## _ ## y
#define EVALUATOR(x,y)  PASTER(x,y)
#define ARGONAUT_NAME(function) EVALUATOR(function, SHARC_ARGONAUT_SUFFIX)

#define SHARC_ARGONAUT_HASH_BITS                            16
#define SHARC_ARGONAUT_HASH_OFFSET_BASIS                    (uint32_t)(2885564586)
#define SHARC_ARGONAUT_HASH_PRIME                           16777619

//typedef uint64_t                                            sharc_hash_signature;

typedef struct {
    uint_fast32_t code;
    uint_fast8_t bitSize;
} sharc_argonaut_huffman_code;

#endif
