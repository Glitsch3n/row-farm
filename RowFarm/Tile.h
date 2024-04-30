#ifndef TILE_H
#define TILE_H

/*
** Tile Data Structure =>
** 0b000[A][BBBB]
**
** - A = Is tile blinking
** - B = Tile type
*/

uint8_t getTileType(uint8_t tile) {
  return tile & 0b00001111;
}

bool getTileBlinkingFlag(uint8_t tile) {
  return bool(tile & 0b00010000);
}

void setTileBlinkingFlag(uint8_t& type, bool shouldBlink) {
  type = (shouldBlink ? (type | 0b00010000) : (type & 0b11101111));
}

void drawBox(uint8_t x, uint8_t y, uint8_t type) {
  arduboy.drawBitmap(x, y, blockTypeSprites[type - 1], 8, 8);
}

void drawTile(uint8_t x, uint8_t y, uint8_t tileType, uint8_t tileIsBlinking) {
  if (tileType == 0) {
    return;
  }
  if (!tileIsBlinking || (tileIsBlinking && arduboy.frameCount % 6 > 3)) {
    if (tileType == bonusTile) {
      sprites.drawOverwrite(x, y, block_type_bonus, (arduboy.frameCount % 24) / 4);
    } else {
      drawBox(x, y, tileType);
    }
  }
}

#endif