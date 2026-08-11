/*
 * Custom status screen for the alixw shield.
 *
 * Adapted from ZMK's built-in status screen (app/src/display/status_screen.c),
 * MIT licensed, Copyright (c) 2020 The ZMK Contributors.
 *
 * Identical layout to the built-in screen -- the only change is swapping ZMK's
 * output status widget for the shield-local one, which renders USB/BLE with
 * glyphs that survive 1-bit rendering at 16px. Battery, layer and WPM are
 * ZMK's own widgets, reused as-is.
 */

#include <lvgl.h>
#include <zephyr/kernel.h>

#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>

#include "widgets/layer_status.h"
#include "widgets/os_mode.h"
#include "widgets/output_status.h"

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

static struct alixw_widget_output_status output_status_widget;
static struct alixw_widget_layer_status layer_status_widget;
static struct alixw_widget_os_mode os_mode_widget;

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    alixw_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(alixw_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

    /* Bottom row, both on the theme's small font so the two labels sit on a
     * matching baseline. Layer on the left, OS mode on the right where the WPM
     * counter used to be. */
    alixw_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_set_style_text_font(alixw_widget_layer_status_obj(&layer_status_widget),
                               lv_theme_get_font_small(screen), LV_PART_MAIN);
    lv_obj_align(alixw_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);

    alixw_widget_os_mode_init(&os_mode_widget, screen);
    lv_obj_set_style_text_font(alixw_widget_os_mode_obj(&os_mode_widget),
                               lv_theme_get_font_small(screen), LV_PART_MAIN);
    lv_obj_align(alixw_widget_os_mode_obj(&os_mode_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    return screen;
}
