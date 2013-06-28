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
 * 01/06/13 17:47
 */

#ifndef DIRECT_HASH_CIPHER_H
#define DIRECT_HASH_CIPHER_H

#include "hash_cipher.h"

#include <stdio.h>

#define DIRECT_XOR_MASK 0x00000000

bool directHashEncode(BYTE_BUFFER*, BYTE_BUFFER*);
bool directHashDecode(BYTE_BUFFER*, BYTE_BUFFER*);

#endif