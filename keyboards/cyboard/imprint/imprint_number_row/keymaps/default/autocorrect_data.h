// Copyright 2025 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

/*******************************************************************************
  88888888888 888      d8b                .d888 d8b 888               d8b
      888     888      Y8P               d88P"  Y8P 888               Y8P
      888     888                        888        888
      888     88888b.  888 .d8888b       888888 888 888  .d88b.       888 .d8888b
      888     888 "88b 888 88K           888    888 888 d8P  Y8b      888 88K
      888     888  888 888 "Y8888b.      888    888 888 88888888      888 "Y8888b.
      888     888  888 888      X88      888    888 888 Y8b.          888      X88
      888     888  888 888  88888P'      888    888 888  "Y8888       888  88888P'
                                                        888                 888
                                                        888                 888
                                                        888                 888
     .d88b.   .d88b.  88888b.   .d88b.  888d888 8888b.  888888 .d88b.   .d88888
    d88P"88b d8P  Y8b 888 "88b d8P  Y8b 888P"      "88b 888   d8P  Y8b d88" 888
    888  888 88888888 888  888 88888888 888    .d888888 888   88888888 888  888
    Y88b 888 Y8b.     888  888 Y8b.     888    888  888 Y88b. Y8b.     Y88b 888
     "Y88888  "Y8888  888  888  "Y8888  888    "Y888888  "Y888 "Y8888   "Y88888
         888
    Y8b d88P
     "Y88P"
*******************************************************************************/

#pragma once

// Autocorrection dictionary (7 entries):
//   :i:    -> I
//   :im:   -> I'm
//   :ill:  -> I'll
//   :ive:  -> I've
//   :id:   -> I'd
//   :brnd: -> brendanphillips94@gmail.com
//   :bp:   -> Brendan Phillips

#define AUTOCORRECT_MIN_LENGTH 3 // ":i:"
#define AUTOCORRECT_MAX_LENGTH 6 // ":brnd:"
#define DICTIONARY_SIZE 120

static const uint8_t autocorrect_data[DICTIONARY_SIZE] PROGMEM = {
    0x2C, 0x00, 0x47, 0x15, 0x00, 0x08, 0x42, 0x00, 0x0C, 0x4C, 0x00, 0x0F, 0x51, 0x00, 0x10, 0x5B,
    0x00, 0x13, 0x63, 0x00, 0x00, 0x4C, 0x1C, 0x00, 0x11, 0x23, 0x00, 0x00, 0x2C, 0x00, 0x82, 0x49,
    0x27, 0x64, 0x00, 0x15, 0x05, 0x2C, 0x00, 0x82, 0x65, 0x6E, 0x64, 0x61, 0x6E, 0x70, 0x68, 0x69,
    0x6C, 0x6C, 0x69, 0x70, 0x73, 0x39, 0x34, 0x40, 0x67, 0x6D, 0x61, 0x69, 0x6C, 0x2E, 0x63, 0x6F,
    0x6D, 0x00, 0x19, 0x0C, 0x2C, 0x00, 0x83, 0x49, 0x27, 0x76, 0x65, 0x00, 0x2C, 0x00, 0x81, 0x49,
    0x00, 0x0F, 0x0C, 0x2C, 0x00, 0x83, 0x49, 0x27, 0x6C, 0x6C, 0x00, 0x0C, 0x2C, 0x00, 0x82, 0x49,
    0x27, 0x6D, 0x00, 0x05, 0x2C, 0x00, 0x82, 0x42, 0x72, 0x65, 0x6E, 0x64, 0x61, 0x6E, 0x20, 0x50,
    0x68, 0x69, 0x6C, 0x6C, 0x69, 0x70, 0x73, 0x00
};
