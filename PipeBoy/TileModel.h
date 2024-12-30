#ifndef TILEMODEL_H
#define TILEMODEL_H

#include <Arduino.h>

// Enumerations
enum class Orientation {
  None = 0,
  Horizontal = 1,
  Vertical = 2,
};

enum class Direction {
  Left = 0,
  Up = 1,
  Right = 2,
  Down = 3,
  None = 4
};

enum class PipeState {
  Empty = 0,
  Full = 1,
  Pouring = 2,
  Exploding = 3,
  HalfFull = 4
};

// Pipe Types and related constants
enum PipeType : uint8_t {
    NonUsablePipe = 0,
    StartingPipe,
    EndsUpPipe,
    EndsRightPipe,
    EndsLeftPipe,
    EndsDownPipe,
    ReservoirPipe1,
    ReservoirPipe2,
    ImmutableVerticalPipe,
    ImmutableHorizontalPipe,
    WallPipe,
    CircularPipe1,
    CircularPipe2,
    CircularPipe3,
    CircularPipe4,
    VerticalPipe,
    HorizontalPipe,
    MultiDirectionalPipe,
    BlockingBorderWall,
    OpenBorderWall,
};

// Immutable constants
extern const unsigned char PROGMEM pipesEmpty[];
extern const unsigned char PROGMEM pipesFull[];

extern const uint8_t mutablePipes[];
extern const uint8_t mutablePipesLen;

// Pipe Model Struct
struct TileModel {
  bool entries[4];
  bool isMutable;
  uint8_t emptyPipeIndex;
  uint8_t fullPipeIndex;

  bool haveEntryFrom(Direction dir) const;
  Direction getAnyEntryExcept(Direction dir) const;

  TileModel(bool leftEntry, bool upEntry, bool rightEntry, bool downEntry, bool isMut, uint8_t emptyP, uint8_t fullP);
};

// External definitions
extern const TileModel pipes[];
extern uint8_t randomPipeWeight[];

char* getSpriteFromIndex(uint8_t index, bool isEmpty);

unsigned char blendPipesImages(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  Direction dir,
  Direction previousDir,
  uint8_t amountFilled);

uint8_t getRandomPipe();

#endif
