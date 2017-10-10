#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include "console.h"

bool flashing = false;

long long count1;
long long count2;
long long total;

volatile bool need1;
volatile bool need2;
volatile int turn;

void random_delay(void) {
    int delay = rand() % 100;
    int i;

    for (i = 0; i < delay; i+=1) {
        /* skip */
    }
}

void *led_toggle_thr(void * arg) {
    while (true) {
        if (flashing) {
            led_toggle((leds_t)arg);
            sleep(1);
        }
    }
}

void *count1_thr(void * arg) {
    while (!flashing) {
        need1 = true;
        turn = 2;
        while (need2 && (turn == 2)) {
            /* busy wait */
        }
        count1 += 1;
        random_delay();
        total += 1;
        if ((count1 + count2) != total) {
            flashing = true;
        }
        need1 = false;
        lcd_write_at(1, 0, "count1 = %20d", count1);
    }
    while (true) {
        /* skip */
    }
}

void *count2_thr(void * arg) {
    while (!flashing) {
        need2 = true;
        turn = 1;
        while (need1 && (turn == 1)) {
            /* busy wait */
        }
        count2 += 1;
        random_delay();
        total += 1;
        if ((count1 + count2) != total) {
            flashing = true;
        }
        need2 = false;
        lcd_write_at(2, 0, "count2 = %20d", count2);
    }
    while (true) {
        /* skip */
    }
}

int main (void) {
    pthread_t thread[3];
    int rc;

    console_init();
    srand(time(NULL));

    rc = pthread_create(&thread[0], NULL, led_toggle_thr, (void *)LED_RED);
    assert(rc == 0);
    rc = pthread_create(&thread[1], NULL, count1_thr, NULL);
    assert(rc == 0);
    rc = pthread_create(&thread[2], NULL, count2_thr, NULL);
    assert(rc == 0);

    while (true) {
        /* skip */
    }
    exit(0);
}
