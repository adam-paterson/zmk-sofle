# Unified mise.toml Design Specification

## Overview

This document specifies the unified mise.toml design for the keyboards rig, providing a complete build system for ZMK and QMK firmware with CI/CD integration.

## Goals

1. Single `mise.toml` with all build tasks for both ZMK and QMK firmware
2. Support multiple keyboards (Sofle, Q1) and layouts (Colemak-DH, QWERTY)
3. Matrix-based GitHub Actions workflows for parallel builds
4. No sed commands — all configuration through proper build arguments
5. Clean separation between local development and CI/CD tasks

---

## 1. [tools] Section

```toml
[tools]
python = "3.13"
uv = "latest"
cmake = "latest"
ninja = "latest"
protobuf = "latest"
dtc = "latest"          # Device tree compiler for ZMK
qmk = "latest"          # QMK CLI for QMK builds
keymap-drawer = "latest"  # For generating keymap diagrams
yq = "latest"           # YAML processor for CI matrix generation
jq = "latest"           # JSON processor for matrix manipulation
```

---

## 2. Build Tasks Design

### 2.1 ZMK Build Tasks

#### Task: build:zmk:sofle:colemakdh

```toml
[tasks."build:zmk:sofle:colemakdh"]
description = "Build Sofle ZMK firmware with Colemak-DH layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

cd firmware/zmk

if [ ! -d .west ]; then
  uv run west init -l .
  uv run west update
  uv run west zephyr-export
fi

# Toolchain detection
if command -v arm-none-eabi-gcc &> /dev/null; then
  export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
elif [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ]; then
  export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
else
  echo "Error: No ARM toolchain found"
  exit 1
fi

KEYMAP_FILE="keyboards/eyelash_sofle/keymap/eyelash_sofle_colemakdh.keymap"

# Left half with ZMK Studio
uv run west build -s zmk/app -d build/sofle_colemakdh/left -b eyelash_sofle_left -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP_FILE="${KEYMAP_FILE}" \
  -DCONFIG_ZMK_STUDIO=y

# Right half
uv run west build -s zmk/app -d build/sofle_colemakdh/right -b eyelash_sofle_right -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP_FILE="${KEYMAP_FILE}"
'''
```

#### Task: build:zmk:sofle:qwerty

```toml
[tasks."build:zmk:sofle:qwerty"]
description = "Build Sofle ZMK firmware with QWERTY layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

cd firmware/zmk

if [ ! -d .west ]; then
  uv run west init -l .
  uv run west update
  uv run west zephyr-export
fi

# Toolchain detection
if command -v arm-none-eabi-gcc &> /dev/null; then
  export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
elif [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ]; then
  export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
else
  echo "Error: No ARM toolchain found"
  exit 1
fi

KEYMAP_FILE="keyboards/eyelash_sofle/keymap/eyelash_sofle_qwerty.keymap"

# Left half with ZMK Studio
uv run west build -s zmk/app -d build/sofle_qwerty/left -b eyelash_sofle_left -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP_FILE="${KEYMAP_FILE}" \
  -DCONFIG_ZMK_STUDIO=y

# Right half
uv run west build -s zmk/app -d build/sofle_qwerty/right -b eyelash_sofle_right -- \
  -DZMK_CONFIG="${PWD}/config" \
  -DBOARD_ROOT="${PWD}" \
  -DKEYMAP_FILE="${KEYMAP_FILE}"
'''
```

#### Task: build:zmk:sofle:all

```toml
[tasks."build:zmk:sofle:all"]
description = "Build all Sofle ZMK layouts"
depends = ["build:zmk:sofle:colemakdh", "build:zmk:sofle:qwerty"]
```

### 2.2 QMK Build Tasks

#### Task: build:qmk:q1:colemakdh

```toml
[tasks."build:qmk:q1:colemakdh"]
description = "Build QMK firmware for Keychron Q1 V2 ISO with Colemak-DH layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

# Setup QMK userspace if needed
if [ ! -d firmware/qmk/.build ]; then
  mkdir -p firmware/qmk/.build
fi

cd firmware/qmk

# Configure userspace
qmk config user.overlay_dir="${PWD}"

# Build using qmk compile
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_colemakdh
'''
```

