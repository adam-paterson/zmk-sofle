/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <lvgl.h>

// Custom Logo Widget
struct zmk_widget_logo {
    lv_obj_t *obj;
};

int zmk_widget_logo_init(struct zmk_widget_logo *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_logo_obj(struct zmk_widget_logo *widget);

// Battery Widget
struct zmk_widget_battery_status {
    lv_obj_t *obj;
    lv_obj_t *icon;
    lv_obj_t *label;
};

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget);

// Layer Widget
struct zmk_widget_layer_status {
    lv_obj_t *obj;
    lv_obj_t *label;
};

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget);

// WPM Widget
struct zmk_widget_wpm_status {
    lv_obj_t *obj;
    lv_obj_t *label;
};

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget);
