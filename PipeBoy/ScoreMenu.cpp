#include "Constants.h"
#include "GameManager.h"
#include "ScoreMenu.h"

uint16_t ScoreMenu::highScores[highScoreSize] = { 0 };

ScoreMenu::ScoreMenu() {
  initHighScore();
}

void ScoreMenu::render() {
  arduboy.clear();
  arduboy.setCursor(30, 0);
  arduboy.print(F("HIGH SCORES"));
  arduboy.drawFastHLine(0, 10, WIDTH);

  for (uint8_t i = 0; i < highScoreSize; i++) {
    arduboy.setCursor(5, (i * 15) + 16);
    arduboy.print("[");
    arduboy.print(i + 1);
    arduboy.print("] - ");
    arduboy.print(highScores[i]);
  }
}

void ScoreMenu::update(GameManager *gm) {
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    gm->setState(GameState::MainMenu);
  }
}

void ScoreMenu::updateHighScore(uint16_t score) {
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

void ScoreMenu::initHighScore() {
  bool flagIsCleanEEPROM;
  EEPROM.get(EEPROM_STORAGE_SPACE_START, flagIsCleanEEPROM);

  if (flagIsCleanEEPROM) {
    for (uint8_t i = 0; i < highScoreSize; i++) {
      highScores[i] = 0;
    }
    EEPROM.put(EEPROM_STORAGE_SPACE_START, false);
    EEPROM.put(EEPROM_STORAGE_SPACE_START + 1, highScores);
  }
}