#### Task: build:qmk:q1:qwerty

```toml
[tasks."build:qmk:q1:qwerty"]
description = "Build QMK firmware for Keychron Q1 V2 ISO with QWERTY layout"
run = '''
#!/usr/bin/env bash
set -euo pipefail

# Setup QMK userspace if needed
if [ ! -d firmware/qmk/.build ]; then
  mkdir -p firmware/qmk/.build
fi

cd firmware/qmk

# Configure userspace
qmk config user.overlay_dir="${PWD}"

# Build using qmk compile
qmk compile -kb keychron/q1v2/iso_encoder -km keychron_q1v2_iso_qwerty
'''
```

#### Task: build:qmk:q1:all

```toml
[tasks."build:qmk:q1:all"]
description = "Build all Q1 QMK layouts"
depends = ["build:qmk:q1:colemakdh", "build:qmk:q1:qwerty"]
```

### 2.3 Aggregate Build Tasks

```toml
[tasks."build:all"]
description = "Build all firmware for all keyboards and layouts"
depends = ["build:zmk:sofle:all", "build:qmk:q1:all"]

[tasks."build:colemakdh"]
description = "Build all Colemak-DH layouts"
depends = ["build:zmk:sofle:colemakdh", "build:qmk:q1:colemakdh"]

[tasks."build:qwerty"]
description = "Build all QWERTY layouts"
depends = ["build:zmk:sofle:qwerty", "build:qmk:q1:qwerty"]
```

---

## 3. Draw Tasks Design

### 3.1 Individual Keyboard Draw Tasks

#### Task: draw:sofle

```toml
[tasks."draw:sofle"]
description = "Generate keymap diagrams for Sofle keyboard"
sources = [
  "firmware/zmk/config/keyboards/eyelash_sofle/keymap/*.keymap",
  "keymap_drawer.config.yaml",
  "firmware/zmk/config/keyboards/eyelash_sofle/eyelash_sofle.json",
]
outputs = [
  "keymap-drawer/eyelash_sofle_colemakdh.svg",
  "keymap-drawer/eyelash_sofle_qwerty.svg",
]
run = '''
#!/usr/bin/env bash
set -euo pipefail

mkdir -p keymap-drawer

# Generate diagrams for each layout
for layout in colemakdh qwerty; do
  keymap_file="firmware/zmk/config/keyboards/eyelash_sofle/keymap/eyelash_sofle_${layout}.keymap"
  if [ -f "$keymap_file" ]; then
    echo "Drawing Sofle ${layout} keymap..."
    uv run keymap -c keymap_drawer.config.yaml parse -z "$keymap_file" > "keymap-drawer/eyelash_sofle_${layout}.yaml"
    uv run keymap -c keymap_drawer.config.yaml draw "keymap-drawer/eyelash_sofle_${layout}.yaml" -o "keymap-drawer/eyelash_sofle_${layout}.svg"
  fi
done
'''
```

#### Task: draw:q1

```toml
[tasks."draw:q1"]
description = "Generate keymap diagrams for Q1 keyboard"
sources = [
  "firmware/qmk/keyboards/keychron/q1v2/iso_encoder/keymaps/*/keymap.c",
  "keymap_drawer.config.yaml",
]
outputs = [
  "keymap-drawer/keychron_q1v2_iso_colemakdh.svg",
  "keymap-drawer/keychron_q1v2_iso_qwerty.svg",
]
run = '''
#!/usr/bin/env bash
set -euo pipefail

mkdir -p keymap-drawer

# Generate diagrams for each layout
for layout in colemakdh qwerty; do
  keymap_file="firmware/qmk/keyboards/keychron/q1v2/iso_encoder/keymaps/keychron_q1v2_iso_${layout}/keymap.c"
  if [ -f "$keymap_file" ]; then
    echo "Drawing Q1 ${layout} keymap..."
    # Convert QMK keymap to ZMK-like format for keymap-drawer
    uv run python scripts/keymap-convert.py \
      --input "$keymap_file" \
      --from qmk \
      --to zmk \
      --output "/tmp/q1_${layout}.keymap"
    
    uv run keymap -c keymap_drawer.config.yaml parse -z "/tmp/q1_${layout}.keymap" > "keymap-drawer/keychron_q1v2_iso_${layout}.yaml"
    uv run keymap -c keymap_drawer.config.yaml draw "keymap-drawer/keychron_q1v2_iso_${layout}.yaml" -o "keymap-drawer/keychron_q1v2_iso_${layout}.svg"
  fi
done
'''
```

