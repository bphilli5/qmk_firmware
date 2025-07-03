// altrep.c
#include "altrep.h"
#include "quantum.h"
#include "action_layer.h"
#include "print.h"  // For uprintf debug if needed

// Add processing functions for each alternate repeat key
static void process_altrep1(uint16_t keycode, uint8_t mods) {
    // SFB removal patterns
    switch (keycode) {
        case KC_A: SEND_STRING("o"); break;    // A -> O
        case KC_O: SEND_STRING("a"); break;    // O -> A
        case KC_E: SEND_STRING("u"); break;    // E -> U
        case KC_U: SEND_STRING("e"); break;    // U -> E
        case KC_I: SEND_STRING("a"); break;    // I -> A (for "ia" combinations)
        case KC_R: SEND_STRING("l"); break;    // R -> L (for "rl" combinations)
        case KC_L: SEND_STRING("r"); break;    // L -> R (for "lr" combinations)
        case KC_N: SEND_STRING("t"); break;    // N -> T (for "nt" combinations)
        case KC_T: SEND_STRING("h"); break;    // T -> H (for "th" combinations)
        case KC_S: SEND_STRING("t"); break;    // S -> T (for "st" combinations)
        // Add more SFB patterns as needed
    }
}

static void process_altrep2(uint16_t keycode, uint8_t mods) {
    // Word completion patterns
    switch (keycode) {
        case KC_M: SEND_STRING(/*m*/"ent"); break;     // M -> MENT
        case KC_T: SEND_STRING(/*t*/"ion"); break;     // T -> TION
        case KC_S: SEND_STRING(/*s*/"ion"); break;     // S -> SION
        case KC_N: SEND_STRING(/*n*/"ess"); break;     // N -> NESS
        case KC_L: SEND_STRING(/*l*/"ess"); break;     // L -> LESS
        case KC_I: SEND_STRING(/*i*/"on"); break;      // I -> ION
        case KC_R: SEND_STRING(/*r*/"ight"); break;    // R -> RIGHT
        case KC_W: SEND_STRING(/*w*/"ith"); break;     // W -> WITH
        case KC_H: SEND_STRING(/*h*/"ere"); break;     // H -> HERE
        case KC_A: SEND_STRING(/*a*/"nd"); break;      // A -> AND
        // Add more word completion patterns
    }
}

// // Alternate Repeat Key implementation
// uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
//     keycode = get_tap_keycode(keycode);

//     // Only apply magic when no mods except shift
//     if ((mods & ~MOD_MASK_SHIFT) == 0) {
//         switch (keycode) {
//             // Common SFB removals
//             case KC_A: return KC_O;         // A -> O
//             case KC_O: return KC_A;         // O -> A
//             case KC_E: return KC_U;         // E -> U
//             case KC_U: return KC_E;         // U -> E

//             // Word completions
//             case KC_M: return M_MENT;       // M -> MENT
//             case KC_T: return M_TION;       // T -> TION
//             case KC_S: return M_SION;       // S -> SION
//             case KC_N: return M_NESS;       // N -> NESS
//             case KC_L: return M_LESS;       // L -> LESS
//             case KC_I: return M_ION;        // I -> ION

//             // Space -> THE
//             case KC_SPC:
//             case KC_ENT:
//                 return M_THE;
//         }
//     }

//     return KC_TRNS;
// }

// bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
//                             uint8_t* remembered_mods) {
//     switch (keycode) {
//         case ALTREP1:
//         case ALTREP2:
//             return false;  // Ignore custom alternate repeat keys
//     }
//     return true;  // Other keys can be repeated
// }
