#include <Arduboy2.h>
#include "Constants.h"
#include "TileModel.h"
#include "Tile.h"
#include "TileMap.h"
#include "Level.h"
#include "LevelBuilder.h"

bool checkIfTilesAround(TileMap *tm, int8_t x, int8_t y) {
  constexpr int8_t OFFSETS_TILE[8][2] = {
    { -1, 0 },
    { -1, -1 },
    { 0, -1 },
    { 1, -1 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 },
    { -1, 1 },
  };

  for (uint8_t i = 0; i < 8; i++) {
    const int8_t newX = x + OFFSETS_TILE[i][0];
    const int8_t newY = y + OFFSETS_TILE[i][1];
    const int8_t pipeType = tm->getTileType(newX, newY);

    if (
      pipeType != PipeType::BlockingBorderWall
      && pipeType != PipeType::OpenBorderWall
      && pipeType != 0) {
      return true;
    }
  }
  return false;
}

bool checkIfOpenBorderWallOnSide(TileMap *tm, int8_t x, int8_t y) {
  constexpr int8_t OFFSETS_TILE[4][2] = {
    { -1, 0 },
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
  };

  for (uint8_t i = 0; i < 4; i++) {
    const int8_t newX = x + OFFSETS_TILE[i][0];
    const int8_t newY = y + OFFSETS_TILE[i][1];
    const int8_t pipeType = tm->getTileType(newX, newY);

    if (pipeType == PipeType::OpenBorderWall) {
      return true;
    }
  }
  return false;
}

void setOpenBorderWalls(TileMap *tm, uint8_t nbWallHole) {
  for (uint8_t i = 0; i < nbWallHole; i++) {
    Orientation ori;
    uint8_t positionWall;
    uint8_t xWallPos;
    uint8_t yWallPos;

    do {
      ori = (random(0, 1) == 0)
              ? Orientation::Horizontal
              : Orientation::Vertical;

      positionWall = random(
        ori == Orientation::Horizontal
          ? (MAX_GRID_WIDTH - 1)
          : (MAX_GRID_HEIGHT - 1));

      if (ori == Orientation::Horizontal) {
        xWallPos = positionWall;
        yWallPos = 0;
      } else {
        xWallPos = 0;
        yWallPos = positionWall;
      }

      tm->setTileType(xWallPos, yWallPos, PipeType::OpenBorderWall);

      // Also set the opposite side
      uint8_t xWallPosOpposite = ori == Orientation::Horizontal ? xWallPos : MAX_GRID_WIDTH - 1;
      uint8_t yWallPosOpposite = ori == Orientation::Horizontal ? MAX_GRID_HEIGHT - 1 : yWallPos;
      tm->setTileType(
        xWallPosOpposite, yWallPosOpposite, PipeType::OpenBorderWall);

    } while (
      tm->getTileType(xWallPos, yWallPos) == PipeType::BlockingBorderWall);
  }
}

void setBorderWalls(TileMap *tm) {
  for (uint8_t y = 0; y < MAX_GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < MAX_GRID_WIDTH; x++) {
      bool isBorder = (y == 0 || y == MAX_GRID_HEIGHT - 1 || x == 0 || x == MAX_GRID_WIDTH - 1);

      bool isCorner = (x == 0 && (y == 0 || y == MAX_GRID_HEIGHT - 1)) || (x == MAX_GRID_WIDTH - 1 && (y == 0 || y == MAX_GRID_HEIGHT - 1));

      if (isBorder) {
        tm->setTileType(x, y, PipeType::BlockingBorderWall);
        if (!isCorner) {
          if (y == 0 || y == MAX_GRID_HEIGHT - 1) {
            tm->setTileOrientation(x, y, Orientation::Horizontal);
          } else {
            tm->setTileOrientation(x, y, Orientation::Vertical);
          }
        }
      }
    }
  }
}

void setNRandomItem(
  TileMap *tm,
  uint8_t nbItem,
  PipeType *typeItems,
  uint8_t numTypes,
  uint8_t minX = 0,
  uint8_t maxX = MAX_GRID_WIDTH,
  uint8_t minY = 0,
  uint8_t maxY = MAX_GRID_HEIGHT) {

  for (uint8_t i = 0; i < nbItem; i++) {
    int8_t randX;
    int8_t randY;
    PipeType randType = typeItems[random(numTypes)];

    uint8_t attempts = 0;
    // Levels are made to avoid this issue. But let's add a safe check
    const uint8_t maxAttempts = 100;

    do {
      randX = random(minX, maxX);
      randY = random(minY, maxY);

      attempts++;
    } while (
      (
        tm->getTileType(randX, randY) != 0
        || checkIfTilesAround(tm, randX, randY)
        || checkIfOpenBorderWallOnSide(tm, randX, randY))
      && attempts < maxAttempts);

    if (attempts < maxAttempts) {
      tm->setTileType(randX, randY, randType);
    } else {
#ifdef DEBUG_MODE
      Serial.println(F("Failed to place tile after maximum attempts"));
#endif
    }
  }
}

bool locateStartingPipe(TileMap *tm, uint8_t &posX, uint8_t &posY) {
  for (uint8_t y = 0; y < MAX_GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < MAX_GRID_WIDTH; x++) {
      if (tm->getTileType(x, y) == PipeType::StartingPipe) {
        posX = x;
        posY = y;
        return true;
      }
    }
  }
  return false;
}

void initPipingNetwork(TileMap *tm, uint8_t nextLevel) {
  Level level;
  memcpy_P(&level, &levels[nextLevel], sizeof(Level));
  tm->setPipesLeftToFill(level.pipeDistance);

  setBorderWalls(tm);
  setOpenBorderWalls(tm, level.nbWallHole);

  // Set Starting Pipe
  const PipeType startingPipe[] = {
    PipeType::StartingPipe,
  };
  setNRandomItem(tm, 1, startingPipe, 1,
                 2, MAX_GRID_WIDTH - 2, 2, MAX_GRID_HEIGHT - 2);
  uint8_t departurePosX;
  uint8_t departurePosY;
  locateStartingPipe(tm, departurePosX, departurePosY);
  tm->setDeparturePipe(departurePosX, departurePosY);


  // Set Ending Pipe if exists
  if (level.requiredEndingPipe) {
    const PipeType endingPipes[] = {
      PipeType::EndsDownPipe,
      PipeType::EndsUpPipe,
      PipeType::EndsRightPipe,
      PipeType::EndsLeftPipe,
    };
    setNRandomItem(tm, 1, endingPipes, 4,
                   2, MAX_GRID_WIDTH - 2, 2, MAX_GRID_HEIGHT - 2);
    tm->setRequireEndingPipe(true);
  }

  // Set Reservoir Pipes
  const PipeType reservoirPipes[] = { PipeType::ReservoirPipe1, PipeType::ReservoirPipe2 };
  setNRandomItem(tm, level.nbReservoir, reservoirPipes, 2,
                 2, MAX_GRID_WIDTH - 2, 2, MAX_GRID_HEIGHT - 2);

  // Set Immutable Pipes
  const PipeType immutablePipes[] = { PipeType::ImmutableVerticalPipe, PipeType::ImmutableHorizontalPipe };
  setNRandomItem(tm, level.nbOneWayPipe, immutablePipes, 2,
                 2, MAX_GRID_WIDTH - 2, 2, MAX_GRID_HEIGHT - 2);

  // Set Wall
  const PipeType wallPipe[] = { PipeType::WallPipe };
  setNRandomItem(tm, level.nbBlock, wallPipe, 1);
}