#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

class Menu;
class Player;
class PasswordMenu;
enum class PlayerState;
class TileMap;
class NetworkManager;
class ScoreMenu;

enum class GameState : uint8_t {
  MainMenu,
  PasswordMenu,
  ScoreMenu,
  TwoPlayerWaitingScreen,
  Playing,
  Paused,
  LevelComplete,
  PasswordReveal,
  GameOver,
  GameFinish,
};

class GameManager {
private:
  GameState currentState;
  Player *player;
  TileMap *tileMap;
  Menu *menu;
  PasswordMenu *pwdMenu;
  ScoreMenu *scoreMenu;
  NetworkManager *networkManager;

  int currentLevel;
  uint16_t currentScore;
  bool soundEnabled;

  void render(void (GameManager::*renderMethod)() const);
  void renderMenu() const;
  void renderScoreMenu() const;
  void renderPasswordMenu() const;
  void renderPlaying() const;
  void renderPaused() const;
  void renderPasswordReveal() const;
  void renderGameFinish() const;
  void renderGameOver() const;
  void renderTwoPlayerWaitingScreen() const;

  void updateMenu();
  void updateScoreMenu();
  void updatePasswordMenu();
  void updatePlaying();
  void updatePaused();
  void updatePasswordReveal();
  void updateGameFinish();
  void updateGameOver();
  void updateTwoPlayerWaitingScreen();

public:
  GameManager(Player *playerRef, TileMap *tileMapRef, Menu *menu, PasswordMenu *pwdMenuRef, ScoreMenu *scoreMenuRef);
  //GameManager();

  bool getSoundEnabled() const;
  GameState getState() const;
  Player *getPlayer() const;
  Menu *getMenu() const;
  TileMap *getTileMap() const;

  void loop();
  void setState(GameState newState);
  void beepTone(uint16_t count, uint8_t dur);
  void beepTones(uint16_t *tones);
  void setPlayerState(PlayerState state);
  void setSoundEnabled(bool se);
  void increaseScore(uint16_t additionalScore);

  void setNextLevel();
  void setLevel(uint8_t newLevel);
  void setMenu();
};

#endif