/* Copyright 2021 Matt Gauf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// clang-format off
#include QMK_KEYBOARD_H
#include "keymap_helpers.h"
#include "raw_hid.h"


enum device_codes {
    _HID_LAY_TOG = 0x00,
    _HID_RGB_SET = 0x01,
};


enum custom_codes {
    MG_F17 = SAFE_RANGE,
    MG_F18,
    MG_F19,
    MG_NAV
};


enum layer_names {
    _DEFAULT = 0,
    _MEDIAKY,
    _MOUSEKY,
    _DYNAMIC,
    _EFFECTS,
    _DFUMODE
};


static bool encoder_navigation = false;


#define MODS_SHIFT ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)
#define MODS_CTRL  ((get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL)
#define MODS_ALT   ((get_mods() | get_oneshot_mods()) & MOD_MASK_ALT)
#define MODS_GUI   ((get_mods() | get_oneshot_mods()) & MOD_MASK_GUI)


#define TOG_MED (TG(_MEDIAKY))
#define TOG_KEY (TG(_MOUSEKY))
#define TOG_DYN (TG(_DYNAMIC))
#define MOM_EFF (MO(_EFFECTS))
#define MOM_DYN (MO(_DYNAMIC))
#define MOM_DFU (MO(_DFUMODE))


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_DEFAULT] = LAYOUT(KC_ESC,   KC_F1,   KC_F2,   KC_F3,  KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  MG_F17,       KC_MUTE,
                      KC_GRV,   KC_1,    KC_2,    KC_3,   KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,      KC_HOME,
                      KC_TAB,   KC_Q,    KC_W,    KC_E,   KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,      KC_PGUP,
                      KC_CAPS,  KC_A,    KC_S,    KC_D,   KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,                KC_PGDN,
                      KC_LSFT,  KC_Z,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          KC_UP,        KC_END,
                      KC_LCTL,  KC_LALT, KC_LGUI,                  KC_SPC,                    KC_RGUI, KC_RALT, MOM_EFF,          KC_LEFT, KC_DOWN, KC_RGHT),

  [_MEDIAKY] = LAYOUT(_______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  RGB_VAD, RGB_VAI, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      _______,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),

  [_MOUSEKY] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      KC_BTN1,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      KC_BTN2,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               KC_BTN3,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      XXXXXXX,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),

  [_DYNAMIC] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, DM_PLY1, DM_PLY2, DM_REC1, DM_REC2, DM_RSTP,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      _______,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),

  [_EFFECTS] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MG_F18,       MG_NAV,
                      _______, TOG_MED, TOG_KEY, TOG_DYN, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      RGB_MOD,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      RGB_RMOD,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               RGB_SPI,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_TOG, _______,          RGB_SAI,      RGB_SPD,
                      _______, _______, _______,                   _______,                   MOM_DFU, MOM_DYN, _______,          RGB_HUD, RGB_SAD, RGB_HUI),

  [_DFUMODE] = LAYOUT(RESET,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MG_F19,       DEBUG,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      _______,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),
};


// Runs constantly in the background, in a loop.
void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (IS_LAYER_ON(_MEDIAKY)) {
        rgb_matrix_set_color_keys(PIN_F1,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F2,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F3,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F4,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F5,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F6,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F7,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F8,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F9,  LEDORAN);
        rgb_matrix_set_color_keys(PIN_F10, LEDORAN);
        rgb_matrix_set_color_keys(PIN_F11, LEDORAN);
        rgb_matrix_set_color_keys(PIN_F12, LEDORAN);
    }

    if (IS_LAYER_ON(_MOUSEKY)) {
        rgb_matrix_set_color_keys(PIN_DEL,   LEDFOAM);
        rgb_matrix_set_color_keys(PIN_PGUP,  LEDFOAM);
        rgb_matrix_set_color_keys(PIN_PGDN,  LEDFOAM);
        rgb_matrix_set_color_keys(PIN_END,   LEDFOAM);
    }

    if (IS_LAYER_ON(_DYNAMIC)) {
        rgb_matrix_set_color_keys(PIN_F9,    LEDGREE);
        rgb_matrix_set_color_keys(PIN_F10,   LEDGREE);
        rgb_matrix_set_color_keys(PIN_F11,   LED_RED);
        rgb_matrix_set_color_keys(PIN_F12,   LED_RED);
        rgb_matrix_set_color_keys(PIN_PRINT, LED_MAX);
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color_keys(PIN_CAPS, LED_MAX);
    }

    switch (get_highest_layer(layer_state)) {
        case _MEDIAKY:
            break;
        case _MOUSEKY:
            break;
        case _DYNAMIC:
            break;
        case _EFFECTS:
            rgb_matrix_set_color_keys(PIN_PRINT, LEDGREE);
            break;
        case _DFUMODE:
            rgb_matrix_set_color_both(LED_OFF);
            rgb_matrix_set_color_keys(PIN_ESC,   LED_RED);
            rgb_matrix_set_color_keys(PIN_PRINT, LEDORAN);
            break;
        default:
            break;
    }
}


// Called on start
void dynamic_macro_record_start_user(void) {
    dprint("-- Recording Initiallized\n");
}


// Called on start
void dynamic_macro_record_begin_user(void) {
    dprint("-- Recording Started\n");
}


// Called on end
void dynamic_macro_record_end_user(int8_t direction) {
    dprint("-- Recording Ended\n");
}


// Called on playback
void dynamic_macro_play_user(int8_t direction) {
    dprint("-- Recording Playing\n");
}


// Runs once on keyboard init
void keyboard_post_init_user(void) {
    // debug_enable=true;
    // debug_matrix=true;
    // debug_keyboard=true;
    // debug_mouse=true;
}


// HID Interface
void raw_hid_receive(uint8_t* data, uint8_t length) {
    dprintf("Received USB data from host system (%d):\n", length);

    if (memcmp(data, "dfumode", length) == 0) {
        dprintf("%s\n", data);
        reset_keyboard();
    } else {
        for (uint8_t ii = 0; ii < length; ii++) {
            dprintf("%d: %02x\n", ii, data[ii]);
        }
    }

    switch (data[0]) {
        case _HID_LAY_TOG:
        case _HID_RGB_SET:
        default:
            break;
    }
}


// Runs on encoder event
bool encoder_update_user(uint8_t index, bool clockwise) {
    uint8_t modifier = get_mods();

    if (IS_LAYER_ON(_EFFECTS)) {
        encoder_action_user_rgb_val(clockwise);
    } else {
        if (MODS_GUI) {
            if (MODS_GUI && MODS_SHIFT) {
                clear_mods();
                encoder_action_user_history(clockwise);
                set_mods(modifier);
            } else if (MODS_GUI && MODS_ALT) {
                encoder_action_user_navigate_tabs(clockwise);
            } else {
                encoder_action_user_navigate_apps(clockwise);
            }
        } else {
            if (encoder_navigation) {
                encoder_action_user_navigate(clockwise);
            } else {
                encoder_action_user_volume(clockwise);
            }
        }
    }
    return true;
}


// Runs on key event
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // dprintf("##: %d, %d\n", record->event.key.col, record->event.key.row);

    switch (keycode) {
        case MG_F17:
            if (record->event.pressed) {
                tap_code16(HYPR(KC_F17));
            }
            return false;
        case MG_F18:
            if (record->event.pressed) {
                tap_code16(HYPR(KC_F18));
            }
            return false;
        case MG_F19:
            if (record->event.pressed) {
                tap_code16(HYPR(KC_F19));
            }
            return false;
        case MG_NAV:
            if (record->event.pressed) {
                encoder_navigation = !encoder_navigation;
            }
            return false;
        case RGB_TOG:
            if (record->event.pressed) {
                switch (rgb_matrix_get_flags()) {
                    case RGB_FLAG_ALL:
                        rgb_matrix_set_flags(RGB_FLAG_KEYS);
                        break;
                    case RGB_FLAG_KEYS:
                        rgb_matrix_set_flags(RGB_FLAG_CASE);
                        break;
                    case RGB_FLAG_CASE:
                        rgb_matrix_set_flags(RGB_FLAG_NONE);
                        break;
                    case RGB_FLAG_NONE:
                        rgb_matrix_set_flags(RGB_FLAG_ALL);
                        break;
                    default:
                        rgb_matrix_set_flags(RGB_FLAG_ALL);
                        break;
                }
            }
            return false;
        default:
            return true; // Process all other keycodes normally
    }
}
// clang-format on
