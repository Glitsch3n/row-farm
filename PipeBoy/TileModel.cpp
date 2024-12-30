#include "Constants.h"
#include "TileModel.h"
#include "DrawRam.h"

const unsigned char PROGMEM pipesEmpty[] = {
  0x0c, 0x32, 0x71, 0x40, 0x40, 0x71, 0x32, 0x0c, 
  // TILE 01
  0x00, 0x00, 0x1f, 0x20, 0x20, 0x27, 0x24, 0x24, 
  // TILE 02
  0x24, 0x24, 0x27, 0x20, 0x20, 0x1f, 0x00, 0x00, 
  // TILE 03
  0x00, 0x00, 0xf8, 0x04, 0x04, 0xe4, 0x24, 0x24, 
  // TILE 04
  0x24, 0x24, 0xe4, 0x04, 0x04, 0xf8, 0x00, 0x00, 
  // TILE 05
  0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 
  // TILE 06
  0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 
  // TILE 07
  0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 
  // TILE 08
  0x00, 0x7e, 0x81, 0x00, 0x00, 0x81, 0x7e, 0x00, 
  // TILE 09
  0xe7, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xe7, 
  // TILE 10
  0xff, 0x81, 0xff, 0x00, 0x00, 0xff, 0x81, 0xff, 
  // TILE 11
  0xff, 0xc3, 0xbd, 0xa5, 0xa5, 0xbd, 0xc3, 0xff, 
  // TILE 12
  0x24, 0x24, 0xe7, 0x24, 0x24, 0xe7, 0x24, 0x24, 
  // TILE 13
  0x00, 0x00, 0xfc, 0x02, 0x02, 0xfc, 0x00, 0x00, 
  // TILE 14
  0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00, 
  // TILE 15
  0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 
  // TILE 16
  0x00, 0x00, 0x3f, 0x40, 0x40, 0x3f, 0x00, 0x00, 
};

const unsigned char PROGMEM pipesFull[] = {
  // TILE 00
  0x0c, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0x3e, 0x0c, 
  // TILE 01
  0x00, 0x00, 0x1f, 0x3f, 0x3f, 0x3f, 0x3c, 0x3c, 
  // TILE 02
  0x3c, 0x3c, 0x3f, 0x3f, 0x3f, 0x1f, 0x00, 0x00, 
  // TILE 03
  0x00, 0x00, 0xf8, 0xfc, 0xfc, 0xfc, 0x3c, 0x3c, 
  // TILE 04
  0x3c, 0x3c, 0xfc, 0xfc, 0xfc, 0xf8, 0x00, 0x00, 
  // TILE 05
  0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 
  // TILE 06
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  // TILE 07
  0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 
  // TILE 08
  0x00, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x00, 
  // TILE 09
  0xff, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xff, 
  // TILE 10
  0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 
  // TILE 11
  0x3c, 0x3c, 0xff, 0x3c, 0x3c, 0xff, 0x3c, 0x3c, 
  // TILE 12
  0x24, 0x24, 0xe7, 0xe7, 0xe7, 0xe7, 0x24, 0x24, 
  // TILE 13
  0x3c, 0x3c, 0xff, 0xff, 0xff, 0xff, 0x3c, 0x3c, 
  // TILE 14
  0x00, 0x00, 0xfc, 0xfe, 0xfe, 0xfc, 0x00, 0x00, 
  // TILE 15
  0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0x00, 
  // TILE 16
  0x00, 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 
  // TILE 17
  0x00, 0x00, 0x3f, 0x7f, 0x7f, 0x3f, 0x00, 0x00, 
};

const uint8_t mutablePipes[] = {
    CircularPipe1,
    CircularPipe2,
    CircularPipe3,
    CircularPipe4,
    VerticalPipe,
    HorizontalPipe,
    MultiDirectionalPipe
};
const uint8_t mutablePipesLen = sizeof(mutablePipes) / sizeof(mutablePipes[0]);

