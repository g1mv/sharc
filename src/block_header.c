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

SHARC_FORCE_INLINE SHARC_BLOCK_HEADER sharc_createBlockHeader(const sharc_byte mode, sharc_byte dictionaryFlags, const uint32_t nextBlock) {
    SHARC_BLOCK_HEADER blockHeader;
    blockHeader.mode = mode;
    blockHeader.dictionaryFlags = dictionaryFlags;
    blockHeader.reserved[0] = 0;
    blockHeader.reserved[1] = 0;
    blockHeader.nextBlock = SHARC_LITTLE_ENDIAN_32(nextBlock);
    return blockHeader;
}

SHARC_FORCE_INLINE const size_t sharc_readBlockHeaderFromFile(SHARC_BLOCK_HEADER* blockHeader, FILE* inFile) {
    const size_t sizeRead = fread(blockHeader, sizeof(SHARC_BLOCK_HEADER), 1, inFile);
    blockHeader->nextBlock = SHARC_LITTLE_ENDIAN_32(blockHeader->nextBlock);
    return sizeRead;
}
