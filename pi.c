#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#include "console.h"

#define N_DARTS 4000000000UL
#define N_THREADS 1

static int timer(int stop);
static void start_timer(void);
static int stop_timer(void);
static double estimate_pi(void);

int main (void) {
    int execution_time;
    double pi_approx;

    console_init();
    srand(time(NULL));
    start_timer();
    /*(void)timer(0);*/
    pi_approx = estimate_pi();
    execution_time = stop_timer();
    /*execution_time = timer(1);*/
    lcd_write_at(1, 0, "pi    is %1.10G\n", pi_approx);
    lcd_write_at(2, 0, "time  is %d us\n", execution_time); 

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

static void start_timer(void) {
    (void)timer(0);
}

static int stop_timer(void) {
    return timer(1);
}

static double estimate_pi(void) {
    double r;
    double x;
    double y;
    unsigned long in_board;
    unsigned long i;

    assert((N_DARTS % N_THREADS) == 0);
    for (i = 0; i < (N_DARTS / N_THREADS); i += 1) {
        r = ((double)rand()) / (RAND_MAX);
        x = 2.0 * r - 1.0;
        r = ((double)rand()) / (RAND_MAX);
        y = 2.0 * r - 1.0;
        if (((x * x) + (y * y)) <= 1.0) {
            in_board += 1;
        }
    }
    return 4.0 * ((double)in_board / (double)(N_DARTS));
}

