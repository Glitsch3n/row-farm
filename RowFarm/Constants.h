#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduboy2.h>
#include "Images.h"

constexpr uint8_t frameRate = 40;

// HighScore constants
const uint8_t highScoreSize = 3;

// Menu constants
constexpr uint8_t menuSelectionXPos[] = {
  4, 40, 82
};

// In-Game constants
constexpr uint8_t maxGridWidth = 14;
constexpr uint8_t maxGridHeight = 6;
constexpr uint8_t maxPlayerRowWidth = 3;
constexpr uint8_t playerDefaultPausingTime = 30;
constexpr uint8_t playerDefaultSpeed = 35;

// Sprite constants
constexpr uint8_t* blockTypeSprites[] = {
  block_type_1,
  block_type_2,
  block_type_3,
  block_type_4,
  block_type_5,
};
constexpr uint8_t maxBlockType = (sizeof(blockTypeSprites) / sizeof(blockTypeSprites[0]));
constexpr uint8_t bonusTile = 10;
constexpr uint8_t explodingTile = 11;
constexpr uint8_t explosionSize = 7;

#endif