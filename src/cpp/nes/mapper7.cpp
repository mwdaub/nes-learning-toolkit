#include "mapper7.h"

#include "memory.h"
#include "utils.h"

namespace nes {

void Mapper7::Save(ostream& out) {
  utils::write(out, prgBank);
}

void Mapper7::Load(istream& in) {
  utils::read(in, prgBank);
}

uint8 Mapper7::Read(uint16 address) {
  if (address < 0x2000) {
    return cartridge->CHR[address];
  } else if (address >= 0x8000) {
    int32 index = prgBank*0x8000 + int32(address-0x8000);
    return cartridge->PRG[index];
  } else if (address >= 0x6000) {
    int32 index = int32(address) - 0x6000;
    return cartridge->SRAM[index];
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper7 read at address: 0x%04X", address)
  }
  return 0;
}

void Mapper7::Write(uint16 address, uint8 value) {
  if(address < 0x2000) {
    cartridge->CHR[address] = value;
  } else if (address >= 0x8000) {
    prgBank = int32(value & 7);
    switch (value & 0x10) {
    case 0x00:
      cartridge->mirror = MirrorModes::Single0;
      break;
    case 0x10:
      cartridge->mirror = MirrorModes::Single1;
      break;
    }
  } else if (address >= 0x6000) {
    int32 index = int32(address) - 0x6000;
    cartridge->SRAM[index] = value;
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper7 write at address: 0x%04X", address)
  }
}

}  // namespace nes
