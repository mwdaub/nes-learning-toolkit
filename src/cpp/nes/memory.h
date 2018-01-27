#ifndef NES_MEMORY_H
#define NES_MEMORY_H

#include "types.h"

namespace nes {

enum MirrorModes {
  Horizontal = 0,
  Vertical = 1,
  Single0 = 2,
  Single1 = 3,
  Four = 4
};

uint16 MirrorAddress(uint8 mode, uint16 address);

}  // namespace nes

#endif  // define NES_MEMORY_H
