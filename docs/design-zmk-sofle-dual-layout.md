# ZMK Sofle Dual-Layout Migration Design

> **Issue**: ke-nsv  
> **Parent**: ke-327 (ZMK Sofle Migration)  
> **Status**: Design Document

## Overview

This design document specifies the migration of the existing ZMK Sofle configuration to a dual-layout monorepo structure supporting **Colemak-DH** (primary) and **QWERTY** (fallback) layouts without using sed commands or file mutation.

## Current State Analysis

### Existing Files

```
config/keyboards/eyelash_sofle/
├── eyelash_sofle.conf
├── eyelash_sofle.json
└── keymap/
    ├── eyelash_sofle.keymap              # Current main keymap (Colemak-DH)
    ├── eyelash_sofle_miryoku.keymap      # Minimal Miryoku wrapper
    ├── eyelash_sofle_miryoku_colemak.keymap  # Colemak variant
    └── miryoku/
        ├── custom_config.h               # Defines MIRYOKU_ALPHAS_COLEMAKDH
        └── eyelash_sofle.h               # Layout mapping for Sofle
```

### Current Build Approach (Legacy)

The current `mise.toml` uses the `-DKEYMAP` flag to specify which keymap file to build:

```bash
uv run west build -s zmk/app -b "${KEYBOARD}_left" -- -DZMK_CONFIG="${PWD}/config" -DKEYMAP="${KEYMAP}"
```

This approach requires separate keymap files for each layout variant.

## Proposed Architecture

### Directory Structure

```
firmware/zmk/
├── west.yml
├── build.yaml                    # Updated with dual-layout targets
├── boards/
│   └── arm/
│       └── eyelash_sofle/        # Board definitions (moved from root)
└── config/
    └── keyboards/
        └── eyelash_sofle/
            ├── eyelash_sofle.conf
            ├── eyelash_sofle.json
            └── keymap/
                ├── eyelash_sofle_colemakdh.keymap   # Colemak-DH layout
                ├── eyelash_sofle_qwerty.keymap      # QWERTY layout
                └── miryoku/
                    ├── custom_config.h              # Shared config
                    └── eyelash_sofle.h              # Layout mapping
```

### Keymap File Specifications

#### 1. `eyelash_sofle_colemakdh.keymap`

**Location**: `firmware/zmk/config/keyboards/eyelash_sofle/keymap/eyelash_sofle_colemakdh.keymap`

**Content**:
```dts
// Miryoku Colemak-DH layout for eyelash_sofle
// This is the primary/default layout

#include "miryoku/custom_config.h"
#include "miryoku/eyelash_sofle.h"
#include "../../../miryoku_zmk/miryoku/miryoku.dtsi"

// Pointing device configuration
#define ZMK_POINTING_DEFAULT_MOVE_VAL 1200
#define ZMK_POINTING_DEFAULT_SCRL_VAL 25

#include <input/processors.dtsi>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <behaviors.dtsi>
#include <dt-bindings/zmk/bt.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/outputs.h>
#include <dt-bindings/zmk/pointing.h>
#include <dt-bindings/zmk/rgb.h>

// Input listeners for pointing device
&mmv_input_listener { input-processors = <&zip_xy_scaler 2 1>; };
&msc_input_listener { input-processors = <&zip_scroll_scaler 2 1>; };

// Scroll behavior configuration
&msc {
    acceleration-exponent = <1>;
    time-to-max-speed-ms = <100>;
    delay-ms = <0>;
};

// Move behavior configuration
&mmv {
    time-to-max-speed-ms = <500>;
    acceleration-exponent = <1>;
    trigger-period-ms = <16>;
};

// Soft off configuration
&soft_off { hold-time-ms = <2000>; };
```

**Notes**:
- No explicit `#define MIRYOKU_ALPHAS_COLEMAKDH` needed (this is the Miryoku default)
- Includes all pointing device and behavior configurations from current keymap
- References the shared `custom_config.h` for any local overrides

#### 2. `eyelash_sofle_qwerty.keymap`

