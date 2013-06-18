/*
 * Copyright (c) 2013, Guillaume Voirin (gvoirin@centaurean.com)
 * All rights reserved.
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in licenses/commercial.txt.
 *
 * Centaurean SHARC
 * www.centaurean.com/sharc
 *
 * 01/06/13 17:47
 */

#ifndef DIRECT_HASH_CIPHER_H
#define DIRECT_HASH_CIPHER_H

#include "hash_cipher.h"

#include <stdio.h>

#define DIRECT_XOR_MASK 0x00000000

bool directHashEncode(byte*, uint32_t, byte*, uint32_t);
bool directHashDecode(FILE*, FILE*, const uint64_t);

#endif