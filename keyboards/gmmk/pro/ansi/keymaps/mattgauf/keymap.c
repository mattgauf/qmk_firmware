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
#include "digitizer.h"


typedef union {
  uint32_t raw;
  struct {
    bool DEFAULT_STATE: 1;
    bool MEDIAKY_STATE: 1;
    bool MOUSEKY_STATE: 1;
    bool DYNAMIC_STATE: 1;
    bool EFFECTS_STATE: 1;
    bool DFUMODE_STATE: 1;
  };
} user_config_t;
user_config_t user_config;


enum device_codes {
    _HID_LAY_TOG = 0x00,
    _HID_RGB_SET = 0x01,
};


enum custom_codes {
    MG_F17 = SAFE_RANGE,
    MG_F18,
    MG_F19,
    MG_NAV,
    MG_MAC1,
    MG_MAC2
};


enum layer_names {
    _DEFAULT = 0,
    _MEDIAKY,
    _MOUSEKY,
    _DYNAMIC,
    _EFFECTS,
    _DFUMODE
};


#define MODS_SHIFT ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)
#define MODS_CTRL  ((get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL)
#define MODS_ALT   ((get_mods() | get_oneshot_mods()) & MOD_MASK_ALT)
#define MODS_GUI   ((get_mods() | get_oneshot_mods()) & MOD_MASK_GUI)


#define TOG_MED (TG(_MEDIAKY))
#define TOG_MOU (TG(_MOUSEKY))
#define TOG_DYN (TG(_DYNAMIC))
#define MOM_EFF (MO(_EFFECTS))
#define MOM_DYN (MO(_DYNAMIC))
#define MOM_DFU (MO(_DFUMODE))


static bool encoder_navigation = false;
digitizer_t digitizer = {
    .tipswitch = 0,
    .inrange = 0,
    .id = 0,
    .x = 0,
    .y = 0,
    .status = DZ_INITIALIZED
};


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
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      MG_MAC1,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),

  [_DYNAMIC] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, DM_PLY1, DM_PLY2, DM_REC1, DM_REC2, DM_RSTP,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,               _______,
                      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,      _______,
                      _______, _______, _______,                   _______,                   _______, _______, _______,          _______, _______, _______),

  [_EFFECTS] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MG_F18,       MG_NAV,
                      TOG_DYN, TOG_MED, TOG_MOU, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_DEL,       RGB_MOD,
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


#ifdef _______
#undef _______
#define _______ LEDTRNS

const uint8_t led_pinmap[] = {
  PIN_ESC,    PIN_F1,   PIN_F2,   PIN_F3,    PIN_F4,   PIN_F5, PIN_F6,    PIN_F7,   PIN_F8,    PIN_F9,    PIN_F10,    PIN_F11,      PIN_F12,      PIN_PRINT,
  PIN_GRAVE,  PIN_1,    PIN_2,    PIN_3,     PIN_4,    PIN_5,  PIN_6,     PIN_7,    PIN_8,     PIN_9,     PIN_0,      PIN_MINUS,    PIN_EQUAL,    PIN_BSPACE, PIN_DEL,
  PIN_TAB,    PIN_Q,    PIN_W,    PIN_E,     PIN_R,    PIN_T,  PIN_Y,     PIN_U,    PIN_I,     PIN_O,     PIN_P,      PIN_LBRACKET, PIN_RBRACKET, PIN_BSLASH, PIN_PGUP,
  PIN_CAPS,   PIN_A,    PIN_S,    PIN_D,     PIN_F,    PIN_G,  PIN_H,     PIN_J,    PIN_K,     PIN_L,     PIN_SCOLON, PIN_QUOTE,    PIN_ENTER,                PIN_PGDN,
  PIN_LSHIFT, PIN_Z,    PIN_X,    PIN_C,     PIN_V,    PIN_B,  PIN_N,     PIN_M,    PIN_COMMA, PIN_DOT,   PIN_SLASH,  PIN_RSHIFT,           PIN_UP,           PIN_END,
  PIN_LCTRL,  PIN_LGUI, PIN_LALT,                      PIN_SPACE,                   PIN_RALT,  PIN_FN,    PIN_RCTRL,              PIN_LEFT, PIN_DOWN,  PIN_RIGHT,
};


