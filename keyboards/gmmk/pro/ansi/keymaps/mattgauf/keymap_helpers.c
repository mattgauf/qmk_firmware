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

#include QMK_KEYBOARD_H      // IWYU pragma: keep
#include "keymap_helpers.h"
#include "rgb_matrix.h"
#include "digitizer.h"       // IWYU pragma: keep


// static uint8_t  rgb_pin_fn_column[]      = {PIN_DEL,PIN_PGUP,PIN_PGDN,PIN_END};
static uint8_t  rgb_pin_underglow_west[] = {PIN_LED_L01,PIN_LED_L02,PIN_LED_L03,PIN_LED_L04,PIN_LED_L05,PIN_LED_L06,PIN_LED_L07,PIN_LED_L08};
static uint8_t  rgb_pin_underglow_east[] = {PIN_LED_L11,PIN_LED_L12,PIN_LED_L13,PIN_LED_L14,PIN_LED_L15,PIN_LED_L16,PIN_LED_L17,PIN_LED_L18};

RGB rgb_matrix_get_scale_rgb(uint8_t hue, uint8_t sat, uint8_t val) {
    HSV hsv = {hue, sat, val};
    uint8_t bright = rgb_matrix_get_val();

    if (hsv.v > bright) {
        hsv.v = bright;
    }

    return hsv_to_rgb(hsv);
}

void rgb_matrix_set_color_keys(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val) {
    if (rgb_matrix_get_flags() & RGB_FLAG_KEYS) {
        RGB rgb = rgb_matrix_get_scale_rgb(hue, sat, val);
        rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
    }
}

void rgb_matrix_set_color_case(uint8_t hue, uint8_t sat, uint8_t val) {
    if (rgb_matrix_get_flags() & RGB_FLAG_CASE) {
        RGB rgb = rgb_matrix_get_scale_rgb(hue, sat, val);

        for (uint8_t ii = 0; ii < 8; ii++) {
            rgb_matrix_set_color(rgb_pin_underglow_west[ii], rgb.r, rgb.g, rgb.b);
            rgb_matrix_set_color(rgb_pin_underglow_east[ii], rgb.r, rgb.g, rgb.b);
        }
    }
}

void rgb_matrix_set_color_both(uint8_t hue, uint8_t sat, uint8_t val) {
    if (rgb_matrix_get_flags() & (RGB_FLAG_KEYS | RGB_FLAG_CASE)) {
        RGB rgb = rgb_matrix_get_scale_rgb(hue, sat, val);

        rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
    }
}

void rgb_matrix_set_color_test(void) {
    rgb_matrix_set_color_keys(PIN_ESC, LED_RED);
    rgb_matrix_set_color_keys(PIN_F1 , LEDORAN);
    rgb_matrix_set_color_keys(PIN_F2 , LEDGOLD);
    rgb_matrix_set_color_keys(PIN_F3 , LEDYELL);
    rgb_matrix_set_color_keys(PIN_F4 , LEDCHAR);
    rgb_matrix_set_color_keys(PIN_F5 , LEDGREE);
    rgb_matrix_set_color_keys(PIN_F6 , LEDFOAM);
    rgb_matrix_set_color_keys(PIN_F7 , LEDTEAL);
    rgb_matrix_set_color_keys(PIN_F8 , LEDAZUR);
    rgb_matrix_set_color_keys(PIN_F9 , LEDBLUE);
    rgb_matrix_set_color_keys(PIN_F10, LEDPURP);
    rgb_matrix_set_color_keys(PIN_F11, LEDMAGE);
    rgb_matrix_set_color_keys(PIN_F12, LEDPINK);
}

void encoder_action_user_volume(bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU);
    } else {
        tap_code(KC_VOLD);
    }
}

void encoder_action_user_history(bool clockwise) {
    if (clockwise) {
        tap_code16(LSG(KC_Z));
    } else {
        tap_code16(LGUI(KC_Z));
    }
}

void encoder_action_user_rgb_val(bool clockwise) {
    if (clockwise) {
        rgb_matrix_increase_val();
    } else {
        rgb_matrix_decrease_val();
    }
}

void encoder_action_user_navigate_tabs(bool clockwise) {
    if (clockwise) {
        tap_code16(LAG(KC_RIGHT));
    } else {
        tap_code16(LAG(KC_LEFT));
    }
}

void encoder_action_user_navigate_apps(bool clockwise) {
    if (clockwise) {
        tap_code16(KC_TAB);
    } else {
        tap_code16(LSFT(KC_TAB));
    }
}

void encoder_action_user_navigate(bool clockwise) {
    if (clockwise) {
        tap_code(KC_RIGHT);
    } else {
        tap_code(KC_LEFT);
    }
}
