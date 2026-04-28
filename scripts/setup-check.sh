#!/usr/bin/env bash
set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

FAILED=0

log_ok() {
    echo -e "${GREEN}✓${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}⚠${NC} $1"
}

log_fail() {
    echo -e "${RED}✗${NC} $1"
}

log_info() {
    echo -e "  $1"
}

echo "🔧 Development Environment Validation"
echo "===================================="
echo ""

# 1. Check mise installed and tasks loadable
echo "📦 Checking mise..."
if ! command -v mise >/dev/null 2>&1; then
    log_fail "mise is not installed"
    log_info "Install with: curl https://mise.run | sh"
    log_info "Or see: https://mise.jdx.dev/getting-started.html"
    FAILED=1
else
    MISE_VERSION=$(mise --version 2>/dev/null || echo "unknown")
    log_ok "mise installed (version: $MISE_VERSION)"
    
    # Check if tasks are loadable (tasks command may not work even with experimental=true)
    if [ -f "mise.toml" ]; then
        TASK_COUNT=$(grep -c "^\[tasks\." mise.toml 2>/dev/null || echo "0")
        if [ "$TASK_COUNT" -gt 0 ]; then
            log_ok "mise.toml found with $TASK_COUNT task(s) defined"
        else
            log_warn "mise.toml found but no tasks defined"
        fi
    else
        log_fail "mise.toml not found in project root"
        FAILED=1
    fi
fi
echo ""

# 2. Check west installed and in PATH
echo "🧭 Checking west..."
if ! command -v west >/dev/null 2>&1; then
    # Check if uv can run west
    if command -v uv >/dev/null 2>&1; then
        if uv run west --version >/dev/null 2>&1; then
            WEST_VERSION=$(uv run west --version 2>/dev/null | head -1)
            log_ok "west available via uv ($WEST_VERSION)"
        else
            log_fail "west not installed"
            log_info "Install with: uv pip install west"
            FAILED=1
        fi
    else
        log_fail "west not in PATH and uv not available"
        log_info "Install west with: pip install west"
        FAILED=1
    fi
else
    WEST_VERSION=$(west --version 2>/dev/null | head -1)
    log_ok "west in PATH ($WEST_VERSION)"
fi
echo ""

# 3. Check ARM toolchain (gcc-arm-none-eabi) available
echo "🔨 Checking ARM toolchain..."
ARM_TOOLCHAIN_FOUND=0

if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    ARM_VERSION=$(arm-none-eabi-gcc --version 2>/dev/null | head -1)
    log_ok "arm-none-eabi-gcc found in PATH"
    log_info "  $ARM_VERSION"
    ARM_TOOLCHAIN_FOUND=1
else
    # Check for Homebrew on macOS
    if command -v brew >/dev/null 2>&1; then
        BREW_PREFIX=$(brew --prefix arm-none-eabi-gcc 2>/dev/null || true)
        if [ -n "$BREW_PREFIX" ] && [ -x "$BREW_PREFIX/bin/arm-none-eabi-gcc" ]; then
            log_ok "arm-none-eabi-gcc found via Homebrew"
            log_info "  Location: $BREW_PREFIX/bin/arm-none-eabi-gcc"
            ARM_TOOLCHAIN_FOUND=1
        fi
    fi
fi

if [ $ARM_TOOLCHAIN_FOUND -eq 0 ]; then
    log_warn "GNU Arm Embedded toolchain not found in PATH"
    log_info "  The build scripts will attempt to detect it automatically"
    log_info "  Install with: brew install arm-none-eabi-gcc (macOS)"
    log_info "  Or see: https://developer.arm.com/downloads/view/GNU-RM"
fi
echo ""

# 4. Check Zephyr SDK installed and ZEPHYR_SDK_INSTALL_DIR set
echo "🌊 Checking Zephyr SDK..."
ZEPHYR_SDK_OK=0

