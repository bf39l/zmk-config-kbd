/*
 * Layer status widget for the alixw shield.
 *
 * Adapted from ZMK's built-in widget (app/src/display/widgets/layer_status.c),
 * MIT licensed, Copyright (c) 2020 The ZMK Contributors.
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct alixw_widget_layer_status {
    sys_snode_t node;
    lv_obj_t *obj;
};

int alixw_widget_layer_status_init(struct alixw_widget_layer_status *widget, lv_obj_t *parent);
lv_obj_t *alixw_widget_layer_status_obj(struct alixw_widget_layer_status *widget);
