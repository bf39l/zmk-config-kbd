/*
 * Output status widget for the alixw shield.
 *
 * Adapted from ZMK's built-in widget (app/src/display/widgets/output_status.c),
 * MIT licensed, Copyright (c) 2020 The ZMK Contributors.
 *
 * Two deviations from the stock widget, both for legibility on a 128x32 1-bit
 * SSD1306 at Montserrat 16:
 *
 *   - USB: LV_SYMBOL_USB (U+F287) is a thin diagonal trident. Thresholding its
 *     4bpp anti-aliasing down to 1bpp shreds it into disconnected blobs, so the
 *     literal text "USB" is used instead.
 *   - BLE: stock ZMK uses LV_SYMBOL_WIFI (U+F1EB), a near-solid broadcast fan
 *     that reads as a filled block at this size. LV_SYMBOL_BLUETOOTH (U+F293)
 *     is already compiled into the same font and is far more distinct.
 *
 * The tick / cross / cog markers are kept -- they are thick shapes and survive
 * the 1-bit conversion cleanly.
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

#include "output_status.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    enum zmk_transport preferred_transport;
#if IS_ENABLED(CONFIG_ZMK_BLE)
    /* Taken from the active profile rather than the selected endpoint: when the
     * link drops, selected_endpoint.transport goes to NONE and its ble union
     * member is meaningless, but we still want to name the profile. */
    int active_profile_index;
    bool active_profile_connected;
#endif
};

static struct output_status_state get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoint_get_selected(),
        .preferred_transport = zmk_endpoint_get_preferred_transport(),
#if IS_ENABLED(CONFIG_ZMK_BLE)
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
#endif
    };
}

static void set_status_symbol(lv_obj_t *label, struct output_status_state state) {
    /* Every LV_SYMBOL_* is a 3-byte UTF-8 sequence. Longest string here is
     * BLUETOOTH + " N " + OK = 3 + 3 + 3 = 9 bytes; 24 leaves plenty of slack so
     * a symbol can never be cut mid-sequence (cf. zmkfirmware/zmk#2444, where an
     * undersized buffer truncated symbols into invalid UTF-8). */
    char text[24] = {};

    enum zmk_transport transport = state.selected_endpoint.transport;
    bool connected = transport != ZMK_TRANSPORT_NONE;

    if (!connected) {
#if IS_ENABLED(CONFIG_ZMK_BLE)
        /* Nothing is connected. Stock ZMK falls back to the *preferred*
         * transport here -- and because ZMK_USB=y makes USB the default
         * preferred transport, a dropped Bluetooth link rendered as "USB x",
         * which said nothing about the profile that actually went down. Show
         * the BLE profile instead, so a disconnect always reads as
         * <bluetooth> <profile> <cross>. */
        transport = ZMK_TRANSPORT_BLE;
#else
        // Show what we're *trying* to connect to.
        transport = state.preferred_transport;
#endif
    }

    switch (transport) {
    case ZMK_TRANSPORT_USB:
        strcat(text, "USB");
        if (!connected) {
            strcat(text, " " LV_SYMBOL_CLOSE);
        }
        break;

#if IS_ENABLED(CONFIG_ZMK_BLE)
    case ZMK_TRANSPORT_BLE:
        /* Tick when the profile is up, cross when it is not. Stock ZMK also has
         * a third state -- LV_SYMBOL_SETTINGS for an unbonded/open profile --
         * dropped here because a cog is indistinguishable from a cross at 16px
         * on a 1-bit panel. Add it back in this branch if you want it. */
        snprintf(text, sizeof(text), LV_SYMBOL_BLUETOOTH " %i %s",
                 state.active_profile_index + 1,
                 state.active_profile_connected ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE);
        break;
#endif

    default:
        strcat(text, LV_SYMBOL_CLOSE);
        break;
    }

    lv_label_set_text(label, text);
}

static void output_status_update_cb(struct output_status_state state) {
    struct alixw_widget_output_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

/* Listener name must NOT be alixw_widget_output_status: the macro generates
 * <listener>_init, which would collide with the public init function below. */
ZMK_DISPLAY_WIDGET_LISTENER(alixw_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(alixw_output_status, zmk_endpoint_changed);
// We don't get an endpoint changed event when the active profile connects/disconnects
// but there wasn't another endpoint to switch from/to, so update on BLE events too.
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(alixw_output_status, zmk_ble_active_profile_changed);
#endif

int alixw_widget_output_status_init(struct alixw_widget_output_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    alixw_output_status_init();
    return 0;
}

lv_obj_t *alixw_widget_output_status_obj(struct alixw_widget_output_status *widget) {
    return widget->obj;
}
