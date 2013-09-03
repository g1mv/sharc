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
 * 01/09/13 19:43
 */

#ifndef SHARC_BLOCK_FOOTER_H
#define SHARC_BLOCK_FOOTER_H

#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

#include "globals.h"
#include "byte_buffer.h"

#pragma pack(push)
#pragma pack(4)
typedef struct {
} sharc_block_footer;
#pragma pack(pop)

sharc_bool sharc_block_footer_read(sharc_byte_buffer*, sharc_block_footer*);
uint32_t sharc_block_footer_write(sharc_byte_buffer*, const uint32_t);

#endif