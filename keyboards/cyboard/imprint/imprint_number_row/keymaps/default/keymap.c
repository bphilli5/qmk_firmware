/* Copyright 2023 Cyboard LLC (@Cyboard-DigitalTailor)
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// clang-format off
#include <stdbool.h>
#include <stdint.h>
#include "caps_word.h"
#include "color.h"
#include "config.h"
#include "info_config.h"
#include "keyboard.h"
#include "keycodes.h"
#include "keymap_us.h"
#include "modifiers.h"
#include "pointing_device.h"
#include "process_caps_word.h"
#include "process_tap_dance.h"
#include "quantum.h"
#include "quantum_keycodes.h"
#include "timer.h"
#include QMK_KEYBOARD_H
#include <cyboard.h>
#include "repeat_key.h"
#include "process_key_override.h"  // <- Required for key_override_t
#include "print.h"  // <- Required for debug_print

#ifdef COMBO_ENABLE
#define COMBO_COUNT 2  // Adjust this number based on how many combos you define
#endif

// Suppress IntelliSense warnings for LAYOUT macros
#ifdef __INTELLISENSE__
#pragma diag_suppress 59
#pragma diag_suppress 20
#endif

// Layer definitions
enum layer_names {
    _BASE = 0,
    _SYM = 1,
    _NAV = 2,
    _NUM = 3,
    _FUNC = 4,
    _MOUSE = 5,
    _CTRL = 6,
    _EMPTY7 = 7,
    _EMPTY8 = 8,
    _GAME = 9
};

// LED states for RGB feedback
enum led_states {
    LAYER_BASE,
    LAYER_SYM,
    LAYER_NAV,
    LAYER_NUM,
    LAYER_FUNC,
    LAYER_MOUSE,
    LAYER_CTRL,
    LAYER_EMPTY7,
    LAYER_EMPTY8,
    LAYER_GAME,
    ACTION_CAPS_WORD
};

enum custom_keycodes {

    LMAGIC = SAFE_RANGE,  // Left thumb - for SFB removal
    RMAGIC,  // Right thumb - for word completion

    // Q -> Qu
    M_QU,  // Magic key for "qu"

    // Macros to handle h/v switcher
    M_HV,
    M_VH

};


// Home Row Modifiers
// Right Hand Side
#define HRM_N LALT_T(KC_N)  // Home Row Modifier for N
#define HRM_S LGUI_T(KC_S)  // Home Row Modifier for S
#define HRM_H LSFT_T(KC_H)  // Home Row Modifier for H
#define HRM_T LT(_NAV, KC_T)   // Home Row Modifier for T
#define HRM_R LT(_NAV, KC_R) // Home Row Modifier for R
// Right Non Home Row Modifiers
#define HRM_D LCTL_T(KC_D)  // Modifier for D
#define HRM_J LT(_SYM, KC_J)   // Modifier for J


// Left Hand Side
#define HRM_C LT(_NUM, KC_C) // Home Row Modifier for C
#define HRM_A RSFT_T(KC_A) // Home Row Modifier for A
#define HRM_E RGUI_T(KC_E) // Home Row Modifier for E
#define HRM_I RALT_T(KC_I) // Home Row Modifier for I
#define HRM_SPC LT(_CTRL, KC_SPC) // Home Row Modifier for Space
// Left Non Home Row Modifiers)
#define HRM_W RCTL_T(KC_W) // Modifier for W
#define HRM_SCLN LT(_SYM, KC_SCLN) // Modifier for SCLN
#define HRM_COMM KC_COMM //  Row Modifier for COMM
#define HRM_DEL LT(_FUNC, KC_DEL) // Modifier for DEL
#define HRM_MOUSE LT(_MOUSE, KC_BTN1) // Modifier for Mouse Button 1


// Command shorthands
#define CUT LCTL(KC_X) // Cut command
#define COPY LCTL(KC_C) // Copy command
#define PASTE LCTL(KC_V) // Paste command
#define UNDO LCTL(KC_Z) // Undo command
#define CTL_A LCTL(KC_A) // Control + A command
#define ALTTAB LALT(KC_TAB) // Alt + Tab command
#define GUI_TAB LGUI(KC_TAB) // GUI + Tab command
#define FIND LCTL(KC_F) // Find command
#define OS_LSFT OSM(MOD_LSFT) // OS modifier for Left Shift
#define OS_RSFT OSM(MOD_RSFT) // OS modifier for Right Shift
#define WINSWITCH LGUI(LSFT(KC_RGHT)) // Windows Switch command

// Define the HSV values for the LED colors
// Function to set LED colors based on state
void set_led_colors(enum led_states led_state) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);

    switch (led_state) {
        case LAYER_BASE:
            rgb_matrix_sethsv(HSV_PURPLE);  // Purple for base
            return;
        case LAYER_SYM:
            rgb_matrix_sethsv(HSV_MAGENTA); // Magenta for symbols
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_NAV:
            rgb_matrix_sethsv(HSV_CORAL);   // Coral for navigation
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_NUM:
            rgb_matrix_sethsv(HSV_ORANGE);  // Orange for numbers
            //  rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_FUNC:
            rgb_matrix_sethsv(HSV_CYAN);    // Cyan for function keys
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_MOUSE:
            rgb_matrix_sethsv(HSV_GREEN);   // Green for mouse
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_CTRL:
            rgb_matrix_sethsv(HSV_MAGENTA);   // Magenta for control layer
        case LAYER_EMPTY7:
            rgb_matrix_sethsv(HSV_BLACK);
        case LAYER_EMPTY8:
            rgb_matrix_sethsv(HSV_BLACK);
        case LAYER_GAME:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_RAINBOW_BEACON);
            // rgb_matrix_sethsv(HSV_WHITE); // White for game layer
            return;
        case ACTION_CAPS_WORD:
            rgb_matrix_sethsv(HSV_BLUE);
            return;
        default:
            rgb_matrix_sethsv(HSV_PURPLE);
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
    }
}
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0 - Base layer
    [_BASE] = LAYOUT_num(
        RGB_TOG,  CUT,      PASTE,      COPY,       CTL_A,   UNDO,                      KC_CALC,  KC_WSCH,    KC_WBAK,    KC_WFWD,    KC_WREF,    TO(_GAME),
        KC_TAB,   KC_B,     KC_F,       KC_L,       KC_M,    M_VH,                      KC_SLSH,  KC_G,       KC_O,       KC_U,       KC_DOT,     KC_BSLS,
        KC_CAPS,  HRM_N,    HRM_S,      HRM_H,      HRM_T,   KC_K,                      KC_Y,     HRM_C,      HRM_A,      HRM_E,      HRM_I,      KC_DEL,
        OS_LSFT,  KC_X,     HRM_J,      KC_M,       HRM_D,   M_QU,                      KC_P,     HRM_W,      KC_QUOT,    HRM_SCLN,   HRM_COMM,   OS_RSFT,
                            ALTTAB,     GUI_TAB,    KC_ESC,  KC_NO,   KC_ESC, KC_BTN1,  HRM_DEL,  KC_BTN2,    KC_WBAK,    KC_WFWD,
                                                    LMAGIC,  HRM_R,   KC_ENT, KC_BSPC,  HRM_SPC,  RMAGIC
    ),

    // Layer 1 - Symbols
    [_SYM] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,      KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                        KC_TRNS,    KC_TRNS,       KC_TRNS,       KC_TRNS,    KC_TRNS,  KC_TRNS,
        KC_TRNS,  KC_GRV,       KC_EQL,         KC_BSLS,        KC_MINS,        KC_BSLS,                        LSFT(KC_6), LSFT(KC_LBRC), LSFT(KC_RBRC), LSFT(KC_4), KC_ENT,   KC_TRNS,
        KC_TRNS,  S(KC_1),      S(KC_8),        KC_NO,          KC_EQL,         KC_TRNS,                        LSFT(KC_3), LSFT(KC_9),    LSFT(KC_0),    KC_TRNS,    KC_TRNS,  KC_TRNS,
        KC_TRNS,  S(KC_GRV),    S(KC_EQL),      KC_LBRC,        KC_UNDS,        KC_TRNS,                        LSFT(KC_2), KC_LBRC,       KC_RBRC,       KC_TRNS,    KC_TRNS,  KC_TRNS,
                                KC_TRNS,        KC_TRNS,        KC_TRNS,        QK_LLCK,    KC_TRNS,   KC_TRNS, QK_LLCK,    KC_TRNS,       KC_TRNS,       KC_TRNS,
                                                                KC_TRNS,        KC_TRNS,    KC_TRNS,   KC_TRNS, KC_TRNS,    KC_TRNS
    ),

    // Layer 2 - Navigation
    [_NAV] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_PGUP,  KC_HOME,    KC_UP,      KC_END,     LCTL(KC_F), KC_TRNS,
        KC_TRNS,  KC_LALT,    KC_TRNS,    KC_LSFT,    KC_LCTL,    KC_TRNS,                       KC_PGDN,  KC_LEFT,    KC_DOWN,    KC_RGHT,    KC_DEL,     KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_PGUP,    KC_PGDN,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    QK_LLCK,    KC_TRNS, KC_TRNS,  QK_LLCK,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS
    ),

    // Layer 3 - Numbers
    [_NUM] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_SLSH,    KC_7,       KC_8,       KC_9,       KC_PAST,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_MINS,    KC_1,       KC_2,       KC_3,       KC_PPLS,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_X,       KC_4,       KC_5,       KC_6,       LSFT(KC_5),                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    QK_LLCK,   KC_TRNS,    KC_TRNS,   QK_LLCK,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_0,      KC_TRNS,    KC_TRNS,   KC_TRNS,  KC_TRNS
    ),

    // Layer 4 - Function keys
    [_FUNC] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_F10,     KC_F11,     KC_F12,     KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    QK_BOOT,
        KC_TRNS,  KC_TRNS,    KC_F7,      KC_F8,      KC_F9,      KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  RGB_TOG,    KC_F4,      KC_F5,      KC_F6,      KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_F1,      KC_F2,      KC_F3,      KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    QK_LLCK, KC_TRNS, KC_TRNS, QK_LLCK,  KC_TRNS,   KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS
    ),

    // Layer 5 - Mouse
    [_MOUSE] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_BTN1,    KC_MS_U,    KC_BTN2,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_MS_L,    KC_MS_D,    KC_MS_R,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    QK_LLCK,    KC_TRNS, KC_TRNS,  QK_LLCK,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,KC_TRNS,   KC_TRNS
    ),

    // Layer 6 - Control
    [_CTRL] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,                      KC_CALC,  KC_WSCH,    KC_WBAK,    KC_WFWD,    KC_WREF,    TO(_GAME),
        C(KC_B),  C(KC_P),  C(KC_F),    C(KC_L),    C(KC_A), C(KC_Y),                      KC_P,     KC_G,       KC_O,       KC_U,       KC_DOT,     KC_BSLS,
        C(KC_I),  C(KC_N),  C(KC_S),    C(KC_H),    C(KC_T), C(KC_K),                      KC_Y,     HRM_C,      HRM_A,      HRM_E,      HRM_I,      KC_DEL,
        C(KC_U),  C(KC_X),  C(KC_V),    C(KC_C),    C(KC_D), C(KC_Z),                      KC_SLSH,  HRM_W,      KC_QUOT,    HRM_SCLN,   HRM_COMM,   OS_RSFT,
                            KC_TRNS,     KC_TRNS,    KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_BTN2,    KC_WBAK,    KC_WFWD,
                                                     KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,   RMAGIC
    ),

    [_EMPTY7] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS
    ),

    [_EMPTY8] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS
    ),

    // Layer 9 - GAME layer
    [_GAME] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,  KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    TO(0),
        KC_T,     KC_LCTL,    KC_Q,       KC_W,       KC_E,     KC_R,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,     KC_TRNS,
        KC_G,     KC_LSFT,    KC_A,       KC_S,       KC_D,     KC_F,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,     KC_TRNS,
        KC_B,     KC_TAB,     KC_Z,       KC_X,       KC_C,     KC_V,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,     KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_LSFT,  KC_SPC,  KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS
    )
};

// Combos
const uint16_t PROGMEM TH_TAB[] = {HRM_T, HRM_H, COMBO_END};
const uint16_t PROGMEM HA_CW[] = {HRM_H, HRM_A, COMBO_END};
const uint16_t PROGMEM THS_WSW[] = {HRM_T, HRM_H, HRM_S, COMBO_END};
const uint16_t PROGMEM NS_Z[] = {HRM_N, HRM_S, COMBO_END};


combo_t key_combos[] = {
    COMBO(TH_TAB, KC_TAB),
    COMBO(HA_CW,  CW_TOGG),
    COMBO(THS_WSW, WINSWITCH),
    COMBO(NS_Z,   KC_Z)
};



void pointing_device_init_user(void) {
    charybdis_set_pointer_dragscroll_enabled(true, true);
}

void oneshot_mods_changed_user(uint8_t mods) {
    if (mods & MOD_MASK_SHIFT) {
        set_led_colors(ACTION_CAPS_WORD);
    } else {
        set_led_colors(get_highest_layer(layer_state));
    }
}

void caps_word_set_user(bool active) {
    if (active) {
        set_led_colors(ACTION_CAPS_WORD);
    } else {
        set_led_colors(get_highest_layer(layer_state));
    }
}

bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
	switch (keycode) {
        case HRM_H:
		case CW_TOGG:
        case KC_ESC:
        case KC_BSPC:
        case KC_DEL:

        case LMAGIC:
        case RMAGIC:
        case M_VH:
        case M_HV:
            return false;  // Magic keys will ignore the above keycodes.
    }
    return true;  // Other keys can be repeated.
}

// An enhanced version of SEND_STRING: if Caps Word is active, the Shift key is
// held while sending the string. Additionally, the last key is set such that if
// the Repeat Key is pressed next, it produces `repeat_keycode`.
#define MAGIC_STRING(str, repeat_keycode) \
		magic_send_string_P(PSTR(str), (repeat_keycode))

static void magic_send_string_P(const char* str, uint16_t repeat_keycode) {
	uint8_t saved_mods = 0;

  if (is_caps_word_on()) { // If Caps Word is on, save the mods and hold Shift.
    saved_mods = get_mods();
    register_mods(MOD_BIT(KC_LSFT));
  }

  send_string_with_delay_P(str, TAP_CODE_DELAY);  // Send the string.
  set_last_keycode(repeat_keycode); // 2024-03-09 Disabled sending of string for mag-rep / rep-mag consistency.

  // If Caps Word is on, restore the mods.
  if (is_caps_word_on()) {
    set_mods(saved_mods);
  }
}

static void process_right_magic(uint16_t keycode, uint8_t mods) { // RMAGIC definitions
    switch (keycode) {
		case HRM_A: { MAGIC_STRING("ll ", 		KC_SPC); } break;
	    case  KC_B: { MAGIC_STRING("ecause ",	KC_NO); } break;
	    case HRM_C: { MAGIC_STRING("opy ",		KC_NO); } break;
	    case HRM_D: { MAGIC_STRING("eath ", 		KC_NO); } break;
		case HRM_E: { MAGIC_STRING("very ",			KC_NO); } break;
	    case  KC_F: { MAGIC_STRING("amily ", 		KC_NO); } break;
	    case  KC_G: {
            tap_code(KC_BSPC);
            MAGIC_STRING("GiveWell ", 	KC_NO); } break;
		case  KC_H: { MAGIC_STRING("ouse ", 		KC_NO); } break;
		case HRM_I: { MAGIC_STRING("ng ", 		KC_NO); } break;
	    case  KC_J: { MAGIC_STRING("ust",		KC_NO); } break;
	    case  KC_K: { MAGIC_STRING("now ", 		KC_NO); } break;
	    case  KC_L: { MAGIC_STRING("ove ", 		KC_NO); } break;
	    case  KC_M: { MAGIC_STRING("ent ",		KC_NO); } break;
	    case HRM_N: { MAGIC_STRING("ever ",		KC_NO); } break;
		case  KC_O: { MAGIC_STRING("rder ", 		KC_NO); } break;
	    case  KC_P: { MAGIC_STRING("lease ",    	KC_NO); } break;
		case  M_QU: { MAGIC_STRING("estion ", 		KC_NO); } break;
	    case  KC_R: { MAGIC_STRING("the", 		KC_NO); } break;
	    case HRM_S: { MAGIC_STRING("ome ", 		KC_NO); } break;
        case HRM_T: { MAGIC_STRING("hough ", 		KC_NO); } break;
		case  KC_U: { MAGIC_STRING("nder ", 		KC_NO); } break;
	    case  KC_V: { MAGIC_STRING("ery",	        KC_NO); } break;
	    case HRM_W: { MAGIC_STRING("hich ",		KC_NO); } break;
		case  KC_X: {
            tap_code(KC_BSPC);
            MAGIC_STRING("exactly ", KC_NO); } break;
		case  KC_Y: { MAGIC_STRING("ou ", 		KC_NO); } break;
	    case  KC_Z: { MAGIC_STRING("ation ", 		KC_NO); } break;
        case  KC_SPC: { MAGIC_STRING("the ", 		KC_NO); } break;
        case HRM_SPC: { MAGIC_STRING("the ", 		KC_NO); } break;
		case HRM_COMM: { MAGIC_STRING(" and ",    KC_NO); } break;
    }
}

static void process_left_magic(uint16_t keycode, uint8_t mods) { // LMAGIC definitions
    switch (keycode) {
	    case HRM_A: { MAGIC_STRING("nd ",     	KC_NO); } break;
	    case  KC_B: { MAGIC_STRING("s", 		KC_NO); } break;
	    case HRM_C: { MAGIC_STRING("y", 		KC_NO); } break;
	    case HRM_D: { MAGIC_STRING("d", 		KC_NO); } break;
	    case HRM_E: { MAGIC_STRING("e", 		KC_NO); } break;
	    case  KC_F: { MAGIC_STRING("f", 		KC_NO); } break;
	    case  KC_G: { MAGIC_STRING("y", 		KC_NO); } break;
	    case  KC_H: {
            tap_code(KC_BSPC);
            MAGIC_STRING("v", 	KC_NO); } break;
	    case HRM_I: { MAGIC_STRING("on ",    	KC_NO); } break;
	    case  KC_J: { MAGIC_STRING("oke ", 		KC_NO); } break;
	    case  KC_K: { MAGIC_STRING("ind ", 		KC_NO); } break;
	    case  KC_L: { MAGIC_STRING("l", 		KC_NO); } break;
	    case  KC_M: { MAGIC_STRING("ing ", 		KC_NO); } break;
	    case HRM_N: { MAGIC_STRING("n", 		KC_NO); } break;
	    case  KC_O: { MAGIC_STRING("a", 		KC_NO); } break;
	    case  KC_P: { MAGIC_STRING("a", 		KC_NO); } break;
	    case  M_QU: {
            tap_code(KC_BSPC);
            tap_code(KC_BSPC);
            MAGIC_STRING("QMK ", KC_NO); } break;
	    case  KC_R: { MAGIC_STRING("r ", 		KC_NO); } break;
	    case HRM_S: { MAGIC_STRING("s", 		KC_NO); } break;
	    case HRM_T: { MAGIC_STRING("t", 		KC_NO); } break;
	    case  KC_U: { MAGIC_STRING("e", 		KC_NO); } break;
	    case  KC_V: {
            tap_code(KC_BSPC);
            MAGIC_STRING("h", 		KC_NO); } break;
	    case HRM_W: { MAGIC_STRING("ould ", 	KC_NO); } break;
	    case  KC_X: {
            tap_code(KC_BSPC);
            MAGIC_STRING("expect ", KC_NO); } break;
	    case  KC_Y: { MAGIC_STRING("o",    	KC_NO); } break;
	    case  KC_Z: { MAGIC_STRING("z", 		KC_NO); } break;

	    case HRM_COMM: { MAGIC_STRING(" but",    KC_NO); } break;
		case KC_SPC: { MAGIC_STRING(" the", 	KC_NO); } break;
    }
}

/* ───────── h / v macros ───────── */

