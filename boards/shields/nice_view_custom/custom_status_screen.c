/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include "widgets/custom_widgets.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Custom status screen for the right side
struct custom_status_screen {
    lv_obj_t *screen;
    struct zmk_widget_logo logo;
    struct zmk_widget_battery_status battery;
    struct zmk_widget_layer_status layer;
    struct zmk_widget_wpm_status wpm;
};

static struct custom_status_screen custom_screen;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    
    // Create a container for layout
    lv_obj_t *main_cont = lv_cont_create(screen);
    lv_obj_set_size(main_cont, 128, 64);
    lv_cont_set_layout(main_cont, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit(main_cont, LV_FIT_NONE);
    lv_obj_align(main_cont, NULL, LV_ALIGN_CENTER, 0, 0);
    
    // Add your custom logo at the top (40x40 pixels)
    zmk_widget_logo_init(&custom_screen.logo, main_cont);
    lv_obj_align(zmk_widget_logo_obj(&custom_screen.logo), NULL, LV_ALIGN_IN_TOP_MID, 0, 2);
    
    // Add layer status below the logo
    zmk_widget_layer_status_init(&custom_screen.layer, main_cont);
    lv_obj_align(zmk_widget_layer_status_obj(&custom_screen.layer), NULL, LV_ALIGN_IN_TOP_MID, 0, 44);
    
    // Add battery and WPM in a row at the bottom
    lv_obj_t *bottom_row = lv_cont_create(main_cont);
    lv_obj_set_size(bottom_row, 120, 16);
    lv_cont_set_layout(bottom_row, LV_LAYOUT_ROW_MID);
    lv_cont_set_fit(bottom_row, LV_FIT_NONE);
    lv_obj_align(bottom_row, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -2);
    
    zmk_widget_battery_status_init(&custom_screen.battery, bottom_row);
    zmk_widget_wpm_status_init(&custom_screen.wpm, bottom_row);
    
    return screen;
}
