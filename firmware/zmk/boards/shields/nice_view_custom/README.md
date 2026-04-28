# Custom Nice!View Shield for Right Side

This directory contains a custom OLED display configuration for the right side of your Sofle keyboard.

## Features

- **Custom Logo**: Display your own 40x40 pixel bitmap logo
- **Layer Indicator**: Shows current keymap layer name
- **Battery Status**: Shows battery level with icon and percentage
- **WPM Counter**: Shows typing speed (words per minute)

## Customizing Your Logo

### Step 1: Create Your Logo Image

1. Design a **40x40 pixel** black and white image
2. Use tools like:
   - GIMP (free)
   - Photoshop
   - Online: https://javl.github.io/image2cpp/

### Step 2: Convert to C Array

Use [image2cpp](https://javl.github.io/image2cpp/) or similar tool:

1. Upload your 40x40 PNG/BMP
2. Settings:
   - Canvas size: 40x40
   - Background color: Black
   - Invert image colors: Yes (for OLED)
   - Code output format: Arduino code
   - Draw mode: Horizontal - 1 bit per pixel
3. Generate code
4. Copy the array output

### Step 3: Add to Firmware

Replace the placeholder in `widgets/custom_widgets.c`:

```c
// Replace this placeholder:
static const uint8_t custom_logo_bitmap[] = {
    0x00, 0x00, ... // Your logo data here
};

// With your generated array:
static const uint8_t custom_logo_bitmap[] = {
    0xFF, 0x81, 0x81, ... // Your 40x40 bitmap (200 bytes)
};
```

Then add the LVGL image declaration:

```c
const lv_img_dsc_t custom_logo = {
    .header.cf = LV_IMG_CF_ALPHA_1BIT,
    .header.w = 40,
    .header.h = 40,
    .data_size = 200,
    .data = custom_logo_bitmap,
};
```

## Widget Layout

```
┌─────────────────┐
│                 │
│   [LOGO]        │  <- 40x40px custom logo
│                 │
│   LAYER 0       │  <- Current layer name
│                 │
│ [BAT] WPM:45    │  <- Battery icon + WPM
└─────────────────┘
```

## Adding Custom Widgets

### Example: Add a Clock Widget

1. Create new files:
   - `widgets/clock_widget.h`
   - `widgets/clock_widget.c`

2. In `clock_widget.h`:
```c
#pragma once
#include <lvgl.h>

struct zmk_widget_clock {
    lv_obj_t *obj;
    lv_obj_t *time_label;
};

int zmk_widget_clock_init(struct zmk_widget_clock *widget, lv_obj_t *parent);
```

3. In `clock_widget.c`:
```c
#include "clock_widget.h"
#include <zephyr/kernel.h>

int zmk_widget_clock_init(struct zmk_widget_clock *widget, lv_obj_t *parent) {
    widget->obj = lv_cont_create(parent);
    widget->time_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->time_label, "12:00");
    return 0;
}
```

4. Add to `CMakeLists.txt`:
```cmake
target_sources(app PRIVATE widgets/clock_widget.c)
```

5. Include in `custom_status_screen.c` and position it

## Configuration Options

In `nice_view_custom.conf`, you can adjust:

```conf
# Display refresh rate
CONFIG_LV_DISP_DEF_REFR_PERIOD=16  # 60 FPS

# Font sizes
CONFIG_LV_FONT_MONTSERRAT_12=y     # Small text
CONFIG_LV_FONT_MONTSERRAT_16=y     # Medium text  
CONFIG_LV_FONT_MONTSERRAT_24=y     # Large text

# Enable/disable specific widgets
CONFIG_ZMK_WIDGET_BATTERY_STATUS=y
CONFIG_ZMK_WIDGET_LAYER_STATUS=y
CONFIG_ZMK_WIDGET_WPM_STATUS=y
```

## Building the Firmware

1. Push changes to GitHub
2. GitHub Actions will automatically build
3. Download the artifact: `eyelash_sofle_right_custom.uf2`
4. Flash to the right side keyboard

## Troubleshooting

### Display not showing
- Check wiring and connections
- Verify `nice_view_custom.overlay` matches your hardware
- Enable logging: `CONFIG_ZMK_USB_LOGGING=y`

### Logo not displaying
- Ensure bitmap is exactly 40x40 pixels
- Check array size matches (40x40 = 200 bytes for 1-bit)
- Verify `LV_IMG_CF_ALPHA_1BIT` format

### Widgets overlapping
- Adjust positioning in `custom_status_screen.c`
- Check LVGL layout settings
- Reduce widget sizes

## Resources

- [ZMK Display Documentation](https://zmk.dev/docs/features/displays)
- [LVGL Documentation](https://docs.lvgl.io/8.3/)
- [image2cpp](https://javl.github.io/image2cpp/) - Image to C array converter
- [GIMP](https://www.gimp.org/) - Free image editor
