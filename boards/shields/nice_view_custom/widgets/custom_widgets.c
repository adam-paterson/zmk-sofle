/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: MIT
 */

#include "custom_widgets.h"
#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/wpm.h>
#include <zmk/battery.h>
#include <zmk/keymap.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// ============ CUSTOM LOGO WIDGET ============
// 128x64 pixel monochrome bitmap - replace with your own!
// This is a simple placeholder pattern showing a keyboard icon

static const uint8_t custom_logo_bitmap[] = {
    // 128x64 bitmap data (1024 bytes)
    // You can generate this from a PNG/BMP using tools like:
    // https://javl.github.io/image2cpp/ or GIMP export to XBM
    
    // Simple keyboard icon pattern (example)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ... (your bitmap data here)
    // For now, this is a placeholder - you would replace with your actual logo
};

LV_IMG_DECLARE(custom_logo);

int zmk_widget_logo_init(struct zmk_widget_logo *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);
    lv_img_set_src(widget->obj, &custom_logo);
    return 0;
}

lv_obj_t *zmk_widget_logo_obj(struct zmk_widget_logo *widget) {
    return widget->obj;
}

// ============ BATTERY WIDGET ============

static void battery_status_update_cb(struct battery_state state) {
    struct zmk_widget_battery_status *widget = CONTAINER_OF(state, struct zmk_widget_battery_status, obj);
    
    uint8_t level = state.level;
    
    // Update icon based on battery level
    if (level > 80) {
        lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_FULL);
    } else if (level > 60) {
        lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_3);
    } else if (level > 40) {
        lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_2);
    } else if (level > 20) {
        lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_1);
    } else {
        lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_EMPTY);
    }
    
    // Update percentage text
    char text[5];
    snprintf(text, sizeof(text), "%d%%", level);
    lv_label_set_text(widget->label, text);
}

static void battery_state_changed_cb(const struct zmk_battery_state_changed *eh) {
    struct zmk_widget_battery_status *widget = CONTAINER_OF(eh, struct zmk_widget_battery_status, obj);
    battery_status_update_cb(widget, eh->state);
}

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_cont_create(parent);
    lv_obj_set_size(widget->obj, 64, 32);
    
    // Battery icon
    widget->icon = lv_label_create(widget->obj);
    lv_label_set_text(widget->icon, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(widget->icon, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    
    // Battery percentage
    widget->label = lv_label_create(widget->obj);
    lv_label_set_text(widget->label, "100%");
    lv_obj_align(widget->label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    
    // Subscribe to battery events
    zmk_battery_state_changed_subscribe(widget, battery_state_changed_cb);
    
    return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
    return widget->obj;
}

// ============ LAYER WIDGET ============

static void layer_status_update_cb(struct zmk_widget_layer_status *widget, uint8_t index) {
    const char *layer_name = zmk_keymap_layer_name(index);
    lv_label_set_text(widget->label, layer_name);
}

static void layer_state_changed_cb(const struct zmk_layer_state_changed *eh) {
    struct zmk_widget_layer_status *widget = CONTAINER_OF(eh, struct zmk_widget_layer_status, obj);
    layer_status_update_cb(widget, eh->layer);
}

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_cont_create(parent);
    lv_obj_set_size(widget->obj, 128, 20);
    
    // Layer name label
    widget->label = lv_label_create(widget->obj);
    lv_label_set_text(widget->label, "LAYER 0");
    lv_obj_align(widget->label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_style(widget->label, &lv_style_plain);
    
    // Subscribe to layer events
    zmk_layer_state_changed_subscribe(widget, layer_state_changed_cb);
    
    return 0;
}

lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget) {
    return widget->obj;
}

// ============ WPM WIDGET ============

static void wpm_status_update_cb(struct zmk_widget_wpm_status *widget, uint8_t wpm) {
    char text[8];
    snprintf(text, sizeof(text), "WPM:%d", wpm);
    lv_label_set_text(widget->label, text);
}

static void wpm_state_changed_cb(const struct zmk_wpm_state_changed *eh) {
    struct zmk_widget_wpm_status *widget = CONTAINER_OF(eh, struct zmk_widget_wpm_status, obj);
    wpm_status_update_cb(widget, eh->state);
}

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent) {
    widget->obj = lv_cont_create(parent);
    lv_obj_set_size(widget->obj, 64, 20);
    
    // WPM label
    widget->label = lv_label_create(widget->obj);
    lv_label_set_text(widget->label, "WPM:0");
    lv_obj_align(widget->label, NULL, LV_ALIGN_CENTER, 0, 0);
    
    // Subscribe to WPM events
    zmk_wpm_state_changed_subscribe(widget, wpm_state_changed_cb);
    
    return 0;
}

lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget) {
    return widget->obj;
}
