#ifndef PLAYER_H
#define PLAYER_H

#include <SpritesB.h>
#include "Constants.h"
#include "Utils.h"
#include "TileMap.h"

enum class ReasonPause { NoPause,
                         BlinkingState,   // (Just Waitng time OR Blinking Time)
                         ExplosionState,  // (Explosion Time)
                         GameOver };

class Player {
private:
  int8_t pausingTime = 0;  // -1 stands for an infinite pause
  ReasonPause pausingReason = ReasonPause::NoPause;
  uint8_t inputFrameCounter = 0;
  uint8_t xStartGridPos = maxGridWidth;  // The position where we start to draw the blocks
  uint8_t yGridPos = 3;                  // The current yPos
  uint8_t playerSpeed;
  uint16_t numberOfClearSinceLastBonus = 0;
  bool justFullyVisible = false;

  uint8_t rowContent[maxPlayerRowWidth];
  uint8_t nextRowContent[maxPlayerRowWidth];
  TileMap *tileMapRef;

  void handleInput() {
    if (arduboy.justPressed(A_BUTTON) == true) {
      uint8_t contentRowBuffer[maxPlayerRowWidth] = { 0 };
      for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
        contentRowBuffer[x] = rowContent[(x + 2) % maxPlayerRowWidth];
      }
      memcpy(rowContent, contentRowBuffer, sizeof(rowContent));
      beepTone(beep.freq(200), 3);
    }
    if (arduboy.pressed(LEFT_BUTTON)) {
      playerSpeed = 3;
    }

    if (
      arduboy.pressed(UP_BUTTON) == true and yGridPos > 0
      and (xStartGridPos >= maxGridWidth or tileMapRef->getTileTypeFrom(xStartGridPos, yGridPos - 1) == 0)) {
      // We allow to move the player even when he is outside
      if (inputFrameCounter == 0 || (inputFrameCounter >= 6 && inputFrameCounter % 3 == 0)) {
        yGridPos -= 1;
      }
      inputFrameCounter++;
    } else if (
      arduboy.pressed(DOWN_BUTTON) == true and yGridPos < maxGridHeight - 1
      and (xStartGridPos >= maxGridWidth or tileMapRef->getTileTypeFrom(xStartGridPos, yGridPos + 1) == 0)) {
      // We allow to move the player even when he is outside
      if (inputFrameCounter == 0 || (inputFrameCounter >= 6 && inputFrameCounter % 3 == 0)) {
        yGridPos += 1;
      }
      inputFrameCounter++;
    } else {
      inputFrameCounter = 0;
    }
  }

  void updateIfCollisions() {
    // When the player is completely outside (overflow), we don't do any check
    if (xStartGridPos > maxGridWidth) {
      return;
    }
    if (xStartGridPos == 0 or tileMapRef->getTileTypeFrom(xStartGridPos - 1, yGridPos) != 0) {
      // Let's copy the content of the player in the tile map
      for (uint8_t x = 0; x < maxPlayerRowWidth && xStartGridPos + x < maxGridWidth; x++) {
        tileMapRef->setTileTypeFrom(xStartGridPos + x, yGridPos, rowContent[x]);
      }

      // This is a partial overflow
      if (xStartGridPos + maxPlayerRowWidth > maxGridWidth) {
        pausingTime = 75;
        pausingReason = ReasonPause::GameOver;
        tileMapRef->doBlinkAllTiles();
      }

      // We only want to `clear` the player in case there is no GameOver
      if (pausingReason != ReasonPause::GameOver) {
        bool matchingTiles = false;

        if (rowContent[0] == bonusTile) {
          uint8_t tileTypeColliding = xStartGridPos == 0 ? 0 : tileMapRef->getTileTypeFrom(xStartGridPos - 1, yGridPos);
          matchingTiles = tileMapRef->findMatchingTilesWith(tileTypeColliding);
        }
        matchingTiles = tileMapRef->findMatchingTiles() ? true : matchingTiles;

        for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
          rowContent[x] = nextRowContent[x];
        }
        beepTone(beep.freq(100), 3);
        clear();

        pausingTime = playerDefaultPausingTime;
        if (matchingTiles) {
          pausingReason = ReasonPause::BlinkingState;
        } else {
          pausingReason = ReasonPause::NoPause;
        }
      }
    }
  }

  bool isNextTileBonus() {
    const uint8_t currentLevel = getLevel();
    // Higher the level is, Lower is the chance to generate a bonus
    const uint8_t bonusRangeLuck = currentLevel > 40 ? 40 : currentLevel;

    // Don't allow to generate 2 bonus one after the other
    // Let's wait at least 30 generation
    const bool isBonusTile =
      numberOfClearSinceLastBonus > 20 ? random(0, bonusRangeLuck) == 0 : false;
    return isBonusTile;
  }

  void updateIfFullyVisible() {
    if (xStartGridPos + maxPlayerRowWidth <= maxGridWidth
        && justFullyVisible == false) {
      const bool isBonusTile = isNextTileBonus();

      for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
        if (isBonusTile) {
          nextRowContent[x] = bonusTile;
        } else {
          nextRowContent[x] = random(1, maxBlockType + 1);
        }
      }
      justFullyVisible = true;
      numberOfClearSinceLastBonus =
        isBonusTile ? 0 : numberOfClearSinceLastBonus + 1;
    }
  }

  void resolvePause() {
    if (pausingReason == ReasonPause::GameOver) {
      tileMapRef->doCleanTiles();
      updateHighScore(score);
      setGameState(GameState::GameOver);
      return;

    } else if (pausingReason == ReasonPause::BlinkingState) {
      beepTone(beep.freq(50), 5);
      addScore(tileMapRef->doDestroyTiles());
      pausingReason = ReasonPause::ExplosionState;
      pausingTime = 20;

    } else if (pausingReason == ReasonPause::ExplosionState) {
      tileMapRef->doCleanTiles();
      uint16_t matchingTiles = tileMapRef->findMatchingTiles();
      pausingTime = playerDefaultPausingTime;
      pausingReason = ReasonPause::NoPause;

      //addScore(nbMatchingTiles);
      if (matchingTiles) {
        pausingTime = playerDefaultPausingTime;
        pausingReason = ReasonPause::BlinkingState;
      } else {
        pausingReason = ReasonPause::NoPause;
      }
    }
  }

  void clear() {
    xStartGridPos = maxGridWidth + 1;
    yGridPos = 3;
    justFullyVisible = false;
    pausingTime = 0;
    pausingReason = ReasonPause::NoPause;
  }

  /*
  ** Getters & Setters
  */

  uint8_t computeSpeed() {
    // Always reset playerSpeed
    playerSpeed = playerDefaultSpeed;
    uint8_t frameFactor = getLevel();

    if (frameFactor > 40) {
      frameFactor = 40;
    }
    // reduce of 2 frames every 3 levels
    playerSpeed = playerDefaultSpeed - ((frameFactor / 3) * 2);
  }

  bool isPaused() {
    return pausingTime > 0 || pausingTime == -1;
  }

  void elapsePause() {
    if (pausingTime != -1) {
      pausingTime--;
    }
  }

  void addScore(uint16_t incrScore) {
    if (score >= 10000) {
      score = 10000;
      return;
    }
    score += incrScore;
  }

