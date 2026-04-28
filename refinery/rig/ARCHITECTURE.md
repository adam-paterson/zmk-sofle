# Multi-Firmware Keyboard Monorepo Architecture

> **Status:** Design Document  
> **Scope:** Unified build system supporting ZMK and QMK firmware  
> **Pattern:** Monorepo with firmware-specific workspaces

## Overview

This document defines the architecture for a multi-firmware keyboard monorepo supporting both ZMK (Zephyr-based) and QMK firmware. The design enables shared keymap definitions, consistent build tooling, and automated CI/CD across multiple keyboard targets.

## Goals

1. **Unified Keymap Source**: Single keymap definition drives both ZMK and QMK builds
2. **Firmware Agnostic Layouts**: Common layer structure with firmware-specific implementations
3. **Consistent Tooling**: `mise` tasks for all local operations
4. **Automated CI/CD**: GitHub Actions matrix builds for all firmware/target combinations
5. **Modular Board Support**: Easy addition of new keyboards
6. **Miryoku Integration**: Native support for Miryoku layout system

## Directory Structure

```
keyboards/
├── refinery/
│   └── rig/
│       └── ARCHITECTURE.md          # This document
├── docs/                             # User-facing documentation
│   ├── layouts/                      # Layout visualizations
│   └── setup.md                      # Setup instructions
├── lib/
│   ├── keymaps/                      # Shared keymap definitions
│   │   ├── base/                     # Base layer definitions (firmware-agnostic)
│   │   │   ├── qerty.dtsi            # QWERTY base layer
│   │   │   ├── colemak.dtsi          # Colemak-DH base layer
│   │   │   └── layers.yaml         # Layer metadata
│   │   ├── miryoku/                  # Miryoku variations
│   │   │   ├── qerty.dtsi
│   │   │   └── colemak.dtsi
│   │   └── custom/                   # Custom non-Miryoku layouts
│   │       └── default.dtsi
│   └── behaviors/                    # Shared behavior definitions
│       ├── combos.yaml               # Combo definitions
│       ├── macros.yaml               # Macro definitions
│       └── mod_morph.dtsi            # Mod-morph behaviors
├── firmware/                           # Firmware-specific workspaces
│   ├── zmk/
│   │   ├── west.yml                  # ZMK manifest
│   │   ├── boards/                   # Board definitions (ZMK-specific)
│   │   │   ├── arm/
│   │   │   │   └── eyelash_sofle/    # Moved from root boards/arm/
│   │   │   └── shields/
│   │   │       └── nice_view_custom/ # Moved from root boards/shields/
│   │   ├── config/
│   │   │   ├── eyelash_sofle.keymap  # References lib/keymaps/
│   │   │   ├── eyelash_sofle.conf
│   │   │   └── miryoku/
│   │   │       ├── custom_config.h
│   │   │       └── eyelash_sofle.h
│   │   └── build.yaml                # ZMK build matrix config
│   └── qmk/
│       ├── keyboards/                # QMK keyboard definitions
│       │   └── eyelash_sofle/
│       │       ├── config.h
│       │       ├── rules.mk
│       │       └── keymaps/
│       │           └── default/
│       │               ├── keymap.c   # References lib/keymaps/
│       │               └── config.h
│       └── userspace/
│           └── miryoku/              # QMK Miryoku integration
├── keymap-drawer/                    # Generated diagrams
│   ├── config.yaml                   # Global keymap-drawer config
│   └── output/                       # Generated SVGs
├── scripts/                          # Build and utility scripts
│   ├── build.sh                      # Unified build entrypoint
│   ├── keymap-convert.py             # ZMK ↔ QMK keymap conversion
│   └── flash.sh                      # Flashing utilities
├── mise.toml                         # Task definitions
└── .github/
    └── workflows/
        ├── build.yml                 # Unified build matrix
        ├── draw.yml                  # Keymap diagram generation
        └── release.yml               # Multi-firmware releases
```

## File Path Specifications

### Keyboard Definitions

