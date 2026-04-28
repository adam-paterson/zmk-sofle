# Flashing Guide

Complete guide for flashing and updating firmware on the Eyelash Sofle keyboard.

## Overview

The Eyelash Sofle uses nice!nano v2 controllers. Firmware is distributed as `.uf2` files that can be flashed via USB in bootloader mode.

**Firmware Files:**
| File | Purpose | When to Flash |
|------|---------|---------------|
| `adams_sofle_studio_left.uf2` | Left half with ZMK Studio | New install or updates |
| `adams_sofle_right.uf2` | Right half firmware | New install or updates |
| `settings_reset-nice_nano_v2-zmk.uf2` | Clear all settings | Bluetooth issues, bricked device |

## Initial Flash (New Keyboard)

### Step 1: Download Firmware

1. Go to [Releases](../../releases)
2. Download the latest `firmware.zip`
3. Extract to get the three `.uf2` files

### Step 2: Flash Left Half

1. **Disconnect** the USB cable from the right half
2. **Connect** USB cable to the **left** half
3. **Enter bootloader mode**: Press the reset button on the left controller **twice quickly**
   - The controller will appear as a USB drive named `NICENANO`
4. **Copy firmware**: Drag `adams_sofle_studio_left.uf2` to the `NICENANO` drive
5. **Wait**: The controller will automatically reboot when done
   - The drive will eject itself

### Step 3: Flash Right Half

1. **Disconnect** USB from left half
2. **Connect** USB cable to the **right** half
3. **Enter bootloader mode**: Press the reset button on the right controller **twice quickly**
4. **Copy firmware**: Drag `adams_sofle_right.uf2` to the `NICENANO` drive
5. **Wait**: The controller will automatically reboot

### Step 4: Connect Halves

1. Connect the USB cable to the **left** half (primary/controller)
2. The right half connects via the TRRS cable
3. Both halves should now communicate

### Step 5: Pair with Device

1. The left half is now in pairing mode (flashing LED)
2. On your computer/phone, open Bluetooth settings
3. Look for `Adams Sofle` or similar
4. Click connect/pair
5. Accept any pairing prompts

## Update Existing Firmware

Updating is the same as initial flash - the new firmware overwrites the old.

1. Download latest firmware from [Releases](../../releases)
2. Flash left half with `adams_sofle_studio_left.uf2`
3. Flash right half with `adams_sofle_right.uf2`
4. **Settings are preserved** (Bluetooth pairings, keymap changes via Studio)

## Left vs Right Half Considerations

| Aspect | Left Half | Right Half |
|--------|-----------|------------|
| **Primary** | Yes (connects to PC) | No (connects to left) |
| **USB** | Connects to computer | Gets power via TRRS |
| **Firmware** | `adams_sofle_studio_left.uf2` | `adams_sofle_right.uf2` |
| **ZMK Studio** | Supported | Not needed |
| **Bluetooth** | Direct to host | Via left half |

**Important:** Always flash the correct firmware to each half. Swapping them will cause communication issues.

## Recovery Mode (Bricked Device)

Symptoms of a bricked device:
- Keyboard doesn't respond
- No LED activity
- Won't enter bootloader mode normally
- Bluetooth won't connect

### Soft Recovery: Settings Reset

Try this first before full recovery:

1. Download `settings_reset-nice_nano_v2-zmk.uf2` from [Releases](../../releases)
2. Flash it to **both halves** (same process as normal firmware)
3. This clears all Bluetooth pairings and settings
4. Re-pair with your devices

### Hard Recovery: Forced Bootloader

If the reset button doesn't work:

1. **Disconnect USB and TRRS cables**
2. **Hold the reset button down** while plugging in USB
3. Continue holding for 2 seconds, then release
4. The `NICENANO` drive should appear
5. Flash `settings_reset-nice_nano_v2-zmk.uf2` first
6. Then flash the normal firmware

### Last Resort: Re-flash Bootloader

If the above doesn't work, the bootloader itself may need re-flashing. This requires:
- A SWD programmer (like a Raspberry Pi Pico or J-Link)
- Direct access to the controller pins
- Advanced technical knowledge

Contact the keyboard vendor or ZMK community for assistance.

## Troubleshooting

### Bluetooth Won't Connect

1. Flash `settings_reset-nice_nano_v2-zmk.uf2` to **both halves**
2. Re-flash normal firmware
3. Clear the device from your computer's Bluetooth settings
4. Re-pair from scratch

### Split Halves Won't Talk to Each Other

1. Reset both halves at the same time:
   - Press both reset buttons simultaneously
   - Or power cycle both
2. Check the TRRS cable is fully inserted
3. Try a different TRRS cable
4. Flash settings reset to both, then re-flash firmware

### Flash Drive Doesn't Appear

1. Try a different USB cable (must be data-capable, not charge-only)
2. Try a different USB port
3. Ensure you're double-clicking the reset button quickly
4. Try the "hold reset while plugging USB" method

### ZMK Studio Not Working

1. Ensure you flashed the `_studio_left.uf2` file
2. Connect to the left half via USB
3. Visit [ZMK Studio](https://zmk.studio/) in a WebHID-capable browser (Chrome, Edge)
4. Click "Connect" and select your keyboard

## Quick Reference

### Enter Bootloader
- **Normal**: Double-press reset button quickly
- **Forced**: Hold reset while plugging in USB

### File Locations
- **Latest firmware**: [GitHub Releases](../../releases)
- **CI artifacts**: Actions → Build ZMK firmware → latest run → Artifacts

### Key Combo: Deep Sleep
Hold `Q + S + Z` for 2 seconds to enter deep sleep mode (preserves battery).

## Advanced: Split Half Roles

The left half is always the **central** (connects to host). The right is the **peripheral**.

**To swap roles** (not recommended for this firmware):
- You would need to modify the firmware and rebuild
- This firmware is configured for left-as-central

## Safety Notes

- Always disconnect TRRS cable before plugging/unplugging USB
- This prevents potential short circuits
- Use quality USB cables (some cheap cables are charge-only)

## Resources

- [ZMK Troubleshooting](https://zmk.dev/docs/troubleshooting)
- [nice!nano Documentation](https://nicekeyboard.com/docs/nice-nano/)
- [Eyelash Sofle Build Guide](https://github.com/a741725193/eyelash_sofle)