if [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ] && [ -d "$ZEPHYR_SDK_INSTALL_DIR" ]; then
    log_ok "ZEPHYR_SDK_INSTALL_DIR is set and directory exists"
    log_info "  Path: $ZEPHYR_SDK_INSTALL_DIR"
    ZEPHYR_SDK_OK=1
elif [ -n "${ZEPHYR_SDK_INSTALL_DIR:-}" ]; then
    log_warn "ZEPHYR_SDK_INSTALL_DIR is set but directory does not exist"
    log_info "  Expected: $ZEPHYR_SDK_INSTALL_DIR"
else
    log_info "ZEPHYR_SDK_INSTALL_DIR not set (optional if using GNU Arm Embedded)"
fi

# Check for common Zephyr SDK locations
if [ $ZEPHYR_SDK_OK -eq 0 ]; then
    COMMON_LOCATIONS=(
        "$HOME/zephyr-sdk"
        "/opt/zephyr-sdk"
        "/usr/local/zephyr-sdk"
    )
    for loc in "${COMMON_LOCATIONS[@]}"; do
        if [ -d "$loc" ]; then
            log_warn "Zephyr SDK found at $loc but ZEPHYR_SDK_INSTALL_DIR not set"
            log_info "  Set with: export ZEPHYR_SDK_INSTALL_DIR=$loc"
            break
        fi
    done
fi
echo ""

# 5. Check Python dependencies from zmk requirements
echo "🐍 Checking Python dependencies..."
PYTHON_OK=0

if command -v uv >/dev/null 2>&1; then
    if [ -f "pyproject.toml" ]; then
        # Check if dependencies are installed in venv
        if uv pip list 2>/dev/null | grep -q "west\|keymap-drawer" 2>/dev/null; then
            log_ok "Python dependencies available via uv"
            PYTHON_OK=1
        else
            log_warn "Python dependencies may not be installed"
            log_info "  Run: uv sync or uv pip install -e ."
        fi
    else
        log_warn "pyproject.toml not found"
    fi
elif command -v pip >/dev/null 2>&1; then
    if pip list 2>/dev/null | grep -q "west" 2>/dev/null; then
        log_ok "west Python package installed"
        PYTHON_OK=1
    else
        log_warn "west Python package not found"
        log_info "  Install with: pip install west"
    fi
else
    log_fail "Neither uv nor pip found"
    log_info "  Install uv: curl https://astral.sh/uv/install.sh | sh"
    FAILED=1
fi
echo ""

# 6. Check Git submodules initialized (miryoku_zmk)
echo "📁 Checking Git submodules..."
if [ -f ".gitmodules" ]; then
    # Check each submodule
    while IFS= read -r line; do
        if [[ $line =~ path\ *=\ *(.+) ]]; then
            SUBMODULE_PATH="${BASH_REMATCH[1]}"
            if [ -d "$SUBMODULE_PATH" ]; then
                # Check if submodule has content (not just an empty directory)
                if [ "$(ls -A "$SUBMODULE_PATH" 2>/dev/null | wc -l)" -gt 0 ]; then
                    log_ok "Submodule initialized: $SUBMODULE_PATH"
                else
                    log_fail "Submodule not initialized: $SUBMODULE_PATH"
                    log_info "  Run: git submodule update --init --recursive"
                    FAILED=1
                fi
            else
                log_fail "Submodule directory missing: $SUBMODULE_PATH"
                log_info "  Run: git submodule update --init --recursive"
                FAILED=1
            fi
        fi
    done < <(grep "path" .gitmodules 2>/dev/null || true)
else
    log_info "No .gitmodules file found"
fi
echo ""

# Summary
echo "===================================="
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All checks passed!${NC}"
    echo ""
    echo "Your development environment is ready."
    echo "Run 'mise run setup' to initialize the west workspace."
    exit 0
else
    echo -e "${RED}✗ Some checks failed.${NC}"
    echo ""
    echo "Please fix the issues above and run this check again."
    exit 1
fi