| Path | Purpose | Firmware |
|------|---------|----------|
| `firmware/zmk/boards/arm/{keyboard}/` | ZMK ARM board definitions | ZMK |
| `firmware/zmk/boards/shields/{shield}/` | ZMK shield definitions | ZMK |
| `firmware/qmk/keyboards/{keyboard}/` | QMK keyboard definitions | QMK |

### Keymap Files

| Path | Purpose | Format |
|------|---------|--------|
| `lib/keymaps/{layout}/{base}.dtsi` | Firmware-agnostic base layers | Custom DTSI-like |
| `firmware/zmk/config/{keyboard}.keymap` | ZMK keymap includes | ZMK keymap |
| `firmware/qmk/keyboards/{keyboard}/keymaps/{name}/keymap.c` | QMK keymap | QMK C |
| `lib/behaviors/*.yaml` | Shared behavior definitions | YAML |

### Build Configuration

| Path | Purpose |
|------|---------|
| `firmware/zmk/build.yaml` | ZMK build matrix (targets, shields) |
| `firmware/qmk/qmk.json` | QMK userspace configuration |
| `mise.toml` | Unified task definitions |
| `.github/workflows/build.yml` | CI build orchestration |

## Decision Records

### DR-001: Separate Keymap Files vs Compile-Time Defines

**Decision:** Use separate keymap files instead of compile-time defines for layout selection.

**Rationale:**
- **Clarity:** Each keymap is a complete, readable file
- **CI Efficiency:** Build matrix can target specific files without recompilation
- **Tooling:** keymap-drawer and other tools work better with concrete files
- **Debugging:** Firmware artifacts are traceable to specific keymap source

**Rejected Alternative:** Single keymap with `#ifdef COLEMAK` style compile-time switching would reduce file count but complicate CI and tooling integration.

### DR-002: Firmware-Native Workspaces vs Unified Abstraction

**Decision:** Maintain firmware-native directory structures under `firmware/{zmk,qmk}/` rather than creating a custom abstraction layer.

**Rationale:**
- **Compatibility:** Native `west` and `qmk` CLI tools work without modification
- **Upstream Sync:** Easier to track upstream firmware changes
- **Community:** Contributors familiar with either firmware can navigate easily
- **CI Integration:** GitHub Actions from ZMK/QMK work out of the box

**Rejected Alternative:** Unified abstraction would require custom build tooling and break native workflows.

### DR-003: Shared Library vs Per-Firmware Duplication

**Decision:** Create `lib/` for shared definitions with firmware-specific adaptation layers.

**Rationale:**
- **Single Source of Truth:** Base layer layouts defined once in `lib/keymaps/`
- **Layer Metadata:** `layers.yaml` drives both firmwares and keymap-drawer
- **Behavior Definitions:** Combos/macros defined once, rendered per-firmware
- **Conversion Scripts:** `scripts/keymap-convert.py` handles ZMK ↔ QMK syntax

**Trade-off:** Requires maintaining conversion scripts but eliminates dual maintenance of layouts.

### DR-004: mise.toml vs Makefile

**Decision:** Use `mise` for task orchestration with shell script backends.

**Rationale:**
- **Cross-platform:** Works on macOS and Linux
- **Version Management:** Manages tool versions (Python, CMake, etc.)
- **Task Dependencies:** Built-in task dependency graph
- **Documentation:** Tasks are self-documenting

### DR-005: Matrix Build vs Sequential Builds

**Decision:** Use GitHub Actions matrix strategy for parallel firmware builds.

**Rationale:**
- **Speed:** ZMK and QMK builds run in parallel
- **Isolation:** Failures in one firmware don't block others
- **Scalability:** Adding new keyboards is a matrix entry, not new workflow
- **Artifacts:** Separate artifacts per target for selective flashing

## mise.toml Task Design

