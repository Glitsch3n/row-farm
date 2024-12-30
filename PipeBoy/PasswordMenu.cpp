#include "Constants.h"
#include "GameManager.h"
#include "PasswordMenu.h"
#include "Level.h"


const char *getPasswordFromLevel(uint8_t level) {
  PasswordToLevel pwdToLevel;

  for (uint8_t i = 0; i < NB_PWD; i++) {
    memcpy_P(
      &pwdToLevel, &passwordToLevelList[i], sizeof(PasswordToLevel));

    if (level == pwdToLevel.level) {
      return pwdToLevel.password;
    }
  }
  return nullptr;
}

PasswordMenu::PasswordMenu() {
  reset();
}

void PasswordMenu::reset() {
  selectedOption = 0;

  for (uint8_t i = 0; i < NB_PWD_CHARACTERS; i++) {
    password[i] = 'A';
  }
}

int8_t PasswordMenu::getLevel() {
  PasswordToLevel pwdToLevel;

  for (uint8_t curPwdIndex = 0; curPwdIndex < NB_PWD; curPwdIndex++) {
    memcpy_P(
      &pwdToLevel, &passwordToLevelList[curPwdIndex], sizeof(PasswordToLevel));
    bool isValid = true;

    for (uint8_t curChar = 0; curChar < NB_PWD_CHARACTERS; curChar++) {
      if (password[curChar] != pwdToLevel.password[curChar]) {
        isValid = false;
        break;
      }
    }

    if (isValid) {
      return pwdToLevel.level;
    }
  }
  return 0;
}

void PasswordMenu::update(GameManager *gm) {
  if (arduboy.justPressed(LEFT_BUTTON)) {
    selectedOption =
      (selectedOption - 1 + NB_PWD_CHARACTERS) % NB_PWD_CHARACTERS;
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  } else if (arduboy.justPressed(RIGHT_BUTTON)) {
    selectedOption = (selectedOption + 1) % NB_PWD_CHARACTERS;
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  } else if (arduboy.justPressed(UP_BUTTON)) {
    password[selectedOption] =
      (password[selectedOption] + 1 > 'Z') ? 'A' : password[selectedOption] + 1;
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    password[selectedOption] =
      (password[selectedOption] - 1 < 'A') ? 'Z' : password[selectedOption] - 1;
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }

  if (arduboy.justPressed(B_BUTTON)) {
    gm->setState(GameState::MainMenu);
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  } else if (arduboy.justPressed(A_BUTTON)) {
    const int8_t nextLevel = getLevel();
    if (nextLevel != 0) {
      gm->setLevel(nextLevel);
      gm->setState(GameState::Playing);
    }
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }
}

void PasswordMenu::render(GameManager *gm) const {
  arduboy.setCursor(25, 15);
  arduboy.print(F("ENTER PASSWORD"));

  for (int i = 0; i < NB_PWD_CHARACTERS; i++) {
    if (arduboy.frameCount % 20 > 5) {
      if (i == selectedOption) {
        arduboy.setCursor(PASSWORD_MENU_POSITION_X + i * 10, PASSWORD_MENU_POSITION_Y + 5);
        arduboy.write('_');
      }
    }

    arduboy.setCursor(PASSWORD_MENU_POSITION_X + i * 10, PASSWORD_MENU_POSITION_Y);
    arduboy.write(password[i]);
  }
}