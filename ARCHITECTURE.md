# Architecture: ZMK Sofle Firmware Monorepo

This document describes the architecture of the ZMK-based keyboard firmware repository for the custom Eyelash Sofle split keyboard.

## Overview

This monorepo contains everything needed to build and maintain firmware for a custom Sofle split keyboard variant. The architecture separates concerns into:

- **Hardware definitions**: Board descriptions in Zephyr DTS format
- **Firmware configuration**: Keymaps and build configuration
- **Visual documentation**: Auto-generated keymap diagrams
- **Build orchestration**: Local (mise) and CI (GitHub Actions) workflows

```
┌─────────────────────────────────────────────────────────────────┐
│                        Repository Root                           │
├─────────────────────────────────────────────────────────────────┤
│  boards/      config/      keymap-drawer/      miryoku_zmk/      │
│     │            │              │                   │            │
│  ┌──┴──┐    ┌────┴────┐    ┌───┴───┐         ┌─────┴─────┐      │
│  │arm/ │    │keymaps/ │    │.yaml  │         │ (submodule)│      │
│  │shlds│    │west.yml │    │.svg   │         │            │      │
│  └─────┘    └─────────┘    └───────┘         └────────────┘      │
└─────────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────┴─────────┐
                    ▼                   ▼
              Local Build            CI Build
              (mise tasks)      (GitHub Actions)
                    │                   │
                    └─────────┬─────────┘
                              ▼
                        ZMK Firmware
                              │
                    ┌─────────┴─────────┐
                    ▼                   ▼
              Left Half .uf2      Right Half .uf2
         (with ZMK Studio support)
```

## Directory Structure

### `boards/` — Hardware Definitions

Zephyr board definitions for the custom hardware. ZMK discovers these via `BOARD_ROOT` pointing at the repo root.

```
boards/
├── arm/
│   └── eyelash_sofle/           # Main board definition (nRF52840-based)
│       ├── eyelash_sofle.dtsi   # Common hardware description (encoders, matrix, LEDs)
│       ├── eyelash_sofle_left.dts   # Left half-specific config
│       ├── eyelash_sofle_right.dts  # Right half-specific config
│       ├── eyelash_sofle.yaml   # Board metadata for ZMK
│       ├── eyelash_sofle.zmk.yml    # ZMK-specific board info
│       ├── Kconfig.board        # Board Kconfig symbols
│       └── Kconfig.defconfig    # Default configuration values
│
└── shields/
    └── nice_view_custom/        # Custom nice!view display shield
        ├── nice_view_custom.overlay   # Device tree overlay
        ├── custom_status_screen.c     # Custom display widgets
        ├── widgets/                   # UI component implementations
        └── README.md                  # Shield documentation
```

**Key concepts**:
- **Board**: Complete hardware definition (MCU, matrix, pins)
- **Shield**: Add-on hardware (displays, sensors) that overlays onto a base board
- **DTS/I files**: Device Tree Source files define hardware at compile-time
- **Split keyboard**: Left/right are separate "siblings" in the ZMK board system

### `config/` — Firmware Configuration

Active firmware configuration and the west manifest.

```
config/
├── west.yml                     # West manifest: declares ZMK dependency
│
├── eyelash_sofle.conf           # Kconfig options (features, behaviors)
├── eyelash_sofle.json           # ZMK Studio layout definition
│
├── eyelash_sofle.keymap         # **Active keymap** (includes miryoku)
├── eyelash_sofle_miryoku.keymap # Miryoku variant (QWERTY)
├── eyelash_sofle_miryoku_colemak.keymap  # Miryoku variant (Colemak-DH)
│
└── miryoku/                     # Local Miryoku customization
    ├── custom_config.h          # Layout selection (Colemak-DH vs QWERTY)
    └── eyelash_sofle.h          # Physical layout mapping (64 keys → Miryoku 3x5+3)
```

**Data flow for keymaps**:

```
eyelash_sofle.keymap
       │
       ├──► miryoku/custom_config.h  (layout selection: MIRYOKU_ALPHAS_COLEMAKDH)
       │
       ├──► miryoku/eyelash_sofle.h  (physical key mapping)
       │
       └──► ../miryoku_zmk/miryoku/miryoku.dtsi  (base Miryoku implementation)
                    │
                    └──► Defines BASE, SYMS, NAV, UTIL, MOUSE, etc. layers
```

### `keymap-drawer/` — Visual Documentation

Auto-generated keymap diagrams maintained as committed artifacts.

```
keymap-drawer/
├── eyelash_sofle.yaml    # Parsed keymap representation (auto-generated)
└── eyelash_sofle.svg     # Visual diagram (auto-generated)
```

