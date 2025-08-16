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

#define COMBO_COUNT 7  // Adjust this number based on how many combos you define

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
    _MEDIA = 7,
    _ENTHIUM = 8,
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
    LAYER_MEDIA,
    LAYER_ENTHIUM,
    LAYER_GAME,
    ACTION_CAPS_WORD,
    ACTION_CAPS_LOCK
};

enum custom_keycodes {

    LMAGIC = SAFE_RANGE,  // Left thumb - for SFB removal
    RMAGIC,  // Right thumb - for word completion

    // Q -> Qu
    M_QU,  // Magic key for "qu"

    // Braces helper
    BRACES,  // For sending braces with Shift/Ctrl/Alt/Gui

    // Text selection
    SELWORD,  // Select word
    SELLINE,  // Select line

    QUOP, // Quopostrokey

};

// Home Row Modifiers
// Right Hand Side
#define HRM_S LALT_T(KC_S)  // Home Row Modifier for S
#define HRM_N LGUI_T(KC_N)  // Home Row Modifier for N
#define HRM_T LSFT_T(KC_T)  // Home Row Modifier for T
#define HRM_H LCTL_T(KC_H)   // Home Row Modifier for H
#define HRM_R LT(_NAV, KC_R) // Home Row Modifier for R
// Right Non Home Row Modifiers
#define HRM_M LT(_SYM, KC_M)  // Modifier for M
// #define HRM_J LT(_SYM, KC_J)   // Modifier for J

// Left Hand Side
#define HRM_A LCTL_T(KC_A) // Home Row Modifier for A
#define HRM_E RSFT_T(KC_E) // Home Row Modifier for E
#define HRM_I RGUI_T(KC_I) // Home Row Modifier for I
#define HRM_C RALT_T(KC_C) // Home Row Modifier for C
#define HRM_SPC LT(_NUM, KC_SPC) // Home Row Modifier for Space
// Left Non Home Row Modifiers
// #define HRM_W LT(_SYM, KC_W) // Modifier for W
// #define HRM_SCLN LT(_SYM, KC_SCLN) // Modifier for SCLN
// #define HRM_COMM KC_COMM //  Row Modifier for COMM
#define HRM_BSPC LT(_FUNC, KC_BSPC) // Modifier for BSPC
#define HRM_DEL LT(_FUNC, KC_DEL) // Modifier for DELs
#define HRM_MOUSE LT(_MOUSE, KC_BTN1) // Modifier for Mouse Button 1

// Command shorthands
#define OS_LSFT OSM(MOD_LSFT) // OS modifier for Left Shift
#define OS_RSFT OSM(MOD_RSFT) // OS modifier for Right Shift
#define WINSWITCH LGUI(LSFT(KC_RGHT)) // Windows Switch command