const uint16_t PROGMEM ledmap[][82][3] = {
    [_MEDIAKY] = {
        {_______}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {LEDORAN}, {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},                 {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},            {_______},      {_______},
        {_______}, {_______}, {_______},                       {_______},                       {_______}, {_______}, {_______},            {_______}, {_______}, {_______}
    },
    [_MOUSEKY] = {
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {LEDFOAM},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {LEDFOAM},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},                 {LEDFOAM},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},            {_______},      {LEDFOAM},
        {_______}, {_______}, {_______},                       {_______},                       {_______}, {_______}, {_______},            {_______}, {_______}, {_______}
    },
    [_DYNAMIC] = {
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {LEDGREE}, {LEDGREE}, {LED_RED}, {LED_RED}, {LED_MAX},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},                 {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},            {_______},      {_______},
        {_______}, {_______}, {_______},                       {_______},                       {_______}, {_______}, {_______},            {_______}, {_______}, {_______}
    },
    [_EFFECTS] = {
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {LEDGREE},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},                 {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},            {_______},      {_______},
        {_______}, {_______}, {_______},                       {_______},                       {_______}, {_______}, {_______},            {_______}, {_______}, {_______}
    },
    [_DFUMODE] = {
        {LED_RED}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {LEDORAN},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},      {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},                 {_______},
        {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______}, {_______},            {_______},      {_______},
        {_______}, {_______}, {_______},                       {_______},                       {_______}, {_______}, {_______},            {_______}, {_______}, {_______}
    },
};
#undef _______
#define _______ KC_TRNS
#endif


/**
 * Runs on EEPROM reset
**/
void eeconfig_init_user(void) {
    user_config.raw = 0;
    user_config.DEFAULT_STATE = true;
    user_config.MEDIAKY_STATE = false;
    user_config.MOUSEKY_STATE = false;
    user_config.DYNAMIC_STATE = false;
    user_config.EFFECTS_STATE = false;
    user_config.DFUMODE_STATE = false;

    eeconfig_update_user(user_config.raw); // Write default value to EEPROM
}


/**
 * Runs once on keyboard init
 * Options for debugging:
 *
 * debug_enable=true;
 * debug_mouse=true;
 * debug_keyboard=true;
 * debug_matrix=true;
**/
void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();

    if (user_config.MEDIAKY_STATE) {
        layer_on(_MEDIAKY);
    }
    if (user_config.MOUSEKY_STATE) {
        layer_on(_MOUSEKY);
    }

    dprint("-- Keyboard initialized\n");
}


// Called on start
void dynamic_macro_record_start_user(void) {
    // layer_on(_DYNAMIC);
    eeconfig_init_user();
}


// Called when layer state changes
layer_state_t layer_state_set_user(layer_state_t state) {
    user_config.MEDIAKY_STATE = IS_LAYER_ON_STATE(state, _MEDIAKY);
    user_config.MOUSEKY_STATE = IS_LAYER_ON_STATE(state, _MOUSEKY);
    eeconfig_update_user(user_config.raw);

    return state;
}


// Sets the layer color if it's enabled
void set_layer_color(int layer) {
    if (IS_LAYER_OFF(layer)) { return; }

    for (int ii = 0; ii < 82; ii++) {
        if (ledmap[layer][ii][0] == 0xffff || ledmap[layer][ii][1] == 0xffff || ledmap[layer][ii][2] == 0xffff) {
            continue;
        }

        rgb_matrix_set_color_keys(led_pinmap[ii], ledmap[layer][ii][0], ledmap[layer][ii][1], ledmap[layer][ii][2]);
    }
}


// Runs constantly in the background, in a loop.
void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    set_layer_color(_MEDIAKY);
    set_layer_color(_MOUSEKY);
    set_layer_color(_DYNAMIC);
    set_layer_color(_EFFECTS);

    switch (get_highest_layer(layer_state)) {
        case _MEDIAKY:
            break;
        case _MOUSEKY:
            break;
        case _DYNAMIC:
            break;
        case _EFFECTS:
            break;
        case _DFUMODE:
            rgb_matrix_set_color_both(LED_OFF);
            set_layer_color(_DFUMODE);
            break;
        default:
            break;
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color_keys(PIN_CAPS, LED_MAX);
    }

    // rgb_matrix_set_color_test();
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
        case MG_MAC1:
            if (record->event.pressed) {
                tap_code16(KC_MPLY);
                tap_code(KC_K);
            }
            return false;
        case MG_MAC2:
            if (!record->event.pressed) {
                // digitizer_send_update(0.25, 0.5, 1);
                // wait_ms(250);
                // digitizer_send_update(0.75, 0.5, 1);
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


// Checks if the digitizer needs updating and sends to host
void digitizer_send(void) {
    if (digitizer.status & DZ_UPDATED) {
        dprintf("DG: x: %x, y: %x, inrange: %x, status: %x\n", digitizer.x, digitizer.y, digitizer.inrange, digitizer.status);

        host_digitizer_send(&digitizer);
        digitizer.status &= ~DZ_UPDATED;
    }
}


//
void digitizer_send_finish(void) {
    digitizer_send();

    digitizer.tipswitch = 0;
    digitizer.inrange   = 0;
}


// Commit an update to the digitizer task
void digitizer_send_update(float x, float y, int8_t tipswitch) {
    digitizer.x         = x;
    digitizer.y         = y;
    digitizer.inrange   = 1;
    digitizer.tipswitch = tipswitch;
    digitizer.status   |= DZ_UPDATED;
    digitizer_send_finish();
}
// clang-format on
