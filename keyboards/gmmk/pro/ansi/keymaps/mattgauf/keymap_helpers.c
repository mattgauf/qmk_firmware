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

#include QMK_KEYBOARD_H
#include "keymap_helpers.h"
#include "rgb_matrix.h"


// static uint8_t  rgb_pin_fn_column[]      = {PIN_DEL,PIN_PGUP,PIN_PGDN,PIN_END};
static uint8_t  rgb_pin_underglow_west[] = {PIN_LED_L01,PIN_LED_L02,PIN_LED_L03,PIN_LED_L04,PIN_LED_L05,PIN_LED_L06,PIN_LED_L07,PIN_LED_L08};
static uint8_t  rgb_pin_underglow_east[] = {PIN_LED_L11,PIN_LED_L12,PIN_LED_L13,PIN_LED_L14,PIN_LED_L15,PIN_LED_L16,PIN_LED_L17,PIN_LED_L18};


void rgb_matrix_set_color_keys(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    if (rgb_matrix_get_flags() & RGB_FLAG_KEYS) {
        rgb_matrix_set_color(index, red, green, blue);
    }
}

void rgb_matrix_set_color_case(uint8_t red, uint8_t green, uint8_t blue) {
    if (rgb_matrix_get_flags() & RGB_FLAG_CASE) {
        for (uint8_t ii = 0; ii < 8; ii++) {
            rgb_matrix_set_color(rgb_pin_underglow_west[ii], red, green, blue);
            rgb_matrix_set_color(rgb_pin_underglow_east[ii], red, green, blue);
        }
    }
}

void rgb_matrix_set_color_both(uint8_t red, uint8_t green, uint8_t blue) {
    if (rgb_matrix_get_flags() & (RGB_FLAG_KEYS | RGB_FLAG_CASE)) {
        rgb_matrix_set_color_all(red, green, blue);
    }
}

void rgb_matrix_set_color_test(void) {
    rgb_matrix_set_color_keys(PIN_GRAVE, LED_RED);
    rgb_matrix_set_color_keys(PIN_1,     LEDORAN);
    rgb_matrix_set_color_keys(PIN_2,     LEDGOLD);
    rgb_matrix_set_color_keys(PIN_3,     LEDYELL);
    rgb_matrix_set_color_keys(PIN_4,     LEDCHAR);
    rgb_matrix_set_color_keys(PIN_5,     LEDGREE);
    rgb_matrix_set_color_keys(PIN_6,     LEDFOAM);
    rgb_matrix_set_color_keys(PIN_7,     LEDTEAL);
    rgb_matrix_set_color_keys(PIN_8,     LEDAZUR);
    rgb_matrix_set_color_keys(PIN_9,     LEDBLUE);
    rgb_matrix_set_color_keys(PIN_0,     LEDPURP);
    rgb_matrix_set_color_keys(PIN_MINUS, LEDMAGE);
    rgb_matrix_set_color_keys(PIN_EQUAL, LEDPINK);
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
