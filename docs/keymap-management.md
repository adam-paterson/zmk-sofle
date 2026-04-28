# Keymap Management Strategy

This document describes the keymap organization system using miryoku overlays and layer conventions.

## Overview

This repository uses a hybrid approach combining [miryoku](https://github.com/manna-harbour/miryoku) principles with custom layers tailored for macOS development workflows. The keymap is designed for split ergonomic keyboards with 34-36 keys.

## Layer Naming Conventions

### Standard Miryoku Layers

The upstream miryoku defines these layers:

| Layer | Name | Purpose |
|-------|------|---------|
| 0 | BASE | Primary typing layer (Colemak-DH, QWERTY, etc.) |
| 1 | EXTRA | Alternative base layer (e.g., QWERTY) |
| 2 | TAP | Tap-only version of base layer |
| 3 | BUTTON | Mouse button layer |
| 4 | NAV | Navigation (arrows, home/end, page up/down) |
| 5 | MOUSE | Mouse movement and scrolling |
| 6 | MEDIA | Media keys and volume control |
| 7 | NUM | Number pad |
| 8 | SYM | Symbols |
| 9 | FUN | Function keys |

### Local Layer Conventions

This repository uses a simplified layer stack optimized for daily use:

| Layer | Name | Activation | Purpose |
|-------|------|------------|---------|
| 0 | BASE | Default | QWERTY typing with home-row mods |
| 1 | SYMS | Hold left inner thumb | F-keys, numbers, brackets, symbols |
| 2 | NAV | Hold right inner thumb | Arrows, navigation, clipboard |
| 3 | UTIL | Hold SYMS + NAV | Bluetooth, RGB, reset, power |
| 4 | CODE | Hold outer right Cmd thumb | macOS programming shortcuts |

## CODE Layer Philosophy

The CODE layer is designed for terminal-heavy macOS programming workflows. It provides one-handed access to common text navigation shortcuts that would normally require complex chorded combinations.

### Key Placement Logic

The layout follows these principles:

1. **Arrow key mirroring**: The right-hand home row mirrors the arrow key pattern
   - `I` / `O` → `Option+Left` / `Option+Right` (word navigation)
   - `U` / `P` → `Cmd+Left` / `Cmd+Right` (line navigation)

2. **Shifted selection**: Adding Shift to the same keys extends selection
   - `K` / `L` → `Option+Shift+Left` / `Option+Shift+Right` (select word)
   - `J` / `;` → `Cmd+Shift+Left` / `Cmd+Shift+Right` (select to line start/end)

3. **Deletion shortcuts**: Common deletion operations on the bottom row
   - `M` → `Backspace`
   - `,` → `Delete`
   - `'` → `Option+Backspace` (delete word)
   - Top-right `Backspace` → `Cmd+Backspace` (delete to line start)

4. **Clipboard integration**: Standard clipboard keys with Command modifier
   - `Z`, `X`, `C`, `V` → `Cmd+Z`, `Cmd+X`, `Cmd+C`, `Cmd+V`

### CODE Layer Reference

```
|-----|-----|-----|-----|-----|   |-----|-----|-----|-----|-----|
|     |     |     |     |     |   |     |     |     |     |     |
|-----|-----|-----|-----|-----|   |-----|-----|-----|-----|-----|
|     |     |     |     |     |   | Cmd←| Opt←|  ↑  | Opt→| Cmd→|
|-----|-----|-----|-----|-----|   |-----|-----|-----|-----|-----|
| Cmd |     |     |     |     |   |Sft←←|Sft← |Sft→ |Sft→→| Opt⌫|
|-----|-----|-----|-----|-----|   |-----|-----|-----|-----|-----|
|     |     |     |     |     |   |     |  ⌫  |  ⌦  |     |     |
|-----|-----|-----|-----|-----|   |-----|-----|-----|-----|-----|
            |     |     |     |   |     |     |     |
```

Legend:
- `Cmd←/→` = Cmd + Arrow (line start/end)
- `Opt←/→` = Option + Arrow (word backward/forward)
- `Sft←←/→→` = Cmd+Shift+Arrow (select to line start/end)
- `Sft←/→` = Option+Shift+Arrow (select word)
- `⌫` = Backspace
- `⌦` = Delete
- `Opt⌫` = Option+Backspace (delete word)

## Miryoku ZMK Integration

### What Comes From Upstream

The `miryoku_zmk/` directory is a git submodule pointing to the upstream miryoku-zmk repository. This provides:

- **Base layer definitions** (`miryoku_babel/miryoku_layer_alternatives.h`)
- **Home-row mod implementations** via `U_MT()` macros
- **Layer-tap behaviors** via `U_LT()` macros
- **Alternative layouts** for Colemak, Dvorak, Workman, etc.
- **Behavior definitions** (mouse keys, tap-dance, etc.)

### Local Overlays

Local customizations in `config/miryoku/` extend or override upstream behavior:

| File | Purpose |
|------|---------|
| `config/miryoku/` | Custom layer definitions that overlay upstream |
| `config/*.keymap` | Active keyboard keymaps using miryoku macros |

### Key Macros

| Macro | Meaning | Usage |
|-------|---------|-------|
| `U_MT(mod, key)` | Mod-tap | Home-row mods (hold for modifier, tap for key) |
| `U_LT(layer, key)` | Layer-tap | Thumb keys (hold for layer, tap for key) |
| `U_NP` | Not present | Unused matrix positions |
| `U_NA` | Not available | Physical keys not in use |
| `U_NU` | Not used | Available but intentionally unused |

### Layer Selection

The active base layer is selected at compile time via build flags:

```bash
# For Colemak-DH (default)
-DMIRYOKU_ALPHAS_COLEMAKDH

# For QWERTY
-DMIRYOKU_ALPHAS_QWERTY

# For flipped (right-hand home row) layouts
-DMIRYOKU_LAYERS_FLIP
```

## Keymap Validation

The CI pipeline validates keymap syntax on every push:

1. **Parse check**: `keymap parse` validates the keymap file syntax
2. **Draw check**: `keymap draw` ensures diagrams can be generated
3. **Build check**: ZMK firmware build validates the complete configuration

See [`.github/workflows/draw.yml`](../.github/workflows/draw.yml) for the automated diagram generation workflow.

## Keymap-Drawer Workflow

Diagrams are auto-generated and kept in sync using [keymap-drawer](https://github.com/caksoylar/keymap-drawer):

### Manual Generation

```bash
mise run draw
```

This will:
1. Parse `config/eyelash_sofle.keymap`
2. Generate `keymap-drawer/eyelash_sofle.yaml` (intermediate representation)
3. Render `keymap-drawer/eyelash_sofle.svg` (visual diagram)

### Automated Sync

On every push to `main` that modifies:
- `config/*.keymap`
- `keymap_drawer.config.yaml`

The [Draw workflow](../.github/workflows/draw.yml) automatically regenerates and commits updated diagrams.

### Configuration

The `keymap_drawer.config.yaml` file controls:
- Layout positioning
- Key styling and colors
- Legend abbreviations
- Combo visualization

## Adding a New Keyboard

See the [template/](../template/) directory for starter files. The typical workflow:

1. Copy `template/` to `config/<keyboard_name>/`
2. Modify the keymap for your keyboard's matrix
3. Update the layout JSON for your physical layout
4. Run `mise run draw` to generate diagrams
5. Add the keyboard to the build matrix in `.github/workflows/build.yml`

## Combos and Behaviors

### Combos

Combos are defined in the keymap file:

```dts
combos {
    compatible = "zmk,combos";
    combo_sleep {
        timeout-ms = <2000>;
        key-positions = <14, 28, 40>;  // Q + S + Z
        bindings = <&kp KP_SLEEP>;
        layers = <0>;
    };
};
```

Current active combo:
- `Q + S + Z` (hold 2 seconds) → Deep sleep

### Behaviors

Custom behaviors extend ZMK's defaults:

| Behavior | Purpose |
|----------|---------|
| `&kp` | Key press (standard) |
| `&mo` | Momentary layer |
| `&lt` | Layer-tap |
| `&mt` | Mod-tap |
| `&td` | Tap-dance |

## Further Reading

- [Miryoku Documentation](https://github.com/manna-harbour/miryoku)
- [Miryoku ZMK Implementation](https://github.com/manna-harbour/miryoku_zmk)
- [ZMK Documentation](https://zmk.dev/)
- [Keymap-Drawer Documentation](https://github.com/caksoylar/keymap-drawer)
