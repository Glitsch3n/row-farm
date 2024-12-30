#ifndef LEVELS_H
#define LEVELS_H

#include <Arduino.h>

struct Level {
  uint8_t pipeDistance;
  uint8_t nbWallHole;
  uint8_t nbBlock;
  uint8_t nbReservoir;
  uint8_t nbOneWayPipe;
  bool requiredEndingPipe;
};

extern const Level PROGMEM levels[];
extern const size_t NUMBER_LEVELS;

#endif