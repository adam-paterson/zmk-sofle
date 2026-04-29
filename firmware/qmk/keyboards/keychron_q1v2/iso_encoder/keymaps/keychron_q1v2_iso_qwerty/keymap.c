// Copyright 2023 QMK Contributors
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Keychron Q1 V2 ISO Encoder - QWERTY Miryoku Layout
// 75% ISO layout with rotary encoder
// This keymap uses QWERTY alphas with Miryoku layer structure

// Layer definitions
enum layers {
    BASE = 0,   // QWERTY base layer
    EXTRA,      // Colemak-DH alternative (accessible via keymap switch)
    TAP,        // Tap-only layer
    BUTTON,
    NAV,        // Navigation
    MOUSE,
    MEDIA,
    NUM,        // Number pad
    SYM,        // Symbols
    FUN,        // Function keys
    UTIL        // Utilities (Bluetooth, RGB)
};

// Home row mod tap-hold timeouts
#define TAPPING_TERM 200
#define QUICK_TAP_TERM 100

// Thumb cluster keys
#define THUMB_L1 LT(NAV, KC_BSPC)      // Left inner: Backspace / NAV on hold
#define THUMB_L2 LT(SYM, KC_TAB)       // Left outer: Tab / SYM on hold
#define THUMB_R1 LT(NUM, KC_SPC)       // Right inner: Space / NUM on hold
#define THUMB_R2 LT(FUN, KC_ENT)       // Right outer: Enter / FUN on hold

// Home row mods (QWERTY)
// Left hand: A (GUI), S (ALT), D (CTRL), F (SHIFT)
// Right hand: J (SHIFT), K (CTRL), L (ALT), ; (GUI)
#define HM_A LGUI_T(KC_A)
#define HM_S LALT_T(KC_S)
#define HM_D LCTL_T(KC_D)
#define HM_F LSFT_T(KC_F)
#define HM_J RSFT_T(KC_J)
#define HM_K RCTL_T(KC_K)
#define HM_L LALT_T(KC_L)
#define HM_SCLN RGUI_T(KC_SCLN)

// Layer-tap keys for home row (optional, for NAV/SYM access)
#define LT_NAV_D LT(NAV, KC_D)
#define LT_SYM_K LT(SYM, KC_K)

