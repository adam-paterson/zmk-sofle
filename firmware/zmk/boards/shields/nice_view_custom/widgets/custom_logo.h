/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: MIT
 */

#ifndef CUSTOM_LOGO_H
#define CUSTOM_LOGO_H

#include <stdint.h>
#include <lvgl.h>

// Sample 40x40 pixel keyboard icon logo
// This is a simple example - replace with your own design!
// Generated using image2cpp: https://javl.github.io/image2cpp/

static const uint8_t keyboard_logo_bitmap[] = {
    // 40x40 pixels = 200 bytes (1 bit per pixel)
    // Simple keyboard icon - box with keys
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    // ... (placeholder - replace with actual bitmap data)
    // Border and keys pattern would go here
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
};

// LVGL image descriptor
const lv_img_dsc_t custom_logo = {
    .header.cf = LV_IMG_CF_ALPHA_1BIT,
    .header.w = 40,
    .header.h = 40,
    .data_size = 200,
    .data = keyboard_logo_bitmap,
};

// Alternative: Text-based "logo" using LVGL label
// You can use this if you don't want to create a bitmap
static void create_text_logo(lv_obj_t *parent) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "SOFLE\nKEYBOARD");
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
}

#endif // CUSTOM_LOGO_H
