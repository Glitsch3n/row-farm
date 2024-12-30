#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduboy2.h>

constexpr uint8_t FRAME_RATE = 40;

// Map Constants
constexpr int8_t startGridX = 10;
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
constexpr uint16_t MENU_SOUND_FREQ = 100;
constexpr uint8_t MENU_SOUND_DUR = 3;


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

//#define DEBUG_MODE

#endif