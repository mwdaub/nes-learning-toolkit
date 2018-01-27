#include "cartridge.h"

#include <cstring>

#include "utils.h"

namespace nes {

void Cartridge::Save(ostream& out) {
  utils::write(out, reinterpret_cast<char*>(&SRAM[0]), sizeof(SRAM));
  utils::write(out, mirror);
}

void Cartridge::Load(istream& in) {
  utils::read(in, reinterpret_cast<char*>(&SRAM[0]), sizeof(SRAM));
  utils::read(in, mirror);
}

}  // namespace nes
