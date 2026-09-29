#include <zephyr/sys/atomic.h>

#include "sensitivity.h"

// 50%, 75%, 100%, 125%, 150%; the denominator is always four.
static atomic_t levels[ROBA_SENSITIVITY_MODE_COUNT] = {ATOMIC_INIT(2), ATOMIC_INIT(2)};

uint8_t roba_sensitivity_numerator(enum roba_sensitivity_mode mode) {
    return (uint8_t)atomic_get(&levels[mode]) + 2;
}

void roba_sensitivity_adjust(enum roba_sensitivity_mode mode, bool increase) {
    atomic_val_t previous;
    atomic_val_t next;

    do {
        previous = atomic_get(&levels[mode]);
        next = previous + (increase ? 1 : -1);
        if (next < 0 || next > 4) {
            return;
        }
    } while (!atomic_cas(&levels[mode], previous, next));
}
