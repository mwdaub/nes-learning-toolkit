#include "cartridge.h"

#include <cstring>

#include "utils.h"

using namespace nes;

namespace nes {

void CartridgeState::Save(ostream& out) {
  utils::writeUint8Array(out, SRAM);
  utils::writeUint8(out, mirror);
}

void CartridgeState::Load(istream& in) {
  utils::readUint8Array(in, SRAM);
  mirror = utils::readUint8(in);
}

}  // namespace nes
