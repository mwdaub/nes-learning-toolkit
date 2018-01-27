#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include "types.h"

namespace nes {

struct RGBA {
  uint8 R;
  uint8 G;
  uint8 B;
  uint8 A;

  RGBA(uint32 x) :
      R(x >> 16),
      G(x >> 8),
      B(x),
      A(0xFF) {}
};

extern RGBA Palette[64];

}  // namespace nes

#endif  // define NES_PALETTE_H
