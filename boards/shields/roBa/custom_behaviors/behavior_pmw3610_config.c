#define DT_DRV_COMPAT zmk_behavior_pmw3610_attr_set

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/matrix.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

// This is a copy of the enum from the driver header
// We can't include the driver header directly in a zmk-config repo
enum pmw3610_attribute {
    PMW3610_ATTR_CPI,
    PMW3610_ATTR_RUN_DOWNSHIFT_TIME,
    PMW3610_ATTR_REST1_DOWNSHIFT_TIME,
    PMW3610_ATTR_REST2_DOWNSHIFT_TIME,
    PMW3610_ATTR_REST1_SAMPLE_TIME,
    PMW3610_ATTR_REST2_SAMPLE_TIME,
    PMW3610_ATTR_REST3_SAMPLE_TIME,
};

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *pmw3610_dev = DEVICE_DT_GET(DT_INST(0, pixart_pmw3610));

struct behavior_pmw3610_attr_set_config {
    uint8_t attr;
    int32_t value;
};

static int behavior_pmw3610_attr_set_init(const struct device *dev) {
    if (!device_is_ready(pmw3610_dev)) {
        LOG_ERR("PMW3610 sensor device is not ready");
        return -ENODEV;
    }
    return 0;
};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_pmw3610_attr_set_config *cfg = dev->config;

    LOG_INF("Setting PMW3610 attribute %d to %d", cfg->attr, cfg->value);

    if (!device_is_ready(pmw3610_dev)) {
        LOG_ERR("PMW3610 sensor device is not ready");
        return ZMK_BEHAVIOR_OPAQUE;
    }

    struct sensor_value val = { .val1 = cfg->value };

    int ret = sensor_attr_set(pmw3610_dev, SENSOR_CHAN_ALL, (enum sensor_attribute)cfg->attr, &val);
    if (ret) {
        LOG_ERR("Failed to set sensor attribute (err %d)", ret);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct zmk_behavior_api behavior_pmw3610_attr_set_api = {
    .binding_pressed = on_keymap_binding_pressed,
};

#define BSN_INST(n) \
    static struct behavior_pmw3610_attr_set_config behavior_pmw3610_attr_set_config_##n = { \
        .attr = DT_INST_PROP(n, attr), \
        .value = DT_INST_PROP(n, value), \
    }; \
    BEHAVIOR_DT_INST_DEFINE(n, \
        behavior_pmw3610_attr_set_init, \
        NULL, \
        NULL, \
        &behavior_pmw3610_attr_set_config_##n, \
        POST_KERNEL, \
        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
        &behavior_pmw3610_attr_set_api);

DT_INST_FOREACH_STATUS_OKAY(BSN_INST)
