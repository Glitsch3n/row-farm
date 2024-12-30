#include "Level.h"

const Level PROGMEM levels[] = {
  { 14, 0, 0, 0, 0, false },   // LV1
  { 15, 2, 0, 0, 0, false },   // LV2
  { 14, 3, 0, 0, 0, false },   // LV3
  { 16, 0, 6, 1, 0, false },   // LV4 --> PWD : HAHA
  { 17, 0, 7, 1, 0, false },   // LV5
  { 18, 0, 8, 2, 0, false },   // LV6
  { 18, 0, 10, 3, 0, false },  // LV7
  { 18, 4, 0, 0, 2, false },   // LV8 --> PWD : GRIN
  { 19, 4, 0, 0, 2, false },   // LV9
  { 20, 4, 2, 0, 2, false },   // LV10
  { 21, 4, 2, 0, 3, false },   // LV11
  { 17, 2, 5, 1, 2, false },   // LV12 --> PWD : REAP
  { 18, 2, 6, 1, 2, false },   // LV13
  { 18, 2, 7, 1, 3, false },   // LV14
  { 19, 2, 7, 2, 3, false },   // LV15
  { 12, 0, 0, 0, 1, false },   // LV16 --> PWD : SEED
  { 13, 0, 1, 1, 1, false },   // LV17
  { 14, 0, 0, 1, 1, true },    // LV18
  { 15, 2, 1, 1, 1, true },    // LV19
  { 16, 3, 8, 1, 0, false },   // LV20 --> PWD : GROW
  { 15, 3, 9, 2, 0, false },   // LV21
  { 16, 3, 10, 2, 0, true },   // LV22
  { 17, 3, 10, 2, 0, true },   // LV23
  { 24, 4, 0, 2, 2, false },   // LV24 --> PWD : TALL
  { 24, 5, 0, 3, 2, false },   // LV25
  { 27, 6, 0, 3, 3, true },    // LV26
  { 27, 0, 0, 0, 3, true },    // LV27 --> PWD : YALI
  { 20, 4, 4, 3, 2, false },   // LV28
  { 21, 5, 4, 3, 2, false },   // LV29
  { 22, 5, 4, 3, 3, true },    // LV30
  { 23, 6, 3, 3, 3, true },    // LV31
  { 25, 6, 6, 3, 2, false },   // LV32
  { 26, 5, 3, 3, 3, true },    // LV34
  { 27, 6, 4, 3, 3, true },    // LV35
};

const size_t NUMBER_LEVELS = sizeof(levels) / sizeof(levels[0]);