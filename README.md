# Sofle Split Keyboard - ZMK Firmware

Custom ZMK firmware for the Eyelash Sofle split keyboard with personalized keymap layers.

## 📋 Table of Contents

- [Features](#features)
- [Layers](#layers)
- [Build & Flash](#build--flash)
- [Combos](#combos)
- [Troubleshooting](#troubleshooting)
- [Credits](#credits)

## ✨ Features

- **5 Custom Layers**: Base, Lower, Raise, Util, and Code layers for different use cases
- **Joystick Encoder Support**: Mouse cursor control via the middle encoder on all layers
- **ZMK Studio Support**: Live keymap editing via web interface (left side only)
- **RGB Underglow**: Customizable lighting with brightness and effect controls
- **Deep Sleep Mode**: Power-saving mode for travel
- **Split OLED Displays**: Layer and battery status on both halves

## 🗂️ Layers

### Base Layer (Layer 0)
Standard QWERTY layout with essential modifiers and navigation.

| Feature | Description |
|---------|-------------|
| **Layout** | Standard QWERTY |
| **Encoder** | Mouse cursor movement (up/down/left/right) |
| **Thumb Keys** | Space, Enter, Layer access |
| **Modifiers** | GUI, Alt, Ctrl, Shift in standard positions |

**Access**: Default layer, always active

---

### Lower Layer (Layer 1)
Numbers, symbols, and F-keys for data entry and shortcuts.

| Row | Left Side | Right Side |
|-----|-------------|------------|
| Top | F1-F6 | F7-F12 |
| Numbers | 1-5 | 6-0 |
| Symbols | ! @ # $ % | ^ & * ( ) |
| Brackets | = - + { } | [ ] ; : \\ |

**Access**: Hold left inner thumb key (next to space)

---

### Raise Layer (Layer 2)
Navigation, media controls, and editing shortcuts.

| Feature | Description |
|---------|-------------|
| **Navigation** | Arrow keys, Page Up/Down, Home/End |
| **Media** | Insert, Print Screen, Context Menu |
| **Editing** | Undo, Cut, Copy, Paste, Delete, Backspace |
| **Numpad** | 0 key for numeric entry |

**Access**: Hold right inner thumb key (next to enter)

---

### Util Layer (Layer 3)
System utilities, Bluetooth management, and RGB controls.

| Section | Keys |
|---------|------|
| **Bluetooth** | BT Clear All, BT Select 0-4, BT Clear |
| **Output Mode** | USB Mode, BLE Mode |
| **System** | Reset, Soft Off, Bootloader |
| **RGB** | RGB Off/On, Effects, Brightness +/-, Speed +/- |

**Access**: Hold **LOWER** and **RAISE** together

This uses ZMK conditional layers: when layer 1 and layer 2 are both active, layer 3 turns on automatically.

---

### Code Layer (Layer 4)
Programming shortcuts for efficient text navigation and editing.

| Feature | Description |
|---------|-------------|
| **Word Navigation** | Alt+Left/Right (skip words) |
| **Line Navigation** | Cmd+Left/Right (line start/end) |
| **Bracket Navigation** | Ctrl+Left/Right (word-level) |

**Access**: Combo - Press **L + ;** together from the base layer

This is a toggle-style access path for programming shortcuts without needing to hold a thumb layer key.

---

## ⌨️ Combos

Combos allow you to press multiple keys together to trigger special actions.

| Combo | Keys | Action |
|-------|------|--------|
| **Soft Off** | Q + S + Z (hold 2 sec) | Enter deep sleep mode |
| **Util Layer** | Hold LOWER + RAISE | Access utilities (BT, RGB, system) |
| **Code Layer** | L + ; | Toggle/access programming shortcuts |

**Note**: The soft off combo requires holding for 2 seconds. To wake from deep sleep, press the reset button once.

## 🔄 Update History

### 2025/8/22
- Updated soft off functionality (Q+S+Z combo for deep sleep)
- Removed GIF animations on right OLED to reduce power consumption
- Updated case designs with thicker frame and better reset switch access

### 2025/3/30
- Added 1-hour sleep timeout
- Increased debounce time for better stability
- Optimized power consumption after sleep

### 2024/12/21
- Added ZMK Studio support (live keymap editing via web interface)

### 2024/10/24
- Modified power supply mode for reduced consumption
- Fixed automatic RGB shut-off feature

## 🛠️ Build & Flash

### Automated Build (Recommended)

This repository includes GitHub Actions workflows for automatic firmware building.

1. **Push changes** to this repository
2. **GitHub Actions** automatically builds firmware
3. **Download firmware** from the latest GitHub Release
4. **Flash** the `.uf2` files to your keyboard

### Manual Build

If you prefer to build locally:

```bash
# Setup ZMK environment
west init -l config
west update

# Build firmware
west build -p -b eyelash_sofle_left -- -DSHIELD=nice_view -DCONFIG_ZMK_STUDIO=y
west build -p -b eyelash_sofle_right -- -DSHIELD=nice_view
```

### Flashing Instructions

1. **Enter bootloader mode**: Press reset button twice quickly
2. **Connect via USB**: Keyboard appears as mass storage device
3. **Copy firmware**:
   - Left side: `eyelash_sofle_studio_left.uf2`
   - Right side: `eyelash_sofle_right.uf2`
4. **Repeat** for the other half

## 🔧 Troubleshooting

### OLED Display Not Working
- Check that both halves are properly connected
- Verify the OLED cables are seated correctly
- Try resetting both halves

### Bluetooth Not Pairing
- Flash `settings_reset.uf2` to clear all pairings
- Re-flash both firmware files
- Pair again from your device

### Keys Not Responding
- Check battery level (OLED shows battery status)
- Verify keymap was built correctly
- Reset both halves to re-pair

### ZMK Studio Not Connecting
- Ensure left side is connected via USB
- Use a compatible browser (Chrome, Edge)
- Check that Studio-enabled firmware was flashed

## 🎨 Keymap Diagram

![Sofle Keymap](keymap-drawer/eyelash_sofle.svg)

*The generated keymap diagram shows all 5 layers with proper icons and legends.*

## 📝 Credits

- **Keyboard**: Eyelash Sofle by [a741725193](https://github.com/a741725193)
- **Firmware**: [ZMK Firmware](https://github.com/zmkfirmware/zmk)
- **Keymap Diagram**: [Keymap Drawer](https://github.com/caksoylar/keymap-drawer)

---

For issues with this specific configuration, please open an issue in this repository.

For hardware issues or 3D model files, contact: 380465425@qq.com
