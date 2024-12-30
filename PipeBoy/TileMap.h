#ifndef TILEMAP_H
#define TILEMAP_H

#include "Constants.h"
#include "Tile.h"
#include "TileModel.h"

class GameManager;

constexpr uint16_t NB_FRAMES_WAITING_TO_START = FRAME_RATE * 25;
constexpr uint16_t NB_FRAMES_TO_FILL_PIPE = FRAME_RATE * 4;
constexpr uint16_t NB_FRAMES_TO_FILL_PIPE_FAST = FRAME_RATE / 2;

enum class TileMapState {
  WaitingToStart = 0,
  Flowing,
  LevelWipe,
  LevelEndWait,
};


class TileMap {
private:
  Tile tileMap[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
  bool requireEndingPipe;
  bool haveCompleteEndingPipe;

  uint8_t currentTileX;
  uint8_t currentTileY;
  uint8_t pouringCompletion;
  Direction currentDirection;
  Direction previousDirection;
  uint16_t levelTimer;
  uint16_t nbFramesToFillPipe;
  TileMapState state;

  uint8_t explosionTimer;
  uint8_t pipesLeftToFill;

  bool tryMove(uint8_t& x, uint8_t& y, Direction& curDir, bool& endPipeFound, Direction entry, uint8_t expectedMoveX, uint8_t expectedMoveY, Orientation orientation);
  bool findNextPipe(uint8_t& x, uint8_t& y, Direction& direction, bool& endPipeFound);
  bool updatePouringFlow();
  bool deleteNextMutablePipe();

public:
  TileMap();

  uint8_t getTileType(uint8_t x, uint8_t y) const;
  PipeState getTileState(uint8_t x, uint8_t y) const;
  bool canTileBeErased(uint8_t x, uint8_t y) const;
  void render() const;

  void setDeparturePipe(uint8_t x, uint8_t y);
  void setRequireEndingPipe(bool newRequireEndingPipe);
  void setPipesLeftToFill(uint8_t pipesLeftToFill);
  void setTileType(uint8_t x, uint8_t y, uint8_t pipe);
  void setTileState(uint8_t x, uint8_t y, PipeState state);
  void setTileOrientation(uint8_t x, uint8_t y, Orientation ori);
  void setFillPipeFast();
  void resetExplosionTimer();
  void update(GameManager* gm);
  void reset();
};

#endif
