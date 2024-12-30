#ifndef PLAYER_H
#define PLAYER_H

#include "TileMap.h"
#include "Queue.h"
#include "Observable.h"

class GameManager;
class NetworkManager;

constexpr uint8_t FRAME_RESET_VALUE = 6;
constexpr uint8_t PIPE_UI_DISTANCE = 12;

enum class PlayerState { Running = 0,
                         AddingPipe,
                         ReplacingPipe,
                         Blocked };

class Player : public Observable {
private:
  TileMap *tileMapRef;
  PlayerState state = PlayerState::Running;
  uint8_t pauseTimer = 0;
  uint8_t inputFrameCounter = 0;
  uint8_t xCursorPos = MAX_GRID_WIDTH / 2;
  uint8_t yCursorPos = MAX_GRID_HEIGHT / 2;
  bool isPlayerTurn = true;

  // Variables related to the UI
  Queue<uint8_t, NB_MAX_PIPE_DISPLAYED> nextPipes;
  uint8_t columnDisplayNextPipeY = 0;
  bool columnDisplayMove = false;

  bool canMove();
  void increaseInputFrameCounter();
  void handleInput(GameManager *gm);
  void handleState();

public:
  Player(TileMap *ref);

  void render() const;
  void update(GameManager *gm);
  void reset();
  void pushTile();
  void setPosition(uint8_t x, uint8_t y);
  void setState(PlayerState state);
  void setIsPlayerTurn(bool isTurn);
  bool getIsPlayerTurn() const;
};

#endif
