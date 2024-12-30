#ifndef TILE_H
#define TILE_H

enum class PipeState;
enum class Orientation;
enum class Direction;

typedef union {
  uint16_t value;
  struct {
    uint8_t pipeType : 5;
    PipeState pipeState : 4;
    Orientation orientation : 4;
  };
} Tile;

bool isEndingPipe(Tile pipe);

void renderBorder(uint8_t x, uint8_t y, Tile pipe);
void renderEmptySprite(uint8_t x, uint8_t y, Tile pipe);
void renderHalfFullSprite(uint8_t x, uint8_t y, Tile pipe);
void renderFullSprite(uint8_t x, uint8_t y, Tile pipe);
void renderPouringSprite(
  uint8_t x,
  uint8_t y,
  Tile pipe,
  Direction currentDirection,
  Direction previousDirection,
  uint8_t pouringCompletion);

#endif