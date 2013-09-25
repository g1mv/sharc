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
 * 25/09/13 16:45
 */

#ifndef SHARC_KERNEL_DECODE_H
#define SHARC_KERNEL_DECODE_H

typedef enum {
    SHARC_KERNEL_DECODE_STATE_READY = 0,
    SHARC_KERNEL_DECODE_STATE_INFO_NEW_BLOCK,
    SHARC_KERNEL_DECODE_STATE_INFO_EFFICIENCY_CHECK,
    SHARC_KERNEL_DECODE_STATE_FINISHED,
    SHARC_KERNEL_DECODE_STATE_STALL_ON_OUTPUT_BUFFER,
    SHARC_KERNEL_DECODE_STATE_STALL_ON_INPUT_BUFFER,
    SHARC_KERNEL_DECODE_STATE_ERROR
} SHARC_KERNEL_DECODE_STATE;

#endif