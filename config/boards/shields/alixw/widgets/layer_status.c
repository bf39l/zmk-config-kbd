/*
 * Layer status widget for the alixw shield.
 *
 * Adapted from ZMK's built-in widget (app/src/display/widgets/layer_status.c),
 * MIT licensed, Copyright (c) 2020 The ZMK Contributors.
 *
 * One deviation: ZMK's version reports zmk_keymap_highest_layer_active(). The
 * OS switch in alixw.keymap works by toggling WINMODE, a flag layer that sits
 * above every real layer -- so with Windows mode on, "highest active" is always
 * WINMODE (or FN_WIN), and the widget would read "WIN" instead of the layer you
 * are actually typing on. This version reports the highest active layer BELOW
 * the OS-mode layers, leaving MAC/WIN to be shown by the os_mode widget.
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#include "layer_status.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_status_state {
    zmk_keymap_layer_index_t index;
    const char *label;
};

static struct layer_status_state get_state(const zmk_event_t *_eh) {
    /* Walk down from just under the OS-mode layers and take the first active
     * one. Index 0 is the default layer and is always active, so it is the
     * natural floor. */
    zmk_keymap_layer_index_t index = 0;

    for (int i = CONFIG_ALIXW_OS_MODE_LAYER - 1; i > 0; i--) {
        zmk_keymap_layer_index_t candidate = (zmk_keymap_layer_index_t)i;
        if (zmk_keymap_layer_active(zmk_keymap_layer_index_to_id(candidate))) {
            index = candidate;
            break;
        }
    }

    return (struct layer_status_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
    };
}

static void set_layer_symbol(lv_obj_t *label, struct layer_status_state state) {
    char text[16] = {};

    if (state.label == NULL || strlen(state.label) == 0) {
        snprintf(text, sizeof(text), LV_SYMBOL_KEYBOARD " %i", state.index);
    } else {
        snprintf(text, sizeof(text), LV_SYMBOL_KEYBOARD " %s", state.label);
    }

    lv_label_set_text(label, text);
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct alixw_widget_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_symbol(widget->obj, state); }
}

/* Listener name must differ from the public init function below: the macro
 * generates <listener>_init. */
ZMK_DISPLAY_WIDGET_LISTENER(alixw_layer_status, struct layer_status_state, layer_status_update_cb,
                            get_state)
ZMK_SUBSCRIPTION(alixw_layer_status, zmk_layer_state_changed);

int alixw_widget_layer_status_init(struct alixw_widget_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    alixw_layer_status_init();
    return 0;
}

lv_obj_t *alixw_widget_layer_status_obj(struct alixw_widget_layer_status *widget) {
    return widget->obj;
}
