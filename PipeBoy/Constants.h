#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduboy2.h>
#include <ArduboyTones.h>

constexpr uint8_t FRAME_RATE = 40;

// Map Constants
constexpr int8_t startGridX = 15;
constexpr int8_t startGridY = -4;
constexpr uint8_t MAX_GRID_WIDTH = 12;
constexpr uint8_t MAX_GRID_HEIGHT = 9;
const uint8_t TILE_SIZE = 8;

// UI Constants
const uint8_t NB_MAX_PIPE_DISPLAYED = 4;
const uint8_t PIPE_LOADING_BAR_HEIGHT = 40;

// Player Constants
constexpr uint8_t REPLACING_PIPE_TIME = 30;

// Sound Constants
constexpr uint16_t MENU_SOUND_FREQ = NOTE_G2;
constexpr uint8_t MENU_SOUND_DUR = 50;

// Music Sequences
const uint16_t gameVictorySoundSequence[] PROGMEM = {
  NOTE_C5, 150, NOTE_E5, 150, NOTE_G5, 150, NOTE_C6, 300,
  TONES_END
};

const uint16_t gameEndSoundSequence[] PROGMEM = {
  NOTE_C4, 300, NOTE_G3, 300, NOTE_E3, 300, NOTE_C3, 600, NOTE_C2, 800,
  TONES_END
};

const uint16_t explosionSoundSequence[] PROGMEM = {
  NOTE_C6, 30, NOTE_G5, 30, NOTE_E5, 30, NOTE_C5, 40, NOTE_G4, 50,
  TONES_END
};

const uint16_t fordbiddenActionSoundSequence[] PROGMEM = {
  NOTE_G4, 100, NOTE_E4, 100, NOTE_C4, 150, 
  TONES_END
};

constexpr uint8_t EXPLOSION_ANIM_NB_FRAME = 7;
const unsigned char PROGMEM explosionAnimation[] =
{
8, 8,
// TILE 00
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 
// TILE 01
0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00, 
// TILE 02
0x00, 0x18, 0x3c, 0x7e, 0x7e, 0x3c, 0x18, 0x00, 
// TILE 03
0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18, 
// TILE 04
0x18, 0x24, 0x42, 0x81, 0x81, 0x42, 0x24, 0x18, 
// TILE 05
0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18, 
// TILE 06
0x24, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x24, 
};

extern Arduboy2 arduboy;
extern BeepPin1 beep;
extern SpritesB sprites;
extern ArduboyTones sound;

// #define DEBUG_MODE

#endif