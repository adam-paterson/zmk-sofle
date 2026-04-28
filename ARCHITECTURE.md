# Architecture: Multi-Firmware Keyboard Monorepo

> **Status:** Multi-Firmware Architecture (ZMK + QMK)  
> **Migration Phase:** Phase 1 Complete - Restructure with backward compatibility  
> **See Also:** [refinery/rig/ARCHITECTURE.md](refinery/rig/ARCHITECTURE.md) for full design specification

## Overview

This monorepo now supports multiple firmware types (ZMK and QMK) with a unified build system. The architecture separates concerns into:

- **Hardware definitions**: Firmware-specific board definitions
- **Shared keymap library**: Firmware-agnostic layer definitions in `lib/keymaps/`
- **Firmware workspaces**: Native `firmware/zmk/` and `firmware/qmk/` directories
- **Visual documentation**: Auto-generated keymap diagrams
- **Build orchestration**: Unified `mise` tasks and CI matrix builds

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      Repository Root                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐               │
│  │  firmware/   │    │    lib/      │    │   scripts/   │               │
│  │  ├── zmk/    │    │  ├── keymaps/ │    │  ├── build.* │               │
│  │  └── qmk/    │    │  └── behaviors│   │  └── *.py    │               │
│  └──────────────┘    └──────────────┘    └──────────────┘               │
│                                                                          │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐               │
│  │    docs/     │    │keymap-drawer/│   │   boards/    │  (legacy)     │
│  │  (user docs) │    │  (diagrams)  │   │  (legacy)    │               │
│  └──────────────┘    └──────────────┘    └──────────────┘               │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
                               │
                     ┌─────────┴─────────┐
                     ▼                   ▼
               Local Build            CI Build
               (mise tasks)      (GitHub Actions)
                     │                   │
           ┌─────────┴─────────┬─────────┘
           ▼                 ▼
     ┌──────────┐      ┌──────────┐
     │   ZMK    │      │   QMK    │  (placeholder)
     │  .uf2    │      │  .bin    │
     └──────────┘      └──────────┘
```

## Directory Structure

### `firmware/` — Firmware Workspaces

Native workspaces for each supported firmware type:

```
firmware/
├── zmk/                           # ZMK (Zephyr-based) firmware
│   ├── west.yml                   # West manifest: ZMK dependency
│   ├── build.yaml                 # ZMK build matrix configuration
│   ├── boards/
│   │   ├── arm/                   # Board definitions (nRF52840, etc.)
│   │   │   └── eyelash_sofle/     # Moved from root boards/arm/
│   │   └── shields/               # Shield definitions
│   │       └── nice_view_custom/  # Moved from root boards/shields/
│   └── config/
│       └── keyboards/
│           └── eyelash_sofle/
│               ├── eyelash_sofle.keymap  # References lib/keymaps/
│               ├── eyelash_sofle.conf
│               ├── eyelash_sofle.json
│               └── keymap/miryoku/  # Miryoku customization
│
└── qmk/                           # QMK (Quantum) firmware
    ├── qmk.json                   # QMK userspace configuration
    └── keyboards/
        └── eyelash_sofle/
            ├── config.h
            ├── rules.mk
            └── keymaps/
                └── default/
                    └── keymap.c   # References lib/keymaps/
```

**Key concepts**:
- **Firmware-native structure**: Each firmware uses its standard directory layout
- **Shared keymap source**: `lib/keymaps/` provides firmware-agnostic definitions
- **Conversion layer**: `scripts/keymap-convert.py` generates firmware-specific formats
- **Backward compatibility**: Legacy paths (root `boards/`, `config/`) still work

### `lib/` — Shared Keymap Library

Firmware-agnostic layer and behavior definitions:

```
lib/
├── keymaps/                       # Shared keymap definitions
│   ├── base/                      # Base layer definitions
│   │   ├── qwerty.dtsi            # QWERTY base layer
│   │   └── colemak.dtsi           # Colemak-DH base layer
│   ├── miryoku/                   # Miryoku variations
│   │   ├── qwerty.dtsi
│   │   └── colemak.dtsi
│   ├── custom/                    # Custom non-Miryoku layouts
│   └── layers.yaml                # Layer metadata for tooling
│
└── behaviors/                     # Shared behavior definitions
    ├── combos.yaml                # Combo definitions
    ├── macros.yaml                # Macro definitions
    └── mod_morph.dtsi             # Mod-morph behaviors
```

**Data flow**:

```
lib/keymaps/base/qwerty.dtsi
       │
       ├──► scripts/keymap-convert.py ──► firmware/zmk/config/*.keymap
       │
       └──► scripts/keymap-convert.py ──► firmware/qmk/keymaps/*/keymap.c
```

### Legacy Paths (Phase 1)

The original paths are maintained for backward compatibility during migration:

```
boards/                      # ZMK board definitions (legacy)
├── arm/eyelash_sofle/
└── shields/nice_view_custom/

config/                      # ZMK config (legacy)
├── west.yml
└── keyboards/eyelash_sofle/

