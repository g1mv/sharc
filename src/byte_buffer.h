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
 * 01/06/13 17:39
 */

#ifndef SHARC_BYTE_BUFFER_H
#define SHARC_BYTE_BUFFER_H

#include "globals.h"

#define SHARC_MAX_BLOCK_SIZE_SHIFT             30

#define SHARC_PREFERRED_BLOCK_SIZE_SHIFT       19
#define SHARC_PREFERRED_BLOCK_SIZE             (1 << SHARC_PREFERRED_BLOCK_SIZE_SHIFT)

typedef struct {
    sharc_byte* pointer;
    uint32_t position;
    uint32_t size;
} sharc_byte_buffer;


sharc_byte_buffer* sharc_byte_buffer_allocate();
void sharc_byte_buffer_deallocate(sharc_byte_buffer*);
void sharc_byte_buffer_rewind(sharc_byte_buffer *);
void sharc_byte_buffer_encapsulate(sharc_byte_buffer *, sharc_byte*, uint32_t, uint32_t);

#endif
