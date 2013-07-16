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
 * 02/07/13 02:34
 */

#include "chrono.h"

FORCE_INLINE void chronoStart(CHRONO* chrono) {
    gettimeofday(&chrono->start, NULL);
}

FORCE_INLINE void chronoStop(CHRONO* chrono) {
    gettimeofday(&chrono->stop, NULL);
}

FORCE_INLINE double chronoElapsed(CHRONO* chrono) {
    return ((chrono->stop.tv_sec * MICROSECONDS + chrono->stop.tv_usec) - (chrono->start.tv_sec * MICROSECONDS + chrono->start.tv_usec)) / MICROSECONDS;
}
