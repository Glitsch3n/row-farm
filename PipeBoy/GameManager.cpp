#include "Menu.h"
#include "PasswordMenu.h"
#include "Player.h"
#include "TileMap.h"
#include "GameManager.h"
#include "Level.h"
#include "LevelBuilder.h"
#include "ScoreMenu.h"
#include "NetworkManager.h"

const int INIT_LEVEL = -1;

GameManager::GameManager(Player *playerRef, TileMap *tileMapRef, Menu *menuRef, PasswordMenu *pwdMenuRef, ScoreMenu *scoreMenuRef)
  : player(playerRef),
    tileMap(tileMapRef),
    menu(menuRef),
    pwdMenu(pwdMenuRef),
    scoreMenu(scoreMenuRef),
    currentState(GameState::MainMenu),
    currentLevel(INIT_LEVEL),
    soundEnabled(false),
    currentScore(0) {

  networkManager = new NetworkManager(this);
}

void GameManager::render(void (GameManager::*renderMethod)() const) {
  arduboy.clear();
  (this->*renderMethod)();
  arduboy.display();
}

void GameManager::loop() {

  switch (currentState) {
    case GameState::MainMenu:
      render(&renderMenu);
      updateMenu();
      break;
    case GameState::PasswordMenu:
      render(&renderPasswordMenu);
      updatePasswordMenu();
      break;
    case GameState::Playing:
      render(&renderPlaying);
      updatePlaying();
      break;
    case GameState::Paused:
      render(&renderPaused);
      updatePaused();
      break;
    case GameState::PasswordReveal:
      render(&renderPasswordReveal);
      updatePasswordReveal();
      break;
    case GameState::GameFinish:
      render(&renderGameFinish);
      updateGameFinish();
      break;
    case GameState::GameOver:
      render(&renderGameOver);
      updateGameOver();
      break;
    case GameState::TwoPlayerWaitingScreen:
      render(&renderTwoPlayerWaitingScreen);
      updateTwoPlayerWaitingScreen();
      break;
    case GameState::ScoreMenu:
      render(&renderScoreMenu);
      updateScoreMenu();
      break;
  }
}

void GameManager::setPlayerState(PlayerState newState) {
  player->setState(newState);
}

void GameManager::beepTone(uint16_t count, uint8_t dur) {
  if (soundEnabled) {
    sound.tone(count, dur);
  }
}

void GameManager::beepTones(uint16_t *tones) {
  if (soundEnabled) {
    sound.tones(tones);
  }
}

void GameManager::setState(GameState newState) {
  currentState = newState;
}

GameState GameManager::getState() const {
  return currentState;
}

void GameManager::setSoundEnabled(bool se) {
  soundEnabled = se;
}

bool GameManager::getSoundEnabled() const {
  return soundEnabled;
}

void GameManager::increaseScore(uint16_t additionalScore) {
  currentScore += additionalScore;
}

void GameManager::setLevel(uint8_t newLevel) {
  currentLevel = newLevel;
  player->reset();
  tileMap->reset();
  initPipingNetwork(tileMap, currentLevel);
}

void GameManager::setNextLevel() {
  ScoreMenu::updateHighScore(currentScore);
  currentLevel++;

  if (currentLevel > NUMBER_LEVELS) {
    setState(GameState::GameFinish);
  } else {
    player->reset();
    tileMap->reset();
    initPipingNetwork(tileMap, currentLevel);

    if (networkManager->isAvailable()) {
      // The Peripheral should always start second
      if (networkManager->getDeviceType() == DeviceType::Peripheral) {
        getPlayer()->setIsPlayerTurn(false);
      }
    } else {
      // Let's not make Password Reveal in case we are in 2 Player mode
      const char *pwd = getPasswordFromLevel(currentLevel);
      if (pwd) {
        setState(GameState::PasswordReveal);
      }
    }
  }
}

void GameManager::setMenu() {
  currentLevel = INIT_LEVEL;
  currentScore = 0;
  player->reset();
  tileMap->reset();
  menu->reset();

  // Force disconnect to NetworkManager even though it's already the case
  networkManager->reset();
  player->detachNetworkManager();

  setState(GameState::MainMenu);
}

/*
** Getters / Setters
*/

TileMap *GameManager::getTileMap() const {
  return tileMap;
}

Player *GameManager::getPlayer() const {
  return player;
}

Menu *GameManager::getMenu() const {
  return menu;
}

/*
** Update + Render for Menu
*/

void GameManager::updateMenu() {
  menu->update(this);
}

void GameManager::renderMenu() const {
  menu->render(this);
}

/*
** Update + Render for ScoreMenu
*/

void GameManager::updateScoreMenu() {
  scoreMenu->update(this);
}