// Adaptive term for quick typing
#define ADAPTIVE_TERM_MS 250  // Only trigger if typed quickly (250ms)

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
            rgb_matrix_sethsv(HSV_YELLOW);   // Yellow for control layer
            return;
        case LAYER_MEDIA:
            rgb_matrix_sethsv(HSV_WHITE); // White for media layer
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_SPLASH);
            return;
        case LAYER_ENTHIUM:
            rgb_matrix_sethsv(HSV_PURPLE);
            return;
        case LAYER_GAME:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_RAINBOW_BEACON);
            // rgb_matrix_sethsv(HSV_WHITE); // White for game layer
            return;
        case ACTION_CAPS_WORD:
            rgb_matrix_sethsv(HSV_BLUE);
            return;
        case ACTION_CAPS_LOCK:
            rgb_matrix_sethsv(HSV_RED);
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
        RGB_TOG,  C(KC_X),  C(KC_V),    C(KC_C),    C(KC_A), C(KC_Z),                 KC_CALC,  KC_WSCH,    KC_WBAK,    KC_WFWD,    KC_WREF,    TO(_GAME),
        KC_TAB,   KC_F,     KC_P,       KC_D,       KC_L,    M_QU,                    KC_MINS,  KC_U,       KC_O,       KC_Y,       KC_B,       KC_BSLS,
        KC_Z,     HRM_S,    HRM_N,      HRM_T,      HRM_H,   KC_K,                    KC_SCLN,  HRM_A,      HRM_E,      HRM_I,      HRM_C,      KC_X,
        OS_LSFT,  KC_V,     KC_W,       KC_G,       HRM_M,   KC_J,                    BRACES,   QK_REP,     QUOP,       KC_DOT,     KC_COMM,    OS_RSFT,
                            A(KC_TAB),  G(KC_TAB),  HRM_R,   KC_ENT, KC_ESC, KC_BTN1, HRM_BSPC, HRM_SPC,    KC_WBAK,    KC_WFWD,
                                                    LMAGIC,  KC_NO,  KC_ENT, KC_BTN2, KC_NO,    RMAGIC
    ),

    // Layer 1 - Symbols
    [_SYM] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,      KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                        KC_TRNS,    KC_TRNS,       KC_TRNS,       KC_TRNS,    KC_TRNS,  KC_TRNS,
        KC_TRNS,  KC_GRV,       KC_EQL,         KC_MINS,        KC_MINS,        KC_BSLS,                        LSFT(KC_6), LSFT(KC_LBRC), LSFT(KC_RBRC), LSFT(KC_4), KC_ENT,   KC_TRNS,
        KC_TRNS,  S(KC_1),      S(KC_8),        KC_EQL,         KC_EQL,         KC_TRNS,                        LSFT(KC_3), LSFT(KC_9),    LSFT(KC_0),    KC_TRNS,    KC_TRNS,  KC_TRNS,
        KC_TRNS,  S(KC_GRV),    S(KC_EQL),      KC_UNDS,        KC_UNDS,        KC_TRNS,                        LSFT(KC_2), KC_LBRC,       KC_RBRC,       KC_TRNS,    KC_TRNS,  KC_TRNS,
                                KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,    KC_TRNS,   KC_TRNS, KC_TRNS,    KC_TRNS,       KC_TRNS,       KC_TRNS,
                                                                QK_LLCK,        KC_TRNS,    KC_TRNS,   KC_TRNS, KC_TRNS,    QK_LLCK
    ),

    // Layer 2 - Navigation
    [_NAV] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_PGUP,  KC_HOME,    KC_UP,      KC_END,     LCTL(KC_F), KC_TRNS,
        KC_TRNS,  KC_LALT,    KC_TRNS,    KC_LSFT,    KC_LCTL,    KC_TRNS,                       KC_PGDN,  KC_LEFT,    KC_DOWN,    KC_RGHT,    KC_DEL,     KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_PGUP,    KC_PGDN,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      QK_LLCK,    KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS,  QK_LLCK
    ),

    // Layer 3 - Numbers
    [_NUM] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_SLSH,    KC_7,       KC_8,       KC_9,       KC_PAST,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_MINS,    KC_1,       KC_2,       KC_3,       KC_PPLS,                          KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_X,       KC_4,       KC_5,       KC_6,       LSFT(KC_5),                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_0,       KC_TRNS,   KC_TRNS,    KC_TRNS,   KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      QK_LLCK,    KC_TRNS,   KC_TRNS,    KC_TRNS,   KC_TRNS,  QK_LLCK
    ),

    // Layer 4 - Function keys
    [_FUNC] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_F10,     KC_F11,     KC_F12,     KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    TO(_BASE),
        KC_TRNS,  KC_TRNS,    KC_F7,      KC_F8,      KC_F9,      KC_TRNS,                   QK_BOOT,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  RGB_TOG,    KC_F4,      KC_F5,      KC_F6,      KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_F1,      KC_F2,      KC_F3,      KC_TRNS,                   KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,
                                                      QK_LLCK,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  QK_LLCK
    ),

    // Layer 5 - Mouse
    [_MOUSE] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_BTN1,    KC_MS_U,    KC_BTN2,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_MS_L,    KC_MS_D,    KC_MS_R,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                       KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      QK_LLCK,    KC_TRNS,    KC_TRNS, KC_TRNS,  QK_LLCK,   KC_TRNS
    ),

    // Layer 6 - Control
    [_CTRL] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS,                      KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,   KC_TRNS,   KC_TRNS,
        C(KC_B),  C(KC_P),  C(KC_F),    C(KC_L),    C(KC_A), C(KC_Y),                      KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,   KC_TRNS,   KC_TRNS,
        C(KC_I),  C(KC_N),  C(KC_S),    C(KC_H),    C(KC_T), C(KC_K),                      KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,   KC_TRNS,   KC_TRNS,
        C(KC_U),  C(KC_X),  C(KC_V),    C(KC_C),    C(KC_D), C(KC_Z),                      KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,   KC_TRNS,   KC_TRNS,
                            KC_TRNS,     KC_TRNS,    KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                     KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS
    ),

    // Layer 7 - Media
    [_MEDIA] = LAYOUT_num(
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_VOLU,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_MPLY,    KC_MNXT,    KC_MPRV,    KC_MUTE,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,                    KC_TRNS,  KC_VOLD,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                      KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS
    ),

    [_ENTHIUM] = LAYOUT_num(
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,                   KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, TO(_BASE),
        KC_TRNS,  KC_Z,    KC_W,    KC_D,     KC_L,    KC_X,                      BRACES,   KC_O,    KC_U,    KC_Y,    M_QU,    KC_TRNS,
        KC_V,     KC_S,    KC_N,    KC_T,     KC_H,    KC_K,                      KC_COMM,  KC_A,    KC_E,    KC_I,    KC_C,    KC_B,
        KC_TRNS,  KC_F,    KC_P,    KC_G,     KC_M,    KC_J,                      KC_SCLN,  KC_DOT,  KC_EQL,  KC_MINS, QUOP,    KC_TRNS,
                           KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,
                                                       KC_TRNS, KC_R,    KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS
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

bool led_update_user(led_t led_state) {
    if (led_state.caps_lock) {                // Caps Lock just turned ON
        set_led_colors(ACTION_CAPS_LOCK);
    } else if (!is_caps_word_on()) {          // avoid wiping the blue Caps-Word colour
        set_led_colors(get_highest_layer(layer_state));
    }
    return true;  // let keyboard-level code (if any) run too
}


bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
	switch (keycode) {
		case CW_TOGG:
        case KC_ESC:

        case LMAGIC:
        case RMAGIC:
            return false;  // Magic keys will ignore the above keycodes.
    }
    return true;  // Other keys can be repeated.
}

