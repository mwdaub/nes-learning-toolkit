#include "mapper225.h"

#include "memory.h"
#include "utils.h"

namespace nes {

void Mapper225State::Save(ostream& out) {
  utils::writeInt32(out, chrBank);
  utils::writeInt32(out, prgBank1);
  utils::writeInt32(out, prgBank2);
}

void Mapper225State::Load(istream& in) {
  chrBank = utils::readInt32(in);
  prgBank1 = utils::readInt32(in);
  prgBank2 = utils::readInt32(in);
}

uint8 Mapper225::Read(uint16 address) {
  if (address < 0x2000) {
    int32 index = chrBank*0x2000 + int32(address);
    return cartridge->CHR[index];
  } else if (address >= 0xC000) {
    int32 index = prgBank2*0x4000 + int32(address-0xC000);
    return cartridge->PRG[index];
  } else if (address >= 0x8000) {
    int32 index = prgBank1*0x4000 + int32(address-0x8000);
    return cartridge->PRG[index];
  } else if (address >= 0x6000) {
    int32 index = int32(address) - 0x6000;
    return cartridge->SRAM[index];
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper225 read at address: 0x%04X", address)
  }
  return 0;
}

void Mapper225::Write(uint16 address, uint8 value) {
  if (address < 0x8000) {
    return;
  }

  int32 A = int32(address);
  int32 bank = (A >> 14) & 1;
  chrBank = (A & 0x3f) | (bank << 6);
  int32 prg = ((A >> 6) & 0x3f) | (bank << 6);
  int32 mode = (A >> 12) & 1;
  if (mode == 1) {
    prgBank1 = prg;
    prgBank2 = prg;
  } else {
    prgBank1 = prg;
    prgBank2 = prg + 1;
  }
  int32 mirr = (A >> 13) & 1;
  if (mirr == 1) {
    cartridge->mirror = MirrorModes::Horizontal;
  } else {
    cartridge->mirror = MirrorModes::Vertical;
  }
}

}  // namespace nes
