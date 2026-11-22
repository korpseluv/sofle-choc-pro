#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE
#include "bitmaps.h"
#endif



enum layers {
    BASE,
    SYM,
    NAV,
    TRI
};

#define _______ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_split_4x6_5(
        /* number row */
        KC_GRV, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_GRV,
        /* top row */
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, S(KC_BSLS),
        /* home row */
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
        /* bottom row (+ encoder pushes) */
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_MUTE, KC_MPLY, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, _______,
        /* thumbs (left 5 outer->inner, right 5 inner->outer) */
        KC_LCTL, KC_LALT, KC_LGUI, MO(SYM), KC_SPC, KC_ENT, MO(NAV), KC_BSPC, OSM(MOD_HYPR), SGUI(KC_SPC)
    ),

    [SYM] = LAYOUT_split_4x6_5(
        /* number row */
        _______, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,
        /* top row */
        KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_F12,
        /* home row */
        _______, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0), S(KC_BSLS),
        /* bottom row (+ encoder pushes) */
        _______, KC_EQL, KC_MINS, S(KC_EQL), S(KC_LBRC), S(KC_RBRC), _______, _______, KC_LBRC, KC_RBRC, KC_SCLN, S(KC_SCLN), KC_BSLS, _______,
        /* thumbs */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [NAV] = LAYOUT_split_4x6_5(
        /* number row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* top row */
        _______, _______, _______, _______, _______, _______, _______, _______, KC_UP, _______, _______, _______,
        /* home row */
        _______, _______, _______, LGUI(KC_P), SGUI(KC_P), KC_CAPS, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
        /* bottom row (+ encoder pushes) */
        _______, LGUI(KC_Z), LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* thumbs */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    [TRI] = LAYOUT_split_4x6_5(
        /* number row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* top row */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* home row */
        _______, _______, _______, _______, _______, _______, _______, LCTL(KC_LEFT), _______, LCTL(KC_RGHT), _______, _______,
        /* bottom row (+ encoder pushes) */
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        /* thumbs */
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

#include "transactions.h"

typedef struct {
    bool oled_on;
} oled_state_m2s_t;

static bool g_oled_init_done = false;
static uint32_t g_user_ontime = 0;

static oled_state_m2s_t g_remote_oled_state = { false };

static inline pin_t get_charge_pump_enable_pin(void) {
    return GP20;
}

void print_current_layer(uint8_t row) {
    char buf[16];

    snprintf(buf, sizeof(buf), "layer: %u",
             (unsigned)get_highest_layer(layer_state));

    oled_set_cursor(0, row);
    oled_write(buf, false);
}

void print_wpm(uint8_t row) {
    char buf[16];

    snprintf(buf, sizeof(buf), "wpm: %u",
             (unsigned)get_current_wpm());

    oled_set_cursor(0, row);
    oled_write(buf, false);
}

static void user_sync_oled_state_slave(uint8_t in_len, const void *in_data,
                                       uint8_t out_len, void *out_data) {
    if (in_len >= sizeof(oled_state_m2s_t)) {
        memcpy(&g_remote_oled_state, in_data, sizeof(oled_state_m2s_t));
    }
}

void keyboard_post_init_user(void) {
    pin_t dsp_pen_pin = get_charge_pump_enable_pin();

    gpio_set_pin_output(dsp_pen_pin);
    gpio_write_pin_low(dsp_pen_pin);
    wait_ms(5);

    transaction_register_rpc(
        USER_SYNC_OLED_STATE,
        user_sync_oled_state_slave
    );

    if (!is_keyboard_master()) {
        wait_ms(90);
    }
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        static uint32_t last_sync = 0;

        if (timer_elapsed32(last_sync) > 500) {
            oled_state_m2s_t oled_state_pkt = { is_oled_on() };

            (void)transaction_rpc_send(
                USER_SYNC_OLED_STATE,
                sizeof(oled_state_pkt),
                &oled_state_pkt
            );

            last_sync = timer_read32();
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    g_user_ontime = timer_read32();
    return true;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

bool oled_post_init(void) {
    if (!g_oled_init_done) {
        pin_t dsp_pen_pin = get_charge_pump_enable_pin();

        gpio_write_pin_high(dsp_pen_pin);
        wait_ms(20);

        oled_clear();

        g_user_ontime = timer_read32();
        g_oled_init_done = true;
    }

    return false;
}

bool oled_task_user(void) {
    oled_post_init();

    // Manage OLED power and synchronize both halves.
    if (is_keyboard_master()) {
        const uint32_t idle_time = timer_elapsed32(g_user_ontime);

        if (!is_oled_on()) {
            if (idle_time > OLED_TIMEOUT_USER) {
                return false;
            }

            oled_on();

            oled_state_m2s_t oled_state_pkt = { true };

            (void)transaction_rpc_send(
                USER_SYNC_OLED_STATE,
                sizeof(oled_state_pkt),
                &oled_state_pkt
            );
        } else if (idle_time > OLED_TIMEOUT_USER) {
            oled_off();

            oled_state_m2s_t oled_state_pkt = { false };

            (void)transaction_rpc_send(
                USER_SYNC_OLED_STATE,
                sizeof(oled_state_pkt),
                &oled_state_pkt
            );

            return false;
        }
    } else {
        if (g_remote_oled_state.oled_on) {
            if (!is_oled_on()) {
                oled_on();
            }
        } else {
            if (is_oled_on()) {
                oled_off();
            }

            return false;
        }
    }

    // Left OLED: lowercase labels, numeric layer, live WPM.
    if (is_keyboard_left()) {
        oled_clear();

        print_current_layer(0);
        print_wpm(2);
    } else {
        // Right OLED: intentionally blank.
        oled_clear();
    }

    return false;
}

#endif // OLED_ENABLE
