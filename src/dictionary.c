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
 * 19/08/13 18:49
 */

#include "dictionary.h"

const SHARC_ENTRY sharc_chameleonDictionary[(1 << SHARC_HASH_BITS) * sizeof(SHARC_ENTRY)] = SHARC_CHAMELEON_DICTIONARY;
const SHARC_ENTRY sharc_compressedDictionary[(1 << SHARC_HASH_BITS) * sizeof(SHARC_ENTRY)] = SHARC_COMPRESSED_DICTIONARY;

SHARC_FORCE_INLINE void sharc_resetDirectDictionary(SHARC_ENTRY* dictionary) {
    memcpy(dictionary, sharc_chameleonDictionary, (1 << SHARC_HASH_BITS) * sizeof(SHARC_ENTRY));
}

SHARC_FORCE_INLINE void sharc_resetCompressedDictionary(SHARC_ENTRY* dictionary) {
    memcpy(dictionary, sharc_compressedDictionary, (1 << SHARC_HASH_BITS) * sizeof(SHARC_ENTRY));
}