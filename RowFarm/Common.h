#ifndef COMMON_H
#define COMMON_H

// System global variables
Arduboy2 arduboy;
BeepPin1 beep;
SpritesB sprites;

enum class GameState { Menu,
                       InGame,
                       InGamePause,
                       GameOver,
                       ScoringTable };

// Globals variables used everywhere
extern GameState currentGameState = GameState::Menu;
extern bool soundEnabled = true;
extern bool firstGameStateDraw = true;
extern uint16_t highScores[highScoreSize] = {0};

#endif