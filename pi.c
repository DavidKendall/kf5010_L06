#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include "console.h"

#define N_DARTS 3000000000UL
#define N_THREADS 1

static unsigned long hit;
static unsigned long tot;

static int timer(int stop);
static void start_timer(void);
static int stop_timer(void);
static void simulate(void);

int main (void) {
    int execution_time;
    double pi_approx;

    console_init();
    srand(time(NULL));
    start_timer();
    simulate();
    pi_approx = 4.0 * ((double)hit / tot);
    execution_time = stop_timer();
    lcd_write_at(1, 0, "pi    is %1.9G\n", pi_approx);
    lcd_write_at(2, 0, "time  is %d us\n", execution_time); 
    lcd_write_at(3, 0, "Thread 0 ratio (hit/total): %ld / %ld", hit, tot);

    while (true) {
        /* skip */
    }
    exit(0);
}

/**
 * @brief Starts and stops a hardware timer, returning the elapsed time
 * @param stop if non-zero stops the timer, if zero starts the timer
 * @return the elapsed time in microseconds from starting the timer to stopping 
 * the timer
 */
static int timer(int stop) {
    static struct timespec begin;
    static struct timespec end;
    int result;

    if (stop) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        result = ((end.tv_sec - begin.tv_sec) * 1000000) +
                 ((end.tv_nsec - begin.tv_nsec) / 1000);
    }
    else {
        clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
        result = 0;
    }
    return result;
}

/** 
 * @brief Calls the timer function to start the timer
 */
static void start_timer(void) {
    (void)timer(0);
}

/**
 * @brief Calls the timer function to stop the timer 
 * @return the time elapsed in microseconds since the timer was started
 */
static int stop_timer(void) {
    return timer(1);
}

/**
 * @brief The Monte Carlo simulation used in the estimation of pi
 */
static void simulate(void) {
    double r;
    double x;
    double y;
    unsigned long hit_board = 0;
    unsigned long total = 0;
    unsigned long i;
    struct drand48_data rand_state;
    int rc;

    assert((N_DARTS % N_THREADS) == 0);
    rc = srand48_r((long)time(NULL), &rand_state);
    assert(rc == 0);
    for (i = 0; i < (N_DARTS / N_THREADS); i += 1) {
        rc = drand48_r(&rand_state, &r);
        x = 2.0 * r - 1.0;
        rc = drand48_r(&rand_state, &r);
        y = 2.0 * r - 1.0;
        if (((x * x) + (y * y)) <= 1.0) {
            hit_board += 1;
        }
        total += 1;
    }
    hit = hit_board;
    tot = total;
}

