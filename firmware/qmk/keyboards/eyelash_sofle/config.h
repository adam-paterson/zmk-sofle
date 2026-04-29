#ifndef CONFIG_H
#define CONFIG_H

// QMK config for eyelash_sofle
// TODO: Configure for split keyboard support

#define MASTER_LEFT
#define SPLIT_USB_DETECT
#define SPLIT_WATCHDOG_ENABLE
#define SPLIT_WATCHDOG_TIMEOUT 3000

// OLED settings (if using nice_view)
#ifdef OLED_ENABLE
#define OLED_DISPLAY_ADDRESS 0x3C
#define OLED_BRIGHTNESS 255
#endif

// RGB settings (if applicable)
#ifdef RGB_MATRIX_ENABLE
#define RGB_MATRIX_SPLIT { 36, 36 }
#endif

#endif
