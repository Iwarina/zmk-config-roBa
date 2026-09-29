#define DT_DRV_COMPAT roba_behavior_sensitivity

#include <errno.h>

#include <zephyr/device.h>
#include <drivers/behavior.h>

#include "sensitivity.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int sensitivity_pressed(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    if (binding->param1 >= ROBA_SENSITIVITY_MODE_COUNT || binding->param2 > 1) {
        return -EINVAL;
    }

    roba_sensitivity_adjust(binding->param1, binding->param2 == 1);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int sensitivity_released(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata modes[] = {
    {.display_name = "Pointer", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = 0},
    {.display_name = "Scroll", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = 1},
};

static const struct behavior_parameter_value_metadata directions[] = {
    {.display_name = "Slower", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = 0},
    {.display_name = "Faster", .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE, .value = 1},
};

static const struct behavior_parameter_metadata_set parameter_sets[] = {{
    .param1_values = modes,
    .param1_values_len = ARRAY_SIZE(modes),
    .param2_values = directions,
    .param2_values_len = ARRAY_SIZE(directions),
}};

static const struct behavior_parameter_metadata metadata = {
    .sets = parameter_sets,
    .sets_len = ARRAY_SIZE(parameter_sets),
};
#endif

static const struct behavior_driver_api sensitivity_driver_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = sensitivity_pressed,
    .binding_released = sensitivity_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif
};

#define SENSITIVITY_INST(n)                                                                        \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &sensitivity_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SENSITIVITY_INST)

#endif
