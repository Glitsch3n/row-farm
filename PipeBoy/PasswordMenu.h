
#include "Constants.h"

constexpr uint8_t PASSWORD_MENU_POSITION_X = 45;
constexpr uint8_t PASSWORD_MENU_POSITION_Y = 33;

constexpr uint8_t NB_PWD_CHARACTERS = 4;
constexpr uint8_t NB_PWD = 7;

struct PasswordToLevel {
  uint8_t level;
  char password[NB_PWD_CHARACTERS];
};

const PasswordToLevel PROGMEM passwordToLevelList[NB_PWD] = {
  { 3, { 'H', 'A', 'H', 'A' } },
  { 7, { 'G', 'R', 'I', 'N' } },
  { 11, { 'R', 'E', 'A', 'P' } },
  { 15, { 'S', 'E', 'E', 'D' } },
  { 19, { 'G', 'R', 'O', 'W' } },
  { 23, { 'T', 'A', 'L', 'L' } },
  { 27, { 'Y', 'A', 'L', 'I' } }
};

const char *getPasswordFromLevel(uint8_t level);

class PasswordMenu {
private:
  uint8_t selectedOption = 0;
  char password[NB_PWD_CHARACTERS] = { 'A', 'A', 'A', 'A' };
  int8_t getLevel();

public:
  PasswordMenu();

  void render(GameManager *gm) const;
  void update(GameManager *gm);
  void reset();
};