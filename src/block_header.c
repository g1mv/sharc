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
 * 17/06/13 19:34
 */

#include "block_header.h"

FORCE_INLINE BLOCK_HEADER createBlockHeader(const byte mode, const uint32_t nextBlock) {
    BLOCK_HEADER blockHeader;
    blockHeader.mode = SHARC_LITTLE_ENDIAN_32((uint32_t)mode);
    blockHeader.nextBlock = SHARC_LITTLE_ENDIAN_32(nextBlock);
    return blockHeader;
}

FORCE_INLINE BLOCK_HEADER readBlockHeader(byte* buffer, const uint32_t position) {
    return *(BLOCK_HEADER*)(buffer + position);
}

FORCE_INLINE size_t readBlockHeaderFromFile(BLOCK_HEADER* blockHeader, FILE* inFile) {
    return fread(blockHeader, sizeof(BLOCK_HEADER), 1, inFile);
}
