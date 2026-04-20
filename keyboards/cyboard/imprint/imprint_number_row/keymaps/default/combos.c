#include QMK_KEYBOARD_H
#include "cyboard.h"

// Combos
const uint16_t PROGMEM TH_TAB[] =   {HRM_H, HRM_T, COMBO_END};
const uint16_t PROGMEM HA_CW[] =    {HRM_T, HRM_E, COMBO_END};
const uint16_t PROGMEM THN_WSW[] =  {HRM_H, HRM_T, HRM_N, COMBO_END};
// const uint16_t PROGMEM NS_Z[] =     {HRM_N, HRM_S, COMBO_END};
const uint16_t PROGMEM NI_CAPS[] =  {HRM_N, HRM_I, COMBO_END};
// const uint16_t PROGMEM TC_SYM[] =   {HRM_C, HRM_T, COMBO_END};
const uint16_t PROGMEM AE_SELW[] =  {HRM_A, HRM_E, COMBO_END};
const uint16_t PROGMEM AEI_SELL[] = {HRM_I, HRM_A, HRM_E, COMBO_END};
const uint16_t PROGMEM GM_BSPC[] =  {KC_G, HRM_M, COMBO_END};
const uint16_t PROGMEM AI_COMBO[] = {HRM_A, HRM_I, COMBO_END};

combo_t key_combos[] = {
    COMBO(TH_TAB, KC_TAB),
    COMBO(HA_CW,  CW_TOGG),
    COMBO(THN_WSW, OS_WINSW),
    // COMBO(NS_Z,   KC_Z),
    COMBO(NI_CAPS,   KC_CAPS),
    // COMBO(TC_SYM, OSL(_SYM)),
    COMBO(AE_SELW, SELWORD),
    COMBO(AEI_SELL, SELLINE),
    COMBO(GM_BSPC, KC_BSPC),
    COMBO(AI_COMBO, AI_CAPS),
};