// Put this near your other small helpers
static inline bool is_spaceish_key(uint16_t kc) {
    switch (kc) {
        case KC_SPC:
        case HRM_SPC:
        case LMAGIC:   // both magic keys insert a trailing space in your maps
        case RMAGIC:
            return true;
    }
    return false;
}

// ============================================================================
// MAGIC KEYS REFACTORING - Add this section to replace your existing magic code
// ============================================================================

// 1. REPLACE your existing word_variants_t structure with this:
typedef struct {
    bool needs_backspace;
    const char* base;
    const char* variations[5];
    uint8_t var_count;
} magic_entry_t;

// 2. REPLACE your existing last_magic_state with this:
static struct {
    const magic_entry_t* entry;  // Changed from word_variants_t*
    uint8_t current_variant;
    uint16_t base_length;
    uint16_t repeat_keycode;
} last_magic_state = {NULL, 0, 0, KC_NO};

// 4. REPLACE all your word_* definitions with these lookup tables:

// RMAGIC lookup table - indexed by keycode directly
static const magic_entry_t rmagic_table[256] = {
    [KC_A] = {true, "all ", {"allow ", "allows ", "allowed ", "allowing ", "allowance "}, 5},
    [KC_B] = {false, "etween ", {NULL}, 0},
    [KC_C] = {true, "copy ", {"copies ", "copied ", "copying ", "copier ", NULL}, 4},
    [KC_D] = {true, "difference ", {"differences ", "differential ", "differentiated ", "differentiation ", NULL}, 4},
    [KC_E] = {true, "every ", {"everyone ", "everything ", "everywhere ", "everybody ", "everyday "}, 5},
    [KC_F] = {true, "family ", {"families ", "familial ", "familiar ", "familiarity ", NULL}, 4},
    [KC_G] = {true, "GiveWell ", {"GiveWell's ", "GiveWell.org ", "GiveWell-style ", "GiveWell-inspired ", NULL}, 4},
    [KC_H] = {false, "owever ", {NULL}, 0},
    [KC_I] = {true, "ing ", {"ings ", "ingly ", NULL}, 2},
    [KC_J] = {true, "just ", {"justify ", "justified ", "justifying ", "justification ", "justice "}, 5},
    [KC_K] = {true, "know ", {"knows ", "knew ", "knowing ", "known ", "knowledge "}, 5},
    [KC_L] = {false, "ater ", {NULL}, 0},
    [KC_M] = {true, "ment ", {"ments ", "mental ", NULL}, 2},
    [KC_N] = {true, "never ", {"nevertheless ", "nevermore ", NULL}, 2},
    [KC_O] = {true, "order ", {"orders ", "ordered ", "ordering ", "orderly ", "disorder "}, 5},
    [KC_P] = {true, "people ", {"person ", "personal ", "personally ", "personnel ", "personalize "}, 5},
    [KC_Q] = {true, "question ", {"questions ", "questioned ", "questioning ", "questionable ", "questionnaire "}, 5},
    [KC_R] = {true, "the ", {" these ", " there ", " then ", " them ", " they "}, 5},
    [KC_S] = {true, "some ", {"something ", "someone ", "somewhere ", "somehow ", "somebody "}, 5},
    [KC_T] = {true, "though ", {"thought ", "thoughts ", "through ", NULL}, 3},
    [KC_U] = {true, "under ", {"understand ", "understood ", "understanding ", "underneath ", "underway "}, 5},
    [KC_V] = {true, "very ", {"verify ", "verified ", "verifying ", "verification ", NULL}, 4},
    [KC_W] = {true, "with ", {"without ", "within ", "withstand ", "withheld ", "withering "}, 5},
    [KC_X] = {true, "expect ", {"expects ", "expected ", "expecting ", "expectation ", "expectedly "}, 5},
    [KC_Y] = {true, "year ", {"years ", "yearly ", "yearn ", "yearning ", NULL}, 4},
    [KC_Z] = {true, "ation ", {"ational ", "ationally ", "ations ", NULL}, 3},
    [KC_SPC] = {false, "the ", {" these ", " there ", " then ", " them ", " they "}, 5},
    [KC_COMM] = {false, " and ", {NULL}, 0},
};