**Location**: `firmware/zmk/config/keyboards/eyelash_sofle/keymap/eyelash_sofle_qwerty.keymap`

**Content**:
```dts
// Miryoku QWERTY layout for eyelash_sofle
// Fallback layout for QWERTY users

// Define QWERTY alphas BEFORE including Miryoku headers
#define MIRYOKU_ALPHAS_QWERTY
#define MIRYOKU_TAP_QWERTY

#include "miryoku/custom_config.h"
#include "miryoku/eyelash_sofle.h"
#include "../../../miryoku_zmk/miryoku/miryoku.dtsi"

// Pointing device configuration
#define ZMK_POINTING_DEFAULT_MOVE_VAL 1200
#define ZMK_POINTING_DEFAULT_SCRL_VAL 25

#include <input/processors.dtsi>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <behaviors.dtsi>
#include <dt-bindings/zmk/bt.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/outputs.h>
#include <dt-bindings/zmk/pointing.h>
#include <dt-bindings/zmk/rgb.h>

// Input listeners for pointing device
&mmv_input_listener { input-processors = <&zip_xy_scaler 2 1>; };
&msc_input_listener { input-processors = <&zip_scroll_scaler 2 1>; };

// Scroll behavior configuration
&msc {
    acceleration-exponent = <1>;
    time-to-max-speed-ms = <100>;
    delay-ms = <0>;
};

// Move behavior configuration
&mmv {
    time-to-max-speed-ms = <500>;
    acceleration-exponent = <1>;
    trigger-period-ms = <16>;
};

// Soft off configuration
&soft_off { hold-time-ms = <2000>; };
```

**Notes**:
- `#define MIRYOKU_ALPHAS_QWERTY` and `#define MIRYOKU_TAP_QWERTY` must be defined BEFORE any Miryoku headers
- Same pointing device and behavior configurations as Colemak-DH version
- Identical structure, only the alpha layout differs

### 3. Shared `custom_config.h`

**Location**: `firmware/zmk/config/keyboards/eyelash_sofle/keymap/miryoku/custom_config.h`

**Content**:
```dts
// Shared Miryoku configuration for eyelash_sofle
// This file is included by both Colemak-DH and QWERTY keymaps
//
// Layout-specific alphas are defined in the keymap files themselves:
//   - Colemak-DH: Default (no define needed)
//   - QWERTY: #define MIRYOKU_ALPHAS_QWERTY

// Place any keyboard-specific Miryoku overrides here
// For now, using Miryoku defaults
```

**Notes**:
- Does NOT define `MIRYOKU_ALPHAS_*` - this is done per-keymap
- Contains only shared keyboard-specific overrides
- Kept minimal to avoid confusion between layout and shared config

### 4. Layout Mapping Header

**Location**: `firmware/zmk/config/keyboards/eyelash_sofle/keymap/miryoku/eyelash_sofle.h`

**Content** (unchanged from current):
```dts
// Map Miryoku's logical 3x5+3 layout onto the custom 64-key eyelash_sofle.
// The extra outer, center-column, and spare thumb positions are left unused so
// the board can trial Miryoku without rewriting the physical board definition.

#if !defined(MIRYOKU_LAYOUTMAPPING_EYELASH_SOFLE)

#define XXX &none

#define MIRYOKU_LAYOUTMAPPING_EYELASH_SOFLE( \
    K00, K01, K02, K03, K04,                K05, K06, K07, K08, K09, \
    K10, K11, K12, K13, K14,                K15, K16, K17, K18, K19, \
    K20, K21, K22, K23, K24,                K25, K26, K27, K28, K29, \
    N30, N31, K32, K33, K34,                K35, K36, K37, N38, N39 \
) \
XXX  XXX  XXX  XXX  XXX  XXX  XXX           XXX  XXX  XXX  XXX  XXX  XXX \
XXX  K00  K01  K02  K03  K04  XXX           K05  K06  K07  K08  K09  XXX \
XXX  K10  K11  K12  K13  K14  XXX           K15  K16  K17  K18  K19  XXX \
XXX  K20  K21  K22  K23  K24  XXX           K25  K26  K27  K28  K29  XXX \
XXX  XXX  K32  K33  K34  XXX                XXX  K35  K36  K37  XXX  XXX

#define MIRYOKU_KLUDGE_THUMBCOMBOS_LEFT 55 56
#define MIRYOKU_KLUDGE_THUMBCOMBOS_RIGHT 59 60

#endif

#define MIRYOKU_MAPPING MIRYOKU_LAYOUTMAPPING_EYELASH_SOFLE
```

