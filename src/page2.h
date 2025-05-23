#include "Arduino.h"
#include "Inkplate.h"
#include "Fonts/FreeSansBold12pt7b.h"

extern Inkplate display;

int bitmap0_x = 700;
int bitmap0_y = 68;
const int bitmap0_w = 60;
const int bitmap0_h = 60;
const uint8_t bitmap0_content[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xe4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xae,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe8,0x0,0xe,0xee,0xec,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0x0,0x2e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xe0,0x2,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0x0,0x2e,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0xe0,0x2,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xea,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xe6,0x8,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xe8,0x2,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x6e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe4,0x0,0xe,0xee,0xee,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xa,0xee,0xee,0xe0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0x20,0xe,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xe0,0xe,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xec,0x2,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xee,0xee,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0x0,0xce,0xe6,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xe0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};
int bitmap1_x = 214;
int bitmap1_y = 69;
const int bitmap1_w = 60;
const int bitmap1_h = 60;
const uint8_t bitmap1_content[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xa2,0x0,0x2c,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe4,0x0,0x0,0x0,0x0,0x0,0xce,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xa0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x20,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x40,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6e,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6e,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xe2,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xe0,0x0,0x0,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0x0,0xe,0xee,0xee,0xee,0xec,0x0,0x0,0x0,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0x2,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x40,0x0,0xa,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x2e,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x40,0xe,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x2,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe2,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe0,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x20,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x4,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x0,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x0,0x0,0x0,0x2,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x40,0x0,0x0,0xe,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xe4,0x0,0x0,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xfe,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xef,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};
int rect0_a_x = 270;
int rect0_a_y = 236;
int rect0_b_x = 405;
int rect0_b_y = 491;
int rect0_fill = -1;
int rect0_radius = -1;
int rect0_color = 0;

int rect1_a_x = 100;
int rect1_a_y = 235;
int rect1_b_x = 236;
int rect1_b_y = 493;
int rect1_fill = -1;
int rect1_radius = -1;
int rect1_color = 0;

int rect2_a_x = 579;
int rect2_a_y = 228;
int rect2_b_x = 719;
int rect2_b_y = 492;
int rect2_fill = -1;
int rect2_radius = -1;
int rect2_color = 0;

int rect3_a_x = 772;
int rect3_a_y = 229;
int rect3_b_x = 904;
int rect3_b_y = 495;
int rect3_fill = -1;
int rect3_radius = -1;
int rect3_color = 0;

String text2_content = "PORT";
int text2_cursor_x = 127;
int text2_cursor_y = 225;
const GFXfont *text2_font = &FreeSansBold12pt7b;

String text3_content = "STRB";
int text3_cursor_x = 300;
int text3_cursor_y = 220;
const GFXfont *text3_font = &FreeSansBold12pt7b;

String text4_content = "STRB";
int text4_cursor_x = 795;
int text4_cursor_y = 216;
const GFXfont *text4_font = &FreeSansBold12pt7b;

String text5_content = "PORT";
int text5_cursor_x = 613;
int text5_cursor_y = 216;
const GFXfont *text5_font = &FreeSansBold12pt7b;

int circle0_center_x = 428;
int circle0_center_y = 720;
int circle0_fill = 1;
int circle0_radius = 10;
int circle0_color = 0;

int circle1_center_x = 493;
int circle1_center_y = 720;
int circle1_fill = -1;
int circle1_radius = 10;
int circle1_color = 0;

void mainDraw() {
    display.drawBitmap3Bit(bitmap0_x, bitmap0_y, bitmap0_content, bitmap0_w, bitmap0_h);
    display.drawBitmap3Bit(bitmap1_x, bitmap1_y, bitmap1_content, bitmap1_w, bitmap1_h);
    if (rect0_radius != -1 && rect0_fill != -1)
       display.fillRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius, rect0_color);
   else if (rect0_radius != -1 && rect0_fill == -1)
       display.drawRoundRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_radius, rect0_color);
   else if (rect0_radius == -1 && rect0_fill != -1)
       display.fillRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);
   else if (rect0_radius == -1 && rect0_fill == -1)
       display.drawRect(rect0_a_x, rect0_a_y, rect0_b_x - rect0_a_x, rect0_b_y - rect0_a_y, rect0_color);

    if (rect1_radius != -1 && rect1_fill != -1)
       display.fillRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius, rect1_color);
   else if (rect1_radius != -1 && rect1_fill == -1)
       display.drawRoundRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_radius, rect1_color);
   else if (rect1_radius == -1 && rect1_fill != -1)
       display.fillRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);
   else if (rect1_radius == -1 && rect1_fill == -1)
       display.drawRect(rect1_a_x, rect1_a_y, rect1_b_x - rect1_a_x, rect1_b_y - rect1_a_y, rect1_color);

    if (rect2_radius != -1 && rect2_fill != -1)
       display.fillRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius, rect2_color);
   else if (rect2_radius != -1 && rect2_fill == -1)
       display.drawRoundRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_radius, rect2_color);
   else if (rect2_radius == -1 && rect2_fill != -1)
       display.fillRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);
   else if (rect2_radius == -1 && rect2_fill == -1)
       display.drawRect(rect2_a_x, rect2_a_y, rect2_b_x - rect2_a_x, rect2_b_y - rect2_a_y, rect2_color);

    if (rect3_radius != -1 && rect3_fill != -1)
       display.fillRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius, rect3_color);
   else if (rect3_radius != -1 && rect3_fill == -1)
       display.drawRoundRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_radius, rect3_color);
   else if (rect3_radius == -1 && rect3_fill != -1)
       display.fillRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);
   else if (rect3_radius == -1 && rect3_fill == -1)
       display.drawRect(rect3_a_x, rect3_a_y, rect3_b_x - rect3_a_x, rect3_b_y - rect3_a_y, rect3_color);

    display.setFont(text2_font);
    display.setTextColor(0, 7);    display.setTextSize(1);    display.setCursor(text2_cursor_x, text2_cursor_y);
    display.print(text2_content);

    display.setFont(text3_font);
    display.setTextColor(0, 7);    display.setTextSize(1);    display.setCursor(text3_cursor_x, text3_cursor_y);
    display.print(text3_content);

    display.setFont(text4_font);
    display.setTextColor(0, 7);    display.setTextSize(1);    display.setCursor(text4_cursor_x, text4_cursor_y);
    display.print(text4_content);

    display.setFont(text5_font);
    display.setTextColor(0, 7);    display.setTextSize(1);    display.setCursor(text5_cursor_x, text5_cursor_y);
    display.print(text5_content);

    if (circle0_fill != -1)
       display.fillCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);
   else
       display.drawCircle(circle0_center_x, circle0_center_y, circle0_radius, circle0_color);

    if (circle1_fill != -1)
       display.fillCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);
   else
       display.drawCircle(circle1_center_x, circle1_center_y, circle1_radius, circle1_color);

}
