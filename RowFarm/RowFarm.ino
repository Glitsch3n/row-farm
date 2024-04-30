#include "Constants.h"
#include "Stack.h"
#include "TileMap.h"
#include "Player.h"

// Menu Variables
int8_t menuSelection = 0;

// In Game Variables
TileMap tm;
Player player = Player(&tm);


void setup() {
  arduboy.begin();
  beep.begin();
  arduboy.setFrameRate(frameRate);
  arduboy.initRandomSeed();
  Serial.begin(9600);
  initHighScore();
}

/*
** Menu
*/

void drawMenu() {
  const uint8_t yLimit = 53;

  if (firstGameStateDraw) {
    arduboy.clear();
    arduboy.drawCompressed(0, 0, title_screen);
    arduboy.drawFastHLine(0, yLimit - 2, WIDTH);
    firstGameStateDraw = false;
  }
  // Only make a partial refresh in order to avoid useless CPU usage
  // through drawCompressed
  arduboy.fillRect(0, yLimit, WIDTH, HEIGHT - yLimit, BLACK);

  // Draw the menu
  arduboy.setCursorY(yLimit + 2);
  arduboy.setCursorX(menuSelectionXPos[0] + 10);
  arduboy.print("PLAY");
  arduboy.setCursorX(menuSelectionXPos[1] + 10);
  arduboy.print("SCORE");
  arduboy.drawBitmap(menuSelectionXPos[2] + 10, yLimit + 2, block_sound, 8, 8);
  arduboy.setCursorX(menuSelectionXPos[2] + 19);
  arduboy.print(soundEnabled == true ? ":ON" : ":OFF");

  // Draw the cursor
  arduboy.drawBitmap(menuSelectionXPos[menuSelection], yLimit + 2, block_cursor, 8, 8);
}

void updateMenu() {
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    beepTone(beep.freq(100), 3);
    menuSelection++;
    if (menuSelection > 2) {
      menuSelection = 0;
    }
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    beepTone(beep.freq(100), 3);
    menuSelection--;
    if (menuSelection < 0) {
      menuSelection = 2;
    }
  }

  if (arduboy.justPressed(A_BUTTON)) {
    beepTone(beep.freq(100), 3);
    if (menuSelection == 0) {
      setGameState(GameState::InGame);
    } else if (menuSelection == 1) {
      setGameState(GameState::ScoringTable);
    } else if (menuSelection == 2) {
      soundEnabled = !soundEnabled;
    }
  }
}

/*
** In Game
*/

void drawInGame() {
  arduboy.clear();
  arduboy.print("Level:" + String(player.getLevel()));
  //arduboy.print("Score:" + String(player.score));
  player.render();
  tm.render();

  if (currentGameState == GameState::InGamePause) {
    arduboy.drawRect(32, 20, 64, 24);
    arduboy.fillRect(33, 21, 62, 22, BLACK);
    arduboy.setCursor(50, 29);
    arduboy.print("PAUSE");
  }
}

void updateInGame() {
  if (arduboy.justPressed(B_BUTTON)) {
    beepTone(beep.freq(100), 3);
    setGameState(currentGameState == GameState::InGame ? GameState::InGamePause : GameState::InGame);
  }
  if (currentGameState == GameState::InGame) {
    player.update();
    tm.update();
  }
}


/*
** Scoring Table
*/

void drawScoringTable() {
  if (firstGameStateDraw) {
    char buffer[40];

    arduboy.clear();
    arduboy.setCursor(30, 0);
    arduboy.print("HIGH SCORES");
    arduboy.drawFastHLine(0, 10, WIDTH);

    for (uint8_t i = 0; i < highScoreSize; i++) {
      arduboy.setCursor(5, (i * 15) + 16);
      sprintf(buffer, "[%d] - %d", i, highScores[i]);
      arduboy.print(buffer);
    }
    firstGameStateDraw = false;
  }
}


void updateScoringTable() {
  if (arduboy.justPressed(A_BUTTON) or arduboy.justPressed(B_BUTTON)) {
    beepTone(beep.freq(100), 3);
    setGameState(GameState::Menu);
  }
}

/*
** Game Over
*/

void drawGameOver() {
  if (firstGameStateDraw) {
    arduboy.clear();
    arduboy.print("Level:" + String(player.getLevel()));
    arduboy.setCursor(0, 10);
    arduboy.print("Score:" + String(player.score));
    arduboy.setCursor(35, 25);
    arduboy.print("Game Over");
    arduboy.setCursor(10, 40);
    arduboy.print("Press A to re-play");
    arduboy.setCursor(15, 50);
    arduboy.print("Press B for menu");
    firstGameStateDraw = false;
  }
}

void updateGameOver() {
  if (arduboy.justPressed(A_BUTTON) == true) {
    setGameState(GameState::InGame);
    player.reset();
  }
  if (arduboy.justPressed(B_BUTTON) == true) {
    setGameState(GameState::Menu);
    player.reset();
  }
}

/*
** Main loop
*/

void loop() {
  if (!(arduboy.nextFrame()))
    return;
  arduboy.pollButtons();

  if (currentGameState == GameState::Menu) {
    drawMenu();
    updateMenu();
  } else if (currentGameState == GameState::InGame or currentGameState == GameState::InGamePause) {
    drawInGame();
    updateInGame();
  } else if (currentGameState == GameState::GameOver) {
    drawGameOver();
    updateGameOver();
  } else if (currentGameState == GameState::ScoringTable) {
    drawScoringTable();
    updateScoringTable();
  }
  arduboy.display();
  beep.timer();
}
