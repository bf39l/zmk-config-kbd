/*
 * OS mode widget for the alixw shield.
 *
 * Renders "MAC" or "WIN" depending on whether the WINMODE flag layer is
 * toggled on -- see the conditional-layers block in alixw.keymap.
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct alixw_widget_os_mode {
    sys_snode_t node;
    lv_obj_t *obj;
};

int alixw_widget_os_mode_init(struct alixw_widget_os_mode *widget, lv_obj_t *parent);
lv_obj_t *alixw_widget_os_mode_obj(struct alixw_widget_os_mode *widget);