miryoku_zmk/                 # Miryoku submodule (unchanged)
```

These are copied to `firmware/zmk/` during Phase 1. Eventually, the root paths will be removed.

## Build System Architecture

### Local Builds (mise)

The `mise.toml` provides both legacy and unified tasks:

**Legacy tasks** (backward compatible):
```bash
mise run setup          # Initialize west workspace (uses config/)
mise run build:left     # Build left half (uses root paths)
mise run draw           # Generate keymap diagram
```

**Unified firmware tasks** (new architecture):
```bash
mise run firmware:setup:zmk     # Setup ZMK workspace
mise run firmware:setup:qmk     # Setup QMK workspace (placeholder)
mise run firmware:build:zmk     # Build ZMK from matrix config
mise run firmware:build:qmk     # Build QMK (placeholder)
mise run keymap:convert         # Convert shared keymaps to all formats
mise run firmware:list          # List available targets
```

### CI Matrix Builds (GitHub Actions)

The unified workflow supports matrix builds for multiple firmware:

```yaml
jobs:
  matrix:
    # Generate build matrices from:
    # - firmware/zmk/build.yaml
    # - firmware/qmk/qmk.json

  build-zmk:
    strategy:
      matrix: ${{ fromJson(needs.matrix.outputs.zmk-matrix) }}

  build-qmk:
    strategy:
      matrix: ${{ fromJson(needs.matrix.outputs.qmk-matrix) }}

  release:
    # Collects artifacts from all firmware types
    # Creates unified release with ZMK + QMK files
```

## Migration Phases

### Phase 1: Restructure (Current) ✅
- [x] Create `firmware/zmk/` structure
- [x] Copy ZMK files to new location
- [x] Create `lib/keymaps/` with sample definitions
- [x] Create `firmware/qmk/` placeholder structure
- [x] Add unified mise tasks
- [x] Update GitHub workflows for matrix builds
- [x] Maintain backward compatibility

### Phase 2: Shared Keymap Library
- [ ] Extract existing keymap to `lib/keymaps/`
- [ ] Create `layers.yaml` metadata
- [ ] Implement `keymap-convert.py` fully
- [ ] Update ZMK keymap to reference shared definitions

### Phase 3: QMK Support
- [ ] Full QMK keyboard definition
- [ ] QMK keymap generation from shared source
- [ ] QMK CI integration
- [ ] QMK-specific documentation

### Phase 4: Unified CI/CD
- [ ] Matrix builds for all firmware/target combinations
- [ ] Unified release with all artifacts
- [ ] Cross-firmware testing

### Phase 5: Cleanup
- [ ] Remove legacy root paths
- [ ] Update all documentation
- [ ] Archive migration guide

## Keymap Architecture

### Shared Layer Definitions

The `lib/keymaps/` directory uses a custom DTSI-like format:

```dts
// lib/keymaps/base/qwerty.dtsi
#define LAYER_BASE \
    RC(0,0) &kp Q       &kp W       &kp E       &kp R       &kp T       \
    RC(1,0) &kp A       &kp S       &kp D       &kp F       &kp G

// Layer metadata for tooling
// LAYER: name=BASE, display=QWERTY, color=blue
```

### Conversion Targets

The `scripts/keymap-convert.py` tool generates:

**ZMK output** (`firmware/zmk/config/*.keymap`):
```dts
#include <behaviors.dtsi>
#include <dt-bindings/zmk/keys.h>

// BASE Layer
#define LAYER_BASE
    &kp Q &kp W &kp E &kp R &kp T
```

**QMK output** (`firmware/qmk/keymaps/*/keymap.c`):
```c
#include QMK_KEYBOARD_H

enum layers { BASE, SYMS, NAV };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_Q, KC_W, KC_E, KC_R, KC_T
    )
};
```

## Decision Records

See [refinery/rig/ARCHITECTURE.md](refinery/rig/ARCHITECTURE.md) for full decision records:

- **DR-001**: Separate keymap files vs compile-time defines
- **DR-002**: Firmware-native workspaces vs unified abstraction
- **DR-003**: Shared library vs per-firmware duplication
- **DR-004**: mise.toml vs Makefile
- **DR-005**: Matrix build vs sequential builds

## Quick Reference

**Build ZMK firmware**:
```bash
# Legacy (backward compatible)
mise run setup
mise run build:left

# New unified approach
mise run firmware:setup:zmk
mise run firmware:build:sofle:zmk
```

**Convert keymaps**:
```bash
mise run keymap:convert
```

**List available targets**:
```bash
mise run firmware:list
```

## See Also

- [README.md](./README.md) — User-facing documentation
- [refinery/rig/ARCHITECTURE.md](./refinery/rig/ARCHITECTURE.md) — Full design specification
- [docs/keymap-management.md](./docs/keymap-management.md) — Keymap organization
- [ZMK Documentation](https://zmk.dev/docs)
- [QMK Documentation](https://docs.qmk.fm)
