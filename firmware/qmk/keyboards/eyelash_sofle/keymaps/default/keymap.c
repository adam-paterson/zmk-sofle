// firmware/qmk/keyboards/eyelash_sofle/keymaps/default/keymap.c
// QMK keymap for eyelash_sofle
// Generated from lib/keymaps/base/qwerty.dtsi
// TODO: Implement full keymap conversion from shared definitions

#include QMK_KEYBOARD_H

// Layer definitions
enum layers {
    BASE,
    SYMS,
    NAV,
    UTIL,
    CODE
};

// Keymap definition - placeholder for QMK support
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        // Left hand
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
        XXXXXXX, KC_ESC,  KC_TAB,  KC_SPC,  KC_ENT,
        // Right hand
        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        XXXXXXX, KC_BSPC, KC_DEL,  KC_RSFT, KC_RALT
    )
};

// TODO: Implement layer switching, combos, and full feature parity with ZMK