## Build Configuration

### Updated `build.yaml`

**Location**: `firmware/zmk/build.yaml`

```yaml
include:
  # Colemak-DH Layout (Primary)
  - board: eyelash_sofle_left
    board_root: .
    snippet: studio-rpc-usb-uart
    artifact-name: adams_sofle_colemakdh_studio_left
    keymap: keyboards/eyelash_sofle/keymap/eyelash_sofle_colemakdh
  - board: eyelash_sofle_right
    board_root: .
    artifact-name: adams_sofle_colemakdh_right
    keymap: keyboards/eyelash_sofle/keymap/eyelash_sofle_colemakdh

  # QWERTY Layout (Fallback)
  - board: eyelash_sofle_left
    board_root: .
    snippet: studio-rpc-usb-uart
    artifact-name: adams_sofle_qwerty_studio_left
    keymap: keyboards/eyelash_sofle/keymap/eyelash_sofle_qwerty
  - board: eyelash_sofle_right
    board_root: .
    artifact-name: adams_sofle_qwerty_right
    keymap: keyboards/eyelash_sofle/keymap/eyelash_sofle_qwerty

  # Settings Reset
  - board: settings_reset
    shield: settings_reset
    artifact-name: settings_reset-nice_nano_v2
    keymap: {}
```

### New Mise Tasks

**Add to `mise.toml`**:

```toml
# ============================================================================
# DUAL-LAYOUT SOFLE TASKS
# ============================================================================

[tasks."build:zmk:sofle:colemakdh"]
description = "Build Sofle ZMK firmware with Colemak-DH layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

cd firmware/zmk

if [ ! -d .west ]; then
  echo "Initializing ZMK workspace..."
  uv run west init -l .
  uv run west update
  uv run west zephyr-export
fi

# Check for arm-none-eabi-gcc (preferred) or Zephyr SDK
if command -v arm-none-eabi-gcc &> /dev/null; then
  echo "Using GNU Arm Embedded toolchain"
  export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
elif [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ]; then
  echo "Using Zephyr SDK"
else
  echo "Warning: No ARM toolchain found. Install arm-none-eabi-gcc or Zephyr SDK."
  echo "  macOS: brew install arm-none-eabi-gcc"
  exit 1
fi

echo "Building Sofle ZMK with Colemak-DH layout..."

# Build left half with ZMK Studio
uv run west build -s zmk/app -d build/colemakdh/left -b eyelash_sofle_left -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP="eyelash_sofle_colemakdh" \
  -DCONFIG_ZMK_STUDIO=y

# Build right half
uv run west build -s zmk/app -d build/colemakdh/right -b eyelash_sofle_right -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP="eyelash_sofle_colemakdh"

echo "Colemak-DH build complete:"
echo "  Left:  firmware/zmk/build/colemakdh/left/zephyr/zmk.uf2"
echo "  Right: firmware/zmk/build/colemakdh/right/zephyr/zmk.uf2"
'''

[tasks."build:zmk:sofle:qwerty"]
description = "Build Sofle ZMK firmware with QWERTY layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

cd firmware/zmk

if [ ! -d .west ]; then
  echo "Initializing ZMK workspace..."
  uv run west init -l .
  uv run west update
  uv run west zephyr-export
fi

# Check for arm-none-eabi-gcc (preferred) or Zephyr SDK
if command -v arm-none-eabi-gcc &> /dev/null; then
  echo "Using GNU Arm Embedded toolchain"
  export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
elif [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ]; then
  echo "Using Zephyr SDK"
else
  echo "Warning: No ARM toolchain found. Install arm-none-eabi-gcc or Zephyr SDK."
  echo "  macOS: brew install arm-none-eabi-gcc"
  exit 1
fi

echo "Building Sofle ZMK with QWERTY layout..."

# Build left half with ZMK Studio
uv run west build -s zmk/app -d build/qwerty/left -b eyelash_sofle_left -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP="eyelash_sofle_qwerty" \
  -DCONFIG_ZMK_STUDIO=y

# Build right half
uv run west build -s zmk/app -d build/qwerty/right -b eyelash_sofle_right -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP="eyelash_sofle_qwerty"

echo "QWERTY build complete:"
echo "  Left:  firmware/zmk/build/qwerty/left/zephyr/zmk.uf2"
echo "  Right: firmware/zmk/build/qwerty/right/zephyr/zmk.uf2"
'''

[tasks."build:zmk:sofle:all"]
description = "Build both Colemak-DH and QWERTY layouts"
depends = ["build:zmk:sofle:colemakdh", "build:zmk:sofle:qwerty"]
```

