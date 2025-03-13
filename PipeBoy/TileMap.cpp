#include "Constants.h"
#include "TileModel.h"
#include "Tile.h"
#include "GameManager.h"
#include "Player.h"

TileMap::TileMap() {
  reset();
}

void TileMap::reset() {
  // We may have to share data with the constructor
  pouringCompletion = 0;
  currentDirection = Direction::Up;
  previousDirection = currentDirection;
  levelTimer = 0;
  explosionTimer = 0;
  state = TileMapState::WaitingToStart;
  nbFramesToFillPipe = NB_FRAMES_TO_FILL_PIPE;
  requireEndingPipe = false;
  haveCompleteEndingPipe = false;

  for (uint8_t x = 0; x < MAX_GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < MAX_GRID_HEIGHT; y++) {
      tileMap[y][x].value = 0;
    }
  }
}

bool TileMap::tryMove(uint8_t& x,
                      uint8_t& y,
                      Direction& curDir,
                      bool& endPipeFound,
                      Direction entry,
                      uint8_t expectedMoveX,
                      uint8_t expectedMoveY,
                      Orientation orientation) {

  uint8_t targetX = x + expectedMoveX;
  uint8_t targetY = y + expectedMoveY;
  const uint8_t nextPipeType = tileMap[targetY][targetX].pipeType;
  TileModel* nextPipe = &pipes[nextPipeType];

  if (nextPipeType == PipeType::BlockingBorderWall) {
    return false;
  }

  if (nextPipeType == PipeType::OpenBorderWall) {
    if (targetX == 0) {
      targetX = MAX_GRID_WIDTH - 2;
    } else if (targetX == MAX_GRID_WIDTH - 1) {
      targetX = 1;
    } else if (targetY == 0) {
      targetY = MAX_GRID_HEIGHT - 2;
    } else if (targetY == MAX_GRID_HEIGHT - 1) {
      targetY = 1;
    }
    nextPipe = &pipes[tileMap[targetY][targetX].pipeType];
  }

  // (When nextPipe = pipe after the blocked one) => If have only one entry, then simply stop here. Don't go even further

  if (nextPipe->haveEntryFrom(entry)) {
    const bool isMultiDirectional = tileMap[y][x].pipeType == PipeType::MultiDirectionalPipe;
    const bool isEmpty = tileMap[y][x].pipeState == PipeState::Empty;

    tileMap[y][x].pipeState = (isMultiDirectional && isEmpty) ? PipeState::HalfFull : PipeState::Full;
    tileMap[targetY][targetX].orientation = orientation;

    x = targetX;
    y = targetY;

    // if no exit, then we simply keep the current direction
    const Direction finalDir = nextPipe->getAnyEntryExcept(entry);
    if (finalDir != Direction::None) {
      curDir = finalDir;
    }

    if (isEndingPipe(tileMap[y][x])) {
      endPipeFound = true;
    }
    return true;
  }
  return false;
}

bool TileMap::findNextPipe(uint8_t& x, uint8_t& y, Direction& direction, bool& endPipeFound) {
  TileModel* currentPipe = &pipes[tileMap[y][x].pipeType];

  if (
    direction == Direction::Left
    && currentPipe->haveEntryFrom(Direction::Left)
    && x > 0
    && tileMap[y][x - 1].pipeType != 0
    && tileMap[y][x - 1].pipeState != PipeState::Exploding) {

    if (tryMove(x, y, direction, endPipeFound, Direction::Right, -1, 0, Orientation::Horizontal)) {
      return true;
    }
  } else if (
    direction == Direction::Up
    && currentPipe->haveEntryFrom(Direction::Up)
    && y > 0
    && tileMap[y - 1][x].pipeType != 0
    && tileMap[y - 1][x].pipeState != PipeState::Exploding) {

    if (tryMove(x, y, direction, endPipeFound, Direction::Down, 0, -1, Orientation::Vertical)) {
      return true;
    }
  } else if (
    direction == Direction::Right
    && currentPipe->haveEntryFrom(Direction::Right)
    && x < MAX_GRID_WIDTH - 1
    && tileMap[y][x + 1].pipeType != 0
    && tileMap[y][x + 1].pipeState != PipeState::Exploding) {

    if (tryMove(x, y, direction, endPipeFound, Direction::Left, 1, 0, Orientation::Horizontal)) {
      return true;
    }
  } else if (
    direction == Direction::Down
    && currentPipe->haveEntryFrom(Direction::Down)
    && y < MAX_GRID_HEIGHT - 1
    && tileMap[y + 1][x].pipeType != 0
    && tileMap[y + 1][x].pipeState != PipeState::Exploding) {

    if (tryMove(x, y, direction, endPipeFound, Direction::Up, 0, 1, Orientation::Vertical)) {
      return true;
    }
  }
  return false;
}

