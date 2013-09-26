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
 * 17/06/13 21:42
 */

#ifndef SHARC_BLOCK_H
#define SHARC_BLOCK_H

#define SHARC_MAX_BLOCK_SIGNATURES_SHIFT                        32

#define SHARC_PREFERRED_BLOCK_SIGNATURES_SHIFT                  11
#define SHARC_PREFERRED_BLOCK_SIGNATURES                        (1 << SHARC_PREFERRED_BLOCK_SIGNATURES_SHIFT)

#define SHARC_PREFERRED_EFFICIENCY_CHECK_SIGNATURES_SHIFT       7
#define SHARC_PREFERRED_EFFICIENCY_CHECK_SIGNATURES             (1 << SHARC_PREFERRED_EFFICIENCY_CHECK_SIGNATURES_SHIFT)

typedef enum {
    SHARC_BLOCK_TYPE_DEFAULT = 0,
    SHARC_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK = 1
} SHARC_BLOCK_TYPE;

typedef enum {
    SHARC_BLOCK_MODE_COPY = 0,
    SHARC_BLOCK_MODE_HASH = 1
} SHARC_BLOCK_MODE;

#endif