void GameManager::renderScoreMenu() const {
  scoreMenu->render();
}

/*
**  Update + Render for PasswordMenu
*/

void GameManager::updatePasswordMenu() {
  pwdMenu->update(this);
}

void GameManager::renderPasswordMenu() const {
  pwdMenu->render(this);
}

/*
** Update + Render when Playing/Paused
*/

void GameManager::updatePlaying() {
  if (player->getIsPlayerTurn() && arduboy.justPressed(B_BUTTON)) {
    setState(GameState::Paused);
    if (networkManager->isAvailable()) {
      networkManager->setPause(true);
    }
    beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }

  tileMap->update(this);
  player->update(this);
  if (networkManager->isAvailable()) {
    networkManager->update();
  }
}

void GameManager::renderPlaying() const {
  tileMap->render();
  player->render();

#ifdef DEBUG_MODE
  arduboy.setCursorY(50);
  arduboy.setCursorX(WIDTH - 23);
  arduboy.print(String(currentLevel));
#endif
}


/*
** Update + Render when Paused
*/

void GameManager::updatePaused() {
  if (player->getIsPlayerTurn()) {
    if (arduboy.justPressed(A_BUTTON)) {
      setState(GameState::Playing);
      tileMap->setFillPipeFast();
      if (networkManager->isAvailable()) {
        networkManager->setPause(false);
        networkManager->setFillPipeFast();
      }
      beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);

    } else if (arduboy.justPressed(B_BUTTON)) {
      setState(GameState::Playing);
      if (networkManager->isAvailable()) {
        networkManager->setPause(false);
      }
      beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
    }
  }

  if (networkManager->isAvailable()) {
    networkManager->update();
  }
}

void GameManager::renderPaused() const {
  arduboy.setCursor(50, 9);
  arduboy.print(F("PAUSE"));
  arduboy.setCursor(40, 25);
  arduboy.print("Score:");
  arduboy.print(currentScore);

  arduboy.setCursor(5, 45);
  arduboy.print(F("Press A : End Level"));
  arduboy.setCursor(5, 55);
  arduboy.print(F("Press B : Resume"));
}

/*
** Update + Render when PasswordReal screen is displayed
*/

void GameManager::updatePasswordReveal() {
  if (arduboy.justPressed(A_BUTTON)) {
    setState(GameState::Playing);
    beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }
}

void GameManager::renderPasswordReveal() const {
  const char *pwd = getPasswordFromLevel(currentLevel);
  arduboy.setCursor(38, 10);
  arduboy.print(F("PASSWORD"));

  for (int i = 0; i < NB_PWD_CHARACTERS; i++) {
    arduboy.setCursor(
      PASSWORD_MENU_POSITION_X + i * 10,
      PASSWORD_MENU_POSITION_Y);
    arduboy.write(pwd[i]);
  }
}

/*
** Update + Render when Game is Finished
*/

void GameManager::updateGameFinish() {
  if (arduboy.justPressed(A_BUTTON)) {
    setMenu();
    beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }
}

void GameManager::renderGameFinish() const {
  arduboy.setCursor(15, 25);
  arduboy.print(F("Congratulations !"));
  arduboy.setCursor(10, 35);
  arduboy.print("Final Score :" + String(currentScore));
}


/*
** Update + Render when Game is Over
*/

void GameManager::updateGameOver() {
  if (arduboy.justPressed(A_BUTTON)) {
    setMenu();
    beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }
}

void GameManager::renderGameOver() const {
  arduboy.setCursor(38, 25);
  arduboy.print(F("Game Over"));
}

/*
** Update + Render Automatic Handshake
*/

void GameManager::updateTwoPlayerWaitingScreen() {

  if (arduboy.justPressed(B_BUTTON)) {
    setMenu();
    beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }

  // Initializing Handshake Process
  if (networkManager->getDeviceType() == DeviceType::NotDefined) {
#ifdef DEBUG_MODE
    Serial.println(F("Device Type not defined... Let's try to handshake"));
#endif
    player->attachNetworkManager(networkManager);
#ifdef DEBUG_MODE
    Serial.println(F("Starting Handshake..."));
#endif
    networkManager->initHandshake();
  }

  // Waiting for the beginning of the handshake (Started means, an I2C connection has just been established)
  if (networkManager->getHandshakeState() == HandshakeState::Started) {
    networkManager->initNetworkTransmission();
    if (networkManager->getDeviceType() == DeviceType::Controller) {
      setState(GameState::Playing);
      setNextLevel();
      getPlayer()->setIsPlayerTurn(true);
    }
  }
}

void GameManager::renderTwoPlayerWaitingScreen() const {
  arduboy.setCursor(15, 25);
  arduboy.print(F("Waiting Player..."));
}