bool TileMap::updatePouringFlow() {
  uint16_t currentNbFramesToFillPipe = nbFramesToFillPipe;

  // Some specific Pipes may take some time to be filled
  if (
    tileMap[currentTileY][currentTileX].pipeType == ReservoirPipe1
    || tileMap[currentTileY][currentTileX].pipeType == ReservoirPipe2) {
    currentNbFramesToFillPipe = nbFramesToFillPipe * 2;
  }
  // Let's make pipe pouring very progressive
  if ((levelTimer % (currentNbFramesToFillPipe / TILE_SIZE)) == 0) {
    pouringCompletion++;
  }
  if (pouringCompletion == TILE_SIZE) {
    return true;
  }
  return false;
}

bool TileMap::deleteNextMutablePipe() {
  for (uint8_t x = 0; x < MAX_GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < MAX_GRID_HEIGHT; y++) {
      if (tileMap[y][x].pipeState == PipeState::Exploding) {
        tileMap[y][x].value = 0;
      } else if (tileMap[y][x].value != 0 && canTileBeErased(x, y)) {
        tileMap[y][x].pipeState = PipeState::Exploding;
        return true;
      }
    }
  }
  return false;
}

void TileMap::setFillPipeFast() {
  nbFramesToFillPipe = NB_FRAMES_TO_FILL_PIPE_FAST;
  if (state == TileMapState::WaitingToStart) {
    state = TileMapState::Flowing;
    levelTimer = 0;
  }
}

void TileMap::setRequireEndingPipe(bool newRequireEndingPipe) {
  requireEndingPipe = newRequireEndingPipe;
}

void TileMap::setPipesLeftToFill(uint8_t newPipesLeftToFill) {
  pipesLeftToFill = newPipesLeftToFill;
}

void TileMap::setDeparturePipe(uint8_t x, uint8_t y) {
  currentTileX = x;
  currentTileY = y;
}

void TileMap::setTileType(uint8_t x, uint8_t y, uint8_t pipe) {
  tileMap[y][x].pipeType = pipe;
}

void TileMap::setTileState(uint8_t x, uint8_t y, PipeState state) {
  tileMap[y][x].pipeState = state;
}

void TileMap::setTileOrientation(uint8_t x, uint8_t y, Orientation ori) {
  tileMap[y][x].orientation = ori;
}

void TileMap::resetExplosionTimer() {
  explosionTimer = 0;
}

bool TileMap::canTileBeErased(uint8_t x, uint8_t y) const {
  const uint8_t tileType = getTileType(x, y);

  return (
    !(x == currentTileX && y == currentTileY)
    && tileType != BlockingBorderWall
    && tileType != OpenBorderWall
    && getTileState(x, y) == PipeState::Empty
    && pipes[tileType].isMutable);
}

uint8_t TileMap::getTileType(uint8_t x, uint8_t y) const {
  return tileMap[y][x].pipeType;
}

PipeState TileMap::getTileState(uint8_t x, uint8_t y) const {
  return tileMap[y][x].pipeState;
}

