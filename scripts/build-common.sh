resolve_host_python() {
	local candidate
	for candidate in \
		"$(command -v python3 2>/dev/null || true)" \
		"$(command -v python 2>/dev/null || true)" \
		/opt/homebrew/bin/python3 \
		/usr/bin/python3; do
		[ -n "$candidate" ] || continue
		[ -x "$candidate" ] || continue
		case "$candidate" in
		*"/pipx-west/"*"/west/bin/"*) continue ;;
		esac
		if "$candidate" -m pip --version >/dev/null 2>&1; then
			printf '%s\n' "$candidate"
			return 0
		fi
	done
	return 1
}

host_python="$(resolve_host_python || true)"
if [ -z "$host_python" ]; then
	echo "Missing host Python with pip; install Python via mise, Xcode Command Line Tools, or Homebrew and rerun." >&2
	exit 1
fi

mkdir -p "$PWD/.zmk-python"
"$host_python" -m pip install --quiet --target "$PWD/.zmk-python" pyelftools
export PYTHONPATH="$PWD/.zmk-python${PYTHONPATH:+:$PYTHONPATH}"

if [ -z "${ZEPHYR_TOOLCHAIN_VARIANT:-}" ] && [ -z "${GNUARMEMB_TOOLCHAIN_PATH:-}" ]; then
	if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
		export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
		export GNUARMEMB_TOOLCHAIN_PATH="$(cd "$(dirname "$(command -v arm-none-eabi-gcc)")/.." && pwd)"
	else
		brew_prefix="$(brew --prefix arm-none-eabi-gcc 2>/dev/null || true)"
		if [ -n "$brew_prefix" ] && [ -x "$brew_prefix/bin/arm-none-eabi-gcc" ]; then
			export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
			export GNUARMEMB_TOOLCHAIN_PATH="$brew_prefix"
		fi
	fi
fi

if [ -z "${ZEPHYR_TOOLCHAIN_VARIANT:-}" ] && [ ! -d "${ZEPHYRSDK_INSTALL_DIR:-}" ]; then
	echo "Missing toolchain: install Zephyr SDK 0.16+ or install Homebrew arm-none-eabi-gcc and rerun." >&2
	echo "Example: brew install arm-none-eabi-gcc" >&2
	exit 1
fi

west build -p -d "$BUILD_DIR" -b "$BUILD_BOARD" -s zmk/app -- \
	-DZMK_CONFIG="$PWD/config" \
	-DBOARD_ROOT="$PWD" \
	-DSHIELD=nice_view \
	${BUILD_EXTRA_CMAKE_ARGS:-}

mkdir -p build
cp "$BUILD_DIR/zephyr/zmk.uf2" "build/${BUILD_BOARD}.uf2"
echo "Wrote build/${BUILD_BOARD}.uf2"