// LMAGIC lookup table - indexed by keycode directly
static const magic_entry_t lmagic_table[256] = {
    [KC_A] = {true, "again ", {"against ", NULL}, 1},
    [KC_B] = {true, "become ", {"became ", "becoming ", NULL}, 2},
    [KC_C] = {true, "could ", {"couldn't ", NULL}, 1},
    [KC_D] = {true, "death ", {"deaths ", "deathly ", NULL}, 2},
    [KC_E] = {true, "example ", {"examples ", "exemplary ", "exemplify ", "exemplification ", NULL}, 4},
    [KC_F] = {true, "find ", {"found ", "finds ", "finding ", "findings ", NULL}, 4},
    [KC_G] = {true, "government ", {"governments ", "government's ", "governmental ", NULL}, 3},
    [KC_H] = {true, "house ", {"houses ", "housed ", "housing ", "household ", NULL}, 4},
    [KC_I] = {true, "include ", {"includes ", "included ", "including ", NULL}, 3},
    [KC_J] = {true, "join ", {"joins ", "joined ", "joining ", "joint ", NULL}, 4},
    [KC_K] = {true, "kind ", {"kinds ", "kindly ", "kindness ", "kinder ", NULL}, 4},
    [KC_L] = {true, "large ", {"larger ", "largest ", "largely ", "largeness ", NULL}, 4},
    [KC_M] = {true, "make ", {"makes ", "made ", "making ", "makeover ", NULL}, 4},
    [KC_N] = {true, "number ", {"numbers ", "numbered ", "numbering ", "numerical ", NULL}, 4},
    [KC_O] = {true, "other ", {"others ", "other's ", "othering ", "otherness ", NULL}, 4},
    [KC_P] = {false, "a", {NULL}, 0},
    [KC_Q] = {true, "QMK ", {"QMK compile ", NULL}, 1},
    [KC_R] = {false, "r ", {NULL}, 0},
    [KC_S] = {false, "everal ", {NULL}, 0},
    [KC_T] = {true, "tion ", {"tions ", "tional ", "tionally ", NULL}, 3},
    [KC_U] = {true, "use ", {"uses ", "used ", "using ", "usability ", "user "}, 5},
    [KC_V] = {true, "value ", {"values ", "valued ", "valuing ", "valuation ", NULL}, 4},
    [KC_W] = {false, "ould ", {NULL}, 0},
    [KC_X] = {true, "except ", {"exception ", "exceptions ", "excepting ", NULL}, 3},
    [KC_Y] = {false, "o", {NULL}, 0},
    [KC_Z] = {false, "z", {NULL}, 0},
    [KC_SPC] = {false, "the ", {" these ", " there ", " then ", " them ", " they "}, 5},
    [KC_COMM] = {false, " but ", {NULL}, 0},
};

// Special handling for M_QU keycode mappings
static const magic_entry_t* get_magic_entry_for_qu(bool is_rmagic) {
    static const magic_entry_t rmagic_qu = {true, "question ", {"questions ", "questioned ", "questioning ", "questionable ", "questionnaire "}, 5};
    static const magic_entry_t lmagic_qu = {true, "QMK ", {"QMK compile ", NULL}, 1};
    return is_rmagic ? &rmagic_qu : &lmagic_qu;
}

