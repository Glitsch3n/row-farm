#ifndef SCOREMENU_H
#define SCOREMENU_H

#include <Arduboy2.h>
#include <EEPROM.h>

class GameManager;

constexpr uint8_t highScoreSize = 3;

class ScoreMenu {
public:
    ScoreMenu();
    void render();
    void update(GameManager *gm);
    static void updateHighScore(uint16_t score);
    static void initHighScore();

private:
    static uint16_t highScores[highScoreSize];
};

#endif