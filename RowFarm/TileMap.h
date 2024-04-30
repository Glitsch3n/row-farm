#ifndef TILEMAP_H
#define TILEMAP_H

#include <Arduboy2.h>
#include <SpritesB.h>
#include "Utils.h"
#include "Tile.h"

struct TileBlock {
  uint8_t x = 0;
  uint8_t y = 0;

  TileBlock() {
    x = 0;
    y = 0;
  }

  TileBlock(uint8_t xPos, uint8_t yPos) {
    x = xPos;
    y = yPos;
  }
};

// A stack in which we can push/pop tileBlock.
// This allow us to track tile that are matching together
Stack<TileBlock, maxGridHeight * maxGridHeight> tileBuffer;

class TileMap {
private:
  uint8_t tileMap[maxGridHeight][maxGridWidth];
  int8_t explosionFrame = -1;

  void checkForDirection(uint8_t startX, uint8_t startY, int8_t stepX, int8_t stepY, uint8_t type) {
    uint8_t oriBufferCursor = tileBuffer.getSize();

    for (
      uint8_t x = startX + stepX, y = startY + stepY;
      x < maxGridWidth && x >= 0 && y < maxGridHeight && y >= 0 && getTileTypeFrom(x, y) == type /*&& !getTileBlinkingFlagFrom(x, y)*/;
      x += stepX, y += stepY) {

      tileBuffer.push(TileBlock(x, y));
    }

    // Then do the same thing with the opposite direction
    stepX = -stepX,
    stepY = -stepY;
    for (
      uint8_t x = startX + stepX, y = startY + stepY;
      x < maxGridWidth && x >= 0 && y < maxGridHeight && y >= 0 && getTileTypeFrom(x, y) == type /*&& !getTileBlinkingFlagFrom(x, y)*/;
      x += stepX, y += stepY) {

      tileBuffer.push(TileBlock(x, y));
    }

    // Let's reset the tileBuffer in case it's < 2
    uint8_t diff = tileBuffer.getSize() - oriBufferCursor;
    if (diff < maxPlayerRowWidth - 1) {
      tileBuffer.pop(diff);
    }
  }

  bool checkMatchingTilesForGivenBlock(uint8_t oriX, uint8_t oriY) {
    const uint8_t type = getTileTypeFrom(oriX, oriY);
    const bool isBlinking = getTileBlinkingFlagFrom(oriX, oriY);
    const uint8_t oriBufferCursor = tileBuffer.getSize();
    bool matchingTiles = false;

    if (isBlinking) {
      return 0;
    }
    checkForDirection(oriX, oriY, 1, 0, type);   // check horizontal
    checkForDirection(oriX, oriY, 0, 1, type);   // check vertical
    checkForDirection(oriX, oriY, 1, 1, type);   // check down-right diagonal to top-left diagonal
    checkForDirection(oriX, oriY, -1, 1, type);  // check down-left to top-right diagonal

    if (tileBuffer.getSize() - oriBufferCursor >= maxPlayerRowWidth - 1) {
      tileBuffer.push(TileBlock(oriX, oriY));

      for (uint8_t i = 0; i < tileBuffer.getSize(); i++) {
        const TileBlock currentTile = tileBuffer.getItem(i);
        setTileBlinkingFlag(tileMap[currentTile.y][currentTile.x], true);
      }
      matchingTiles = true;
    }
    tileBuffer.clear();
    return matchingTiles;
  }

  template<typename Func>
  void iterateOnGrid(Func myFunc) {
    for (uint8_t y = 0; y < maxGridHeight; y++) {
      for (uint8_t x = 0; x < maxGridWidth; x++) {
        if (getTileTypeFrom(x, y) == 0) {
          break;
        } else {
          myFunc(x, y);
        }
      }
    }
  }

public:
  void render() {
    for (uint8_t y = 0; y < maxGridHeight; y++) {
      for (uint8_t x = 0; x < maxGridWidth; x++) {
        const uint8_t tileType = getTileTypeFrom(x, y);
        const uint8_t tileIsBlinking = getTileBlinkingFlagFrom(x, y);

        if (tileType == explodingTile) {
          sprites.drawOverwrite(getXCoordinate(x), getYCoordinate(y), explosion, explosionFrame);
        } else {
          drawTile(getXCoordinate(x), getYCoordinate(y), tileType, tileIsBlinking);
        }
      }
    }
  }

  void update() {
    if (arduboy.everyXFrames(2) && explosionFrame >= 0) {
      if (explosionFrame < explosionSize) {
        explosionFrame++;
      }
    }
  }

  bool findMatchingTiles() {
    bool matchingTiles = false;

    auto lambda = [&, &matchingTiles](uint8_t x, uint8_t y) {
      bool isTileMatchingOtherTiles = checkMatchingTilesForGivenBlock(x, y);
      if (isTileMatchingOtherTiles) {
        matchingTiles = true;
      }
    };
    iterateOnGrid(lambda);
    return matchingTiles;
  }

  bool findMatchingTilesWith(uint8_t typeToMatch) {
    bool matchingTiles = false;

    auto lambda = [&, &matchingTiles, typeToMatch](uint8_t x, uint8_t y) {
      if (getTileTypeFrom(x, y) == typeToMatch) {
        setTileBlinkingFlag(tileMap[y][x], true);
        matchingTiles = true;
      }
    };
    iterateOnGrid(lambda);
    return matchingTiles;
  }

  void doCleanTiles() {
    for (uint8_t y = 0; y < maxGridHeight; y++) {
      uint8_t counter = 0;
      uint8_t horizontalTileStack[maxGridWidth] = { 0 };

      // Adding tiles to the stack
      for (uint8_t x = 0; x < maxGridWidth; x++) {
        if (getTileTypeFrom(x, y) != 0 && getTileTypeFrom(x, y) != explodingTile && !getTileBlinkingFlagFrom(x, y)) {
          horizontalTileStack[counter] = tileMap[y][x];
          counter++;
        }
      }

      // Adding stack to the tiles
      for (uint8_t x = 0; x < maxGridWidth; x++) {
        tileMap[y][x] = horizontalTileStack[x];
      }
    }
  }

  uint16_t doDestroyTiles() {
    explosionFrame = 0;
    uint16_t nbMatchingTiles = 0;

    auto lambda = [&, &nbMatchingTiles](uint8_t x, uint8_t y) {
      if (getTileBlinkingFlagFrom(x, y)) {
        setTileTypeFrom(x, y, explodingTile);
        nbMatchingTiles++;
      }
    };
    iterateOnGrid(lambda);
    return nbMatchingTiles;
  }

  void doBlinkAllTiles() {
    auto lambda = [&](uint8_t x, uint8_t y) {
      setTileBlinkingFlag(tileMap[y][x], true);
    };
    iterateOnGrid(lambda);
  }

  /*
  ** Some Getters / Setters
  */

  bool getTileBlinkingFlagFrom(uint8_t x, uint8_t y) {
    return getTileBlinkingFlag(tileMap[y][x]);
  }

  uint8_t getTileTypeFrom(uint8_t x, uint8_t y) {
    return getTileType(tileMap[y][x]);
  }

  void setTileTypeFrom(uint8_t x, uint8_t y, uint8_t type) {
    tileMap[y][x] = type;
  }
};

#endif