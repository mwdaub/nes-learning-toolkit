#include "mapper1.h"

#include "memory.h"
#include "utils.h"

namespace nes {

void Mapper1State::Save(ostream& out) {
  utils::writeUint8(out, shiftRegister);
  utils::writeUint8(out, control);
  utils::writeUint8(out, prgMode);
  utils::writeUint8(out, chrMode);
  utils::writeUint8(out, prgBank);
  utils::writeUint8(out, chrBank0);
  utils::writeUint8(out, chrBank1);
  utils::writeInt32Array(out, &prgOffsets[0], sizeof(prgOffsets));
  utils::writeInt32Array(out, &chrOffsets[0], sizeof(chrOffsets));
}

void Mapper1State::Load(istream& in) {
  shiftRegister = utils::readUint8(in);
  control = utils::readUint8(in);
  prgMode = utils::readUint8(in);
  chrMode = utils::readUint8(in);
  prgBank = utils::readUint8(in);
  chrBank0 = utils::readUint8(in);
  chrBank1 = utils::readUint8(in);
  utils::readInt32Array(in, &prgOffsets[0], kPrgOffsetsSize);
  utils::readInt32Array(in, &chrOffsets[0], kChrOffsetsSize);
}

uint8 Mapper1::Read(uint16 address) {
  if (address < 0x2000) {
    uint16 bank = address / 0x1000;
    uint16 offset = address % 0x1000;
    return cartridge->CHR[chrOffsets[bank]+offset];
  } else if (address >= 0x8000) {
    address = address - 0x8000;
    uint16 bank = address / 0x4000;
    uint16 offset = address % 0x4000;
    return cartridge->PRG[prgOffsets[bank]+offset];
  } else if (address >= 0x6000) {
    return cartridge->SRAM[address-0x6000];
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper1 read at address: 0x%04X", address)
  }
  return 0;
}

void Mapper1::Write(uint16 address, uint8 value) {
  if(address < 0x2000) {
    uint16 bank = address / 0x1000;
    uint16 offset = address % 0x1000;
    cartridge->CHR[chrOffsets[bank]+offset] = value;
  } else if (address >= 0x8000) {
    loadRegister(address, value);
  } else if (address >= 0x6000) {
    cartridge->SRAM[address-0x6000] = value;
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper1 write at address: 0x%04X", address)
  }
}

void Mapper1::loadRegister(uint16 address, uint8 value) {
  if ((value&0x80) == 0x80) {
    shiftRegister = 0x10;
    writeControl(control | 0x0C);
  } else {
    bool complete = ((shiftRegister&1) == 1);
    shiftRegister >>= 1;
    shiftRegister |= ((value & 1) << 4);
    if (complete) {
      writeRegister(address, shiftRegister);
      shiftRegister = 0x10;
    }
  }
}

void Mapper1::writeRegister(uint16 address, uint8 value) {
  if (address <= 0x9FFF) {
    writeControl(value);
  } else if (address <= 0xBFFF) {
    writeCHRBank0(value);
  } else if (address <= 0xDFFF) {
    writeCHRBank1(value);
  } else {
    writePRGBank(value);
  }
}

// Control (internal, $8000-$9FFF)
void Mapper1::writeControl(uint8 value) {
  control = value;
  chrMode = (value >> 4) & 1;
  prgMode = (value >> 2) & 3;
  uint8 mirror = value & 3;
  switch (mirror) {
  case 0:
    cartridge->mirror = MirrorModes::Single0;
    break;
  case 1:
    cartridge->mirror = MirrorModes::Single1;
    break;
  case 2:
    cartridge->mirror = MirrorModes::Vertical;
    break;
  case 3:
    cartridge->mirror = MirrorModes::Horizontal;
    break;
  }
  updateOffsets();
}

// CHR bank 0 (internal, $A000-$BFFF)
void Mapper1::writeCHRBank0(uint8 value) {
  chrBank0 = value;
  updateOffsets();
}

// CHR bank 1 (internal, $C000-$DFFF)
void Mapper1::writeCHRBank1(uint8 value) {
  chrBank1 = value;
  updateOffsets();
}

// PRG bank (internal, $E000-$FFFF)
void Mapper1::writePRGBank(uint8 value) {
  prgBank = value & 0x0F;
  updateOffsets();
}

int32 Mapper1::prgBankOffset(int32 index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->prgLength / 0x4000;
  int32 offset = index * 0x4000;
  if (offset < 0) {
    offset += cartridge->prgLength;
  }
  return offset;
}

int32 Mapper1::chrBankOffset(int32 index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->chrLength / 0x1000;
  int32 offset = index * 0x1000;
  if (offset < 0) {
    offset += cartridge->chrLength;
  }
  return offset;
}

// PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
//                    2: fix first bank at $8000 and switch 16 KB bank at $C000;
//                    3: fix last bank at $C000 and switch 16 KB bank at $8000)
// CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
void Mapper1::updateOffsets() {
  switch (prgMode) {
  case 0:
    prgOffsets[0] = prgBankOffset(prgBank & 0xFE);
    prgOffsets[1] = prgBankOffset(prgBank | 0x01);
    break;
  case 1:
    prgOffsets[0] = prgBankOffset(prgBank & 0xFE);
    prgOffsets[1] = prgBankOffset(prgBank | 0x01);
    break;
  case 2:
    prgOffsets[0] = 0;
    prgOffsets[1] = prgBankOffset(prgBank);
    break;
  case 3:
    prgOffsets[0] = prgBankOffset(prgBank);
    prgOffsets[1] = prgBankOffset(-1);
    break;
  }
  switch (chrMode) {
  case 0:
    chrOffsets[0] = chrBankOffset(chrBank0 & 0xFE);
    chrOffsets[1] = chrBankOffset(chrBank0 | 0x01);
    break;
  case 1:
    chrOffsets[0] = chrBankOffset(chrBank0);
    chrOffsets[1] = chrBankOffset(chrBank1);
    break;
  }
}

}  // namespace nes
