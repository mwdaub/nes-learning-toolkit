#include "cartridge.h"

#include <cstring>

#include "utils.h"

using namespace nes;

namespace nes {

void CartridgeState::Save(ostream& out) {
  utils::writeUint8(out, PRG.size());
  utils::writeUint8Vector(out, PRG);
  utils::writeUint8(out, CHR.size());
  utils::writeUint8Vector(out, CHR);
  utils::writeUint8Array(out, SRAM);
  utils::writeUint8(out, mapper);
  utils::writeUint8(out, mirror);
}

void CartridgeState::Load(istream& in) {
  uint32 prgSize = utils::readUint32(in);
  PRG.clear();
  PRG.resize(prgSize);
  utils::readUint8Vector(in, PRG);
  uint32 chrSize = utils::readUint32(in);
  CHR.clear();
  CHR.resize(chrSize);
  utils::readUint8Vector(in, CHR);
  utils::readUint8Array(in, SRAM);
  mapper = utils::readUint8(in);
  mirror = utils::readUint8(in);
}

}  // namespace nes
