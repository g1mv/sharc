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
 * 28/08/13 17:19
 */

#include "footer.h"

SHARC_FORCE_INLINE uint_fast32_t sharc_footer_read(sharc_byte_buffer *restrict in, sharc_footer *restrict footer) {
    in->position += sizeof(sharc_footer);

    return sizeof(sharc_footer);
}

SHARC_FORCE_INLINE uint_fast32_t sharc_footer_write(sharc_byte_buffer *out) {
    out->position += sizeof(sharc_footer);

    return sizeof(sharc_footer);
}