// 5. REPLACE your MAGIC_STRING_VAR macro and magic_send_string_var function with:
#define MAGIC_STRING_VAR(entry, repeat_keycode) \
    magic_send_string_entry(entry, (repeat_keycode))

static void magic_send_string_entry(const magic_entry_t* entry, uint16_t repeat_keycode) {
    if (!entry || !entry->base) return;

    uprintf("MAGIC_STRING_VAR: word=%s\n", entry->base);

    last_magic_state.entry = entry;
    last_magic_state.current_variant = 0;
    last_magic_state.base_length = strlen(entry->base);
    last_magic_state.repeat_keycode = repeat_keycode;

    // Send the base form
    uint8_t saved_mods = 0;
    if (is_caps_word_on()) {
        saved_mods = get_mods();
        register_mods(MOD_BIT(KC_LSFT));
    }

    send_string_with_delay(entry->base, TAP_CODE_DELAY);
    set_last_keycode(repeat_keycode);

    if (is_caps_word_on()) {
        set_mods(saved_mods);
    }
}

// 6. REPLACE your cycle_last_magic function with:
static void cycle_last_magic(void) {
    if (!last_magic_state.entry) return;

    // Calculate how many backspaces needed
    uint16_t current_length = (last_magic_state.current_variant == 0)
        ? last_magic_state.base_length
        : strlen(last_magic_state.entry->variations[last_magic_state.current_variant - 1]);

    // Backspace current word
    for (uint16_t i = 0; i < current_length; i++) {
        tap_code(KC_BSPC);
    }

    // Move to next variant
    last_magic_state.current_variant++;
    if (last_magic_state.current_variant > last_magic_state.entry->var_count) {
        last_magic_state.current_variant = 0;
    }

    // Send new variant
    const char* to_send = (last_magic_state.current_variant == 0)
        ? last_magic_state.entry->base
        : last_magic_state.entry->variations[last_magic_state.current_variant - 1];

    send_string(to_send);
    set_last_keycode(last_magic_state.repeat_keycode);
}

// 7. REPLACE your process_right_magic and process_left_magic functions with:
static void process_right_magic(uint16_t keycode, uint8_t mods) {
    last_magic_state.entry = NULL;  // Reset last magic state

    // Normalize the keycode
    uint16_t base_kc = keycode & 0xFF;

    // Special handling for M_QU
    if (keycode == M_QU) {
        const magic_entry_t* entry = get_magic_entry_for_qu(true);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        MAGIC_STRING_VAR(entry, KC_SPC);
        return;
    }

    // Look up in table
    const magic_entry_t* entry = &rmagic_table[base_kc];
    if (!entry->base) return;  // No mapping for this key

    // Apply backspace if needed
    if (entry->needs_backspace) {
        tap_code(KC_BSPC);
    }

    // Send the magic string
    MAGIC_STRING_VAR(entry, KC_SPC);
}

