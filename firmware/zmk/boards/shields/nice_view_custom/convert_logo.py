#!/usr/bin/env python3
"""
Helper script to convert PNG images to C arrays for ZMK OLED displays.

Usage:
    python3 convert_logo.py your_logo.png

Requirements:
    pip install pillow

Output:
    - Creates a C header file with your logo bitmap
    - Prints instructions for integration
"""

from PIL import Image
import sys
import os

def convert_image_to_c_array(image_path, output_name="custom_logo"):
    """Convert a PNG/BMP image to C array for ZMK OLED display."""
    
    # Open and convert image
    img = Image.open(image_path)
    
    # Convert to 1-bit monochrome (black and white)
    img = img.convert('1')
    
    # Resize to 40x40 if needed
    if img.size != (40, 40):
        print(f"Resizing from {img.size} to (40, 40)")
        img = img.resize((40, 40), Image.Resampling.LANCZOS)
    
    width, height = img.size
    pixels = list(img.getdata())
    
    # Convert to bytes (1 bit per pixel, horizontal encoding)
    bytes_array = []
    for row in range(height):
        for col in range(0, width, 8):
            byte = 0
            for bit in range(8):
                if col + bit < width:
                    # Invert because OLED: 0 = black (on), 255 = white (off)
                    pixel = 0 if pixels[row * width + col + bit] == 255 else 1
                    byte |= (pixel << (7 - bit))
            bytes_array.append(byte)
    
    # Generate C code
    c_code = f'''/*
 * Copyright (c) 2024 Your Name
 * SPDX-License-Identifier: MIT
 *
 * Auto-generated logo from: {os.path.basename(image_path)}
 * Size: {width}x{height} pixels
 */

#ifndef {output_name.upper()}_H
#define {output_name.upper()}_H

#include <lvgl.h>

// {width}x{height} pixel bitmap ({len(bytes_array)} bytes)
static const uint8_t {output_name}_bitmap[] = {{
'''
    
    # Format bytes as hex values
    for i, byte in enumerate(bytes_array):
        if i % 10 == 0:
            c_code += "\n    "
        c_code += f"0x{byte:02X}, "
    
    c_code += f'''
}};

// LVGL image descriptor for {output_name}
const lv_img_dsc_t {output_name} = {{
    .header.cf = LV_IMG_CF_ALPHA_1BIT,
    .header.w = {width},
    .header.h = {height},
    .data_size = {len(bytes_array)},
    .data = {output_name}_bitmap,
}};

#endif // {output_name.upper()}_H
'''
    
    # Save to file
    output_file = f"{output_name}.h"
    with open(output_file, 'w') as f:
        f.write(c_code)
    
    print(f"✓ Created: {output_file}")
    print(f"\nNext steps:")
    print(f"1. Copy {output_file} to: boards/shields/nice_view_custom/widgets/")
    print(f"2. Replace #include \"custom_logo.h\" in custom_widgets.c with your new file")
    print(f"3. Update LV_IMG_DECLARE(custom_logo) to match your logo name")
    print(f"4. Build and flash the firmware")
    
    return c_code

def create_placeholder_logo():
    """Create a simple placeholder keyboard logo."""
    
    # Create simple 40x40 keyboard icon
    img = Image.new('1', (40, 40), 1)  # White background
    
    # Draw a simple keyboard shape (black rectangle with some "keys")
    from PIL import ImageDraw
    draw = ImageDraw.Draw(img)
    
    # Outer keyboard box
    draw.rectangle([4, 10, 35, 30], fill=0, outline=0)
    
    # Some key rectangles
    for row in range(3):
        for col in range(8):
            x = 6 + col * 4
            y = 12 + row * 6
            if x + 2 < 35 and y + 4 < 30:
                draw.rectangle([x, y, x+2, y+4], fill=0)
    
    img.save("placeholder_logo.png")
    print("✓ Created: placeholder_logo.png")
    return "placeholder_logo.png"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("ZMK OLED Logo Converter")
        print("=" * 50)
        print("\nUsage:")
        print("  python3 convert_logo.py <image.png>")
        print("\nOr create a placeholder:")
        print("  python3 convert_logo.py --placeholder")
        print("\nRequirements:")
        print("  pip install pillow")
        print("\nImage should be:")
        print("  - 40x40 pixels (recommended)")
        print("  - Black and white only")
        print("  - PNG or BMP format")
        sys.exit(1)
    
    if sys.argv[1] == "--placeholder":
        image_file = create_placeholder_logo()
    else:
        image_file = sys.argv[1]
        if not os.path.exists(image_file):
            print(f"Error: File not found: {image_file}")
            sys.exit(1)
    
    print(f"Converting: {image_file}")
    convert_image_to_c_array(image_file)