### 3.2 Aggregate Draw Task

```toml
[tasks.draw]
description = "Generate all keymap diagrams"
depends = ["draw:sofle", "draw:q1"]
```

---

## 4. GitHub Actions Workflow Design

### 4.1 Build Workflow Matrix Strategy

The CI/CD workflow uses a dynamic matrix approach:

```yaml
name: Build All Firmware

on:
  workflow_dispatch:
  push:
    branches: [main]
    paths:
      - "firmware/**"
      - "lib/**"
      - ".github/workflows/build.yml"
  pull_request:
    paths:
      - "firmware/**"
      - "lib/**"

jobs:
  # Generate build matrix from configuration files
  matrix:
    runs-on: ubuntu-latest
    outputs:
      build-matrix: ${{ steps.generate.outputs.matrix }}
    steps:
      - uses: actions/checkout@v4

      - name: Setup mise
        uses: jdx/mise-action@v2
        with:
          install: true

      - name: Generate Build Matrix
        id: generate
        run: |
          # Create unified matrix combining ZMK and QMK targets
          matrix=$(mise run ci:matrix)
          echo "matrix=${matrix}" >> $GITHUB_OUTPUT

  # Build all firmware using the generated matrix
  build:
    needs: matrix
    runs-on: ubuntu-latest
    strategy:
      matrix: ${{ fromJson(needs.matrix.outputs.build-matrix) }}
      fail-fast: false
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Setup mise
        uses: jdx/mise-action@v2
        with:
          install: true

      - name: Build Firmware
        run: |
          mise run build:${{ matrix.firmware }}:${{ matrix.keyboard }}:${{ matrix.layout }}

      - name: Upload Artifacts
        uses: actions/upload-artifact@v4
        with:
          name: firmware-${{ matrix.firmware }}-${{ matrix.keyboard }}-${{ matrix.layout }}
          path: |
            firmware/zmk/build/**/*.uf2
            firmware/qmk/.build/**/*.bin
```

### 4.2 Matrix Generation Task (mise)

```toml
[tasks."ci:matrix"]
description = "Generate CI build matrix JSON"
run = '''
#!/usr/bin/env bash
set -euo pipefail

# Generate unified matrix from build.yaml and qmk.json
jq -n '{
  include: [
    # ZMK targets from build.yaml
    { firmware: "zmk", keyboard: "sofle", layout: "colemakdh" },
    { firmware: "zmk", keyboard: "sofle", layout: "qwerty" },
    # QMK targets from qmk.json
    { firmware: "qmk", keyboard: "q1", layout: "colemakdh" },
    { firmware: "qmk", keyboard: "q1", layout: "qwerty" }
  ]
}'
'''
```

### 4.3 Parallel Job Structure

The matrix produces 4 parallel jobs:

| Job | Firmware | Keyboard | Layout | Command |
|-----|----------|----------|--------|---------|
| 1 | zmk | sofle | colemakdh | `mise run build:zmk:sofle:colemakdh` |
| 2 | zmk | sofle | qwerty | `mise run build:zmk:sofle:qwerty` |
| 3 | qmk | q1 | colemakdh | `mise run build:qmk:q1:colemakdh` |
| 4 | qmk | q1 | qwerty | `mise run build:qmk:q1:qwerty` |

---

## 5. Helper/Utility Tasks

