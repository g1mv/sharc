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
 * 01/06/13 20:03
 */

#ifndef SHARC_XOR_HASH_TRANSFORM_H
#define SHARC_XOR_HASH_TRANSFORM_H

#include "hash_transform.h"

#include <stdio.h>

#define SHARC_XOR_MASK    0x2AE2752F

sharc_bool sharc_xorHashEncode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_ENTRY*);
sharc_bool sharc_xorHashDecode(SHARC_BYTE_BUFFER*, SHARC_BYTE_BUFFER*, SHARC_ENTRY*);

#endif