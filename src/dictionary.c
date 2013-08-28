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

const sharc_dictionary_entry sharc_dictionary_chameleon[(1 << SHARC_HASH_BITS) * sizeof(sharc_dictionary_entry)] = SHARC_DICTIONARY_CHAMELEON;
const sharc_dictionary_entry sharc_dictionary_compressed[(1 << SHARC_HASH_BITS) * sizeof(sharc_dictionary_entry)] = SHARC_DICTIONARY_COMPRESSED;

SHARC_FORCE_INLINE sharc_dictionary* sharc_dictionary_allocate() {
    sharc_dictionary* created = (sharc_dictionary*)malloc(sizeof(sharc_dictionary));
    created->entries = (sharc_dictionary_entry*)malloc((1 << SHARC_HASH_BITS) * sizeof(sharc_dictionary_entry));
    return created;
}

SHARC_FORCE_INLINE void sharc_dictionary_deallocate(sharc_dictionary* dictionary) {
    free(dictionary->entries);
    free(dictionary);
}

SHARC_FORCE_INLINE void sharc_dictionary_resetDirect(sharc_dictionary * dictionary) {
    memcpy(dictionary, sharc_dictionary_chameleon, (1 << SHARC_HASH_BITS) * sizeof(sharc_dictionary_entry));
}

SHARC_FORCE_INLINE void sharc_dictionary_resetCompressed(sharc_dictionary * dictionary) {
    memcpy(dictionary, sharc_dictionary_compressed, (1 << SHARC_HASH_BITS) * sizeof(sharc_dictionary_entry));
}