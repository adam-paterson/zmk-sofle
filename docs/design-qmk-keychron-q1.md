# Design: QMK Userspace Integration for Keychron Q1 V2 ISO

## Overview

This document describes the QMK userspace integration for the Keychron Q1 V2 ISO keyboard, supporting dual layouts (Colemak-DH and QWERTY) within the existing multi-firmware monorepo architecture.

## Goals

- Integrate QMK firmware support for Keychron Q1 V2 ISO alongside existing ZMK support
- Support both Colemak-DH and QWERTY layouts using Miryoku-inspired layer structure
- Follow QMK userspace conventions with `user.overlay_dir` configuration
- Enable builds via `qmk compile` command
- Maintain consistency with existing project architecture

## Research Summary

### QMK Userspace Structure

QMK userspace allows maintaining custom keymaps outside the main QMK firmware repository. The standard structure:

```
qmk_userspace/
├── qmk.json              # Build configuration
├── keyboards/            # Keyboard-specific definitions
│   └── <keyboard_name>/
│       └── keymaps/
│           └── <keymap_name>/
│               ├── keymap.c
│               ├── rules.mk
│               └── config.h
└── users/                # Shared user code (optional)
```

### QMK Configuration

Userspace is activated by setting the overlay directory:
```bash
qmk config user.overlay_dir=/path/to/userspace
```

Build command structure:
```bash
qmk compile -kb <keyboard> -km <keymap>
```

For Keychron Q1 V2 ISO:
- Keyboard identifier: `keychron/q1v2/iso_encoder`
- Encoder support included via `/iso_encoder` suffix

### Keymap Structure Differences (ZMK vs QMK)

| Aspect | ZMK | QMK |
|--------|-----|-----|
| Layer definition | `#define LAYER_NAME` macro | `const uint16_t PROGMEM keymaps[]` array |
| Key codes | `&kp Q`, `&kp TAB` | `KC_Q`, `KC_TAB` |
| Layer access | `&mo LAYER` | `MO(LAYER)` |
| Hold-tap | `&mt MOD KEY` | `MT(MOD, KC)` |
| Combos | `combo` node in devicetree | `COMBO` action in keymap |
| Mod-morph | `&mm` behavior | Custom macro or `LM` |

## Proposed Directory Structure

```
config/qmk/
└── keychron_q1v2/
    └── iso_encoder/
        └── keymaps/
            ├── keychron_q1v2_iso_colemakdh/
            │   ├── keymap.c
            │   └── rules.mk
            └── keychron_q1v2_iso_qwerty/
                ├── keymap.c
                └── rules.mk
```

**Rationale**: This structure follows QMK userspace conventions where keymaps are organized under `keyboards/<keyboard_path>/keymaps/<keymap_name>/`. The nested directory structure matches QMK's keyboard hierarchy (`keychron/q1v2/iso_encoder`).

## Keymap Design

### Physical Layout: Keychron Q1 V2 ISO

The Q1 V2 ISO features:
- 83 keys (compact TKL with ISO enter and left shift)
- ISO layout (vertical enter, split left shift)
- Rotary encoder (volume knob)
- No split spacebar

### Layer Structure

Both keymaps use a Miryoku-inspired layer system:

| Layer | Purpose | Activation |
|-------|---------|------------|
| `BASE` | Alphanumeric typing | Default |
| `SYMS` | Numbers, F-keys, symbols | Left thumb (LT) |
| `NAV` | Arrows, editing, clipboard | Right thumb (LT) |
| `UTIL` | Bluetooth, RGB, system | Combo: SYMS + NAV |
| `CODE` | Programming shortcuts | Right outer thumb (LT) |
| `ADJUST` | Settings, reset | Combo or triple-tap |

### Keymap Content Plan: Colemak-DH

**File**: `config/qmk/keychron_q1v2/iso_encoder/keymaps/keychron_q1v2_iso_colemakdh/keymap.c`