static void process_left_magic(uint16_t keycode, uint8_t mods) {
    last_magic_state.entry = NULL;  // Reset last magic state

    // Normalize the keycode
    uint16_t base_kc = keycode & 0xFF;

    // Special handling for M_QU
    if (keycode == M_QU) {
        const magic_entry_t* entry = get_magic_entry_for_qu(false);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        MAGIC_STRING_VAR(entry, KC_SPC);
        return;
    }

    // Look up in table
    const magic_entry_t* entry = &lmagic_table[base_kc];
    if (!entry->base) return;  // No mapping for this key

    // Apply backspace if needed
    if (entry->needs_backspace) {
        tap_code(KC_BSPC);
    }

    // Send the magic string - note P and Y,Z don't add space
    uint16_t repeat_kc = (base_kc == KC_P || base_kc == KC_Y || base_kc == KC_Z) ? KC_NO : KC_SPC;
    MAGIC_STRING_VAR(entry, repeat_kc);
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
    // case HRM_J:
    // case HRM_SCLN:
    // case HRM_COMM:
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

static bool process_quopostrokey(uint16_t keycode, keyrecord_t* record) {
  static bool within_word = false;

  if (keycode == QUOP) {
    if (record->event.pressed) {
      if (within_word) {
        tap_code(KC_QUOT);
      } else {
        SEND_STRING("\"\"" SS_TAP(X_LEFT));
      }
    }
    return false;
  }

  switch (keycode) {  // Unpack tapping keycode for tap-hold keys.
#ifndef NO_ACTION_TAPPING
    case QK_MOD_TAP ... QK_MOD_TAP_MAX:
      if (record->tap.count == 0) { return true; }
      keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
      break;
#ifndef NO_ACTION_LAYER
    case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
      if (record->tap.count == 0) { return true; }
      keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
      break;
#endif  // NO_ACTION_LAYER
#endif  // NO_ACTION_TAPPING
  }

  // Determine whether the key is a letter.
  switch (keycode) {
    case KC_A ... KC_Z:
    case KC_BSPC:
      within_word = true;
      break;

    default:
      within_word = false;
  }

  return true;
}

// ─── Adaptive key state ───────────────────────────────────────────────────────
static uint16_t prior_keycode = KC_NO;
static uint16_t preprior_keycode = KC_NO;
static uint16_t prior_keydown_ms = 0;

// Unwrap QK_MOD_TAP / QK_LAYER_TAP to their tap keycode if this press is a tap.
static inline uint16_t unwrap_tap_keycode(uint16_t kc, const keyrecord_t *record) {
#ifndef NO_ACTION_TAPPING
    if (QK_MOD_TAP <= kc && kc <= QK_MOD_TAP_MAX) {
        return (record->tap.count > 0) ? QK_MOD_TAP_GET_TAP_KEYCODE(kc) : kc;
    }
#ifndef NO_ACTION_LAYER
    if (QK_LAYER_TAP <= kc && kc <= QK_LAYER_TAP_MAX) {
        return (record->tap.count > 0) ? QK_LAYER_TAP_GET_TAP_KEYCODE(kc) : kc;
    }
#endif
#endif
    return kc;
}

// Ignore contexts where adaptives would be dangerous/noisy.
static inline bool adaptive_forbidden_context(uint16_t kc, uint8_t mods) {
    // Don’t adapt while chords/shortcuts are likely (Ctrl/Alt/Gui held),
    // or on your special macro keys.
    if (mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)) return true;
    switch (kc) {
        case LMAGIC: case RMAGIC: case QK_REP:
        case BRACES: case M_QU: case QUOP:
            return true;
    }
    return false;
}

// Utilities for concise rules
#define SEND_UNSHIFTED(k)      do { tap_code(k); } while (0)
#define SEND_SHIFTED_IF_NEEDED(k) do { \
    if (is_caps_word_on() || (get_mods() & (MOD_BIT(KC_LSFT)|MOD_BIT(KC_RSFT)))) tap_code16(S(k)); \
    else tap_code(k); \
} while (0)

// Replace the just-typed previous char with X (backspace, then X)
static inline void replace_prev_with(uint16_t k) {
    tap_code(KC_BSPC);
    SEND_SHIFTED_IF_NEEDED(k);
}

// Insert a char before letting current key pass
static inline void insert_before_current(uint16_t k) {
    SEND_SHIFTED_IF_NEEDED(k);
}

// Suppress current key (return false from handler)
static inline bool suppress_current(void) { return true; }

// Core: returns false if it fully handled output (suppressing original key).
static bool process_adaptive_promethium(uint16_t keycode, const keyrecord_t *record) {
    if (!record->event.pressed) return true;

    if (timer_elapsed(prior_keydown_ms) > ADAPTIVE_TERM_MS) return true;

    uint8_t mods = get_mods();
    if (adaptive_forbidden_context(keycode, mods)) return true;

    uint16_t cur  = unwrap_tap_keycode(keycode, record);
    uint16_t prev = prior_keycode;

    // LEFT-HAND
    if (cur == KC_P && prev == KC_F) { replace_prev_with(KC_S); return true; }   // let P through
    if (cur == KC_D && prev == KC_P) { insert_before_current(KC_W); return false; }
    if (cur == KC_G && prev == KC_K) { SEND_UNSHIFTED(KC_L); return false; }
    if (cur == KC_G && prev == KC_W) { SEND_UNSHIFTED(KC_D); return false; }
    if (cur == KC_V && prev == KC_G) { SEND_UNSHIFTED(KC_T); return false; }
    if (cur == KC_K && prev == KC_M) { replace_prev_with(KC_L); return true; }   // let K through
    if (cur == KC_K && prev == KC_H) { replace_prev_with(KC_N); return true; }   // let K through
    if (cur == KC_M && prev == KC_G) { SEND_UNSHIFTED(KC_L); return false; }
    if (cur == KC_J && prev == KC_G) { send_string("th"); return false; }
    if (cur == KC_W && prev == KC_M) { SEND_UNSHIFTED(KC_P); return false; }
    if (cur == KC_L && (prev == KC_P || prev == KC_B || prev == KC_S)) {
        SEND_UNSHIFTED(KC_L); return false;
    }

    // RIGHT-HAND
    if (cur == KC_B && prev == KC_Y) { replace_prev_with(KC_I); return true; }   // let B through
    if ((cur == HRM_E || cur == KC_E) && (prev == HRM_A || prev == KC_A)) {
        SEND_SHIFTED_IF_NEEDED(KC_U); return false;
    }
    if (cur == KC_O && prev == KC_U) {
        SEND_SHIFTED_IF_NEEDED(KC_A); return false;
    }

    return true; // no adaptive change
}