```toml
[settings]
experimental = true

[tools]
python = "3.13"
uv = "latest"
cmake = "latest"
ninja = "latest"
dtc = "latest"          # Device tree compiler for ZMK
qmk_cli = "latest"      # QMK CLI tool

# Setup tasks
[tasks.setup]
description = "Setup all firmware workspaces"
depends = ["setup:zmk", "setup:qmk"]

[tasks.setup:zmk]
description = "Initialize ZMK west workspace"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/zmk
if [ ! -d .west ]; then
  uv run west init -l .
fi
uv run west update
uv run west zephyr-export
'''

[tasks.setup:qmk]
description = "Setup QMK userspace"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/qmk
qmk setup --yes
'''

# Build tasks
[tasks.build]
description = "Build all firmware for all targets"
depends = ["build:zmk", "build:qmk"]

[tasks.build:zmk]
description = "Build ZMK firmware (matrix from firmware/zmk/build.yaml)"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/zmk
# Parse build.yaml and build each target
uv run python ../../scripts/build-zmk.py --config build.yaml
'''

[tasks.build:qmk]
description = "Build QMK firmware (matrix from firmware/qmk/qmk.json)"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/qmk
# Build each keyboard/keymap combination
qmk compile -j$(nproc)
'''

# Single target builds
[tasks.build:sofle:zmk]
description = "Build Sofle ZMK firmware"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/zmk
west build -b eyelash_sofle_left -s zmk/app -- -DZMK_CONFIG=$(pwd)/config
west build -b eyelash_sofle_right -s zmk/app -- -DZMK_CONFIG=$(pwd)/config
'''

[tasks.build:sofle:qmk]
description = "Build Sofle QMK firmware"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/qmk
qmk compile -kb eyelash_sofle -km default
'''

# Keymap tasks
[tasks.keymap:convert]
description = "Convert keymap between ZMK and QMK formats"
run = '''
#!/usr/bin/env bash
set -euo pipefail
# Convert lib/keymaps/ to firmware-specific formats
uv run python scripts/keymap-convert.py --input lib/keymaps --output firmware/
'''

[tasks.draw]
description = "Regenerate all keymap diagrams"
sources = [
  "lib/keymaps/**/*.dtsi",
  "keymap-drawer/config.yaml",
]
outputs = [
  "keymap-drawer/output/*.svg",
]
run = '''
#!/usr/bin/env bash
set -euo pipefail
mkdir -p keymap-drawer/output
for keymap in lib/keymaps/**/*.dtsi; do
  name=$(basename "$keymap" .dtsi)
  uv run keymap -c keymap-drawer/config.yaml parse "$keymap" > "keymap-drawer/output/${name}.yaml"
  uv run keymap -c keymap-drawer/config.yaml draw "keymap-drawer/output/${name}.yaml" -o "keymap-drawer/output/${name}.svg"
done
'''

# Utility tasks
[tasks.clean]
description = "Clean all build artifacts"
run = '''
#!/usr/bin/env bash
set -euo pipefail
rm -rf firmware/zmk/build
rm -rf firmware/zmk/.west
rm -rf firmware/qmk/.build
rm -rf keymap-drawer/output/*.svg
'''

[tasks.lint]
description = "Lint keymap definitions and configs"
run = '''
#!/usr/bin/env bash
set -euo pipefail
# Lint keymap files
uv run python scripts/lint-keymaps.py lib/keymaps/
# Check DTSI syntax
for f in lib/keymaps/**/*.dtsi; do
  dtc -I dts -O dtb -o /dev/null "$f" 2>/dev/null || echo "Warning: $f has DTS syntax issues"
done
'''

[tasks.test]
description = "Run tests on keymap conversions"
run = '''
#!/usr/bin/env bash
set -euo pipefail
uv run pytest scripts/tests/ -v
'''
```

## CI Matrix Workflow

### Unified Build Workflow (`.github/workflows/build.yml`)

