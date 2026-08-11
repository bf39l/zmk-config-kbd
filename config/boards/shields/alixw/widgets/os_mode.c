/*
 * OS mode widget for the alixw shield.
 *
 * Replaces the stock WPM widget in the bottom-right corner. ZMK cannot detect
 * the host OS (zmkfirmware/zmk#2553 is open, and over BLE there is no USB
 * enumeration to fingerprint), so the mode is whatever WINMODE is toggled to --
 * this widget just reports that state back so it is visible at a glance.
 *
 * Layer state is not persisted, so this reads MAC again after any power cycle.
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#include "os_mode.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct os_mode_state {
    bool win;
};

static struct os_mode_state get_state(const zmk_event_t *_eh) {
    return (struct os_mode_state){
        .win = zmk_keymap_layer_active(zmk_keymap_layer_index_to_id(CONFIG_ALIXW_OS_MODE_LAYER)),
    };
}

static void set_os_mode(lv_obj_t *label, struct os_mode_state state) {
    lv_label_set_text(label, state.win ? "WIN" : "MAC");
}

static void os_mode_update_cb(struct os_mode_state state) {
    struct alixw_widget_os_mode *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_os_mode(widget->obj, state); }
}

/* Listener name must differ from the public init function below: the macro
 * generates <listener>_init. */
ZMK_DISPLAY_WIDGET_LISTENER(alixw_os_mode, struct os_mode_state, os_mode_update_cb, get_state)
ZMK_SUBSCRIPTION(alixw_os_mode, zmk_layer_state_changed);

int alixw_widget_os_mode_init(struct alixw_widget_os_mode *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    alixw_os_mode_init();
    return 0;
}

lv_obj_t *alixw_widget_os_mode_obj(struct alixw_widget_os_mode *widget) { return widget->obj; }