// Add this near your other static variables at the top
static bool last_key_added_space = false;
static uint16_t space_adding_keys[] = {
    LMAGIC, RMAGIC, KC_SPC, HRM_SPC
    // Add any other keys that might add spaces here
};

// Add this helper function
static bool is_space_adding_key(uint16_t keycode) {
    for (int i = 0; i < sizeof(space_adding_keys)/sizeof(space_adding_keys[0]); i++) {
        if (space_adding_keys[i] == keycode) {
            return true;
        }
    }
    return false;
}

// Use this instead of is_punctuation()
static inline bool will_emit_punctuation_km(uint16_t kc, uint8_t mods) {
    bool shifted = (mods & MOD_MASK_SHIFT) != 0;

    switch (kc) {
        /* Keys that are punctuation unshifted already */
        case KC_DOT:    // . (or ? when shifted)
        case KC_COMM:   // , (or / when shifted because of your custom_shift_keys)
        case KC_SCLN:   // ; (or : when shifted)
        case KC_SLSH:   // /
        case KC_BSLS:   //
        case KC_MINS:   // -
        case KC_EQL:    // =
        case KC_GRV:    // `
            return true;

        /* Number row becomes punctuation when shifted: !@#$%^&*() */
        case KC_1: case KC_2: case KC_3: case KC_4: case KC_5:
        case KC_6: case KC_7: case KC_8: case KC_9: case KC_0:
            return shifted;

        default:
            return false;
    }
}

// Convenience: apply unwrap + oneshot
static inline bool will_emit_punctuation(uint16_t keycode, const keyrecord_t* record) {
    uint16_t kc  = unwrap_tap_keycode(keycode, record);
    uint8_t  mods = get_mods() | get_oneshot_mods();
    return will_emit_punctuation_km(kc, mods);
}



bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (record->event.pressed) {
        uprintf("Processing key: %u, repeat count: %d\n", keycode, get_repeat_key_count());
    }

    uint16_t cur_unwrapped = unwrap_tap_keycode(keycode, record);

    if (!process_quopostrokey(keycode, record)) { return false; }


    if (record->event.pressed && last_key_added_space && will_emit_punctuation(keycode, record)) {
        tap_code(KC_BSPC);              // remove the trailing magic space
        last_key_added_space = false;
    }


    // Handle space key when it's being repeated
    if (keycode == KC_SPC && get_repeat_key_count() > 0) {
        uprintf("Space is being repeated! Magic state: %s\n",
               last_magic_state.entry ? "exists" : "null");

        if (last_magic_state.entry && record->event.pressed) {
            uprintf("Cycling magic word instead of repeating space\n");
            cycle_last_magic();
            return false;  // Don't send the space
        }
        // Otherwise, let normal space repeat happen
    }

    // Handle space + e + repeat = " ex" rather than " ee"
    if ((keycode == KC_E || keycode == HRM_E) && get_repeat_key_count() > 0
        && is_spaceish_key(preprior_keycode)) {
            tap_code(KC_X);
            return false;  // Don't send the space
        }

    if (record->event.pressed) {
        uprintf("Key pressed: %u\n", keycode);
        last_key_added_space = is_space_adding_key(keycode);

        switch (keycode) {
            case LMAGIC:
            case RMAGIC:
            case QK_REP:
                break;                /* keep cycling */
            default:
                last_magic_state.entry = NULL;
                break;
        }
    }
    switch (keycode) {
        case LMAGIC:
            if (record->event.pressed) {
                process_left_magic(get_last_keycode(), get_last_mods());
                last_key_added_space = true;  // These always add spaces
                }
            return false;

        case RMAGIC:
            if (record->event.pressed) {
                // uprintf("RMAGIC pressed! Last key: %u\n", get_last_keycode());
                process_right_magic(get_last_keycode(), get_last_mods());
                last_key_added_space = true;  // These always add spaces
            }
            return false;

        case M_QU:
            return process_qu_macro(keycode, record);
        case BRACES: {                                       // (), [], {}, <> helper
            if (record->event.pressed) {
                uint8_t active_mods = get_mods() | get_oneshot_mods();

                clear_oneshot_mods();
                unregister_mods(MOD_MASK_CSAG);            // Ctrl-Shift-Alt-Gui

                if (active_mods & MOD_MASK_SHIFT) {        // ⇧ → []
                    SEND_STRING("[]");
                } else if (active_mods & MOD_MASK_CTRL) {  // ⌃ → {}
                    SEND_STRING("{}");
                } else if (active_mods & MOD_MASK_ALT) {   // ⌥ → <>
                    SEND_STRING("<>");
                } else {                                   // (no mod) → ()
                    SEND_STRING("()");
                }
                tap_code(KC_LEFT);                         // cursor inside the pair

                register_mods(active_mods);
            }
            return false;          // Tell QMK we handled this key completely
        }
        case SELWORD:   // Select Word
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_RGHT) SS_LSFT(SS_TAP(X_LEFT))));
            }
            return false;
        case SELLINE:   // Select Line
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END))));
            }
            return false;
        case QK_REP:
            if (record->event.pressed) {
                // Add debug
                if (last_magic_state.entry) {
                    uprintf("REP: word exists, variant=%d\n", last_magic_state.current_variant);
                    cycle_last_magic();
                    return false;
                } else {
                    uprintf("REP: no word stored\n");
                }
    }
    break;
    }

    // Adaptive logic, now pure (doesn't mutate history):
    bool pass_through = process_adaptive_promethium(keycode, record);
    if (!pass_through) {
        // swallowed; do not advance history here
        return false;
    }

    // If we got here, we’re actually sending this tap.
    // Advance history ONCE, here.
    preprior_keycode = prior_keycode;
    prior_keycode    = cur_unwrapped;
    prior_keydown_ms = timer_read();

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