## Build Directory Structure

After building both layouts:

```
firmware/zmk/build/
├── colemakdh/
│   ├── left/
│   │   └── zephyr/
│   │       └── zmk.uf2
│   └── right/
│       └── zephyr/
│           └── zmk.uf2
└── qwerty/
    ├── left/
    │   └── zephyr/
    │       └── zmk.uf2
    └── right/
        └── zephyr/
            └── zmk.uf2
```

## GitHub Actions Matrix

The CI workflow should build both layouts as separate artifacts:

```yaml
# .github/workflows/build.yml (relevant section)
strategy:
  matrix:
    include:
      # Colemak-DH builds
      - board: eyelash_sofle_left
        artifact-name: adams_sofle_colemakdh_studio_left
        keymap: eyelash_sofle_colemakdh
      - board: eyelash_sofle_right
        artifact-name: adams_sofle_colemakdh_right
        keymap: eyelash_sofle_colemakdh
      # QWERTY builds
      - board: eyelash_sofle_left
        artifact-name: adams_sofle_qwerty_studio_left
        keymap: eyelash_sofle_qwerty
      - board: eyelash_sofle_right
        artifact-name: adams_sofle_qwerty_right
        keymap: eyelash_sofle_qwerty
```

## Verification Checklist

- [ ] `eyelash_sofle_colemakdh.keymap` exists with correct content (no explicit alpha define)
- [ ] `eyelash_sofle_qwerty.keymap` exists with `#define MIRYOKU_ALPHAS_QWERTY` before includes
- [ ] `custom_config.h` does NOT contain `MIRYOKU_ALPHAS_*` definitions
- [ ] `eyelash_sofle.h` layout mapping is preserved
- [ ] `mise run build:zmk:sofle:colemakdh` produces artifacts in `build/colemakdh/`
- [ ] `mise run build:zmk:sofle:qwerty` produces artifacts in `build/qwerty/`
- [ ] No `sed` commands in any proposed solution
- [ ] Both layouts compile without errors

## Migration Path

1. **Phase 1**: Create new keymap files in `firmware/zmk/config/`
2. **Phase 2**: Update `build.yaml` with dual-layout targets
3. **Phase 3**: Add mise tasks for layout-specific builds
4. **Phase 4**: Update CI workflow for matrix builds
5. **Phase 5**: Deprecate and remove legacy `config/keyboards/eyelash_sofle/` (optional, post-migration)

## Open Questions

1. Should we keep the legacy `config/` path for backward compatibility during transition?
2. Do we need a `build:zmk:sofle:flash` task that detects which layout to flash?
3. Should we add a default `eyelash_sofle.keymap` symlink to the Colemak-DH version?

## References

- [Miryoku ZMK Documentation](https://github.com/manna-harbour/miryoku_zmk)
- [ZMK Keymap Configuration](https://zmk.dev/docs/config/keymap)
- [West Build System](https://docs.zephyrproject.org/latest/develop/west/build-flash-debug.html)
