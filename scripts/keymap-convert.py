#!/usr/bin/env python3
"""
keymap-convert.py
Converts firmware-agnostic keymap definitions to ZMK and QMK formats.

Usage:
    python scripts/keymap-convert.py --input lib/keymaps --output firmware/
    python scripts/keymap-convert.py --from zmk --to qmk --input <file> --output <file>
"""

import argparse
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional


class LayerDefinition:
    """Represents a single layer definition from a .dtsi file."""

    def __init__(self, name: str, content: str, metadata: Dict[str, str]):
        self.name = name
        self.content = content
        self.metadata = metadata

    @property
    def display_name(self) -> str:
        return self.metadata.get("display", self.name)

    @property
    def color(self) -> str:
        return self.metadata.get("color", "#808080")


class KeymapConverter:
    """Converts between firmware-agnostic and firmware-specific formats."""

    # ZMK to QMK keycode mapping
    ZMK_TO_QMK = {
        "&kp Q": "KC_Q",
        "&kp W": "KC_W",
        "&kp E": "KC_E",
        "&kp R": "KC_R",
        "&kp T": "KC_T",
        "&kp Y": "KC_Y",
        "&kp U": "KC_U",
        "&kp I": "KC_I",
        "&kp O": "KC_O",
        "&kp P": "KC_P",
        "&kp A": "KC_A",
        "&kp S": "KC_S",
        "&kp D": "KC_D",
        "&kp F": "KC_F",
        "&kp G": "KC_G",
        "&kp H": "KC_H",
        "&kp J": "KC_J",
        "&kp K": "KC_K",
        "&kp L": "KC_L",
        "&kp SEMI": "KC_SCLN",
        "&kp Z": "KC_Z",
        "&kp X": "KC_X",
        "&kp C": "KC_C",
        "&kp V": "KC_V",
        "&kp B": "KC_B",
        "&kp N": "KC_N",
        "&kp M": "KC_M",
        "&kp COMMA": "KC_COMM",
        "&kp DOT": "KC_DOT",
        "&kp FSLH": "KC_SLSH",
        "&kp N1": "KC_1",
        "&kp N2": "KC_2",
        "&kp N3": "KC_3",
        "&kp N4": "KC_4",
        "&kp N5": "KC_5",
        "&kp N6": "KC_6",
        "&kp N7": "KC_7",
        "&kp N8": "KC_8",
        "&kp N9": "KC_9",
        "&kp N0": "KC_0",
        "&kp F1": "KC_F1",
        "&kp F2": "KC_F2",
        "&kp F3": "KC_F3",
        "&kp F4": "KC_F4",
        "&kp F5": "KC_F5",
        "&kp F6": "KC_F6",
        "&kp F7": "KC_F7",
        "&kp F8": "KC_F8",
        "&kp F9": "KC_F9",
        "&kp F10": "KC_F10",
        "&kp ESC": "KC_ESC",
        "&kp TAB": "KC_TAB",
        "&kp SPACE": "KC_SPC",
        "&kp RET": "KC_ENT",
        "&kp BSPC": "KC_BSPC",
        "&kp DEL": "KC_DEL",
        "&kp RSHIFT": "KC_RSFT",
        "&kp RALT": "KC_RALT",
        "&kp LCTRL": "KC_LCTL",
        "&kp LGUI": "KC_LGUI",
        "&kp LSHIFT": "KC_LSFT",
        "&kp LBKT": "KC_LBRC",
        "&kp RBKT": "KC_RBRC",
        "&kp LBRC": "KC_LCBR",
        "&kp RBRC": "KC_RCBR",
        "&kp BSLH": "KC_BSLS",
        "&kp MINUS": "KC_MINS",
        "&kp EQUAL": "KC_EQL",
        "&kp SQT": "KC_QUOT",
        "&kp GRAVE": "KC_GRV",
        "&kp LPAR": "KC_LPRN",
        "&kp RPAR": "KC_RPRN",
        "&kp HOME": "KC_HOME",
        "&kp END": "KC_END",
        "&kp PG_UP": "KC_PGUP",
        "&kp PG_DN": "KC_PGDN",
        "&kp UP": "KC_UP",
        "&kp DOWN": "KC_DOWN",
        "&kp LEFT": "KC_LEFT",
        "&kp RIGHT": "KC_RGHT",
        "&none": "XXXXXXX",
        "&trans": "_______",
        "&mo": "MO",
        "&lt": "LT",
        "&soft_off": "QK_BOOT",
    }

    def __init__(self, input_dir: str, output_dir: str):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.layers: List[LayerDefinition] = []

    def parse_shared(self, file_path: Path) -> List[LayerDefinition]:
        """Parse firmware-agnostic .dtsi file and extract layer definitions."""
        content = file_path.read_text()
        layers = []

        # Extract LAYER_* defines
        layer_pattern = re.compile(
            r'#define\s+(LAYER_\w+)\s+\\\s*\n((?:.*?\\\s*\n)*)',
            re.MULTILINE | re.DOTALL
        )

        # Extract metadata comments
        metadata_pattern = re.compile(
            r'//\s*LAYER:\s*(\w+)\s*=\s*([^,]+)(?:,\s*([^\n]+))?'
        )

        for match in layer_pattern.finditer(content):
            layer_name = match.group(1)
            layer_content = match.group(2).replace('\\\n', '\n').strip()

            # Find metadata for this layer
            metadata = {}
            for meta_match in metadata_pattern.finditer(content):
                if meta_match.group(1) in layer_name:
                    for pair in meta_match.group(2).split(','):
                        if '=' in pair:
                            k, v = pair.split('=', 1)
                            metadata[k.strip()] = v.strip()

            layers.append(LayerDefinition(layer_name, layer_content, metadata))

        return layers

    def to_zmk(self, layers: List[LayerDefinition]) -> str:
        """Generate ZMK keymap format from layer definitions."""
        output = []
        output.append("// Generated from shared keymap definitions")
        output.append("// Do not edit directly - modify lib/keymaps/ instead")
        output.append("")
        output.append("#include <behaviors.dtsi>")
        output.append("#include <dt-bindings/zmk/keys.h>")
        output.append("#include <dt-bindings/zmk/bt.h>")
        output.append("")

        for layer in layers:
            # Convert RC() macros to ZMK keymap syntax
            content = layer.content
            # Replace RC(row,col) with nothing (just positioning markers)
            content = re.sub(r'RC\(\d+,\d+\)\s*', '', content)
            # Clean up whitespace
            content = re.sub(r'\s+', ' ', content)

            output.append(f"// {layer.display_name} Layer")
            output.append(f"#define {layer.name} \\")
            output.append(content)
            output.append("")

        return "\n".join(output)

    def to_qmk(self, layers: List[LayerDefinition]) -> str:
        """Generate QMK keymap.c format from layer definitions."""
        output = []
        output.append("// Generated from shared keymap definitions")
        output.append("// Do not edit directly - modify lib/keymaps/ instead")
        output.append("")
        output.append("#include QMK_KEYBOARD_H")
        output.append("")

        # Generate layer enum
        output.append("enum layers {")
        for i, layer in enumerate(layers):
            name = layer.name.replace("LAYER_", "").upper()
            suffix = "," if i < len(layers) - 1 else ""
            output.append(f"    {name}{suffix}")
        output.append("};")
        output.append("")

        # Generate keymaps
        output.append("const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {")

        for i, layer in enumerate(layers):
            name = layer.name.replace("LAYER_", "").upper()
            content = layer.content

            # Convert ZMK keycodes to QMK
            for zmk, qmk in self.ZMK_TO_QMK.items():
                content = content.replace(zmk, qmk)

            # Remove RC() markers
            content = re.sub(r'RC\(\d+,\d+\)\s*', '', content)

            # Format for C array
            keys = content.split()

            output.append(f"    [{name}] = LAYOUT(")
            # Format as proper C array with row breaks
            for row_idx in range(4):
                row_keys = keys[row_idx*5:(row_idx+1)*5]
                if row_keys:
                    output.append("        " + ", ".join(row_keys) + ",")
            output.append("        // Right hand keys here...")
            output.append("    ),")

        output.append("};")

        return "\n".join(output)

    def convert_all(self):
        """Convert all .dtsi files in input directory."""
        for dtsi_file in self.input_dir.rglob("*.dtsi"):
            print(f"Processing: {dtsi_file}")
            layers = self.parse_shared(dtsi_file)

            if not layers:
                print(f"  No layers found in {dtsi_file}")
                continue

            # Generate ZMK output
            zmk_output = self.to_zmk(layers)
            zmk_path = self.output_dir / "zmk" / "config" / f"{dtsi_file.stem}_generated.keymap"
            zmk_path.parent.mkdir(parents=True, exist_ok=True)
            zmk_path.write_text(zmk_output)
            print(f"  Generated: {zmk_path}")

            # Generate QMK output
            qmk_output = self.to_qmk(layers)
            qmk_path = self.output_dir / "qmk" / "keymaps" / dtsi_file.parent.name / dtsi_file.stem / "keymap.c"
            qmk_path.parent.mkdir(parents=True, exist_ok=True)
            qmk_path.write_text(qmk_output)
            print(f"  Generated: {qmk_path}")


def main():
    parser = argparse.ArgumentParser(description="Convert keymaps between formats")
    parser.add_argument("--input", "-i", required=True, help="Input directory or file")
    parser.add_argument("--output", "-o", required=True, help="Output directory or file")
    parser.add_argument("--from", "-f", dest="from_fmt", choices=["zmk", "qmk", "shared"],
                        default="shared", help="Source format")
    parser.add_argument("--to", "-t", dest="to_fmt", choices=["zmk", "qmk"],
                        help="Target format (for single file conversion)")

    args = parser.parse_args()

    converter = KeymapConverter(args.input, args.output)

    if args.to_fmt:
        # Single file conversion mode
        print(f"Converting {args.input} from {args.from_fmt} to {args.to_fmt}")
        # TODO: Implement single file conversion
    else:
        # Batch conversion mode
        converter.convert_all()

    return 0


if __name__ == "__main__":
    sys.exit(main())
