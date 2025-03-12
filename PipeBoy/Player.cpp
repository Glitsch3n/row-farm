#include "Player.h"
#include "GameManager.h"
#include "NetworkManager.h"
#include "Observable.h"

Player::Player(TileMap *ref)
  : tileMapRef(ref) {
  reset();
}

void Player::reset() {
  // We may have to share data with the constructor
  columnDisplayNextPipeY = 0;
  state = PlayerState::Running;
  columnDisplayMove = false;
  xCursorPos = MAX_GRID_WIDTH / 2;
  yCursorPos = MAX_GRID_HEIGHT / 2;
  inputFrameCounter = 0;
  isPlayerTurn = true;

  for (int i = 0; i < mutablePipesLen; i++) {
    randomPipeWeight[i] = 0;
  }

  nextPipes.clear();
  for (uint8_t i = 0; i < NB_MAX_PIPE_DISPLAYED; i++) {
    nextPipes.push(getRandomPipe());
  }
}

bool Player::canMove() {
  return (
    inputFrameCounter == 0
    || (inputFrameCounter > FRAME_RESET_VALUE && inputFrameCounter % 4 == 0));
}

void Player::increaseInputFrameCounter() {
  if (arduboy.everyXFrames(1)) {
    if (inputFrameCounter < UINT8_MAX) {
      inputFrameCounter++;
    } else {
      inputFrameCounter = FRAME_RESET_VALUE;
    }
  }
}

void Player::handleInput(GameManager *gm) {
  bool haveMoved = false;

  if (arduboy.pressed(UP_BUTTON) && yCursorPos > 1) {
    if (canMove()) {
      yCursorPos -= 1;
      haveMoved = true;
    }
    increaseInputFrameCounter();
  } else if (arduboy.pressed(DOWN_BUTTON) && yCursorPos < MAX_GRID_HEIGHT - 2) {
    if (canMove()) {
      yCursorPos += 1;
      haveMoved = true;
    };
    increaseInputFrameCounter();
  } else if (arduboy.pressed(LEFT_BUTTON) && xCursorPos > 1) {
    if (canMove()) {
      xCursorPos -= 1;
      haveMoved = true;
    };
    increaseInputFrameCounter();
  } else if (arduboy.pressed(RIGHT_BUTTON) && xCursorPos < MAX_GRID_WIDTH - 2) {
    if (canMove()) {
      xCursorPos += 1;
      haveMoved = true;
    };
    increaseInputFrameCounter();
  } else {
    inputFrameCounter = 0;
  }

  if (haveMoved && networkManager) {
    networkManager->setPlayerPosition(xCursorPos, yCursorPos);
  }

  if (arduboy.justPressed(A_BUTTON)) {
    pushTile();
    if (networkManager) {
      networkManager->setPipe();
    }
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }
}

void Player::setState(PlayerState newState) {
  state = newState;
}

void Player::setPosition(uint8_t x, uint8_t y) {
  xCursorPos = x;
  yCursorPos = y;
}

void Player::setIsPlayerTurn(bool isTurn) {
  isPlayerTurn = isTurn;
}

bool Player::getIsPlayerTurn() const {
  return isPlayerTurn;
}

void Player::pushTile() {
  uint8_t tileType = tileMapRef->getTileType(xCursorPos, yCursorPos);

  if (tileType == 0) {
    tileMapRef->setTileType(xCursorPos, yCursorPos, nextPipes[0]);
    state = PlayerState::AddingPipe;
    columnDisplayMove = true;

  } else if (tileMapRef->canTileBeErased(xCursorPos, yCursorPos)) {
    tileMapRef->setTileType(xCursorPos, yCursorPos, nextPipes[0]);
    tileMapRef->setTileState(xCursorPos, yCursorPos, PipeState::Exploding);
    tileMapRef->resetExplosionTimer();
    state = PlayerState::ReplacingPipe;
    columnDisplayMove = true;
  }
}

void Player::handleState() {
  if (columnDisplayMove) {
    columnDisplayNextPipeY += 1;

    if (columnDisplayNextPipeY > PIPE_UI_DISTANCE) {
      columnDisplayNextPipeY = 0;
      uint8_t nextPipe = 0;
      nextPipes.pop(nextPipe);
      nextPipes.push(getRandomPipe());
      columnDisplayMove = false;

      if (state == PlayerState::AddingPipe) {
        state = PlayerState::Running;
      }
    }
  }

  if (state == PlayerState::ReplacingPipe) {
    pauseTimer += 1;
    if (pauseTimer >= REPLACING_PIPE_TIME) {
      pauseTimer = 0;
      // Update the tile state so it came back as a empty pipe
      // Now it have been fully replaced
      tileMapRef->setTileState(xCursorPos, yCursorPos, PipeState::Empty);
      state = PlayerState::Running;
    }
  }
}

void Player::update(GameManager *gm) {
  if (isPlayerTurn && state == PlayerState::Running) {
    handleInput(gm);
  }
  handleState();
}

void Player::render() const {
  // Render Player Cursor
  // => Only render the cursor when the state is not in pause
  if (state == PlayerState::Running) {
    arduboy.fillRect(startGridX + (xCursorPos * 8), startGridY + (yCursorPos * 8), 8, 8, BLACK);
    arduboy.drawRect((startGridX - 1) + (xCursorPos * 8), (startGridY - 1) + (yCursorPos * 8), 10, 10, WHITE);

    if (isPlayerTurn) {
      renderEmptySprite(
        xCursorPos, yCursorPos, Tile{ .pipeType = nextPipes[0] });
    } else {
      renderFullSprite(
        xCursorPos, yCursorPos, Tile{ .pipeType = nextPipes[0] });
    }
  }

  // Render left column
  // => All `NB_MAX_PIPE_DISPLAYED` are displayed on the screen
  // once the columnDisplayMove has ended (meaning is == false)
  for (uint8_t i = columnDisplayMove ? 1 : 0; i < NB_MAX_PIPE_DISPLAYED; i++) {
    arduboy.drawBitmap(6, columnDisplayNextPipeY + 45 - (i * PIPE_UI_DISTANCE), getSpriteFromIndex(nextPipes[i], true), 8, 8);
    arduboy.drawRoundRect(5, columnDisplayNextPipeY + 44 - (i * PIPE_UI_DISTANCE), 10, 10, 1);
  }
}