static inline bool is_prev_key_vowel(uint16_t kc) {
    switch (kc) {
        case KC_A: case HRM_A:
        case KC_E: case HRM_E:
        case KC_I: case HRM_I:
        case KC_O:
        case KC_U:
            return true;
    }
    return false;
}

static void process_hv_macro(bool reverse, uint16_t last_keycode) {
    uint8_t mods = get_mods();
    bool has_mods = mods & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT) | MOD_BIT(KC_LGUI) |
                           MOD_BIT(KC_RCTL) | MOD_BIT(KC_RALT) | MOD_BIT(KC_RGUI));
    bool caps_word = is_caps_word_on();

    uint16_t out;

    if (has_mods) {
        // With non-shift modifiers: M_HV always outputs H, M_VH always outputs V
        out = reverse ? KC_V : KC_H;
    } else {
        // Normal behavior: check previous key
        bool vowel = is_prev_key_vowel(last_keycode);
        /*  regular   :  vowel ⇒ V , else ⇒ H
            'reverse' :  vowel ⇒ H , else ⇒ V   */
        out = (vowel ^ reverse) ? KC_V : KC_H;
    }

    // Apply shift if Caps Word is active
    if (caps_word) {
        tap_code16(S(out));
    } else {
        tap_code(out);
    }

    set_last_keycode(out);
}


