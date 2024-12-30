#include "Constants.h"
#include "DrawRam.h"
#include "Tile.h"
#include "TileMap.h"

char* getSpriteFromTile(Tile tile, bool isEmpty) {
  return getSpriteFromIndex(tile.pipeType, isEmpty);
}

bool isEndingPipe(Tile pipe) {
  return (
    pipe.pipeType == EndsDownPipe
    || pipe.pipeType == EndsUpPipe
    || pipe.pipeType == EndsRightPipe
    || pipe.pipeType == EndsLeftPipe);
}

void renderBorder(uint8_t x, uint8_t y, Tile pipe) {
  int8_t drawX = startGridX + x * TILE_SIZE;
  int8_t drawY = startGridY + y * TILE_SIZE;

  if (pipe.orientation == Orientation::Vertical) {
    arduboy.drawFastVLine(drawX + (x == 0 ? TILE_SIZE - 1 : 0), drawY, 8);
  } else if (pipe.orientation == Orientation::Horizontal) {
    arduboy.drawFastHLine(drawX, drawY + (y == 0 ? TILE_SIZE - 1 : 0), 8);
  }
}

void renderEmptySprite(uint8_t x, uint8_t y, Tile pipe) {
  arduboy.drawBitmap(
    startGridX + x * TILE_SIZE,
    startGridY + y * TILE_SIZE,
    getSpriteFromTile(pipe, true),
    TILE_SIZE,
    TILE_SIZE);
}

void renderHalfFullSprite(uint8_t x, uint8_t y, Tile pipe) {
  char* pipeSprite;
  if (pipe.pipeType == PipeType::MultiDirectionalPipe) {
    const uint8_t fullPipeIndex = pipes[PipeType::MultiDirectionalPipe].fullPipeIndex;

    if (pipe.orientation == Orientation::Horizontal) {
      pipeSprite = &pipesFull[fullPipeIndex * 8];
    } else if (pipe.orientation == Orientation::Vertical) {
      pipeSprite = &pipesFull[(fullPipeIndex + 1) * 8];
    }
  }
  arduboy.drawBitmap(
    startGridX + x * TILE_SIZE,
    startGridY + y * TILE_SIZE,
    pipeSprite,
    TILE_SIZE,
    TILE_SIZE);
}

void renderFullSprite(uint8_t x, uint8_t y, Tile pipe) {
  char* pipeSprite;

  if (pipe.pipeType == PipeType::MultiDirectionalPipe) {
    const uint8_t fullPipeIndex = pipes[PipeType::MultiDirectionalPipe].fullPipeIndex;
    pipeSprite = &pipesFull[(fullPipeIndex + 2) * 8];
  } else {
    pipeSprite = getSpriteFromTile(pipe, false);
  }
  arduboy.drawBitmap(
    startGridX + x * TILE_SIZE,
    startGridY + y * TILE_SIZE,
    pipeSprite,
    TILE_SIZE,
    TILE_SIZE);
}

void renderPouringSprite(
  uint8_t x,
  uint8_t y,
  Tile pipe,
  Direction currentDirection,
  Direction previousDirection,
  uint8_t pouringCompletion) {
  unsigned char pipeSprite[8];

  char* startSprite;
  char* endSprite;

  if (pipe.pipeType == PipeType::MultiDirectionalPipe) {
    const uint8_t fullPipeIndex = pipes[PipeType::MultiDirectionalPipe].fullPipeIndex;

    if (pipe.pipeState == PipeState::Empty) {
      startSprite = getSpriteFromTile(pipe, true);
      if (pipe.orientation == Orientation::Horizontal) {
        endSprite = &pipesFull[fullPipeIndex * 8];
      } else if (pipe.orientation == Orientation::Vertical) {
        endSprite = &pipesFull[(fullPipeIndex + 1) * 8];
      }
    } else {
      if (pipe.orientation == Orientation::Horizontal) {
        startSprite = &pipesFull[(fullPipeIndex + 1) * 8];
      } else if (pipe.orientation == Orientation::Vertical) {
        startSprite = &pipesFull[(fullPipeIndex)*8];
      }
      endSprite = &pipesFull[(fullPipeIndex + 2) * 8];
    }
  } else {
    startSprite = getSpriteFromTile(pipe, true);
    endSprite = getSpriteFromTile(pipe, false);
  }
  blendPipesImages(
    startSprite,
    endSprite,
    pipeSprite,
    currentDirection,
    previousDirection,
    pouringCompletion);
  drawBitmapFromRam(
    startGridX + x * TILE_SIZE,
    startGridY + y * TILE_SIZE,
    pipeSprite,
    TILE_SIZE,
    TILE_SIZE,
    WHITE);
}