```yaml
name: Build All Firmware

on:
  push:
    branches: [main]
    paths-ignore:
      - "keymap-drawer/output/**"
      - "docs/**"
      - "*.md"
  pull_request:
    paths-ignore:
      - "keymap-drawer/output/**"
      - "docs/**"
      - "*.md"
  workflow_dispatch:

jobs:
  # Generate build matrix from config files
  matrix:
    runs-on: ubuntu-latest
    outputs:
      zmk-matrix: ${{ steps.zmk-matrix.outputs.matrix }}
      qmk-matrix: ${{ steps.qmk-matrix.outputs.matrix }}
    steps:
      - uses: actions/checkout@v4
      
      - name: Generate ZMK Build Matrix
        id: zmk-matrix
        run: |
          matrix=$(yq -o json firmware/zmk/build.yaml | jq -c '.include')
          echo "matrix={\"include\":$matrix}" >> $GITHUB_OUTPUT
      
      - name: Generate QMK Build Matrix
        id: qmk-matrix
        run: |
          matrix=$(cat firmware/qmk/qmk.json | jq -c '.keyboards | to_entries | map({keyboard: .key, keymaps: .value.keymaps})')
          echo "matrix={\"include\":$matrix}" >> $GITHUB_OUTPUT

  # Build ZMK firmware
  build-zmk:
    needs: matrix
    runs-on: ubuntu-latest
    strategy:
      matrix: ${{ fromJson(needs.matrix.outputs.zmk-matrix) }}
      fail-fast: false
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup ZMK
        uses: zmkfirmware/zmk/.github/workflows/build-user-config.yml@main
        with:
          board: ${{ matrix.board }}
          shield: ${{ matrix.shield }}
          snippet: ${{ matrix.snippet }}
          cmake-args: ${{ matrix.cmake-args }}

  # Build QMK firmware
  build-qmk:
    needs: matrix
    runs-on: ubuntu-latest
    strategy:
      matrix: ${{ fromJson(needs.matrix.outputs.qmk-matrix) }}
      fail-fast: false
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup QMK
        uses: qmk/qmk_userspace/.github/workflows/build.yml@main
        with:
          keyboard: ${{ matrix.keyboard }}
          keymap: ${{ matrix.keymaps }}

  # Collect all artifacts and create release
  release:
    needs: [build-zmk, build-qmk]
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    permissions:
      contents: write
    steps:
      - uses: actions/checkout@v4
      
      - name: Download ZMK artifacts
        uses: actions/download-artifact@v4
        with:
          pattern: zmk-firmware-*
          merge-multiple: true
          path: firmware-output/zmk
      
      - name: Download QMK artifacts
        uses: actions/download-artifact@v4
        with:
          pattern: qmk-firmware-*
          merge-multiple: true
          path: firmware-output/qmk
      
      - name: Create Release
        uses: softprops/action-gh-release@v2
        with:
          tag_name: firmware-${{ github.run_number }}
          name: Multi-Firmware Build #${{ github.run_number }}
          body_path: .github/release-template.md
          files: |
            firmware-output/zmk/*.uf2
            firmware-output/zmk/*.hex
            firmware-output/qmk/*.bin
            firmware-output/qmk/*.hex
```

### ZMK Build Configuration (`firmware/zmk/build.yaml`)

```yaml
# ZMK build targets
include:
  # Sofle with custom Eyelash board
  - board: nice_nano_v2
    shield: eyelash_sofle_left nice_view_custom
    artifact-name: sofle_zmk_left
    
  - board: nice_nano_v2
    shield: eyelash_sofle_right nice_view_custom
    artifact-name: sofle_zmk_right
    
  # Sofle with ZMK Studio
  - board: nice_nano_v2
    shield: eyelash_sofle_left nice_view_custom
    snippet: studio-rpc-usb-uart
    cmake-args: -DCONFIG_ZMK_STUDIO=y
    artifact-name: sofle_zmk_left_studio
    
  # Settings reset
  - board: nice_nano_v2
    shield: settings_reset
    artifact-name: settings_reset
```

### QMK Userspace Configuration (`firmware/qmk/qmk.json`)

```json
{
  "userspace_version": "1.0",
  "build_targets": {
    "eyelash_sofle": {
      "default": {
        "keyboard": "eyelash_sofle",
        "keymap": "default"
      },
      "miryoku": {
        "keyboard": "eyelash_sofle",
        "keymap": "miryoku"
      }
    }
  },
  "keyboards": {
    "eyelash_sofle": {
      "keymaps": ["default", "miryoku"]
    }
  }
}
```

## Keymap Conversion System

### Shared Layer Definition Format

