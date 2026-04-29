# Build Options for Keychron Q1 V2 ISO Colemak-DH Miryoku Layout
# Enable features needed for Miryoku-style keymaps

# Encoder support (required for Q1 V2 with knob)
ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes

# Combo support for utility layer access
COMBO_ENABLE = yes

# Tap-hold configuration
TAP_DANCE_ENABLE = no

# RGB Matrix (Q1 V2 has per-key RGB)
RGB_MATRIX_ENABLE = yes

# Mouse keys for MOUSE layer
MOUSEKEY_ENABLE = yes

# Extra keys for ISO layout
KEY_LOCK_ENABLE = no

# Debug options (disable for production builds)
CONSOLE_ENABLE = no
COMMAND_ENABLE = no

# Bootmagic for emergency key combinations
BOOTMAGIC_ENABLE = yes

# Space savings for smaller builds
LTO_ENABLE = yes

# Disable unused features to save space
MUSIC_ENABLE = no
AUDIO_ENABLE = no
BACKLIGHT_ENABLE = no
UNICODE_ENABLE = no
KEY_OVERRIDE_ENABLE = no

# Miryoku-specific: Use custom tapping term
TAPPING_TERM = 200
QUICK_TAP_TERM = 100
