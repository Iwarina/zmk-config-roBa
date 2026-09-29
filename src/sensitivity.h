#pragma once

#include <stdbool.h>
#include <stdint.h>

enum roba_sensitivity_mode {
    ROBA_SENSITIVITY_POINTER,
    ROBA_SENSITIVITY_SCROLL,
    ROBA_SENSITIVITY_MODE_COUNT,
};

uint8_t roba_sensitivity_numerator(enum roba_sensitivity_mode mode);
void roba_sensitivity_adjust(enum roba_sensitivity_mode mode, bool increase);