```dts
// lib/keymaps/base/qwerty.dtsi
// Firmware-agnostic layer definition

#define LAYER_BASE \
    RC(0,0) &kp Q       &kp W       &kp E       &kp R       &kp T       \
    RC(1,0) &kp A       &kp S       &kp D       &kp F       &kp G       \
    RC(2,0) &kp Z       &kp X       &kp C       &kp V       &kp B       \
    RC(3,0) &none       &kp ESC     &kp TAB     &kp SPACE   &kp RET

#define LAYER_SYMS \
    RC(0,0) &kp N1      &kp N2      &kp N3      &kp N4      &kp N5      \
    RC(1,0) &kp F1      &kp F2      &kp F3      &kp F4      &kp F5      \
    RC(2,0) &none       &none       &none       &none       &none       \
    RC(3,0) &none       &trans      &trans      &trans      &trans

// Layer metadata for tooling
// LAYER: name=BASE, display=QWERTY, color=blue
// LAYER: name=SYMS, display=Symbols, color=green, position=left-thumb
```

### Conversion Script Architecture

```python
# scripts/keymap-convert.py
# Converts firmware-agnostic definitions to ZMK/QMK formats

class KeymapConverter:
    def __init__(self, input_dir: str, output_dir: str):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
    
    def parse_shared(self, file: Path) -> LayerDefinition:
        """Parse firmware-agnostic .dtsi file"""
        # Extract LAYER_* defines
        # Extract metadata comments
        # Return structured layer definition
        pass
    
    def to_zmk(self, layers: List[LayerDefinition]) -> str:
        """Generate ZMK keymap format"""
        # Convert &kp KEY to ZMK syntax
        # Generate compatible.dtsi includes
        # Output: firmware/zmk/config/{keyboard}.keymap
        pass
    
    def to_qmk(self, layers: List[LayerDefinition]) -> str:
        """Generate QMK keymap.c format"""
        # Convert &kp KEY to QMK KC_* syntax
        # Generate C array syntax
        # Output: firmware/qmk/keyboards/{keyboard}/keymaps/{name}/keymap.c
        pass
```

## Migration Path from Current Structure

### Current State

```
keyboards/
├── boards/              # ZMK board definitions
├── config/              # ZMK keymap and west.yml
├── miryoku_zmk/         # Miryoku submodule
└── keymap-drawer/       # Generated diagrams
```

### Migration Steps

#### Phase 1: Restructure (No functional changes)

1. **Create new directory layout:**
   ```bash
   mkdir -p firmware/zmk/{boards,config}
   mkdir -p lib/keymaps
   mkdir -p scripts
   ```

2. **Move ZMK files:**
   ```bash
   mv boards/* firmware/zmk/boards/
   mv config/* firmware/zmk/config/
   mv west.yml firmware/zmk/
   ```

3. **Update mise.toml:**
   - Add `firmware/zmk/` prefix to existing paths
   - Add new unified tasks

4. **Update GitHub Actions:**
   - Update paths in workflow files
   - Test builds still work

#### Phase 2: Add Shared Keymap Library

1. **Extract common layers:**
   ```bash
   # Convert existing keymap to shared format
   python scripts/extract-layers.py \
     firmware/zmk/config/eyelash_sofle.keymap \
     lib/keymaps/base/qwerty.dtsi
   ```

2. **Create layer metadata:**
   ```yaml
   # lib/keymaps/layers.yaml
   layers:
     - name: BASE
       display: QWERTY
       description: Standard alphanumeric layout
       thumb: false
     - name: SYMS
       display: Symbols
       description: Numbers, F-keys, brackets
       thumb: left
     - name: NAV
       display: Navigation
       description: Arrows, editing, clipboard
       thumb: right
   ```

#### Phase 3: Add QMK Support

1. **Initialize QMK userspace:**
   ```bash
   mkdir -p firmware/qmk
   qmk setup --yes --branch firmware/qmk
   ```

2. **Create QMK keyboard definition:**
   ```bash
   mkdir -p firmware/qmk/keyboards/eyelash_sofle
   # Copy/adapt from ZMK board definition
   ```