```c
#include QMK_KEYBOARD_H

// Layer enumeration
enum layers {
    BASE,
    SYMS,
    NAV,
    UTIL,
    CODE,
    ADJUST
};

// Layer-tap definitions for thumbs
#define THUMB_LEFT   LT(SYMS, KC_TAB)
#define THUMB_RIGHT  LT(NAV, KC_BSPC)
#define THUMB_OUTER  LT(CODE, KC_ENT)

// Home row mods (Colemak-DH positioning)
// Left: A (GUI), R (ALT), S (CTL), T (SFT)
// Right: N (SFT), E (CTL), I (ALT), O (GUI)
#define HM_A MT(MOD_LGUI, KC_A)
#define HM_R MT(MOD_LALT, KC_R)
#define HM_S MT(MOD_LCTL, KC_S)
#define HM_T MT(MOD_LSFT, KC_T)
#define HM_N MT(MOD_RSFT, KC_N)
#define HM_E MT(MOD_RCTL, KC_E)
#define HM_I MT(MOD_LALT, KC_I)  // Note: LALT for right side in QMK
#define HM_O MT(MOD_LGUI, KC_O)  // Note: LGUI for right side in QMK

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // BASE: Colemak-DH
    [BASE] = LAYOUT_iso_83(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,    KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_LBRC, KC_RBRC, KC_ENT,  KC_PGUP,
        KC_CAPS, HM_A,    HM_R,    HM_S,    HM_T,    KC_G,    KC_M,    HM_N,    HM_E,    HM_I,    HM_O,    KC_QUOT, KC_BSLS,          KC_PGDN,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,    KC_K,    KC_H,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(UTIL),THUMB_OUTER,KC_LEFT,KC_DOWN,KC_RGHT
    ),

    // SYMS: Numbers, F-keys, brackets
    [SYMS] = LAYOUT_iso_83(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______,
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_LBRC, KC_RBRC, _______, _______,
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_QUOT, _______,          _______,
        _______, _______, KC_BSLS, KC_PIPE, KC_UNDS, KC_MINS, KC_PLUS, KC_EQL,  KC_TILD, KC_GRV,  KC_COLN, KC_SCLN, _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    // NAV: Arrows, editing, clipboard
    [NAV] = LAYOUT_iso_83(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, _______, _______, _______, _______,
        _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    // UTIL: Bluetooth, RGB, system controls
    [UTIL] = LAYOUT_iso_83(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    // CODE: Programming shortcuts (macOS focused)
    [CODE] = LAYOUT_iso_83(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    // ADJUST: Settings, firmware reset
    [ADJUST] = LAYOUT_iso_83(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    )
};

// Encoder handling
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { // First encoder
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return true;
}

// Combo definitions (SYMS + NAV = UTIL)
const uint16_t PROGMEM util_combo[] = {LT(SYMS, KC_TAB), LT(NAV, KC_BSPC), COMBO_END};

combo_t key_combos[] = {
    COMBO(util_combo, MO(UTIL))
};
```

### Keymap Content Plan: QWERTY

**File**: `config/qmk/keychron_q1v2/iso_encoder/keymaps/keychron_q1v2_iso_qwerty/keymap.c`

The QWERTY keymap is identical in structure to Colemak-DH, with only the alphas changed:

```c
// Only the BASE layer differs - QWERTY alphas
[BASE] = LAYOUT_iso_83(
    KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_MUTE,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_ENT,  KC_PGUP,
    KC_CAPS, HM_A,    HM_S,    HM_D,    HM_F,    KC_G,    KC_H,    HM_J,    HM_K,    HM_L,    HM_SCLN, KC_QUOT, KC_BSLS,          KC_PGDN,
    KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(UTIL),THUMB_OUTER,KC_LEFT,KC_DOWN,KC_RGHT
),
```

All other layers (SYMS, NAV, UTIL, CODE, ADJUST) remain identical to the Colemak-DH version.

### rules.mk Content

Both keymaps share the same `rules.mk`:

**File**: `config/qmk/keychron_q1v2/iso_encoder/keymaps/*/rules.mk`

```makefile
# Build options
COMBO_ENABLE = yes          # Enable combo features
ENCODER_ENABLE = yes        # Enable rotary encoder
ENCODER_MAP_ENABLE = no     # Use custom encoder_update_user instead

# Optional features (uncomment if needed)
# TAP_DANCE_ENABLE = yes
# CAPS_WORD_ENABLE = yes
# AUTO_SHIFT_ENABLE = yes

# Disable unused features to save space
MOUSEKEY_ENABLE = no
EXTRAKEY_ENABLE = yes       # Audio/system control keys
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
NKRO_ENABLE = yes
BACKLIGHT_ENABLE = no
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = yes     # Keychron Q1 has per-key RGB
```

## QMK Userspace Configuration

### Initial Setup

Users configure the overlay directory once:

```bash
# Clone the main keyboard firmware repo
qmk setup

# Point QMK to this repo's userspace
qmk config user.overlay_dir=/path/to/this/repo/config/qmk
```

### Build Commands

After configuration, build commands are:

```bash
# Build Colemak-DH keymap
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_colemakdh

# Build QWERTY keymap
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_qwerty
```

### Firmware Output

QMK outputs firmware to:
```
~/.qmk_firmware/.build/keychron_q1v2_iso_encoder_<keymap>.bin
```

## mise.toml Task Definitions

Add the following tasks to `mise.toml`:

