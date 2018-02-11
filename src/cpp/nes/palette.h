#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include "types.h"

namespace nes {

struct RGB {
  uint8 R;
  uint8 G;
  uint8 B;

  RGB(uint32 x) :
      R(x >> 16),
      G(x >> 8),
      B(x) {}
};

extern const RGB Palette[64];

class Screen {
  public:
    Screen() : pixels{} {};

    uint8 pixels[240*256];

    void SetPixel(int32 x, int32 y, uint8 val);
    void GetPixelIndexes(uint8* data);
    void GetPixelValues(uint8* data);
};

}  // namespace nes

#endif  // define NES_PALETTE_H
