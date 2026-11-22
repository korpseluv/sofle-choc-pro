// Copyright 2026 korpseluv
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Sofle Choc Pro (Keebart) - keymap ported from the Keebart Studio export.
// Key order follows LAYOUT_split_4x6_5 in keyboard.json:
//   each row = 6 left keys, then 6 right keys (inner -> outer),
//   bottom row has the two encoder pushes in the middle,
//   thumbs = 5 left (outer -> inner), then 5 right (inner -> outer).

#include QMK_KEYBOARD_H

enum layers {
    BASE,
    SYM,  // F-keys, numbers, shifted symbols, brackets
    NAV,  // arrows + cmd-z/x/c/v
    TRI,  // reached by holding SYM + NAV (currently empty, falls through)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_split_4x6_5(
        /* number row */
        KC_GRV, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TILD,
        /* top row */
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_PIPE,
        /* home row */
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
        /* bottom row (+ encoder pushes) */
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_MUTE, KC_MPLY, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_NO,
        /* thumbs (left 5, right 5) */
        KC_LCTL, KC_LALT, KC_LGUI, MO(SYM), KC_SPC, KC_ENT, MO(NAV), KC_BSPC, KC_HYPR, LSG(KC_SPC)
    ),

    [SYM] = LAYOUT_split_4x6_5(
        /* number row */
        _______, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,
        /* top row */
        _______, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_F12,
        /* home row */
        _______, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
        /* bottom row (+ encoder pushes) */
        KC_CAPS, _______, KC_EQL, KC_MINS, KC_NO /* UNKNOWN 0x80f7 */, _______, _______, _______, _______, KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, _______,
        /* thumbs (left 5, right 5) */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [NAV] = LAYOUT_split_4x6_5(
        /* number row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* top row */
        _______, _______, _______, _______, _______, _______, _______, _______, KC_UP, _______, _______, _______,
        /* home row */
        _______, _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
        /* bottom row (+ encoder pushes) */
        _______, G(KC_Z), G(KC_X), G(KC_C), G(KC_V), _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* thumbs (left 5, right 5) */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [TRI] = LAYOUT_split_4x6_5(
        /* number row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* top row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* home row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* bottom row (+ encoder pushes) */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* thumbs (left 5, right 5) */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_WH_D, KC_WH_U)},
    [SYM]  = {ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______)},
    [NAV]  = {ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______)},
    [TRI]  = {ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______)},
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYM, NAV, TRI);
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;  // portrait; use OLED_ROTATION_90 if it is upside down
}

bool oled_task_user(void) {
    oled_set_cursor(0, 0);
    oled_write_ln_P(PSTR("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"), false);
    oled_write_ln_P(is_keyboard_left() ? PSTR("LEFT") : PSTR("RIGHT"), false);
    return false;
}
#endif
