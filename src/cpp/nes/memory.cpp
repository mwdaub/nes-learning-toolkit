#include "memory.h"

namespace nes {

uint16 MirrorLookup[5][4] = {
  {0, 0, 1, 1},
  {0, 1, 0, 1},
  {0, 0, 0, 0},
  {1, 1, 1, 1},
  {0, 1, 2, 3},
};

uint16 MirrorAddress(uint8 mode, uint16 address) {
  address = (address - 0x2000) % 0x1000;
  uint16 table = address / 0x0400;
  uint16 offset = address % 0x0400;
  return 0x2000 + MirrorLookup[mode][table]*0x0400 + offset;
}

}  // namespace nes