```toml
[tasks."build:qmk:q1:colemakdh"]
description = "Build Keychron Q1 V2 ISO Colemak-DH firmware"
run = '''
#!/usr/bin/env bash
set -euo pipefail

echo "Building Keychron Q1 V2 ISO Colemak-DH..."

# Verify QMK CLI is available
if ! command -v qmk &> /dev/null; then
    echo "Error: QMK CLI not found. Install with: pip install qmk"
    exit 1
fi

# Set overlay directory to this repo's QMK config
qmk config user.overlay_dir="${PWD}/config/qmk"

# Build firmware
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_colemakdh

echo "Build complete. Firmware location:"
echo "  ~/.qmk_firmware/.build/keychron_q1v2_iso_encoder_keychron_q1v2_iso_colemakdh.bin"
'''

[tasks."build:qmk:q1:qwerty"]
description = "Build Keychron Q1 V2 ISO QWERTY firmware"
run = '''
#!/usr/bin/env bash
set -euo pipefail

echo "Building Keychron Q1 V2 ISO QWERTY..."

# Verify QMK CLI is available
if ! command -v qmk &> /dev/null; then
    echo "Error: QMK CLI not found. Install with: pip install qmk"
    exit 1
fi

# Set overlay directory to this repo's QMK config
qmk config user.overlay_dir="${PWD}/config/qmk"

# Build firmware
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_qwerty

echo "Build complete. Firmware location:"
echo "  ~/.qmk_firmware/.build/keychron_q1v2_iso_encoder_keychron_q1v2_iso_qwerty.bin"
'''

[tasks."build:qmk:q1:all"]
description = "Build all Keychron Q1 V2 ISO firmware variants"
depends = ["build:qmk:q1:colemakdh", "build:qmk:q1:qwerty"]
```

## Flashing Instructions

QMK provides multiple flashing methods for Keychron boards:

### Method 1: QMK CLI (Recommended)

```bash
# Enter bootloader first: hold ESC while plugging in USB cable, or use reset combo
qmk flash -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_colemakdh
```

### Method 2: Manual DFU

1. Enter bootloader: Hold ESC key while connecting USB cable
2. Flash with QMK Toolbox or:
```bash
qmk flash -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_colemakdh --bootloader dfu
```

### Method 3: Via Keychron Launcher

1. Build firmware with `qmk compile`
2. Open Keychron Launcher (web-based)
3. Enter bootloader mode
4. Upload the `.bin` file

## Keymap-Drawer Integration

To support visual keymap diagrams:

1. Create `config/qmk/keychron_q1v2_iso.json` with physical layout description
2. Add QMK-to-keymap-drawer conversion in `scripts/keymap-convert.py`
3. Generate diagrams via: `mise run keymap:draw`

## Dependencies

- QMK CLI: `pip install qmk`
- QMK Firmware repo (auto-cloned by `qmk setup`)
- Keychron Q1 V2 ISO keyboard definition (in qmk_firmware repo)

## Migration Path

The design maintains backward compatibility:

1. **Phase 1**: Design document (this document) - READY FOR REVIEW
2. **Phase 2**: Implement directory structure and keymap files
3. **Phase 3**: Add mise.toml tasks and CI integration
4. **Phase 4**: Test builds on actual hardware
5. **Phase 5**: Document flashing procedures

## Acceptance Criteria Verification

| Criterion | Status | Notes |
|-----------|--------|-------|
| Design doc has QMK userspace directory structure under config/qmk/ | ✅ | Proposed: `config/qmk/keychron_q1v2/iso_encoder/keymaps/` |
| keychron_q1v2_iso_colemakdh/keymap.c content plan documented | ✅ | Layer structure, key codes, home row mods defined |
| keychron_q1v2_iso_qwerty/keymap.c content plan documented | ✅ | Same structure, QWERTY alphas only |
| rules.mk content specified for each keymap | ✅ | Build options, features enabled/disabled |
| qmk userspace configuration documented | ✅ | `user.overlay_dir` setup, build commands |
| mise.toml task definitions using qmk compile specified | ✅ | `build:qmk:q1:colemakdh`, `build:qmk:q1:qwerty`, `build:qmk:q1:all` |
| Design reviewed and approved | ⏳ | Awaiting review |

## Open Questions

1. **Encoder mapping**: Should encoder functions be layer-dependent? Currently fixed to volume control.
2. **RGB integration**: Keychron Q1 has per-key RGB. Should we add RGB layer indicators?
3. **Bootmagic**: Enable Bootmagic Lite for easy bootloader entry without key combo?
4. **VIA support**: Should keymaps be VIA-compatible for runtime configuration?

## References

- [QMK Userspace Documentation](https://docs.qmk.fm/newbs)
- [QMK Keymap Documentation](https://docs.qmk.fm/keymap)
- [Keychron Q1 V2 QMK Source](https://github.com/qmk/qmk_firmware/tree/master/keyboards/keychron/q1/q1v2)
- [Miryoku QMK](https://github.com/manna-harbour/miryoku_qmk)
- Project: `ARCHITECTURE.md` for multi-firmware design patterns
