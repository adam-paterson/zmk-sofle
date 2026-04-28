# Contributing Guide

This document describes the complete firmware build, artifact, and release workflow for maintainers.

## Overview

This repository uses GitHub Actions to build ZMK firmware and create releases automatically. The firmware is built for a custom Eyelash Sofle keyboard using nice!nano v2 controllers.

## GitHub Actions Workflows

### Build Workflow (`.github/workflows/build.yml`)

**Triggers:**
- `workflow_dispatch` - Manual trigger from GitHub UI
- `push` to any branch (ignores `keymap-drawer/**`, `README.md`, `*.md` files)

**Jobs:**

#### 1. Build Job
Uses the official ZMK reusable workflow:
```yaml
uses: zmkfirmware/zmk/.github/workflows/build-user-config.yml@v0.3.0
```

This builds the firmware based on the configuration in `config/` and board definitions in `boards/`.

**Build Matrix:**
- Left half: `adams_sofle_studio_left` (with ZMK Studio support)
- Right half: `adams_sofle_right`
- Settings reset: `settings_reset-nice_nano_v2-zmk` (for clearing Bluetooth pairings)

#### 2. Release Job
Runs only on pushes to `main` branch.

**Permissions Required:**
- `contents: write` - to create releases

**Release Naming Convention:**
- Tag: `firmware-${{ github.run_number }}`
- Name: `Adam's Sofle Firmware #${{ github.run_number }}`

The release is created using `softprops/action-gh-release@v2` and includes:
- All `.uf2` firmware files as attachments
- Auto-generated release notes with:
  - Build commit SHA
  - Download instructions
  - File reference table
  - Keymap layer documentation
  - Combo documentation
  - Troubleshooting steps

#### 3. Notify Job
Posts a commit comment with:
- Link to the release
- Build details URL
- Quick flash instructions

### Draw Workflow (`.github/workflows/draw.yml`)

**Triggers:**
- `workflow_dispatch` - Manual trigger
- `push` to `config/**`, `.github/workflows/draw.yml`, or `keymap_drawer.config.yaml`

Uses the keymap-drawer reusable workflow to regenerate the SVG diagram and commit it automatically.

## Artifact Naming and Retention

**Generated Artifacts:**
| Artifact Name | Source | Retention |
|--------------|--------|-----------|
| `firmware` | ZMK build | Default GitHub retention |
| `adams_sofle_studio_left.uf2` | Left half with Studio | Attached to release |
| `adams_sofle_right.uf2` | Right half | Attached to release |
| `settings_reset-nice_nano_v2-zmk.uf2` | Settings reset | Attached to release |

## Release Process

### Automatic Releases (Standard Flow)

Every push to `main` automatically:
1. Builds firmware using ZMK v0.3.0
2. Creates a GitHub release with tag `firmware-{run_number}`
3. Attaches all `.uf2` files
4. Posts commit comment with download link

### Manual Releases

To trigger a manual build:

1. Go to **Actions** → **Build ZMK firmware**
2. Click **Run workflow**
3. Select branch (usually `main`)
4. Click **Run workflow**

Note: Manual runs do not create releases (only push to `main` does).

### Version Tagging Convention

This repo uses **build number tagging** (not semantic versioning):
- Format: `firmware-{run_number}`
- Example: `firmware-42`

For semantic versioning (if needed in future):
- Use `v{major}.{minor}.{patch}` format
- Examples: `v1.0.0`, `v1.0.0-rc1`

To create a manually-tagged release:
```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

Then create a release from that tag via GitHub UI.

## Downloading CI Artifacts for Testing

### From Releases (Recommended)

1. Go to **Releases** section of the repo
2. Find the latest release (or specific build)
3. Download `firmware.zip` from Assets
4. Extract the `.uf2` files

### From Workflow Runs

1. Go to **Actions** → Select a workflow run
2. Scroll to **Artifacts** section
3. Download `firmware` artifact
4. Extract and use the `.uf2` files

**Note:** Artifact retention is limited (default 90 days). Releases are permanent.

## CI Badges

Add these badges to documentation:

```markdown
[![Build ZMK firmware](https://github.com/{owner}/{repo}/actions/workflows/build.yml/badge.svg)](https://github.com/{owner}/{repo}/actions/workflows/build.yml)
[![Draw Keymap](https://github.com/{owner}/{repo}/actions/workflows/draw.yml/badge.svg)](https://github.com/{owner}/{repo}/actions/workflows/draw.yml)
```

## Local Development

### Prerequisites

- `mise` installed (see [mise.jdx.dev](https://mise.jdx.dev/))
- Zephyr SDK OR GNU Arm Embedded toolchain
  - macOS: `brew install arm-none-eabi-gcc`

### Setup

```bash
mise install
mise run setup
```

### Build Commands

```bash
mise run build:left        # Build left half
mise run build:left --studio  # Build with ZMK Studio
mise run build:right       # Build right half
mise run build:all         # Build both halves
```

### Keymap Diagram

```bash
mise run draw              # Regenerate SVG
```

## Troubleshooting CI/CD

### Build Failures

1. Check the ZMK version in `config/west.yml` matches the workflow version
2. Verify board definitions in `boards/` are valid
3. Check keymap syntax in `config/*.keymap`

### Release Failures

1. Ensure `GITHUB_TOKEN` has `contents: write` permission
2. Check that the build job succeeded before release job
3. Verify no existing release with the same tag name

### Draw Workflow Failures

1. Validate `keymap_drawer.config.yaml` syntax
2. Ensure `config/*.keymap` files are valid
3. Check `config/*.json` files match keymap layout

## Maintainers Checklist

Before merging changes:
- [ ] Firmware builds locally: `mise run build:all`
- [ ] Keymap diagram generates: `mise run draw`
- [ ] CI workflows are valid (check in GitHub UI)
- [ ] Version in `west.yml` matches workflow version

## Resources

- [ZMK Documentation](https://zmk.dev/)
- [ZMK Firmware Repo](https://github.com/zmkfirmware/zmk)
- [keymap-drawer](https://github.com/caksoylar/keymap-drawer)
- [mise](https://mise.jdx.dev/)
