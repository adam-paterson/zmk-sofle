# Keymap Template

This directory contains starter files for adding a new keyboard to the repository.

## Quick Start

1. **Copy this template** to `config/<keyboard_name>/`:
   ```bash
   cp -r template/config/* config/my_keyboard/
   ```

2. **Customize the keymap** for your keyboard's matrix layout

3. **Update the JSON layout** to match your physical key positions

4. **Test locally**:
   ```bash
   mise run draw
   ```

5. **Add to CI** (optional): Update `.github/workflows/build.yml` with your board name

## Files Overview

| File | Purpose |
|------|---------|
| `template_keymap.keymap` | Starter keymap with all layers defined |
| `template.json` | Physical layout definition for keymap-drawer |
| `template.conf` | Kconfig settings for ZMK features |
| `west.yml` | West workspace configuration |

## Customization Guide

### Step 1: Rename Files

```bash
mv template_keymap.keymap my_keyboard.keymap
mv template.json my_keyboard.json
mv template.conf my_keyboard.conf
```

### Step 2: Update Matrix Size

In the `.keymap` file, adjust the matrix dimensions:

```dts
/ {
    keymap {
        compatible = "zmk,keymap";
        // Rows x cols must match your keyboard's physical matrix
    };
};
```

### Step 3: Define Key Positions

In the `.json` file, update the layout array to match your keyboard's physical positions. Each key needs:

- `row`, `col`: Matrix position
- `x`, `y`: Visual position for diagram (in key units)
- `label`: Optional label for debugging

### Step 4: Customize Layers

The template uses miryoku-style home-row mods and layer-taps. Adapt the bindings to your preference:

- **Base layer**: Choose your preferred alpha layout
- **SYMS layer**: F-keys, numbers, symbols
- **NAV layer**: Arrows, clipboard, navigation
- **UTIL layer**: Bluetooth, RGB, system
- **CODE layer**: macOS shortcuts (optional)

### Step 5: Add Combos (Optional)

Define combos for common shortcuts:

```dts
combos {
    compatible = "zmk,combos";
    combo_esc {
        timeout-ms = <50>;
        key-positions = <1 2>;  // Adjacent keys
        bindings = <&kp ESC>;
        layers = <0>;
    };
};
```

## Layer Reference

See [docs/keymap-management.md](../docs/keymap-management.md) for full layer documentation.

## Troubleshooting

**Keymap won't parse?**
- Check matrix dimensions match physical layout
- Verify all key positions are unique
- Ensure all referenced layers exist

**Diagram looks wrong?**
- Adjust `x`, `y` coordinates in JSON
- Check `keymap_drawer.config.yaml` settings
- Verify row/col mapping

**Build fails?**
- Ensure board definition exists in `boards/`
- Check Kconfig options in `.conf` file
- Verify west workspace is initialized: `mise run setup`
