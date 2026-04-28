# Adding a New Keyboard

This monorepo supports multiple keyboard variants. Follow these steps to add a new keyboard.

## Directory Structure

```
config/keyboards/
├── <keyboard_name>/
│   ├── <keyboard_name>.conf        # ZMK configuration (Kconfig settings)
│   ├── <keyboard_name>.json        # Keymap drawer metadata
│   └── keymap/
│       ├── <keyboard_name>.keymap  # Main keymap file
│       ├── miryoku/
│       │   ├── custom_config.h     # Miryoku layout options
│       │   └── <keyboard_name>.h   # Miryoku layout mapping
│       └── <keyboard_name>_miryoku.keymap      # Optional: Miryoku variant
│       └── <keyboard_name>_miryoku_colemak.keymap # Optional: Miryoku Colemak variant

boards/
├── arm/
│   └── <keyboard_name>/            # Board definition files
│       ├── <keyboard_name>.dts
│       ├── <keyboard_name>_left.dts
│       ├── <keyboard_name>_right.dts
│       ├── <keyboard_name>_defconfig
│       ├── Kconfig.board
│       └── ...
└── shields/
    └── <optional_shield>/          # Shield definitions (if needed)

keymap-drawer/
└── <keyboard_name>.svg               # Generated keymap visualization
```

## 3-Step Process to Add a New Keyboard

### Step 1: Create Board Hardware Definition

In `boards/arm/<keyboard_name>/`, create the ZMK board definition files:

- `<keyboard_name>.dts` - Main device tree (or split it into `_left.dts` and `_right.dts` for split keyboards)
- `<keyboard_name>_defconfig` - Default Kconfig settings
- `Kconfig.board` - Board Kconfig
- `board.cmake` - Build configuration
- `<keyboard_name>.yaml` - Board metadata for ZMK

See `boards/arm/eyelash_sofle/` as a reference for split keyboard definitions.

### Step 2: Create Keymap Configuration

Create the config directory structure:

```bash
mkdir -p config/keyboards/<keyboard_name>/keymap/miryoku
```

Copy and adapt these files from an existing keyboard:

1. **`<keyboard_name>.conf`** - ZMK config options (sleep timeout, RGB, pointing device, etc.)
2. **`<keyboard_name>.json`** - Keymap drawer physical layout description
3. **`<keyboard_name>.keymap`** - Main keymap with includes
4. **`miryoku/custom_config.h`** - Miryoku options (alpha layout selection)
5. **`miryoku/<keyboard_name>.h`** - Miryoku layout mapping for your physical keys

### Step 3: Build and Verify

Build the firmware:

```bash
# Build for your new keyboard
KEYBOARD=<keyboard_name> mise run build

# Generate keymap visualization
KEYBOARD=<keyboard_name> mise run draw
```

## Naming Conventions

- **Keyboard names**: Use lowercase with underscores (e.g., `eyelash_sofle`, `sofle_choc`, `corne_v4`)
- **Board files**: Match the keyboard name exactly in filenames
- **Keymap files**: Use `<keyboard_name>.keymap` as the default keymap

## Example: Adding `sofle_choc`

```bash
# 1. Create board definition
mkdir -p boards/arm/sofle_choc
cp boards/arm/eyelash_sofle/* boards/arm/sofle_choc/
# Edit files to match sofle_choc hardware specifics

# 2. Create config structure
mkdir -p config/keyboards/sofle_choc/keymap/miryoku
cp config/keyboards/eyelash_sofle/sofle_choc.conf config/keyboards/sofle_choc/
cp config/keyboards/eyelash_sofle/sofle_choc.json config/keyboards/sofle_choc/
cp config/keyboards/eyelash_sofle/keymap/sofle_choc.keymap config/keyboards/sofle_choc/keymap/
cp -r config/keyboards/eyelash_sofle/keymap/miryoku/* config/keyboards/sofle_choc/keymap/miryoku/

# 3. Customize for sofle_choc specifics
# - Adjust layout mapping in miryoku/sofle_choc.h for choc spacing
# - Update JSON for choc key sizes in sofle_choc.json

# 4. Build
KEYBOARD=sofle_choc mise run build
```

## Available Mise Tasks

```bash
# List available keyboards
mise run list-keyboards

# Build specific keyboard
KEYBOARD=eyelash_sofle mise run build

# Draw keymap for specific keyboard
KEYBOARD=eyelash_sofle mise run draw

# Setup west workspace
mise run setup

# Clean build artifacts
mise run clean
```

## Keymap Include Path Reference

When including miryoku_zmk from within keymap files, use the correct relative path:

```c
// From: config/keyboards/<keyboard>/keymap/<keyboard>.keymap
// To:   miryoku_zmk/miryoku/miryoku.dtsi
#include "../../../miryoku_zmk/miryoku/miryoku.dtsi"
```
