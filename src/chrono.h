/*
 * Centaurean Sharc
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 02/07/13 02:33
 */

#ifndef SHARC_CHRONO_H
#define SHARC_CHRONO_H

#include <sys/resource.h>

#include "globals.h"

#define SHARC_CHRONO_MICROSECONDS    1000000.0

typedef struct {
    struct timeval start;
    struct timeval stop;
} sharc_chrono;

void sharc_chrono_start(sharc_chrono *);
void sharc_chrono_stop(sharc_chrono *);
double sharc_chrono_elapsed(sharc_chrono *);

#endif