bool caps_word_press_user(uint16_t keycode) {
  switch (keycode) {
    case KC_A ... KC_Z:
      add_weak_mods(MOD_BIT_LSHIFT);
      return true;
    case KC_1 ... KC_0:
    case KC_BSPC:
    case KC_DEL:
    case KC_UNDS:
    case KC_COLN:
    case HRM_J:
    case HRM_SCLN:
    case HRM_COMM:
    case M_QU:

      return true;
    default:
      return false;
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    set_led_colors(get_highest_layer(state));
    return state;
}
// Add this function before process_record_user
static bool process_qu_macro(uint16_t keycode, keyrecord_t* record) {
    static uint16_t q_timer;

    if (record->event.pressed) {
        q_timer = timer_read();
    } else {
        if (timer_elapsed(q_timer) < TAPPING_TERM) {
            uint8_t mods = get_mods();
            bool shift = mods & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
            bool caps_word = is_caps_word_on();

            if (caps_word) {
                // Caps Word: QU (both capitals)
                tap_code16(S(KC_Q));
                tap_code16(S(KC_U));
            } else if (shift) {
                // Shift only: Qu (only Q capitalized)
                del_mods(MOD_MASK_SHIFT);
                tap_code16(S(KC_Q));
                tap_code(KC_U);
                set_mods(mods);
            } else {
                // Normal: qu (both lowercase)
                tap_code(KC_Q);
                tap_code(KC_U);
            }
        } else {
            // For hold behavior
            if (is_caps_word_on()) {
                tap_code16(S(KC_Q));
            } else {
                tap_code(KC_Q);
            }
        }
    }
    return false;
}

// Then in process_record_user, replace the M_QU case with:
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case HRM_H:
            if (record->tap.count && record->event.pressed) {
                process_hv_macro(false, get_last_keycode());
                return false; // Prevent default tap behavior
            }
            break;

        case M_HV:
            if (record->event.pressed) {
                process_hv_macro(false, get_last_keycode());
            }
            return false;

        case M_VH:
            if (record->event.pressed) {
                process_hv_macro(true, get_last_keycode());
            }
            return false;

        case LMAGIC:
            if (record->event.pressed) {
                process_left_magic(get_last_keycode(), get_last_mods());
                set_last_keycode(KC_SPC);
            }
            return false;

        case RMAGIC:
            if (record->event.pressed) {
                process_right_magic(get_last_keycode(), get_last_mods());
                set_last_keycode(KC_SPC);
            }
            return false;

        case M_QU:
            return process_qu_macro(keycode, record);
    }
    return true;
}

const key_override_t *key_overrides[] = {
  NULL
};

#ifdef CHORDAL_HOLD
// Handedness for Chordal Hold.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
  LAYOUT_num(
  '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    , '*'    ,
  '*'    , 'L'    , 'L'    , 'L'    , 'L'    , 'L'    , 'R'    , 'R'    , 'R'    , 'R'    , 'R'    , '*'    ,
  '*'    , 'L'    , 'L'    , 'L'    , 'L'    , 'L'    , 'R'    , 'R'    , 'R'    , 'R'    , 'R'    , '*'    ,
  '*'    , 'L'    , 'L'    , 'L'    , 'L'    , 'L'    , 'R'    , 'R'    , 'R'    , 'R'    , 'R'    , '*'    ,
           'L'    , 'L'    , 'L'    , 'L'    , 'L'    , 'R'    , 'R'    , 'R'    , 'R'    , 'R'    ,
                             'L'    , 'L'    , 'L'    , 'R'    , 'R'    , 'R'

);
#endif  // CHORDAL_HOLD
