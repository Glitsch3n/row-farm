#include "Menu.h"
#include "Constants.h"
#include "GameManager.h"

Menu::Menu() {
  reset();
}

void Menu::reset() {
  selectedOption = 0;
  menuType = MenuType::MenuSelection;

  for (int i = 0; i < NB_WATER_DROP; i++) {
    waterDrops[i].raw = 0;
    waterDrops[i].y = WATER_DROP_INITIAL_Y;
    waterDrops[i].counter = random(
      WATER_DROP_MIN_COUNTDOWN, WATER_DROP_MAX_COUNTDOWN);
  }
}

uint8_t Menu::getNbMaxOptions() const {
  return menuType == MenuType::MenuSelection ? NB_OPTIONS : NB_OPTIONS_PLAY;
}

void Menu::update(GameManager *gm) {
  const uint8_t nbMaxOptions = getNbMaxOptions();
  
  if (arduboy.justPressed(UP_BUTTON)) {
    selectedOption = ((selectedOption - 1) % nbMaxOptions + nbMaxOptions) % nbMaxOptions;
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    selectedOption = (selectedOption + 1) % getNbMaxOptions();
    gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
  }

  if (menuType == MenuType::MenuSelection) {
    if (arduboy.justPressed(A_BUTTON)) {
      const MenuSelection mSelect = static_cast<MenuSelection>(selectedOption);

      if (mSelect == MenuSelection::Play) {
        menuType = MenuType::MenuSelectionPlay;
        selectedOption = 0;
      } else if (mSelect == MenuSelection::Sound) {
        gm->setSoundEnabled(!gm->getSoundEnabled());
      } else if (mSelect == MenuSelection::Score) {
        gm->setState(GameState::ScoreMenu);
      }
      gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
    }
  } else if (menuType == MenuType::MenuSelectionPlay) {
    if (arduboy.justPressed(A_BUTTON)) {
      const MenuSelectionPlay mSelect = static_cast<MenuSelectionPlay>(selectedOption);

      if (mSelect == MenuSelectionPlay::OnePlayer) {
        gm->setNextLevel();
        gm->setState(GameState::Playing);
      } else if (mSelect == MenuSelectionPlay::TwoPlayers) {
        // should launch a 2 player Game
        gm->setState(GameState::TwoPlayerWaitingScreen);
      } else if (mSelect == MenuSelectionPlay::Password) {
        gm->setState(GameState::PasswordMenu);
      }
      gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
    }

    if (arduboy.justPressed(B_BUTTON)) {
      menuType = MenuType::MenuSelection;
      selectedOption = 0;
      gm->beepTone(MENU_SOUND_FREQ, MENU_SOUND_DUR);
    }
  }

  // Handle Water Drop Update
  for (int i = 0; i < NB_WATER_DROP; i++) {
    // Handle velocity
    if (waterDrops[i].counter == 0) {
      if (waterDrops[i].velocity < 3 && arduboy.everyXFrames(5)) {
        waterDrops[i].velocity += 1;
      }

      // Handle end of drop of water
      waterDrops[i].y += waterDrops[i].velocity;
      if (waterDrops[i].y > HEIGHT) {
        waterDrops[i].y = WATER_DROP_INITIAL_Y;
        waterDrops[i].velocity = 0;
        waterDrops[i].counter = random(
          WATER_DROP_MIN_COUNTDOWN, WATER_DROP_MAX_COUNTDOWN);
      }
      // Handle countdown before new drop of water
    } else {
      waterDrops[i].counter--;
    }
  }
}

void Menu::render(GameManager *gm) const {
  arduboy.drawBitmap(0, 0, titlescreen, 128, 30);

  const uint8_t menuTypePos = static_cast<uint8_t>(menuType);
  for (int i = 0; i < getNbMaxOptions(); i++) {

    if (i == selectedOption) {
      arduboy.setCursor(MENU_POSITION_X, MENU_POSITION_Y + i * 10);
      arduboy.print("> ");
    } else {
      arduboy.setCursor(MENU_POSITION_X, MENU_POSITION_Y + i * 10);
      arduboy.print("  ");
    }

    arduboy.setCursor(MENU_POSITION_X + 10, MENU_POSITION_Y + i * 10);
    arduboy.print(options[menuTypePos][i]);

    if (menuType == MenuType::MenuSelection) {
      const MenuSelection iSelect = (MenuSelection)i;
      if (iSelect == MenuSelection::Sound) {
        arduboy.print(gm->getSoundEnabled() ? "On" : "Off");
      }
    }
  }

  // Render Drop of Water
  for (int i = 0; i < NB_WATER_DROP; i++) {
    if (waterDrops[i].counter == 0) {
      const uint8_t xPos = (i == 0) ? WATER_DROP_INITIAL_X : WATER_DROP_2_INITIAL_X;

      arduboy.drawFastVLine(xPos, waterDrops[i].y, 2);
    }
  }
}
