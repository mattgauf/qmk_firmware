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
