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
 * 26/08/13 23:20
 */

#include "state.h"

SHARC_FORCE_INLINE sharc_state *sharc_state_allocate() {
    sharc_state * created = (sharc_state *)malloc(sizeof(sharc_state));
    created->workBuffer = sharc_byte_buffer_allocate();
    sharc_byte *workBuffer = (sharc_byte *) malloc(SHARC_INTERNAL_STATE_PREFERRED_WORK_BUFFER_SIZE * sizeof(sharc_byte));
    sharc_byte_buffer_encapsulate(created->workBuffer, workBuffer, 0, SHARC_INTERNAL_STATE_PREFERRED_WORK_BUFFER_SIZE);
    created->dictionary_a = sharc_dictionary_allocate();
    created->dictionary_b = sharc_dictionary_allocate();
    return created;
}

SHARC_FORCE_INLINE void sharc_state_deallocate(sharc_state * internalState) {
    sharc_dictionary_deallocate(internalState->dictionary_b);
    sharc_dictionary_deallocate(internalState->dictionary_a);
    free(internalState->workBuffer->pointer);
    sharc_byte_buffer_deallocate(internalState->workBuffer);
    free(internalState);
}