TileModel::TileModel(bool leftEntry, bool upEntry, bool rightEntry, bool downEntry, bool isMut, uint8_t emptyP, uint8_t fullP)
    : isMutable(isMut), emptyPipeIndex(emptyP), fullPipeIndex(fullP) {
  entries[0] = leftEntry;
  entries[1] = upEntry;
  entries[2] = rightEntry;
  entries[3] = downEntry;
}

bool TileModel::haveEntryFrom(Direction dir) const {
  return entries[static_cast<uint8_t>(dir)];
}

Direction TileModel::getAnyEntryExcept(Direction dir) const {
  uint8_t facingDir = (static_cast<uint8_t>(dir) + 2) % 4;
  if (entries[facingDir]) {
    return static_cast<Direction>(facingDir);
  }
  for (uint8_t i = 0; i < 4; ++i) {
    if (i != static_cast<uint8_t>(dir) && entries[i]) {
      return static_cast<Direction>(i);
    }
  }
  return Direction::None;
}


// Should probaly be transformed in a HashMap Instead.
// This works right now, but it have some limitation
// especially if the index/ids are not incremental in the future
const TileModel pipes[] = {
  // This is Non usable Pipe.
  // We need to have it, so pipes[index = 0] targets
  // an empty TileModel
  TileModel(false, false, false, false, true, 0, 0),

  // Starting Pipe
  TileModel(false, true, false, false, false, 0, 0),
  // One Way Pipe
  TileModel(false, false, false, true, false, 13, 14),
  TileModel(true, false, false, false, false, 14, 15),
  TileModel(false, false, true, false, false, 15, 16),
  TileModel(false, true, false, false, false, 16, 17),

  // Reservoir Pipes
  TileModel(true, false, true, false, false, 7, 7),
  TileModel(false, true, false, true, false, 8, 8),
  // Vertical + Horizontal immutable Pipes
  TileModel(true, false, true, false, false, 9, 9),
  TileModel(false, true, false, true, false, 10, 10),
  // A simple wall
  TileModel(false, false, false, false, false, 11, 0),

  // Circular Pipes
  TileModel(false, true, true, false, true, 1, 1),
  TileModel(true, true, false, false, true, 2, 2),
  TileModel(false, false, true, true, true, 3, 3),
  TileModel(true, false, false, true, true, 4, 4),
  // Vertical + Horizontal Pipes
  TileModel(true, false, true, false, true, 5, 5),
  TileModel(false, true, false, true, true, 6, 6),
  // Multi-Directional Pipes
  TileModel(true, true, true, true, true, 12, 11),
};

char* getSpriteFromIndex(uint8_t index, bool isEmpty) {
  const TileModel* pipe = &pipes[index]; 

  if (isEmpty) {
    return &pipesEmpty[pipe->emptyPipeIndex * 8];
  }
  return &pipesFull[pipe->fullPipeIndex * 8];
}

unsigned char blendPipesImages(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  Direction dir,
  Direction previousDir,
  uint8_t amountFilled) {
  
  // Horizontal
  if (dir == Direction::Left || dir == Direction::Right) {
    blendSpriteHorizontally(
      initialPipe, endPipe, blendedPipe, dir == Direction::Right, amountFilled);
  }
  // Vertical
  else if (dir == Direction::Up || dir == Direction::Down) {
    blendSpriteVertically(
      initialPipe, endPipe, blendedPipe, dir == Direction::Up, amountFilled);
  }
}

uint8_t randomPipeWeight[mutablePipesLen];

uint8_t getRandomPipe() {
  uint8_t totalWeight = 0;

  for (int i = 0; i < mutablePipesLen; i++) {
    totalWeight += randomPipeWeight[i];
  }

  uint8_t resetThreshold = mutablePipesLen * 2;
  if (totalWeight >= resetThreshold) {
    // Let's reset the weight if it exceed a certain limit
    for (int i = 0; i < mutablePipesLen; i++) {
      randomPipeWeight[i] = 0;
    }
  }

  uint8_t ran;
  do {
    ran = random(mutablePipesLen);
  } while (randomPipeWeight[ran] > 1);
  randomPipeWeight[ran] += 1;
  return mutablePipes[ran];
}