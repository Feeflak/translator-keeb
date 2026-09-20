#include <inttypes.h>
#include "print.h"
#include QMK_KEYBOARD_H
#include "pointing_device.h"
#include "transactions.h"
#include "split_util.h"
#include "drivers/sensors/pmw33xx_common.h"

enum _layers {
    _BASE,
    _FPS,
    _FUNC,
    _SYMB,
    _MS

};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BASE] = LAYOUT(
        LT(_SYMB, KC_ENT),     KC_Z,       KC_A,         KC_Q,
        LSFT_T(KC_F2),         KC_X,       KC_S,         KC_W,
        LGUI_T(KC_DEL),        KC_C,       KC_D,         KC_E,
                               KC_V,       LT(_MS,KC_F), KC_R,
                               KC_B,       RALT_T(KC_G), KC_T,

        KC_ESC,                KC_N,       KC_H,         KC_Y,
        LT(_FUNC, KC_SPC),     KC_M,       LCTL_T(KC_J), KC_U,
        LSFT_T(KC_BSPC),       KC_COMM,    KC_K,         KC_I,
                               KC_DOT,     KC_L,         KC_O,
                               KC_SLSH,    KC_SCLN,      KC_P
    ),
    [_FPS] = LAYOUT(
        KC_LSFT,               KC_Z,       KC_G,     _______,
        KC_LCTL,               KC_X,       KC_A ,        KC_Q ,
        XXXXXXX,               KC_C,       KC_S ,         KC_W ,
                               KC_V,       KC_D,         KC_E ,
                               KC_B,       KC_F,         KC_R ,

        KC_ESC,              KC_N,       KC_H,         KC_T,
        KC_SPC,                KC_M,       MS_BTN3,      KC_U,
        TG(_FPS),               KC_COMM,    MS_BTN1,      KC_I,
                               KC_DOT,     MS_BTN2,      KC_O,
                               KC_SLSH,    KC_SCLN,      KC_P
    ),

    [_FUNC] = LAYOUT(
        TG(_FPS),              KC_MNXT,    KC_MUTE,     XXXXXXX,
        _______,               KC_VOLD,    KC_VOLU,     XXXXXXX,
        _______,               KC_BRID,    KC_MPLY,     KC_PLUS,
                               KC_BRID,    KC_TAB,      KC_UNDS,
                               XXXXXXX,    XXXXXXX,     XXXXXXX,

        _______,               XXXXXXX,    KC_LEFT,     QK_BOOT,
        _______,               KC_NUHS,    KC_DOWN,     KC_MINS,
        _______,               XXXXXXX,    KC_UP,       KC_EQL,
                               XXXXXXX,    KC_RGHT,     XXXXXXX,
                               XXXXXXX,    XXXXXXX,     KC_PSCR
    ),

    [_SYMB] = LAYOUT(
        _______,               XXXXXXX,    KC_9,        QK_BOOT,
        _______,               XXXXXXX,    KC_0,        XXXXXXX,
        _______,               KC_LBRC,    KC_1,        KC_QUOT,
                               KC_RBRC,    KC_2,        KC_GRV,
                               XXXXXXX,    KC_3,        XXXXXXX,

        _______,               XXXXXXX,    KC_4,        XXXXXXX,
        _______,               KC_HASH,    KC_5,        KC_ASTR,
        _______,               KC_BSLS,    KC_6,        KC_PIPE,
                               XXXXXXX,    KC_7,        XXXXXXX,
                               XXXXXXX,    KC_8,        XXXXXXX
    ),

    [_MS] = LAYOUT(
        _______,               _______,    _______,     _______,
        _______,               _______,    _______,     _______,
        _______,               _______,    MS_BTN3,     _______,
                               _______,    _______,     _______,
                               _______,    _______,     _______,

        _______,               _______,    _______,     _______,
        _______,               _______,    _______,     _______,
        _______,               _______,    MS_BTN1,     _______,
                               _______,    MS_BTN2,     _______,
                               _______,    _______,     _______
    )

};

void keyboard_post_init_user(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;
}

void pointing_device_init_kb(void) {
    pointing_device_init_user();
}

void matrix_scan_user(void) {
    // COMBINED mode handles both sensors via pointing_device_task_combined_user.
}

#define SCROLL_DIVISOR_X 150
#define SCROLL_DIVISOR_Y 150
#define MOVE_SCALE_NUMERATOR 1
#define MOVE_SCALE_DENOMINATOR 1

static int16_t scroll_accum_x = 0;
static int16_t scroll_accum_y = 0;

// USB mouse reports are int8_t; clamp so extreme deltas don't wrap around.
static inline int8_t clamp_mouse_axis(int16_t value) {
    if (value > 127) return 127;
    if (value < -127) return -127;
    return (int8_t)value;
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    report_mouse_t report = {0};

    // Sensors only act as mouse/scroll while the mouse layer is active.
    if (!layer_state_is(_MS) && !layer_state_is(_FPS)) {
        return report;
    }

    // // Right sensor moves the cursor.
    // int16_t move_x = ((int16_t)right_report.x * MOVE_SCALE_NUMERATOR) / MOVE_SCALE_DENOMINATOR;
    // int16_t move_y = ((int16_t)right_report.y * MOVE_SCALE_NUMERATOR) / MOVE_SCALE_DENOMINATOR;
    // report.x = clamp_mouse_axis(move_x);
    // report.y = clamp_mouse_axis(move_y);

    report.x = right_report.x;
    report.y = right_report.y;


    // Left sensor scrolls horizontally/vertically.
    // Accumulate fractional movement so slow slides still register.
    scroll_accum_x += (int16_t)left_report.x;
    scroll_accum_y += (int16_t)left_report.y;

    int16_t scroll_h = scroll_accum_x / SCROLL_DIVISOR_X;
    int16_t scroll_v = -scroll_accum_y / SCROLL_DIVISOR_Y;
    scroll_accum_x %= SCROLL_DIVISOR_X;
    scroll_accum_y %= SCROLL_DIVISOR_Y;

    report.h = clamp_mouse_axis(scroll_h);
    report.v = clamp_mouse_axis(scroll_v);

    return report;
}
