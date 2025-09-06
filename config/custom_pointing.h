#pragma once

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#ifdef ZMK_POINTING_CUSTOM_SCROLL_CALCULATION
#undef zmk_pointing_calculate_scroll

// Define the default scroll value, using the modern constant name.
#define ZMK_POINTING_DEFAULT_SCRL_VAL 100

// Custom scroll calculation function for acceleration.
static int zmk_pointing_calculate_scroll(int dx, int dy) {
    int scrl_val;
    // Check whether horizontal or vertical movement is greater.
    if (abs(dx) > abs(dy)) {
        // Horizontal scroll acceleration
        if (abs(dx) > 20) {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL * 3;
        } else if (abs(dx) > 10) {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL * 2;
        } else {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL;
        }
    } else {
        // Vertical scroll acceleration
        if (abs(dy) > 20) {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL * 3;
        } else if (abs(dy) > 10) {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL * 2;
        } else {
            scrl_val = ZMK_POINTING_DEFAULT_SCRL_VAL;
        }
    }
    return scrl_val;
}
#endif