inline bool TileMap::isLevelSuccessfullyCompleted() {
  return pipesLeftToFill == 0
         && (requireEndingPipe ? haveCompleteEndingPipe : true);
}

void TileMap::update(GameManager* gm) {
  // Timer Update
  levelTimer++;
  if (arduboy.everyXFrames(2)) {
    explosionTimer++;
  }

  // State Management
  if (state == TileMapState::WaitingToStart) {
    if (levelTimer > NB_FRAMES_WAITING_TO_START) {
      state = TileMapState::Flowing;
      levelTimer = 0;
    }
  } else if (state == TileMapState::Flowing) {
    const bool isPipeFull = updatePouringFlow();

    if (isPipeFull) {
      // Store old value of currentDirection before modifying it
      previousDirection = currentDirection;
      const bool foundNextPipe = findNextPipe(currentTileX, currentTileY, currentDirection, haveCompleteEndingPipe);

      if (pipesLeftToFill > 0) {
        pipesLeftToFill--;
      }
      if (!foundNextPipe) {
        explosionTimer = 0;
        levelTimer = 0;
        state = TileMapState::LevelWipe;
        gm->setPlayerState(PlayerState::Blocked);
      } else {
        pouringCompletion = 0;
        gm->increaseScore(1);
      }
    }
  } else if (state == TileMapState::LevelWipe) {
    if (explosionTimer == EXPLOSION_ANIM_NB_FRAME) {
      const bool mutablePipeExists = deleteNextMutablePipe();

      if (!mutablePipeExists) {
        state = TileMapState::LevelEndWait;
        levelTimer = 0;
        gm->beepTones(
          isLevelSuccessfullyCompleted() ? gameVictorySoundSequence : gameEndSoundSequence);
      } else {
        gm->beepTones(explosionSoundSequence);
      }
      explosionTimer = 0;
    }
  } else if (state == TileMapState::LevelEndWait) {
    if (levelTimer / FRAME_RATE > 1) {
      if (isLevelSuccessfullyCompleted()) {
        gm->setNextLevel();
      } else {
        gm->setState(GameState::GameOver);
      }
    }
  }
}

void TileMap::render() const {
  // Render Score display
  arduboy.setCursorY(5);
  arduboy.setCursorX(WIDTH - 18);
  arduboy.print(String(pipesLeftToFill));


  // Render loading of the pipe
  if (state == TileMapState::WaitingToStart) {
    const uint8_t currentBarLevel = PIPE_LOADING_BAR_HEIGHT - ((levelTimer * PIPE_LOADING_BAR_HEIGHT) / NB_FRAMES_WAITING_TO_START);

    arduboy.fillRect(
      115, startGridY + 20, 2, currentBarLevel);
  }

  for (uint8_t x = 0; x < MAX_GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < MAX_GRID_HEIGHT; y++) {

      if (tileMap[y][x].pipeType) {
        const Tile pipe = tileMap[y][x];

        if (tileMap[y][x].pipeType == PipeType::OpenBorderWall) {
          continue;
        }
        if (tileMap[y][x].pipeType == PipeType::BlockingBorderWall) {
          renderBorder(x, y, pipe);

        } else if (currentTileX == x && currentTileY == y) {
          renderPouringSprite(
            x, y, pipe, currentDirection, previousDirection, pouringCompletion);

        } else if (tileMap[y][x].pipeState == PipeState::Empty) {
          renderEmptySprite(x, y, pipe);

        } else if (tileMap[y][x].pipeState == PipeState::HalfFull) {
          renderHalfFullSprite(x, y, pipe);

        } else if (tileMap[y][x].pipeState == PipeState::Full) {
          renderFullSprite(x, y, pipe);

        } else if (tileMap[y][x].pipeState == PipeState::Exploding) {
          sprites.drawOverwrite(
            startGridX + x * TILE_SIZE,
            startGridY + y * TILE_SIZE,
            explosionAnimation,
            explosionTimer % EXPLOSION_ANIM_NB_FRAME);
        }
      }
    }
  }
}
