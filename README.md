# Adam's Keyboard Firmware Monorepo

[![Build ZMK firmware](https://github.com/adam-paterson/zmk-sofle/actions/workflows/build.yml/badge.svg)](https://github.com/adam-paterson/zmk-sofle/actions/workflows/build.yml)
[![Draw Keymap](https://github.com/adam-paterson/zmk-sofle/actions/workflows/draw.yml/badge.svg)](https://github.com/adam-paterson/zmk-sofle/actions/workflows/draw.yml)

This repo contains ZMK firmware configurations for multiple custom keyboard variants. The local `boards/` tree holds hardware definitions, `config/keyboards/<keyboard>/` contains per-keyboard firmware configurations, and `keymap-drawer/` contains committed diagram output.

📚 **Documentation**: See [`docs/keymap-management.md`](docs/keymap-management.md) for the complete keymap organization strategy, layer conventions, and miryoku integration details.

🚀 **New Keyboards**: Use the [`template/`](template/) directory to quickly add new keyboards following the established conventions. See [docs/ADDING_KEYBOARD.md](docs/ADDING_KEYBOARD.md) for the full guide.

## Supported Keyboards

- **eyelash_sofle** - Custom split keyboard based on Eyelash Sofle

To add a new keyboard, see [docs/ADDING_KEYBOARD.md](docs/ADDING_KEYBOARD.md).

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
| `mise run list-keyboards` | Show available keyboards |
| `mise run clean` | Remove build artifacts, dependencies, and caches |

### Building Specific Keyboards

```bash
# Build default keyboard (eyelash_sofle)
mise run build

# Build a specific keyboard
KEYBOARD=eyelash_sofle mise run build

# Draw keymap for specific keyboard
KEYBOARD=eyelash_sofle mise run draw
```

### Build Standardization

Local builds are configured to match CI builds:

- **ZMK Version**: v0.3.0 (pinned in `west.yml` and CI workflow)
- **Build flags**: Identical between local (`west build`) and CI (`build-user-config.yml`)
- **Board root**: Points to repo root so Zephyr discovers custom board definitions
- **Config**: `config/` directory used as `ZMK_CONFIG`
- **Studio**: Left half always built with ZMK Studio enabled (matches CI artifacts)

### Environment Variables

| Variable | Purpose | Required |
|----------|---------|----------|
| `ZEPHYR_SDK_INSTALL_DIR` | Path to Zephyr SDK installation | Only if not using `arm-none-eabi-gcc` |

### Flashing Firmware

1. Build the firmware: `mise run build:left` (or `build:right`)
2. Put the keyboard half in bootloader mode: **double-press the reset button quickly**
3. A USB drive named `NICE_NANO` or `RPI-RP2` will appear
4. Flash: `mise run flash left` (or `flash right`)
5. The keyboard will automatically reboot when complete

## Firmware Builds

GitHub Actions builds firmware artifacts for each keyboard.

For local builds, `mise` tasks call `west build` against `zmk/app` with `config/` as the active `ZMK_CONFIG` and `BOARD_ROOT` pointed at the repo root so Zephyr can discover custom boards under `boards/`.

The build tasks prefer GNU Arm Embedded automatically when `arm-none-eabi-gcc` is installed. If it is not available, they fall back to the Zephyr SDK environment variables.

## Diagrams

### eyelash_sofle

![Adam's Sofle keymap](keymap-drawer/eyelash_sofle.svg)

The committed diagram is generated from `config/keyboards/eyelash_sofle/keymap/eyelash_sofle.keymap`, not edited by hand. See [`docs/keymap-management.md`](docs/keymap-management.md#keymap-drawer-workflow) for details on the keymap-drawer workflow.

## CI Workflows

Three GitHub Actions workflows manage the repository:

| Workflow | File | Purpose |
|----------|------|---------|
| Build | `.github/workflows/build.yml` | Build ZMK firmware and create releases |
| Draw | `.github/workflows/draw.yml` | Auto-generate keymap diagrams |
| Validate | `.github/workflows/validate.yml` | Validate keymap syntax and structure |

The validate workflow runs on every PR and push to ensure keymap files are syntactically correct before merging.

## Repository Structure

```
.
├── boards/
│   ├── arm/<keyboard>/          # Board hardware definitions
│   └── shields/<shield>/        # Shield definitions
├── config/
│   ├── keyboards/
│   │   └── <keyboard>/          # Per-keyboard configuration
│   │       ├── <keyboard>.conf
│   │       ├── <keyboard>.json
│   │       └── keymap/
│   │           ├── <keyboard>.keymap
│   │           └── miryoku/
│   ├── west.yml                 # West manifest (shared)
├── keymap-drawer/               # Generated keymap diagrams
├── docs/
│   ├── keymap-management.md   # Keymap organization guide
│   └── ADDING_KEYBOARD.md      # Guide for adding keyboards
├── mise.toml                    # Task runner configuration
├── scripts/                     # Utility scripts
│   └── setup-check.sh           # Development environment validation
└── README.md                    # This file
```

## Documentation

- **[FLASHING.md](FLASHING.md)** - Complete flashing guide: initial setup, updates, recovery mode, troubleshooting
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - CI/CD workflows, release process, and maintainer guide

## Credits

- Board design based on Eyelash Sofle by [a741725193](https://github.com/a741725193)
- Firmware powered by [ZMK](https://github.com/zmkfirmware/zmk)
- Diagram generation powered by [keymap-drawer](https://github.com/caksoylar/keymap-drawer)
- Layout system powered by [Miryoku](https://github.com/manna-harbour/miryoku)
