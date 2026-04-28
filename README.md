# Adam's Keyboard Firmware Monorepo

[![Build All Firmware](https://github.com/adam-paterson/zmk-sofle/actions/workflows/build.yml/badge.svg)](https://github.com/adam-paterson/zmk-sofle/actions/workflows/build.yml)
[![Draw Keymap](https://github.com/adam-paterson/zmk-sofle/actions/workflows/draw.yml/badge.svg)](https://github.com/adam-paterson/zmk-sofle/actions/workflows/draw.yml)

This repo contains firmware configurations for multiple custom keyboard variants, supporting both **ZMK** (Zephyr-based) and **QMK** (Quantum) firmware. The new multi-firmware architecture uses a shared keymap library in `lib/keymaps/` that drives both firmware types.

📚 **Documentation**: See [`docs/keymap-management.md`](docs/keymap-management.md) for the complete keymap organization strategy, layer conventions, and miryoku integration details.

🚀 **New Keyboards**: Use the [`template/`](template/) directory to quickly add new keyboards following the established conventions. See [docs/ADDING_KEYBOARD.md](docs/ADDING_KEYBOARD.md) for the full guide.

🏗️ **Architecture**: See [ARCHITECTURE.md](./ARCHITECTURE.md) for the technical architecture and [refinery/rig/ARCHITECTURE.md](./refinery/rig/ARCHITECTURE.md) for the full design specification.

## Supported Keyboards

- **eyelash_sofle** - Custom split keyboard based on Eyelash Sofle

To add a new keyboard, see [docs/ADDING_KEYBOARD.md](docs/ADDING_KEYBOARD.md).

## Supported Firmware

| Firmware | Status | Path |
|----------|--------|------|
| **ZMK** | ✅ Active | `firmware/zmk/` |
| **QMK** | 🚧 Placeholder | `firmware/qmk/` *(Phase 3)* |

## Layout (eyelash_sofle)

- `BASE`: everyday QWERTY typing
- `SYMS`: F-keys, numbers, brackets, and symbols on the left inner thumb
- `NAV`: arrows, page navigation, and clipboard shortcuts on the right inner thumb
- `UTIL`: Bluetooth, RGB, reset, and power controls when `SYMS` and `NAV` are both held
- `CODE`: macOS programming shortcuts on the outer right Command thumb (`tap = Cmd`, `hold = CODE`)

The CODE layer is aimed at terminal-heavy macOS use:

- `U` / `P`: `Cmd+Left` / `Cmd+Right`
- `I` / `O`: `Option+Left` / `Option+Right`
- `J` / `;`: `Cmd+Shift+Left` / `Cmd+Shift+Right`
- `K` / `L`: `Option+Shift+Left` / `Option+Shift+Right`
- `'`: `Option+Backspace`
- top-right `Backspace`: `Cmd+Backspace`
- `M` / `,`: plain `Backspace` / `Delete`

The only combo left in the active keymap is `Q + S + Z` held for two seconds to enter deep sleep.

## Local Tooling

This repo uses [`mise`](https://mise.jdx.dev/) for local tooling.

### Quick Setup Validation

Validate your development environment with one command:

```bash
mise run check
```

This checks for all required tools and dependencies, reporting exactly what's missing with specific install commands.

### Prerequisites

1. Install `mise`
2. Run `mise install` to get Python, uv, cmake, ninja, and protobuf
3. Install an ARM toolchain (choose one):
   - **GNU Arm Embedded** (preferred on macOS): `brew install arm-none-eabi-gcc`
   - **Zephyr SDK**: Download from [Zephyr SDK releases](https://github.com/zephyrproject-rtos/sdk-ng/releases)
4. Run `mise run setup` to initialize the west workspace

### Available Tasks

#### Legacy Tasks (Backward Compatible)

| Task | Description |
|------|-------------|
| `mise run check` | Validate development environment setup |
| `mise run setup` | Initialize west workspace and fetch ZMK dependencies |
| `mise run update` | Refresh west dependencies without changing tracked sources |
| `mise run build` | Build firmware for KEYBOARD (default: eyelash_sofle) |
| `mise run build:left` | Build left half firmware **with ZMK Studio enabled** |
| `mise run build:right` | Build right half firmware |
| `mise run build:all` | Build both halves (runs left then right) |
| `mise run flash [left\|right]` | Flash firmware to keyboard (auto-detects bootloader) |
| `mise run draw` | Regenerate keymap diagram for KEYBOARD |
| `mise run clean` | Remove build artifacts, dependencies, and caches |

#### New Unified Firmware Tasks

| Task | Description |
|------|-------------|
| `mise run firmware:setup` | Setup all firmware workspaces (ZMK + QMK) |
| `mise run firmware:setup:zmk` | Setup ZMK workspace in `firmware/zmk/` |
| `mise run firmware:setup:qmk` | Setup QMK workspace *(placeholder)* |
| `mise run firmware:build` | Build all firmware for all targets |
| `mise run firmware:build:zmk` | Build ZMK from `firmware/zmk/build.yaml` |
| `mise run firmware:build:qmk` | Build QMK *(placeholder)* |
| `mise run firmware:build:sofle` | Build Sofle for all firmware |
| `mise run keymap:convert` | Convert shared keymaps to firmware formats |
| `mise run keymap:draw` | Regenerate diagrams from `lib/keymaps/` |
| `mise run firmware:list` | List available firmware and keyboards |
| `mise run firmware:clean` | Clean all firmware build artifacts |

### Building Firmware

```bash
# Legacy approach (still works)
mise run setup
mise run build:left

# New unified approach
mise run firmware:setup:zmk
mise run firmware:build:sofle:zmk
```

### Building Specific Keyboards

```bash
# Build default keyboard (eyelash_sofle)
mise run build

# Build a specific keyboard
KEYBOARD=eyelash_sofle mise run build

# Draw keymap for specific keyboard
KEYBOARD=eyelash_sofle mise run draw
```

### Converting Keymaps

The new architecture uses shared keymap definitions:

```bash
# Convert lib/keymaps/ to firmware-specific formats
mise run keymap:convert

# Regenerate all diagrams
mise run keymap:draw
```

### Flashing Firmware

1. Build the firmware: `mise run build:left` (or `build:right`)
2. Put the keyboard half in bootloader mode: **double-press the reset button quickly**
3. A USB drive named `NICE_NANO` or `RPI-RP2` will appear
4. Flash: `mise run flash left` (or `flash right`)
5. The keyboard will automatically reboot when complete

## Repository Structure

```
.
├── firmware/                      # Firmware workspaces (NEW)
│   ├── zmk/                     # ZMK firmware
│   │   ├── west.yml
│   │   ├── build.yaml
│   │   ├── boards/
│   │   └── config/
│   └── qmk/                     # QMK firmware (placeholder)
│       ├── qmk.json
│       └── keyboards/
│
├── lib/                           # Shared keymap library (NEW)
│   ├── keymaps/                   # Firmware-agnostic keymaps
│   │   ├── base/
│   │   ├── miryoku/
│   │   └── layers.yaml
│   └── behaviors/                 # Shared behaviors
│       ├── combos.yaml
│       └── macros.yaml
│
├── boards/                        # ZMK board definitions (legacy)
├── config/                        # ZMK config (legacy)
├── miryoku_zmk/                   # Miryoku submodule
├── keymap-drawer/                 # Generated keymap diagrams
│   └── output/                    # New diagram output location
├── docs/
│   ├── keymap-management.md       # Keymap organization guide
│   └── ADDING_KEYBOARD.md         # Guide for adding keyboards
├── scripts/                       # Build and utility scripts
│   ├── keymap-convert.py          # Keymap conversion tool
│   └── setup-check.sh             # Environment validation
├── mise.toml                      # Task runner configuration
├── ARCHITECTURE.md                # Architecture documentation
└── README.md                      # This file
```

## CI Workflows

Three GitHub Actions workflows manage the repository:

| Workflow | File | Purpose |
|----------|------|---------|
| Build | `.github/workflows/build.yml` | Build all firmware (ZMK + QMK matrix) and create releases |
| Draw | `.github/workflows/draw.yml` | Auto-generate keymap diagrams from shared definitions |
| Validate | `.github/workflows/validate.yml` | Validate keymap syntax and structure |

The build workflow now uses a matrix strategy to support multiple firmware types in parallel.

## Multi-Firmware Architecture

This repository is migrating to a multi-firmware architecture:

**Current Status (Phase 1):**
- ✅ ZMK fully functional (moved to `firmware/zmk/`)
- ✅ Shared keymap library created (`lib/keymaps/`)
- ✅ QMK placeholder structure (`firmware/qmk/`)
- ✅ Unified mise tasks
- ✅ Matrix CI builds
- ✅ Backward compatibility maintained

**Roadmap:**
- Phase 2: Extract keymaps to shared library
- Phase 3: Full QMK support
- Phase 4: Unified CI/CD
- Phase 5: Remove legacy paths

See [ARCHITECTURE.md](./ARCHITECTURE.md) for details.

## Documentation

- **[FLASHING.md](FLASHING.md)** - Complete flashing guide: initial setup, updates, recovery mode, troubleshooting
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - CI/CD workflows, release process, and maintainer guide
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Technical architecture documentation
- **[refinery/rig/ARCHITECTURE.md](refinery/rig/ARCHITECTURE.md)** - Full design specification

## Credits

- Board design based on Eyelash Sofle by [a741725193](https://github.com/a741725193)
- ZMK firmware powered by [ZMK](https://github.com/zmkfirmware/zmk)
- QMK firmware powered by [QMK](https://github.com/qmk/qmk_firmware)
- Diagram generation powered by [keymap-drawer](https://github.com/caksoylar/keymap-drawer)
- Layout system powered by [Miryoku](https://github.com/manna-harbour/miryoku)
