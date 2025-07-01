/* Copyright 2023 Cyboard LLC (@Cyboard-DigitalTailor)
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include QMK_KEYBOARD_H
#include <cyboard.h>
#include "color.h"
#include "rgb_matrix.h"
#include "timer.h"
#include "process_caps_word.h"
#include "repeat_key.h"

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
    _EMPTY6 = 6,
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
    LAYER_GAME,
    ACTION_CAPS_WORD
};

enum custom_keycodes {
    // Magic key macros
    M_THE = SAFE_RANGE,  // Magic key for "the "
    M_ION,
    M_TION,
    M_SION,
    M_MENT,
    M_NESS,
    M_LESS,
    M_ENCE,
    M_ANCE
};


// Alternate Repeat keys
#define ALTREP1 QK_AREP  // Left thumb - for SFB removal
#define ALTREP2 QK_REP   // Right thumb - for word completion
// Home Row Modifiers
#define HRM_N LALT_T(KC_N)  // Home Row Modifier for N
#define HRM_S LGUI_T(KC_S)  // Home Row Modifier for S
#define HRM_H LSFT_T(KC_H)  // Home Row Modifier for H
#define HRM_T LT(_NAV, KC_T)   // Home Row Modifier for T
#define HRM_D LCTL_T(KC_D)  // Home Row Modifier for D
#define HRM_V LT(_SYM, KC_V)   // Home Row Modifier for V
#define HRM_C LT(_NUM, KC_C) // Home Row Modifier for C
#define HRM_A RSFT_T(KC_A) // Home Row Modifier for A
#define HRM_E RGUI_T(KC_E) // Home Row Modifier for E
#define HRM_I RALT_T(KC_I) // Home Row Modifier for I
#define HRM_W RCTL_T(KC_W) // Home Row Modifier for W
#define HRM_SCLN LT(_SYM, KC_SCLN) // Home Row Modifier for SCLN
#define HRM_COMM RGUI_T(KC_COMM) // Home Row Modifier for COMM
#define HRM_DEL LT(_FUNC, KC_DEL) // Home Row Modifier for DEL
#define HRM_MOUSE LT(_MOUSE, KC_BTN1) // Home Row Modifier for Mouse Button 1
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

// Define the HSV values for the LED colors
// Function to set LED colors based on state
void set_led_colors(enum led_states led_state) {
    switch (led_state) {
        case LAYER_BASE:
            rgb_matrix_sethsv(HSV_PURPLE);  // Purple for base
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_SYM:
            rgb_matrix_sethsv(HSV_MAGENTA); // Magenta for symbols
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_NAV:
            rgb_matrix_sethsv(HSV_CORAL);   // Coral for navigation
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_NUM:
            rgb_matrix_sethsv(HSV_ORANGE);  // Orange for numbers
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_FUNC:
            rgb_matrix_sethsv(HSV_CYAN);    // Cyan for function keys
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_MOUSE:
            rgb_matrix_sethsv(HSV_GREEN);   // Green for mouse
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case LAYER_GAME:
            rgb_matrix_sethsv(HSV_BLUE);    // Blue for GAME
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
        case ACTION_CAPS_WORD:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_GRADIENT_UP_DOWN);
            return;
        default:
            rgb_matrix_sethsv(HSV_PURPLE);
            rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
            return;
    }
}
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0 - Base layer (corrected from inverted VIA export)
    [0] = LAYOUT(
        // Left side
        RGB_TOG,  CUT,        COPY,       PASTE,      CTL_A,      UNDO,
        KC_TAB,   KC_B,       KC_F,       KC_L,       KC_M,       KC_Q,
        KC_CAPS,  HRM_N,      HRM_S,      HRM_H,      HRM_T,      KC_K,
        OS_LSFT,  KC_X,       HRM_V,      KC_J,       HRM_D,      KC_Z,
                              ALTTAB,     GUI_TAB,
        
        KC_R,       KC_ENT,    HRM_DEL,
        ALTREP1,    KC_HOME,   KC_ENT,
        
        // Right side
        KC_CALC,  KC_WSCH,    KC_WBAK,    KC_WFWD,    KC_WREF,    TO(_GAME),
        KC_P,     KC_G,       KC_O,       KC_U,       KC_DOT,     KC_BSLS,
        KC_Y,     HRM_C,      HRM_A,      HRM_E,      HRM_I,      KC_DEL,
        KC_SLSH,  HRM_W,      KC_QUOT,    HRM_SCLN,   HRM_COMM,   OS_RSFT,
                              KC_NO,      KC_NO,
        
        HRM_MOUSE,KC_BSPC,    KC_SPC,
        KC_BTN2,  KC_BSPC,    ALTREP2
    ),

    // Layer 1 - Symbols
    [1] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_GRV,     LSFT(KC_COMM), LSFT(KC_DOT), KC_MINS, KC_BSLS,
        KC_TRNS,  LSFT(KC_1), LSFT(KC_8), KC_SLSH,    KC_EQL,     KC_TRNS,
        KC_TRNS,  LSFT(KC_GRV), LSFT(KC_EQL), KC_LBRC, KC_RBRC,   KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        LSFT(KC_6), LSFT(KC_LBRC), LSFT(KC_RBRC), LSFT(KC_4), KC_ENT, KC_TRNS,
        LSFT(KC_3), LSFT(KC_9), LSFT(KC_0), KC_SCLN,  KC_QUOT,    KC_TRNS,
        LSFT(KC_2), KC_LBRC,  KC_RBRC,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layer 2 - Navigation
    [2] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_LALT,    KC_TRNS,    KC_LSFT,    KC_LCTL,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_PGUP,    KC_PGDN,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_PGUP,  KC_HOME,    KC_UP,      KC_END,     LCTL(KC_F), KC_TRNS,
        KC_PGDN,  KC_LEFT,    KC_DOWN,    KC_RGHT,    KC_DEL,     KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layer 3 - Numbers
    [3] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_SLSH,    KC_7,       KC_8,       KC_9,       KC_PAST,
        KC_TRNS,  KC_MINS,    KC_4,       KC_5,       KC_6,       KC_PPLS,
        KC_TRNS,  KC_X,       KC_1,       KC_2,       KC_3,       LSFT(KC_5),
                              KC_TRNS,    KC_TRNS,
        
        KC_0,     KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layer 4 - Function keys
    [4] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_F10,     KC_F11,     KC_F12,     KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_F7,      KC_F8,      KC_F9,      KC_TRNS,
        KC_TRNS,  RGB_TOG,    KC_F4,      KC_F5,      KC_F6,      KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_F1,      KC_F2,      KC_F3,      KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,   KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layer 5 - Mouse
    [5] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_BTN1,    KC_MS_U,    KC_BTN2,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_MS_L,    KC_MS_D,    KC_MS_R,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layers 6, 7, and 8 - Empty
    [6] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),
    
    [7] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    [8] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                              KC_TRNS,    KC_TRNS,
        
        KC_TRNS,  KC_TRNS,    KC_TRNS,
        KC_TRNS,  KC_TRNS,    KC_TRNS
    ),

    // Layer 9 - GAME layer
    [9] = LAYOUT(
        // Left side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
        KC_T,     KC_LCTL,    KC_Q,       KC_W,       KC_E,       KC_R,
        KC_G,     KC_LSFT,    KC_A,       KC_S,       KC_D,       KC_F,
        KC_B,     KC_TAB,     KC_Z,       KC_X,       KC_C,       KC_V,
                              KC_TRNS,    KC_TRNS,
        
        KC_SPC,   KC_LSFT,     KC_TRNS,
        KC_TRNS,  KC_TRNS,     KC_TRNS,
        
        // Right side
        KC_TRNS,  KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    TO(0),
        KC_BSLS,  KC_P,       KC_O,       KC_I,       KC_U,       KC_Y,
        KC_QUOT,  KC_SCLN,    KC_L,       KC_K,       KC_J,       KC_H,
        KC_RSFT,  KC_SLSH,    KC_DOT,     KC_COMM,    KC_M,       KC_N,
                              KC_RBRC,    KC_LBRC,
        
        KC_SPC,   KC_BSPC,    KC_BTN1,
        KC_GRV,   KC_SPC,     KC_BSPC
    )
};

// Combos
const uint16_t PROGMEM combo1[] = {KC_L, KC_M, COMBO_END};
const uint16_t PROGMEM combo2[] = {HRM_T, HRM_H, COMBO_END};
const uint16_t PROGMEM sentence_case_combo[] = {HRM_H, HRM_A, COMBO_END}; // Both shift keys


combo_t key_combos[] = {
    COMBO(combo1, KC_TAB),
    COMBO(combo2, KC_TAB),
    COMBO(sentence_case_combo, SENTENCE_CASE_ON),

};

// Helper function to send a string while handling Caps Word
static void magic_send_string_P(const char* str, uint16_t repeat_keycode) {
    uint8_t saved_mods = 0;
    // If Caps Word is on, save the mods and hold Shift.
    if (is_caps_word_on()) {
        saved_mods = get_mods();
        register_mods(MOD_BIT_LSHIFT);
    }

    send_string_P(str);
    set_last_keycode(repeat_keycode);

    // If Caps Word is on, restore the mods.
    if (is_caps_word_on()) {
        set_mods(saved_mods);
    }
}

#define MAGIC_STRING(str, repeat_keycode) \
  magic_send_string_P(PSTR(str), (repeat_keycode))

// Get tap keycode from tap-hold keys
static uint16_t get_tap_keycode(uint16_t keycode) {
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            return QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    return keycode;
}

// Alternate Repeat Key implementation
uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    keycode = get_tap_keycode(keycode);
    
    // Only apply magic when no mods except shift
    if ((mods & ~MOD_MASK_SHIFT) == 0) {
        switch (keycode) {
            // Common SFB removals
            case KC_A: return KC_O;         // A -> O
            case KC_O: return KC_A;         // O -> A
            case KC_E: return KC_U;         // E -> U
            case KC_U: return KC_E;         // U -> E
            
            // Word completions
            case KC_M: return M_MENT;       // M -> MENT
            case KC_T: return M_TION;       // T -> TION
            case KC_S: return M_SION;       // S -> SION
            case KC_N: return M_NESS;       // N -> NESS
            case KC_L: return M_LESS;       // L -> LESS
            case KC_I: return M_ION;        // I -> ION
            
            // Space -> THE
            case KC_SPC:
            case KC_ENT:
                return M_THE;
        }
    }
    
    return KC_TRNS;
}

// Repeat key implementation for word completions
bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
    keycode = get_tap_keycode(keycode);
    
    // Handle sentence case capitalization
    if (is_sentence_case_on() && is_sentence_case_primed()) {
        // Check if this is a letter that should be capitalized
        if (keycode >= KC_A && keycode <= KC_Z) {
            *remembered_mods |= MOD_BIT_LSHIFT;
        }
    }
    
    return true;
}

// Sentence case configuration
char sentence_case_press_user(uint16_t keycode, keyrecord_t* record,
                              uint8_t mods) {
    if ((mods & ~(MOD_MASK_SHIFT | MOD_BIT_RALT)) == 0) {
        const bool shifted = mods & MOD_MASK_SHIFT;
        switch (keycode) {
            case KC_A ... KC_Z:
            case M_THE:
            case M_ION:
            case M_MENT:
            case M_TION:
            case M_SION:
                return 'a';  // Letter key.

            case KC_DOT:  // Both . and Shift . (?) punctuate sentence endings.
            case KC_EXLM:
            case KC_QUES:
                return '.';

            case KC_SPC:
                return ' ';  // Space key.

            case KC_QUOT:
            case KC_DQUO:
                return '\'';  // Quote key.
        }
    }

    // Otherwise clear Sentence Case to initial state.
    sentence_case_clear();
    return '\0';
}

// Handle Caps Word visual feedback
void caps_word_set_user(bool active) {
    if (active) {
        set_led_colors(ACTION_CAPS_WORD);
    } else {
        set_led_colors(get_highest_layer(layer_state));
    }
}

// Handle layer changes for RGB
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    
    // Don't override special modes
    if (!caps_word_is_active()) {
        switch (layer) {
            case _BASE:
                set_led_colors(LAYER_BASE);
                break;
            case _SYM:
                set_led_colors(LAYER_SYM);
                break;
            case _NAV:
                set_led_colors(LAYER_NAV);
                break;
            case _NUM:
                set_led_colors(LAYER_NUM);
                break;
            case _FUNC:
                set_led_colors(LAYER_FUNC);
                break;
            case _MOUSE:
                set_led_colors(LAYER_MOUSE);
                break;
            case _GAME:
                set_led_colors(LAYER_GAME);
                break;
            default:
                set_led_colors(LAYER_BASE);
                break;
        }
    }
    
    return state;
}

// Process custom keycodes
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (record->event.pressed) {
        switch (keycode) {
            // Magic key macros
            case M_THE:     MAGIC_STRING("the ", KC_N); break;
            case M_ION:     MAGIC_STRING("ion ", KC_S); break;
            case M_TION:    MAGIC_STRING("tion ", KC_S); break;
            case M_SION:    MAGIC_STRING("sion ", KC_S); break;
            case M_MENT:    MAGIC_STRING("ment ", KC_S); break;
            case M_NESS:    MAGIC_STRING("ness ", KC_E); break;
            case M_LESS:    MAGIC_STRING("less ", KC_N); break;
            case M_ENCE:    MAGIC_STRING("ence ", KC_S); break;
            case M_ANCE:    MAGIC_STRING("ance ", KC_S); break;
        }
    }
    
    return true;
}

// Optional: Add any keyboard-specific initialization
void keyboard_post_init_user(void) {
    // Initialize RGB
    rgb_matrix_enable();
    set_led_colors(LAYER_BASE);
}