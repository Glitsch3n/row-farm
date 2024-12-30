#ifndef DRAWRAM_H
#define DRAWRAM_H

void drawBitmapFromRam(
  int16_t x, int16_t y, const uint8_t* bitmap, uint8_t w, uint8_t h, uint8_t color);

void blendSpriteHorizontally(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  bool toRight,
  uint8_t amountFilled);

void blendSpriteVertically(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  bool toUp,
  uint8_t amountFilled);

#endif