public:
  uint16_t score = 0;

  Player(TileMap *ref) {
    tileMapRef = ref;
    reset();
  }

  void render() {
    // render Player + Related objects
    if (!isPaused()) {

      for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
        drawTile(getXCoordinate(xStartGridPos + x), getYCoordinate(yGridPos), rowContent[x], false);
      }

      // Drawl Lines
      uint8_t xStartLine = 0;
      for (uint8_t x = 0; x < maxGridWidth && tileMapRef->getTileTypeFrom(x, yGridPos); x++) {
        xStartLine = x + 1;
      }
      const uint8_t lineWidth = getXCoordinate(xStartGridPos) - getXCoordinate(xStartLine);
      arduboy.drawFastHLine(
        getXCoordinate(xStartLine), getYCoordinate(yGridPos) + 3, lineWidth);
      arduboy.drawFastHLine(
        getXCoordinate(xStartLine), getYCoordinate(yGridPos) + 4, lineWidth);
    }

    // render futur tiles
    arduboy.setCursorX(55);
    arduboy.print("Next:<");
    arduboy.setCursorX(119);
    arduboy.print(">");
    for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
      drawTile(getXCoordinate(x + (maxGridWidth - 4)), 0, nextRowContent[x], false);
    }
  }


  void update() {
    computeSpeed();

    if (!isPaused()) {
      handleInput();
      if (arduboy.everyXFrames(playerSpeed)) {
        updateIfCollisions();
        updateIfFullyVisible();
        xStartGridPos -= 1;
      }
    } else {
      elapsePause();
      if (pausingTime == 0) {
        resolvePause();
      }
    }
  }

  void reset() {
    for (uint8_t x = 0; x < maxPlayerRowWidth; x++) {
      const uint8_t rand = random(1, maxBlockType + 1);
      rowContent[x] = rand;
      nextRowContent[x] = rand;
    }
    clear();
    score = 0;
    pausingTime = playerDefaultPausingTime;
    numberOfClearSinceLastBonus = 0;
  }

  uint16_t getLevel() {
    return score / 12;
  };
};

#endif