// QMK Keyboard Matrix for Keychron Q1 V2 ISO
// Matrix is 6 rows x 15 columns
// Row 0: ESC, F1-F12, INS, DEL, Encoder button
// Row 1: `, 1-9, 0, -, =, BSPC, PGUP
// Row 2: TAB, Q-P, [, ], ISO extra key, PGDN
// Row 3: CAPS, A-L, ;, ', Enter, HOME
// Row 4: LSHIFT, ISO backslash, Z-M, ,, ., /, RSHIFT, UP, END
// Row 5: LCTRL, LGUI, LALT, Space, RALT, FN, RCTRL, LEFT, DOWN, RIGHT

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // BASE layer: QWERTY with home row mods
    [BASE] = LAYOUT_iso_83(
        // Row 0: Function keys and extras
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   KC_MUTE,
        // Row 1: Number row
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        // Row 2: QWERTY row
        THUMB_L2, KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
        // Row 3: Home row - QWERTY: A, S, D, F, G / H, J, K, L, ;
        KC_CAPS,  HM_A,     HM_S,     HM_D,     HM_F,     KC_G,     KC_H,     HM_J,     HM_K,     HM_L,     HM_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        // Row 4: Bottom row - QWERTY: Z, X, C, V, B / N, M
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_END,
        // Row 5: Modifiers and thumb area
        KC_LCTL,  KC_LGUI,  KC_LALT,                      THUMB_R1,                     KC_RALT,  MO(UTIL), KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT
    ),

    // EXTRA layer: Colemak-DH with home row mods (same structure, different alpha keys)
    [EXTRA] = LAYOUT_iso_83(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_F,     KC_P,     KC_B,     KC_J,     KC_L,     KC_U,     KC_Y,     KC_SCLN,  KC_LBRC,  KC_RBRC,                      KC_PGDN,
        KC_CAPS,  LGUI_T(KC_A), LALT_T(KC_R), LCTL_T(KC_S), LSFT_T(KC_T), KC_G,
                  KC_M, RSFT_T(KC_N), RCTL_T(KC_E), LALT_T(KC_I), RGUI_T(KC_O), KC_QUOT, KC_NUHS, KC_ENT, KC_HOME,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_D,     KC_V,     KC_K,     KC_H,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                       KC_RALT,  MO(UTIL), KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT
    ),

    // TAP layer: QWERTY without mods (for gaming or when mods interfere)
    [TAP] = LAYOUT_iso_83(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   KC_MUTE,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                       KC_RALT,  _______,  KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT
    ),

    // BUTTON layer: Mouse buttons (for one-handed operation)
    [BUTTON] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MS_L,  KC_MS_D,  KC_MS_U,  KC_MS_R,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      KC_BTN1,                      KC_BTN2,  _______,  _______,            _______,  _______,  _______
    ),

    // NAV layer: Navigation and editing
    [NAV] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  KC_INS,   KC_HOME,  KC_PGDN,  KC_PGUP,  KC_END,   _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  KC_LEFT,  KC_DOWN,  KC_UP,    KC_RGHT,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_BSPC,  KC_DEL,   _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,                      _______,  _______,  _______,            _______,  _______,  _______
    ),

    // MOUSE layer: Mouse movement and scrolling
    [MOUSE] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  KC_MS_L,  KC_MS_D,  KC_MS_U,  KC_MS_R,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_WH_L,  KC_WH_D,  KC_WH_U,  KC_WH_R,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      KC_BTN1,                      KC_BTN2,  _______,  _______,            _______,  _______,  _______
    ),

    // MEDIA layer: Media controls and volume
    [MEDIA] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MUTE,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  KC_MPRV,  KC_VOLD,  KC_VOLU,  KC_MNXT,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      KC_MPLY,                      KC_MSTP,  _______,  _______,            _______,  _______,  _______
    ),

    // NUM layer: Number pad
    [NUM] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_P7,    KC_P8,    KC_P9,    _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_P4,    KC_P5,    KC_P6,    _______,  _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  KC_P1,    KC_P2,    KC_P3,    _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_P0,    KC_PDOT,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,                      _______,  _______,  _______,            _______,  _______,  _______
    ),

    // SYM layer: Symbols
    [SYM] = LAYOUT_iso_83(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  KC_EXLM,  KC_AT,    KC_HASH,  KC_DLR,   KC_PERC,  KC_CIRC,  KC_AMPR,  KC_ASTR,  KC_LPRN,  KC_RPRN,  _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  KC_MINS,  KC_EQL,   KC_LBRC,  KC_RBRC,  KC_SCLN,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_UNDS,  KC_PLUS,  KC_LCBR,  KC_RCBR,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,                      _______,  _______,  _______,            _______,  _______,  _______
    ),

    // FUN layer: Function keys and utilities
    [FUN] = LAYOUT_iso_83(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______,  _______,
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  KC_LGUI,  KC_LALT,  KC_LCTL,  KC_LSFT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,                      _______,  _______,  _______,            _______,  _______,  _______
    ),

    // UTIL layer: System controls, RGB, and Bluetooth
    [UTIL] = LAYOUT_iso_83(
        QK_BOOT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  RGB_TOG,  RGB_MOD,  RGB_HUI,  RGB_SAI,  RGB_VAI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  RGB_M_P,  RGB_M_B,  RGB_M_R,  RGB_M_SW, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,                      _______,  _______,  _______,            _______,  _______,  _______
    )
};

// Encoder mapping
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [EXTRA]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [TAP]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [BUTTON] = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U) },
    [NAV]    = { ENCODER_CCW_CW(KC_PGDN, KC_PGUP) },
    [MOUSE]  = { ENCODER_CCW_CW(KC_WH_D, KC_WH_U) },
    [MEDIA]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [NUM]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [SYM]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [FUN]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [UTIL]   = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
};
#endif // ENCODER_MAP_ENABLE

// Combo definitions for layer access (optional, for advanced users)
// This enables holding multiple thumb keys for utility layer
const uint16_t PROGMEM combo_util[] = {THUMB_L1, THUMB_R1, COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_util, MO(UTIL)),
};

// Custom key processing (for advanced features)
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}

// Layer change callback (for RGB feedback)
layer_state_t layer_state_set_user(layer_state_t state) {
    return state;
}
