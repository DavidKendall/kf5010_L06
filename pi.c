#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include "console.h"

#define N_DARTS 300000000UL
#define N_THREADS 3

static unsigned long hit[N_THREADS];
static unsigned long tot[N_THREADS];

static int timer(int stop);
static void start_timer(void);
static int stop_timer(void);
static void *estimate_pi_thr(void *arg);

int main (void) {
    unsigned long hit_board = 0;
    unsigned long total = 0;
    int execution_time;
    double pi_approx;
    unsigned long i;
    int rc;
    pthread_t thread[N_THREADS];

    console_init();
    srand(time(NULL));
    start_timer();
    for (i = 0; i < N_THREADS; i += 1) {
        rc = pthread_create(&thread[i], NULL, estimate_pi_thr, (void *)i);
        assert(rc == 0);
    }
    for (i = 0; i < N_THREADS; i += 1) {
        rc = pthread_join(thread[i], NULL);
        assert(rc == 0);
        hit_board += hit[i];
        total += tot[i];
    }
    pi_approx = 4.0 * ((double)hit_board / total);
    execution_time = stop_timer();
    lcd_write_at(1, 0, "pi    is %1.9G\n", pi_approx);
    lcd_write_at(2, 0, "time  is %d us\n", execution_time);
    for (i = 0; i < N_THREADS; i += 1) {
        lcd_write_at(3+i, 0, "Thread %d ratio (hit/total): %d / %d\n", i, hit[i], tot[i]);
    }
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
 * @brief Call timer function to start timer
 */
static void start_timer(void) {
    (void)timer(0);
}

/**
 * @brief Call timer function to stop timer
 * @return Time elapsed in microseconds since timer was started
 */
static int stop_timer(void) {
    return timer(1);
}

static void *estimate_pi_thr(void *arg) {
    double r;
    double x;
    double y;
    unsigned long hit_board = 0;
    unsigned long total = 0;
    unsigned long id = (unsigned long)arg;
    unsigned long i;
    struct drand48_data rand_state;
    int rc;

    assert((N_DARTS % N_THREADS) == 0);
    rc = srand48_r((long)id * 10 + time(NULL), &rand_state); 
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
    hit[id] = hit_board;
    tot[id] = total;
    pthread_exit(NULL);
}

