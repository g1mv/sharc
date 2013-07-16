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
 * 02/07/13 02:33
 */

#ifndef CHRONO_H
#define CHRONO_H

#include <sys/time.h>
#include <stddef.h>

#include "globals.h"

#define MICROSECONDS    1000000.0

typedef struct {
    struct timeval start;
    struct timeval stop;
} CHRONO;

void chronoStart(CHRONO*);
void chronoStop(CHRONO*);
double chronoElapsed(CHRONO*);

#endif
