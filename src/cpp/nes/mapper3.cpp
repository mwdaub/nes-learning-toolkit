#include "mapper3.h"

#include "utils.h"

namespace nes {

void Mapper3State::Save(ostream& out) {
  utils::writeInt32(out, chrBank);
  utils::writeInt32(out, prgBank1);
  utils::writeInt32(out, prgBank2);
}

void Mapper3State::Load(istream& in) {
  chrBank = utils::readInt32(in);
  prgBank1 = utils::readInt32(in);
  prgBank2 = utils::readInt32(in);
}

uint8 Mapper3::Read(uint16 address) {
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
    // log.Fatalf("unhandled mapper3 read at address: 0x%04X", address)
  }
  return 0;
}

void Mapper3::Write(uint16 address, uint8 value) {
  if(address < 0x2000) {
    int32 index = chrBank*0x2000 + int32(address);
    cartridge->CHR[index] = value;
  } else if (address >= 0x8000) {
    chrBank = int32(value & 3);
  } else if (address >= 0x6000) {
    int32 index = int32(address) - 0x6000;
    cartridge->SRAM[index] = value;
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper3 write at address: 0x%04X", address)
  }
}

}  // namespace nes
