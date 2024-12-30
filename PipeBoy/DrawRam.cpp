#include "Constants.h"
#include "DrawRam.h"

void drawBitmapFromRam(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
  // no need to draw at all if we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h / 8;
  if (h % 8 != 0) rows++;
  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT / 8) - 1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol < w; iCol++) {
        if (iCol + x > (WIDTH - 1)) break;
        if (iCol + x >= 0) {
          uint8_t byte = bitmap[(a * w) + iCol];
          ;
          if (bRow >= 0) {
            if (color == WHITE)
              arduboy.sBuffer[(bRow * WIDTH) + x + iCol] |= byte << yOffset;
            else if (color == BLACK)
              arduboy.sBuffer[(bRow * WIDTH) + x + iCol] &= ~(byte << yOffset);
            else
              arduboy.sBuffer[(bRow * WIDTH) + x + iCol] ^= byte << yOffset;
          }
          if (yOffset && bRow < (HEIGHT / 8) - 1 && bRow > -2) {
            if (color == WHITE)
              arduboy.sBuffer[((bRow + 1) * WIDTH) + x + iCol] |= byte >> (8 - yOffset);
            else if (color == BLACK)
              arduboy.sBuffer[((bRow + 1) * WIDTH) + x + iCol] &= ~(byte >> (8 - yOffset));
            else
              arduboy.sBuffer[((bRow + 1) * WIDTH) + x + iCol] ^= byte >> (8 - yOffset);
          }
        }
      }
    }
  }
}

void blendSpriteHorizontally(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  bool toRight,
  uint8_t amountFilled) {
  for (uint8_t i = 0; i <= (TILE_SIZE - 1); i++) {
    uint8_t curPos = toRight ? i : ((TILE_SIZE - 1) - i);

    if (i <= amountFilled) {
      blendedPipe[curPos] = pgm_read_byte(&endPipe[curPos]);
    } else {
      blendedPipe[curPos] = pgm_read_byte(&initialPipe[curPos]);
    }
  }
}

void blendSpriteVertically(
  uint8_t* initialPipe,
  uint8_t* endPipe,
  unsigned char* blendedPipe,
  bool toUp,
  uint8_t amountFilled) {

  for (uint8_t colPos = 0; colPos < TILE_SIZE; colPos++) {
    uint8_t maskFull = toUp ? 0xFF << (TILE_SIZE - amountFilled) : 0xFF >> (TILE_SIZE - amountFilled);
    uint8_t maskEmpty = toUp ? 0xFF >> amountFilled : 0xFF << amountFilled;

    uint8_t first4Bits = pgm_read_byte(&endPipe[colPos]) & maskFull;
    uint8_t last4Bits = pgm_read_byte(&initialPipe[colPos]) & maskEmpty;

    blendedPipe[colPos] = first4Bits | last4Bits;
  }
}