3. **Generate QMK keymap:**
   ```bash
   python scripts/keymap-convert.py \
     --from zmk \
     --to qmk \
     --input firmware/zmk/config/eyelash_sofle.keymap \
     --output firmware/qmk/keyboards/eyelash_sofle/keymaps/default/keymap.c
   ```

#### Phase 4: Unified CI/CD

1. **Update build workflow:**
   - Replace single ZMK workflow with matrix
   - Add QMK build job
   - Unified release job

2. **Test parallel builds:**
   ```bash
   mise run build
   ```

3. **Verify artifacts:**
   - ZMK: `.uf2` files for left/right
   - QMK: `.bin` or `.hex` files

#### Phase 5: Documentation & Cleanup

1. **Update README.md:**
   - Document multi-firmware support
   - Update build instructions
   - Add migration notes

2. **Update CLAUDE.md:**
   - Document new architecture for AI agents

3. **Remove deprecated paths:**
   - After verification, remove old `boards/`, `config/` at root
   - Update `.gitignore`

## Appendix A: Keymap-Drawer Integration

### Configuration

```yaml
# keymap-drawer/config.yaml
parse_config:
  zmk_keycode_map:
    # Custom keycode translations
    "&kp Q": "Q"
    "&kp N1": "1"
    "&mo": "MO"
    "&lt": "LT"
  
  # Layer colors matching physical keymap
  layer_style:
    BASE:
      fill: "#e8f4f8"    # Light blue
    SYMS:
      fill: "#e8f8e8"    # Light green
    NAV:
      fill: "#f8f4e8"    # Light orange
    UTIL:
      fill: "#f8e8e8"    # Light red
```

### CI Integration

```yaml
# .github/workflows/draw.yml
name: Generate Keymap Diagrams

on:
  push:
    paths:
      - "lib/keymaps/**"
      - "keymap-drawer/config.yaml"

jobs:
  draw:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup keymap-drawer
        uses: caksoylar/keymap-drawer/.github/workflows/draw-zmk.yml@v0.23.0
        with:
          keymap_patterns: "firmware/zmk/config/*.keymap"
          config: "keymap-drawer/config.yaml"
          output: "keymap-drawer/output"
          commit_message: "[Draw] Update keymap diagrams"
```

## Appendix B: Miryoku Integration

### ZMK Miryoku

```dts
// lib/keymaps/miryoku/colemak.dtsi
// Miryoku Colemak-DH variant

#include <miryoku/miryoku.dtsi>

&default_bindings {
    // Override specific bindings if needed
    // Default Miryoku behavior is in the submodule
};
```

### QMK Miryoku

```c
// firmware/qmk/keyboards/eyelash_sofle/keymaps/miryoku/keymap.c
#include QMK_KEYBOARD_H
#include "miryoku.h"  // From miryoku_qmk submodule

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_miryoku(
        // Miryoku base layer
        // Generated from lib/keymaps/miryoku/colemak.dtsi
    )
};
```

## Appendix C: Troubleshooting Guide

| Issue | Solution |
|-------|----------|
| `west` not found | Run `mise run setup:zmk` |
| `qmk` CLI errors | Run `mise run setup:qmk` |
| Keymap conversion fails | Check `lib/keymaps/` syntax with `mise run lint` |
| CI build fails | Check matrix config in `firmware/zmk/build.yaml` |
| Firmware too large | Disable features in `firmware/{zmk,qmk}/config/*.conf` |

## References

- [ZMK Multi-Keymap Support Discussion #1931](https://github.com/zmkfirmware/zmk/issues/1931)
- [urob/zmk-config](https://github.com/urob/zmk-config) - Advanced ZMK configuration patterns
- [caksoylar/zmk-config](https://github.com/caksoylar/zmk-config) - Keymap-drawer integration
- [qmk/qmk_userspace](https://github.com/qmk/qmk_userspace) - QMK userspace documentation
- [Miryoku ZMK](https://github.com/manna-harbour/miryoku_zmk) - Miryoku implementation
- [Miryoku QMK](https://github.com/manna-harbour/miryoku_qmk) - QMK Miryoku variant