const custom_shift_key_t custom_shift_keys[] = {
  {KC_DOT , KC_QUES},  // Shift . is ?
  {KC_COMM, KC_SLSH},  // Shift , is /
  {KC_BSPC, KC_DEL},   // Shift Backspace is Delete]
};

#ifdef COMBO_MUST_TAP_PER_COMBO
bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    // If you want all combos to be tap-only, just uncomment the next line
    // return true

    // If you want *all* combos, that have Mod-Tap/Layer-Tap/Momentary keys in its chord, to be tap-only, this is for you:
    uint16_t key;
    uint8_t idx = 0;
    while ((key = pgm_read_word(&combo->keys[idx])) != COMBO_END) {
        switch (key) {
            case QK_MOD_TAP...QK_MOD_TAP_MAX:
            case QK_LAYER_TAP...QK_LAYER_TAP_MAX:
            case QK_MOMENTARY...QK_MOMENTARY_MAX:
                return true;
        }
        idx += 1;
    }
    return false;

}
#endif



char sentence_case_press_user(uint16_t keycode,
                              keyrecord_t *record,
                              uint8_t mods) {

    /* 1️⃣ Ignore the *modifier key* itself – do nothing, don’t clear state.   */
    if (keycode == KC_LSFT || keycode == KC_RSFT) {
        return '\0';                // no state change, no classification
    }

    /* 2️⃣ Copy the stock logic, with one tweak for KC_DOT when shifted.       */
    if ((mods & ~(MOD_MASK_SHIFT | MOD_BIT(KC_RALT))) == 0) {
        const bool shifted = mods & MOD_MASK_SHIFT;

        switch (keycode) {
            case KC_A ... KC_Z:      return 'a';            // letters
            /* -----------  OUR ONE-LINE CHANGE  ---------------------------- */
            case KC_DOT:             return shifted ? '.'   // Shift-DOT = “?”
                                                : '.';      // plain DOT
            /* -------------------------------------------------------------- */
            case KC_1:               /* fall through */
            case KC_SLSH:            return shifted ? '.' : '#';
            case KC_EXLM:
            case KC_QUES:            return '.';
            case KC_2 ... KC_0:
            case KC_AT ... KC_RPRN:
            case KC_MINS ... KC_SCLN:
            case KC_UNDS ... KC_COLN:
            case KC_GRV:
            case KC_COMM:            return '#';
            case KC_SPC:             return ' ';
            case KC_QUOT:            return '\'';
        }
    }

    /* Any other key (navigation, hot-key, etc.) → reset Sentence Case.        */
    sentence_case_clear();
    return '\0';
}