```toml
[tasks.setup]
description = "Setup all development environments"
depends = ["setup:zmk", "setup:qmk"]

[tasks."setup:zmk"]
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

[tasks."setup:qmk"]
description = "Setup QMK userspace"
run = '''
#!/usr/bin/env bash
set -euo pipefail
cd firmware/qmk
qmk config user.overlay_dir="${PWD}"
qmk setup --yes
'''

[tasks.clean]
description = "Clean all build artifacts"
run = '''
#!/usr/bin/env bash
set -euo pipefail
rm -rf firmware/zmk/build
rm -rf firmware/qmk/.build
rm -rf keymap-drawer/*.yaml keymap-drawer/*.svg
'''

[tasks.list]
description = "List all available build targets"
run = '''
#!/usr/bin/env bash
echo "Available build targets:"
echo ""
echo "ZMK (Sofle):"
echo "  mise run build:zmk:sofle:colemakdh"
echo "  mise run build:zmk:sofle:qwerty"
echo "  mise run build:zmk:sofle:all"
echo ""
echo "QMK (Keychron Q1):"
echo "  mise run build:qmk:q1:colemakdh"
echo "  mise run build:qmk:q1:qwerty"
echo "  mise run build:qmk:q1:all"
echo ""
echo "All keyboards:"
echo "  mise run build:colemakdh  # All Colemak-DH layouts"
echo "  mise run build:qwerty     # All QWERTY layouts"
echo "  mise run build:all        # Everything"
'''
```

---

## 6. Key Design Decisions

### 6.1 No Sed Commands

All tasks use proper build arguments instead of sed:

- **ZMK**: Uses `-DKEYMAP_FILE=` to specify keymap at build time
- **QMK**: Uses `qmk compile -km <keymap>` to select keymap
- **Matrix**: Uses jq/yq for JSON/YAML processing, not sed

### 6.2 Directory Structure

```
keyboards/
├── mise.toml                 # Unified task definitions
├── firmware/
│   ├── zmk/
│   │   ├── build.yaml        # ZMK CI matrix config
│   │   ├── config/
│   │   │   └── keyboards/
│   │   │       └── eyelash_sofle/
│   │   │           └── keymap/
│   │   │               ├── eyelash_sofle_colemakdh.keymap
│   │   │               └── eyelash_sofle_qwerty.keymap
│   │   └── zmk/              # ZMK submodule
│   └── qmk/
│       ├── qmk.json          # QMK userspace config
│       └── keyboards/
│           └── keychron/
│               └── q1v2/
│                   └── iso_encoder/
│                       └── keymaps/
│                           ├── keychron_q1v2_iso_colemakdh/
│                           └── keychron_q1v2_iso_qwerty/
└── keymap-drawer/            # Generated diagrams
```

### 6.3 Naming Convention

Task names follow the pattern: `build:<firmware>:<keyboard>:<layout>`

- **firmware**: zmk, qmk
- **keyboard**: sofle, q1
- **layout**: colemakdh, qwerty

---

## 7. Acceptance Criteria Mapping

| Criterion | Status | Implementation |
|-----------|--------|----------------|
| mise.toml design with [tools] section | ✅ | Section 1 defines tools |
| build:zmk:sofle:colemakdh with -DKEYMAP_FILE | ✅ | Section 2.1.1 |
| build:zmk:sofle:qwerty with -DKEYMAP_FILE | ✅ | Section 2.1.2 |
| build:qmk:q1:colemakdh using qmk compile | ✅ | Section 2.2.1 |
| build:qmk:q1:qwerty using qmk compile | ✅ | Section 2.2.2 |
| draw:sofle and draw:q1 tasks | ✅ | Section 3 |
| GitHub Actions matrix for 4 combinations | ✅ | Section 4 |
| No sed commands | ✅ | Section 6.1 |
| Design reviewed and approved | ⏳ | Pending review |

---

## 8. Migration Path

### Phase 1: Design (Current)
- Create design specification (this document)
- Review and approval

### Phase 2: Implementation
- Update mise.toml with new task definitions
- Update GitHub Actions workflows
- Test locally

### Phase 3: Validation
- Run full CI pipeline
- Verify all 4 matrix jobs pass
- Verify artifact generation

---

*Document Version: 1.0*
*Created: 2026-04-29*
*Status: Ready for Review*
