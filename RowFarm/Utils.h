#ifndef UTILS_H
#define UTILS_H

#include <SpritesB.h>
#include <EEPROM.h>
#include "Constants.h"
#include "Common.h"

uint8_t getXCoordinate(uint8_t coord) {
  return 2 + (coord * 9);
}

uint8_t getYCoordinate(uint8_t coord) {
  return (coord * 9) + 9;
}

void beepTone(uint16_t count, uint8_t dur) {
  if (soundEnabled) {
    beep.tone(count, dur);
  }
}

void setGameState(GameState newState) {
  currentGameState = newState;
  firstGameStateDraw = true;
}

void updateHighScore(uint16_t score) {
  uint16_t newHighScores[highScoreSize] = { 0 };
  uint8_t offset = 0;

  for (uint8_t i = 0; i < highScoreSize; i++) {
    if (!offset && score > highScores[i]) {
      newHighScores[i] = score;
      offset = 1;
    } else {
      newHighScores[i] = highScores[i - offset];
    }
  }

  for (uint8_t i = 0; i < highScoreSize; i++) {
    highScores[i] = newHighScores[i];
  }
  EEPROM.put(EEPROM_STORAGE_SPACE_START + 1, highScores);
}

void initHighScore() {
  bool flagIsCleanEEPROM;

  EEPROM.get(EEPROM_STORAGE_SPACE_START, flagIsCleanEEPROM);
  if (flagIsCleanEEPROM == true) {  // It means that the EEPROM have never been used
    for (uint8_t i = 0; i < highScoreSize; i++) {
      highScores[i] = 0;
    }
    EEPROM.put(EEPROM_STORAGE_SPACE_START, false);
    EEPROM.put(EEPROM_STORAGE_SPACE_START + 1, highScores);
  }
}

#endif