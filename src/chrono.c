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
 * 02/07/13 02:34
 */

#include "chrono.h"

#if defined(_WIN64) || defined(_WIN32)
SHARC_FORCE_INLINE void usage_to_timeval(FILETIME *ft, struct timeval *tv) {
    ULARGE_INTEGER time;
    time.LowPart = ft->dwLowDateTime;
    time.HighPart = ft->dwHighDateTime;

    tv->tv_sec = time.QuadPart / 10000000;
    tv->tv_usec = (time.QuadPart % 10000000) / 10;
}

SHARC_FORCE_INLINE int getrusage(int who, struct rusage *usage) {
    FILETIME creation_time, exit_time, kernel_time, user_time;
    memset(usage, 0, sizeof(struct rusage));

    if (who == RUSAGE_SELF) {
        if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)) {
            return -1;
        }
        usage_to_timeval(&kernel_time, &usage->ru_stime);
        usage_to_timeval(&user_time, &usage->ru_utime);
        return 0;
    } else if (who == RUSAGE_THREAD) {
        if (!GetThreadTimes(GetCurrentThread(), &creation_time, &exit_time, &kernel_time, &user_time)) {
            return -1;
        }
        usage_to_timeval(&kernel_time, &usage->ru_stime);
        usage_to_timeval(&user_time, &usage->ru_utime);
        return 0;
    } else {
        return -1;
    }
}
#endif

SHARC_FORCE_INLINE void sharc_chrono_start(sharc_chrono * chrono) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    chrono->start = usage.ru_utime;
}

SHARC_FORCE_INLINE void sharc_chrono_stop(sharc_chrono * chrono) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    chrono->stop = usage.ru_utime;
}

SHARC_FORCE_INLINE double sharc_chrono_elapsed(sharc_chrono * chrono) {
    return ((chrono->stop.tv_sec * SHARC_CHRONO_MICROSECONDS + chrono->stop.tv_usec) - (chrono->start.tv_sec * SHARC_CHRONO_MICROSECONDS + chrono->start.tv_usec)) / SHARC_CHRONO_MICROSECONDS;
}
