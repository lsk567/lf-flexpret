/*************
Copyright (c) 2021, The University of California at Berkeley.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************/

/** Support file for Bare-metal FlexPRET platform.
 *  
 *  @author{Shaokai Lin <shaokai@berkeley.edu>}
 */

#include <stdio.h>
#include <stdlib.h>
#include "lf_flexpret_support.h"
#include "../platform.h"
#include "flexpret.h"

/**
 * @return 0 for success, or -1 for failure
 */

static volatile uint32_t last_time = 0;
static volatile uint64_t epoch = 0;
#define EPOCH_DURATION_NS (1ULL << 32)
int lf_clock_gettime(instant_t* t) {
    uint32_t now = rdtime();
    if (now < last_time) {
        epoch += EPOCH_DURATION_NS;
    }
    *t = now + epoch;
    last_time = now;
    return 0;
}

/**
 * @brief Sleep until the given wakeup time.
 * 
 * @param wakeup_time The time instant at which to wake up.
 * @return int 0 if sleep completed, or -1 if it was interrupted.
 */
int lf_sleep_until_locked(instant_t wakeup_time) {

    // FIXME: Handle sleep durations exceeding 32bit nanoseconds range
    // FIXME: Handle async events
    delay_until(wakeup_time);
    return 0;
}

int lf_sleep(interval_t sleep_duration) {
    // FIXME: Handle sleep durations exceeding 32bit
    delay_for(sleep_duration);
}
/**
 * Initialize the LF clock.
 */
void lf_initialize_clock() {}

// Overwrite print functions with NoOp.
int printf(const char *format, ...) {}
int puts(const char *str) {}
int sprintf(char *str, const char *format, ...) {}
int snprintf(char *str, size_t size, const char *format, ...) {}
int vprintf(const char *format, va_list ap) {}
int vfprintf(FILE *stream, const char *format, va_list arg) {}

// Functions for marking critical sections
int lf_critical_section_enter() {
    // TODO: disable interrupts.
    return 0;
}

int lf_critical_section_exit() {
    // TODO: enable interrupts.
    return 0;
}

int lf_notify_of_event() {
    return 0;
}

/**
 * Pause execution for a number of nanoseconds.
 *
 * @return 0 for success, or -1 for failure. In case of failure, errno will be
 *  set appropriately (see `man 2 clock_nanosleep`).
 */
int lf_nanosleep(interval_t requested_time) {
    instant_t t;
    lf_clock_gettime(&t);
    instant_t expire_time = t + requested_time;
    while (t < expire_time) {
        lf_clock_gettime(&t);
    }
    return 0;
}