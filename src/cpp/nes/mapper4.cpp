#include "mapper4.h"

#include "console.h"
#include "ppu.h"
#include "utils.h"

namespace nes {

void Mapper4::Save(ostream& out) {
  utils::write(out, reg);
  utils::write(out, reinterpret_cast<char*>(&registers[0]), sizeof(registers));
  utils::write(out, prgMode);
  utils::write(out, chrMode);
  utils::write(out, reinterpret_cast<char*>(&prgOffsets[0]), sizeof(prgOffsets));
  utils::write(out, reinterpret_cast<char*>(&chrOffsets[0]), sizeof(chrOffsets));
  utils::write(out, reload);
  utils::write(out, counter);
  utils::write(out, irqEnable);
}

void Mapper4::Load(istream& in) {
  utils::read(in, reg);
  utils::read(in, reinterpret_cast<char*>(&registers[0]), sizeof(registers));
  utils::read(in, prgMode);
  utils::read(in, chrMode);
  utils::read(in, reinterpret_cast<char*>(&prgOffsets[0]), sizeof(prgOffsets));
  utils::read(in, reinterpret_cast<char*>(&chrOffsets[0]), sizeof(chrOffsets));
  utils::read(in, reload);
  utils::read(in, counter);
  utils::read(in, irqEnable);
}

void Mapper4::Step() {
  PPU* ppu = console->ppu;
  if (ppu->cycle != 280) { // TODO: this *should* be 260
    return;
  }
  if ((ppu->scanline > 239) && (ppu->scanline < 261)) {
    return;
  }
  if ((ppu->flagShowBackground == 0) && (ppu->flagShowSprites == 0)) {
    return;
  }
  HandleScanLine();
}

uint8 Mapper4::Read(uint16 address) {
  if (address < 0x2000) {
    uint16 bank = address / 0x0400;
    uint16 offset = address % 0x0400;
    return cartridge->CHR[chrOffsets[bank]+int32(offset)];
  } else if (address >= 0x8000) {
    address = address - 0x8000;
    uint16 bank = address / 0x2000;
    uint16 offset = address % 0x2000;
    return cartridge->PRG[prgOffsets[bank]+int32(offset)];
  } else if (address >= 0x6000) {
    return cartridge->SRAM[int32(address)-0x6000];
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper4 read at address: 0x%04X", address)
  }
  return 0;
}

void Mapper4::Write(uint16 address, uint8 value) {
  if(address < 0x2000) {
    uint16 bank = address / 0x0400;
    uint16 offset = address % 0x0400;
    cartridge->CHR[chrOffsets[bank]+int32(offset)] = value;
  } else if (address >= 0x8000) {
    writeRegister(address, value);
  } else if (address >= 0x6000) {
    cartridge->SRAM[int32(address)-0x6000] = value;
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled mapper4 write at address: 0x%04X", address)
  }
}

void Mapper4::HandleScanLine() {
  if (counter == 0) {
    counter = reload;
  } else {
    counter--;
    if ((counter == 0) && irqEnable) {
      console->cpu->triggerIRQ();
    }
  }
}

void Mapper4::writeRegister(uint16 address, uint8 value) {
  if (address <= 0x9FFF && address%2 == 0) {
    writeBankSelect(value);
  } else if (address <= 0x9FFF && address%2 == 1) {
    writeBankData(value);
  } else if (address <= 0xBFFF && address%2 == 0) {
    writeMirror(value);
  } else if (address <= 0xBFFF && address%2 == 1) {
    writeProtect(value);
  } else if (address <= 0xDFFF && address%2 == 0) {
    writeIRQLatch(value);
  } else if (address <= 0xDFFF && address%2 == 1) {
    writeIRQReload(value);
  } else if (address%2 == 0) {
    writeIRQDisable(value);
  } else if (address%2 == 1) {
    writeIRQEnable(value);
  }
}

void Mapper4::writeBankSelect(uint8 value) {
  prgMode = (value >> 6) & 1;
  chrMode = (value >> 7) & 1;
  reg = value & 7;
  updateOffsets();
}

void Mapper4::writeBankData(uint8 value) {
  registers[reg] = value;
  updateOffsets();
}

void Mapper4::writeMirror(uint8 value) {
  switch (value & 1) {
  case 0:
    cartridge->mirror = MirrorModes::Vertical;
    break;
  case 1:
    cartridge->mirror = MirrorModes::Horizontal;
    break;
  }
}

void Mapper4::writeProtect(uint8 value) {}

void Mapper4::writeIRQLatch(uint8 value) {
  reload = value;
}

void Mapper4::writeIRQReload(uint8 value) {
  counter = 0;
}

void Mapper4::writeIRQDisable(uint8 value) {
  irqEnable = false;
}

void Mapper4::writeIRQEnable(uint8 value) {
  irqEnable = true;
}

int32 Mapper4::prgBankOffset(int32 index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->prgLength / 0x2000;
  int32 offset = index * 0x2000;
  if (offset < 0) {
    offset += cartridge->prgLength;
  }
  return offset;
}

int32 Mapper4::chrBankOffset(int32 index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->chrLength / 0x0400;
  int32 offset = index * 0x0400;
  if (offset < 0) {
    offset += cartridge->chrLength;
  }
  return offset;
}

void Mapper4::updateOffsets() {
  switch (prgMode) {
  case 0:
    prgOffsets[0] = prgBankOffset(int32(registers[6]));
    prgOffsets[1] = prgBankOffset(int32(registers[7]));
    prgOffsets[2] = prgBankOffset(-2);
    prgOffsets[3] = prgBankOffset(-1);
    break;
  case 1:
    prgOffsets[0] = prgBankOffset(-2);
    prgOffsets[1] = prgBankOffset(int32(registers[7]));
    prgOffsets[2] = prgBankOffset(int32(registers[6]));
    prgOffsets[3] = prgBankOffset(-1);
    break;
  }
  switch (chrMode) {
  case 0:
    chrOffsets[0] = chrBankOffset(int32(registers[0] & 0xFE));
    chrOffsets[1] = chrBankOffset(int32(registers[0] | 0x01));
    chrOffsets[2] = chrBankOffset(int32(registers[1] & 0xFE));
    chrOffsets[3] = chrBankOffset(int32(registers[1] | 0x01));
    chrOffsets[4] = chrBankOffset(int32(registers[2]));
    chrOffsets[5] = chrBankOffset(int32(registers[3]));
    chrOffsets[6] = chrBankOffset(int32(registers[4]));
    chrOffsets[7] = chrBankOffset(int32(registers[5]));
    break;
  case 1:
    chrOffsets[0] = chrBankOffset(int32(registers[2]));
    chrOffsets[1] = chrBankOffset(int32(registers[3]));
    chrOffsets[2] = chrBankOffset(int32(registers[4]));
    chrOffsets[3] = chrBankOffset(int32(registers[5]));
    chrOffsets[4] = chrBankOffset(int32(registers[0] & 0xFE));
    chrOffsets[5] = chrBankOffset(int32(registers[0] | 0x01));
    chrOffsets[6] = chrBankOffset(int32(registers[1] & 0xFE));
    chrOffsets[7] = chrBankOffset(int32(registers[1] | 0x01));
    break;
  }
}

}  // namespace nes
