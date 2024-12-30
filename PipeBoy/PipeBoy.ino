#include "Constants.h"
#include "TileMap.h"
#include "Player.h"
#include "Menu.h"
#include "PasswordMenu.h"
#include "GameManager.h"
#include "NetworkManager.h"
#include "ScoreMenu.h"

TileMap tileMap;
Player player = Player(&tileMap);
Menu menu = Menu();
PasswordMenu pwdMenu = PasswordMenu();
ScoreMenu sMenu = ScoreMenu();
GameManager gameManager = GameManager(
  &player, &tileMap, &menu, &pwdMenu, &sMenu);

Arduboy2 arduboy;

void setup() {
  arduboy.begin();
  beep.begin();
  arduboy.setFrameRate(FRAME_RATE);
  arduboy.initRandomSeed();
  Serial.begin(9600);
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;
  arduboy.pollButtons();

  gameManager.loop();

  beep.timer();
}

