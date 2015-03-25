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

#include "globals.h"

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <time.h>

#define RUSAGE_SELF     0
#define RUSAGE_THREAD   1 

struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
    long   ru_maxrss;        /* maximum resident set size */
    long   ru_ixrss;         /* integral shared memory size */
    long   ru_idrss;         /* integral unshared data size */
    long   ru_isrss;         /* integral unshared stack size */
    long   ru_minflt;        /* page reclaims (soft page faults) */
    long   ru_majflt;        /* page faults (hard page faults) */
    long   ru_nswap;         /* swaps */
    long   ru_inblock;       /* block input operations */
    long   ru_oublock;       /* block output operations */
    long   ru_msgsnd;        /* IPC messages sent */
    long   ru_msgrcv;        /* IPC messages received */
    long   ru_nsignals;      /* signals received */
    long   ru_nvcsw;         /* voluntary context switches */
    long   ru_nivcsw;        /* involuntary context switches */
};
#else
#include <sys/resource.h>
#endif

typedef struct {
    struct timeval start;
    struct timeval stop;
} sharc_chrono;

#define SHARC_CHRONO_MICROSECONDS    1000000.0

void sharc_chrono_start(sharc_chrono *);
void sharc_chrono_stop(sharc_chrono *);
double sharc_chrono_elapsed(sharc_chrono *);

#endif