**Generation flow**:

```
config/eyelash_sofle.keymap
            │
            ├──► keymap-drawer parse ──► eyelash_sofle.yaml
            │
            └──► keymap-drawer draw ──► eyelash_sofle.svg
```

**Configuration**: `keymap_drawer.config.yaml` defines:
- Visual styling (colors, fonts, layer accents)
- Icon mappings (Material Design icons for keys)
- Custom legend handling (symbols, modifiers, combos)

### `miryoku_zmk/` — Submodule Integration

Git submodule providing the [Miryoku](https://github.com/manna-harbour/miryoku_zmk) layout standard.

```
miryoku_zmk/
├── miryoku/
│   ├── miryoku.dtsi        # Core layer definitions (BASE, SYMS, NAV, etc.)
│   ├── mapping/            # Board-specific layout mappings
│   └── custom_config.h     # Default Miryoku options
├── config/                 # Reference configurations
└── docs/                   # Miryoku documentation
```

**Why a submodule?**
- Miryoku is an external standard with its own versioning
- Updates to Miryoku can be pulled independently
- Keeps the local repo focused on board-specific customization

**Integration point**: `config/eyelash_sofle.keymap` includes `../miryoku_zmk/miryoku/miryoku.dtsi`

## Build System Architecture

### Local Builds (mise)

The `mise.toml` file defines tasks for local development:

```
┌─────────────────────────────────────────────────────────────┐
│                     mise task flow                            │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  mise run setup                                              │
│       │                                                       │
│       ├──► uv run west init -l config   (initialize west)    │
│       └──► uv run west update           (fetch ZMK deps)    │
│                                                               │
│  mise run build:left                                         │
│       │                                                       │
│       ├──► west build -b eyelash_sofle_left                   │
│       │       -DZMK_CONFIG=/path/to/config                   │
│       │       -- -DBOARD_ROOT=/repo/root                     │
│       │                                                       │
│       └──► Output: build/left/zephyr/zmk.uf2                │
│                                                               │
│  mise run draw                                               │
│       │                                                       │
│       ├──► keymap parse -z config/eyelash_sofle.keymap      │
│       │       > keymap-drawer/eyelash_sofle.yaml             │
│       └──► keymap draw keymap-drawer/eyelash_sofle.yaml      │
│               -o keymap-drawer/eyelash_sofle.svg             │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

**Key environment variables**:
- `ZMK_CONFIG`: Points to `config/` directory
- `BOARD_ROOT`: Points to repo root (so Zephyr finds `boards/`)

### CI Builds (GitHub Actions)

Two workflows handle CI:

| Workflow | File | Trigger | Purpose |
|----------|------|---------|---------|
| Build | `.github/workflows/build.yml` | Push (except keymap-drawer/, *.md) | Build firmware, create release |
| Draw | `.github/workflows/draw.yml` | Push to config/** | Regenerate keymap diagrams |

**Build workflow architecture**:

```
┌─────────────────────────────────────────────────────────────────┐
│                     CI Build Flow                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐                      │
│  │  build  │───►│ release │───►│  notify │                      │
│  │  job    │    │  job    │    │  job    │                      │
│  └────┬────┘    └────┬────┘    └─────────┘                      │
│       │              │                                          │
│       │              └──► Creates GitHub Release                 │
│       │                   • adams_sofle_studio_left.uf2          │
│       │                   • adams_sofle_right.uf2                │
│       │                   • settings_reset-nice_nano_v2-zmk.uf2    │
│       │                                                          │
│       └──► Uses: zmkfirmware/zmk/.github/workflows/build-user    │
│                 -config.yml@v0.3.0                               │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

**Local vs CI relationship**:

| Aspect | Local (mise) | CI (GitHub Actions) |
|--------|--------------|---------------------|
| Toolchain | GNU Arm Embedded or Zephyr SDK | ZMK's containerized toolchain |
| Config | Same `config/` directory | Same `config/` directory |
| Board root | Same `boards/` directory | Same `boards/` directory |
| Outputs | `build/` directory | GitHub Release artifacts |
| ZMK version | From `west.yml` | From `west.yml` |

Both use the **same west manifest** and **same board definitions**, ensuring identical firmware output.

## Keymap Organization

### Layer Architecture (Miryoku-based)

The keymap uses the Miryoku standard with local customizations:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Layer Stack                                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  BASE (0)     QWERTY or Colemak-DH alpha keys                    │
│       │                                                          │
│       ├──► Hold left inner thumb ──► SYMS (F-keys, numbers)       │
│       ├──► Hold right inner thumb ─► NAV (arrows, editing)      │
│       ├──► Hold SYMS + NAV ────────► UTIL (BT, RGB, reset)      │
│       └──► Hold outer right Cmd ───► CODE (macOS programming)    │
│                                                                  │
│  Miryoku provides: BASE, SYMS, NAV, UTIL, MOUSE, MEDIA, etc.   │
│  Local adds: CODE layer for terminal-heavy macOS workflows       │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Layer Naming Conventions

| Layer | Access | Purpose |
|-------|--------|---------|
| `BASE` | Default | Alpha typing (QWERTY/Colemak) |
| `SYMS` | Hold left inner thumb | F-keys, numbers, brackets |
| `NAV` | Hold right inner thumb | Arrows, page nav, clipboard |
| `UTIL` | Hold SYMS + NAV | Bluetooth, RGB, reset, power |
| `CODE` | Hold outer right Cmd | macOS terminal shortcuts |

### Physical Layout Mapping

The Eyelash Sofle has 64 keys (32 per half). Miryoku uses a 3x5+3 layout (38 keys). The mapping bridges this gap:

```
Miryoku logical (3x5+3):
┌─────┬─────┬─────┬─────┬─────┐   ┌─────┬─────┬─────┬─────┬─────┐
│ K00 │ K01 │ K02 │ K03 │ K04 │   │ K05 │ K06 │ K07 │ K08 │ K09 │
├─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┤
│ K10 │ K11 │ K12 │ K13 │ K14 │   │ K15 │ K16 │ K17 │ K18 │ K19 │
├─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┤
│ K20 │ K21 │ K22 │ K23 │ K24 │   │ K25 │ K26 │ K27 │ K28 │ K29 │
└─────┴─────┴─────┴─────┴─────┘   └─────┴─────┴─────┴─────┴─────┘
            ├─────┬─────┬─────┤   ├─────┬─────┬─────┤
            │ K32 │ K33 │ K34 │   │ K35 │ K36 │ K37 │
            └─────┴─────┴─────┘   └─────┴─────┴─────┘

Eyelash Sofle physical (mapped to center 38 keys):
Extra outer/center keys are masked with `&none` (XXX)
```

See `config/miryoku/eyelash_sofle.h` for the full `#define MIRYOKU_LAYOUTMAPPING_EYELASH_SOFLE` implementation.

## Data Flow Summary

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Complete Build Data Flow                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  SOURCE FILES                    BUILD SYSTEM              OUTPUT        │
│  ─────────────                   ────────────              ───────        │
│                                                                          │
│  boards/arm/eyelash_sofle/                                             │
│       ├── *.dts, *.dtsi  ──────┐                                        │
│       └── Kconfig.*           │                                        │
│                               │         ┌───────────────┐              │
│  config/                      │         │   west/zephyr │              │
│       ├── west.yml  ─────────┼────────►│      +        │              │
│       ├── *.conf              │         │   zmk/app     │              │
│       └── *.keymap ──────────┤         │      +        │              │
│                               │         │   keymap      │              │
│  miryoku_zmk/                 │         │   processing    │              │
│       └── miryoku/*.dtsi ────┘         └───────┬───────┘              │
│                                                 │                      │
│                                                 ▼                      │
│                                         ┌───────────────┐              │
│                                         │   firmware    │              │
│                                         │   .uf2 files  │              │
│                                         └───────┬───────┘              │
│                                                 │                      │
│  keymap-drawer/                                 │                      │
│       └── eyelash_sofle.svg ◄───────────────────┘ (docs)               │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

## Current Keyboard Reference

| Property | Value |
|----------|-------|
| **Board ID** | `eyelash_sofle` |
| **MCU** | nRF52840 (Nordic) |
| **Layout** | 64-key split (32 per half) |
| **Features** | Encoders, RGB underglow, OLED display (nice!view) |
| **Connectivity** | USB-C, Bluetooth 5 |
| **ZMK Version** | v0.3.0 (per `west.yml`) |
| **Layout Standard** | Miryoku (Colemak-DH or QWERTY) |

## Quick Reference

**Build firmware locally**:
```bash
mise run setup          # First time only
mise run build:left     # Build left half
mise run build:right    # Build right half
```

**Regenerate keymap diagram**:
```bash
mise run draw
```

**Flash firmware**:
1. Double-press reset button on keyboard half
2. Copy `.uf2` file to the USB mass storage device that appears
3. Keyboard auto-reboots when complete

## See Also

- [README.md](./README.md) — User-facing documentation
- [ZMK Documentation](https://zmk.dev/docs) — Official ZMK reference
- [Miryoku](https://github.com/manna-harbour/miryoku) — Layout standard documentation
- [keymap-drawer](https://github.com/caksoylar/keymap-drawer) — Diagram generation tool
