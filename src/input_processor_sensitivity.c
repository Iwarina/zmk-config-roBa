#define DT_DRV_COMPAT roba_input_processor_sensitivity

#include <zephyr/device.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <drivers/input_processor.h>

#include "sensitivity.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct sensitivity_config {
    enum roba_sensitivity_mode mode;
};

static int sensitivity_handle_event(const struct device *dev, struct input_event *event,
                                    uint32_t param1, uint32_t param2,
                                    struct zmk_input_processor_state *state) {
    if (event->type != INPUT_EV_REL ||
        (event->code != INPUT_REL_X && event->code != INPUT_REL_Y)) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    const struct sensitivity_config *config = dev->config;
    int16_t *remainder = state ? state->remainder : NULL;
    int32_t weighted = event->value * roba_sensitivity_numerator(config->mode);
    if (remainder) {
        weighted += *remainder;
    }

    event->value = weighted / 4;
    if (remainder) {
        *remainder = weighted - (event->value * 4);
    }
    return ZMK_INPUT_PROC_CONTINUE;
}

static const struct zmk_input_processor_driver_api sensitivity_driver_api = {
    .handle_event = sensitivity_handle_event,
};

#define SENSITIVITY_INST(n)                                                                        \
    static const struct sensitivity_config sensitivity_config_##n = {                              \
        .mode = DT_INST_ENUM_IDX(n, mode),                                                         \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, &sensitivity_config_##n, POST_KERNEL,               \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &sensitivity_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SENSITIVITY_INST)

#endif
