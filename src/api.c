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
 * 14/09/13 01:39
 */

#include "api.h"

SHARC_FORCE_INLINE bool sharc_api_utilities_restore_file_attributes(sharc_header *restrict header, const char *restrict fileName) {
    if (header->genericHeader.originType == SHARC_STREAM_ORIGIN_TYPE_FILE)
        return sharc_header_restoreFileAttributes(&header->fileInformationHeader, fileName);
    else
        return false;
}