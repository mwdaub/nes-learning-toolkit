#include "ppu.h"

#include <memory>

#include "console.h"
#include "cpu.h"
#include "palette.h"
#include "utils.h"

using std::unique_ptr;

namespace nes {

void PPUState::Save(ostream& out) {
  utils::writeInt32(out, cycle);
  utils::writeInt32(out, scanline);
  utils::writeUint64(out, frame);
  utils::writeUint8Array(out, paletteData);
  utils::writeUint8Array(out, nameTableData);
  utils::writeUint8Array(out, oamData);
  utils::writeUint16(out, v);
  utils::writeUint16(out, t);
  utils::writeUint8(out, x);
  utils::writeUint8(out, w);
  utils::writeUint8(out, f);
  utils::writeUint8(out, reg);
  utils::writeBool(out, nmiOccurred);
  utils::writeBool(out, nmiOutput);
  utils::writeBool(out, nmiPrevious);
  utils::writeUint8(out, nmiDelay);
  utils::writeUint8(out, nameTableByte);
  utils::writeUint8(out, attributeTableByte);
  utils::writeUint8(out, lowTileByte);
  utils::writeUint8(out, highTileByte);
  utils::writeUint64(out, tileData);
  utils::writeInt32(out, spriteCount);
  utils::writeUint32Array(out, spritePatterns);
  utils::writeUint8Array(out, spritePositions);
  utils::writeUint8Array(out, spritePriorities);
  utils::writeUint8Array(out, spriteIndexes);
  utils::writeUint8(out, flagNameTable);
  utils::writeUint8(out, flagIncrement);
  utils::writeUint8(out, flagSpriteTable);
  utils::writeUint8(out, flagBackgroundTable);
  utils::writeUint8(out, flagSpriteSize);
  utils::writeUint8(out, flagMasterSlave);
  utils::writeUint8(out, flagGrayscale);
  utils::writeUint8(out, flagShowLeftBackground);
  utils::writeUint8(out, flagShowLeftSprites);
  utils::writeUint8(out, flagShowBackground);
  utils::writeUint8(out, flagShowSprites);
  utils::writeUint8(out, flagRedTint);
  utils::writeUint8(out, flagGreenTint);
  utils::writeUint8(out, flagBlueTint);
  utils::writeUint8(out, flagSpriteZeroHit);
  utils::writeUint8(out, flagSpriteOverflow);
  utils::writeUint8(out, oamAddress);
  utils::writeUint8(out, bufferedData);
}

void PPUState::Load(istream& in) {
  cycle = utils::readInt32(in);
  scanline = utils::readInt32(in);
  frame = utils::readUint64(in);
  utils::readUint8Array(in, paletteData);
  utils::readUint8Array(in, nameTableData);
  utils::readUint8Array(in, oamData);
  v = utils::readUint16(in);
  t = utils::readUint16(in);
  x = utils::readUint8(in);
  w = utils::readUint8(in);
  f = utils::readUint8(in);
  reg = utils::readUint8(in);
  nmiOccurred = utils::readBool(in);
  nmiOutput = utils::readBool(in);
  nmiPrevious = utils::readBool(in);
  nmiDelay = utils::readUint8(in);
  nameTableByte = utils::readUint8(in);
  attributeTableByte = utils::readUint8(in);
  lowTileByte = utils::readUint8(in);
  highTileByte = utils::readUint8(in);
  tileData = utils::readUint64(in);
  spriteCount = utils::readInt32(in);
  utils::readUint32Array(in, spritePatterns);
  utils::readUint8Array(in, spritePositions);
  utils::readUint8Array(in, spritePriorities);
  utils::readUint8Array(in, spriteIndexes);
  flagNameTable = utils::readUint8(in);
  flagIncrement = utils::readUint8(in);
  flagSpriteTable = utils::readUint8(in);
  flagBackgroundTable = utils::readUint8(in);
  flagSpriteSize = utils::readUint8(in);
  flagMasterSlave = utils::readUint8(in);
  flagGrayscale = utils::readUint8(in);
  flagShowLeftBackground = utils::readUint8(in);
  flagShowLeftSprites = utils::readUint8(in);
  flagShowBackground = utils::readUint8(in);
  flagShowSprites = utils::readUint8(in);
  flagRedTint = utils::readUint8(in);
  flagGreenTint = utils::readUint8(in);
  flagBlueTint = utils::readUint8(in);
  flagSpriteZeroHit = utils::readUint8(in);
  flagSpriteOverflow = utils::readUint8(in);
  oamAddress = utils::readUint8(in);
  bufferedData = utils::readUint8(in);
}

uint8 PPU::Read(uint16 address) {
  address = address % 0x4000;
  if (address < 0x2000) {
    return console->mapper->Read(address);
  } else if (address < 0x3F00) {
    uint8 mode = console->cartridge->mirror;
    return nameTableData[MirrorAddress(mode, address)%2048];
  } else if (address < 0x4000) {
    return readPalette(address % 32);
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled ppu memory read at address: 0x%04X", address)
  }
  return 0;
}

void PPU::Write(uint16 address, uint8 value) {
  address = address % 0x4000;
  if (address < 0x2000) {
    console->mapper->Write(address, value);
  } else if (address < 0x3F00) {
    uint8 mode = console->cartridge->mirror;
    nameTableData[MirrorAddress(mode, address)%2048] = value;
  } else if (address < 0x4000) {
    writePalette(address%32, value);
  } else {
    // TODO: add logging
    // log.Fatalf("unhandled ppu memory write at address: 0x%04X", address)
  }
}

void PPU::Reset() {
  cycle = 340;
  scanline = 240;
  frame = 0;
  writeControl(0);
  writeMask(0);
  writeOAMAddress(0);
}

uint8 PPU::readPalette(uint16 address) {
  if (address >= 16 && (address%4) == 0) {
    address -= 16;
  }
  return paletteData[address];
}

void PPU::writePalette(uint16 address, uint8 value) {
  if (address >= 16 && (address%4) == 0) {
    address -= 16;
  }
  paletteData[address] = value;
}

uint8 PPU::readRegister(uint16 address) {
  switch (address) {
  case 0x2002:
    return readStatus();
  case 0x2004:
    return readOAMData();
  case 0x2007:
    return readData();
  }
  return 0;
}

void PPU::writeRegister(uint16 address, uint8 value) {
  reg = value;
  switch (address) {
  case 0x2000:
    writeControl(value);
    break;
  case 0x2001:
    writeMask(value);
    break;
  case 0x2003:
    writeOAMAddress(value);
    break;
  case 0x2004:
    writeOAMData(value);
    break;
  case 0x2005:
    writeScroll(value);
    break;
  case 0x2006:
    writeAddress(value);
    break;
  case 0x2007:
    writeData(value);
    break;
  case 0x4014:
    writeDMA(value);
    break;
  }
}

// $2000: PPUCTRL
void PPU::writeControl(uint8 value) {
  flagNameTable = (value >> 0) & 3;
  flagIncrement = (value >> 2) & 1;
  flagSpriteTable = (value >> 3) & 1;
  flagBackgroundTable = (value >> 4) & 1;
  flagSpriteSize = (value >> 5) & 1;
  flagMasterSlave = (value >> 6) & 1;
  nmiOutput = ((value>>7)&1) == 1;
  nmiChange();
  // t: ....BA.. ........ = d: ......BA
  t = (t & 0xF3FF) | ((uint16(value) & 0x03) << 10);
}

// $2001: PPUMASK
void PPU::writeMask(uint8 value) {
  flagGrayscale = (value >> 0) & 1;
  flagShowLeftBackground = (value >> 1) & 1;
  flagShowLeftSprites = (value >> 2) & 1;
  flagShowBackground = (value >> 3) & 1;
  flagShowSprites = (value >> 4) & 1;
  flagRedTint = (value >> 5) & 1;
  flagGreenTint = (value >> 6) & 1;
  flagBlueTint = (value >> 7) & 1;
}

// $2002: PPUSTATUS
uint8 PPU::readStatus() {
  uint8 result = reg & 0x1F;
  result |= flagSpriteOverflow << 5;
  result |= flagSpriteZeroHit << 6;
  if (nmiOccurred) {
    result |= 1 << 7;
  }
  nmiOccurred = false;
  nmiChange();
  // w:                   = 0
  w = 0;
  return result;
}

// $2003: OAMADDR
void PPU::writeOAMAddress(uint8 value) {
  oamAddress = value;
}

// $2004: OAMDATA (read)
uint8 PPU::readOAMData() {
  return oamData[oamAddress];
}

// $2004: OAMDATA (write)
void PPU::writeOAMData(uint8 value) {
  oamData[oamAddress] = value;
  oamAddress++;
}

// $2005: PPUSCROLL
void PPU::writeScroll(uint8 value) {
  if (w == 0) {
    // t: ........ ...HGFED = d: HGFED...
    // x:               CBA = d: .....CBA
    // w:                   = 1
    t = (t & 0xFFE0) | (uint16(value) >> 3);
    x = value & 0x07;
    w = 1;
  } else {
    // t: .CBA..HG FED..... = d: HGFEDCBA
    // w:                   = 0
    t = (t & 0x8FFF) | ((uint16(value) & 0x07) << 12);
    t = (t & 0xFC1F) | ((uint16(value) & 0xF8) << 2);
    w = 0;
  }
}

// $2006: PPUADDR
void PPU::writeAddress(uint8 value) {
  if (w == 0) {
    // t: ..FEDCBA ........ = d: ..FEDCBA
    // t: .X...... ........ = 0
    // w:                   = 1
    t = (t & 0x80FF) | ((uint16(value) & 0x3F) << 8);
    w = 1;
  } else {
    // t: ........ HGFEDCBA = d: HGFEDCBA
    // v                    = t
    // w:                   = 0
    t = (t & 0xFF00) | uint16(value);
    v = t;
    w = 0;
  }
}

// $2007: PPUDATA (read)
uint8 PPU::readData() {
  uint8 value = Read(v);
  // emulate buffered reads
  if (v%0x4000 < 0x3F00) {
    uint8 buffered = bufferedData;
    bufferedData = value;
    value = buffered;
  } else {
    bufferedData = Read(v - 0x1000);
  }
  // increment address
  if (flagIncrement == 0) {
    v += 1;
  } else {
    v += 32;
  }
  return value;
}

// $2007: PPUDATA (write)
void PPU::writeData(uint8 value) {
  Write(v, value);
  if (flagIncrement == 0) {
    v += 1;
  } else {
    v += 32;
  }
}

// $4014: OAMDMA
void PPU::writeDMA(uint8 value) {
  CPU* cpu = console->cpu.get();
  uint16 address = uint16(value) << 8;
  for (int32 i = 0; i < 256; i++) {
    oamData[oamAddress] = cpu->Read(address);
    oamAddress++;
    address++;
  }
  cpu->stall += 513;
  if (cpu->cycles%2 == 1) {
    cpu->stall++;
  }
}

// NTSC Timing Helper Functions

void PPU::incrementX() {
  // increment hori(v)
  // if coarse X == 31
  if ((v&0x001F) == 31) {
    // coarse X = 0
    v &= 0xFFE0;
    // switch horizontal nametable
    v ^= 0x0400;
  } else {
    // increment coarse X
    v++;
  }
}

void PPU::incrementY() {
  // increment vert(v)
  // if fine Y < 7
  if ((v&0x7000) != 0x7000) {
    // increment fine Y
    v += 0x1000;
  } else {
    // fine Y = 0
    v &= 0x8FFF;
    // let y = coarse Y
    uint16 y = (v & 0x03E0) >> 5;
    if (y == 29) {
      // coarse Y = 0
      y = 0;
      // switch vertical nametable
      v ^= 0x0800;
    } else if (y == 31) {
      // coarse Y = 0, nametable not switched
      y = 0;
    } else {
      // increment coarse Y
      y++;
    }
    // put coarse Y back into v
    v = (v & 0xFC1F) | (y << 5);
  }
}

void PPU::copyX() {
  // hori(v) = hori(t)
  // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
  v = (v & 0xFBE0) | (t & 0x041F);
}

void PPU::copyY() {
  // vert(v) = vert(t)
  // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
  v = (v & 0x841F) | (t & 0x7BE0);
}

void PPU::nmiChange() {
  bool nmi = nmiOutput && nmiOccurred;
  if (nmi && !nmiPrevious) {
    // TODO: this fixes some games but the delay shouldn't have to be so
    // long, so the timings are off somewhere
    nmiDelay = 15;
  }
  nmiPrevious = nmi;
}

void PPU::setVerticalBlank() {
  front.swap(back);
  nmiOccurred = true;
  nmiChange();
}

void PPU::clearVerticalBlank() {
  nmiOccurred = false;
  nmiChange();
}

void PPU::fetchNameTableByte() {
  uint16 address = 0x2000 | (v & 0x0FFF);
  nameTableByte = Read(address);
}

void PPU::fetchAttributeTableByte() {
  uint16 address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
  uint16 shift = ((v >> 4) & 4) | (v & 2);
  attributeTableByte = ((Read(address) >> shift) & 3) << 2;
}

void PPU::fetchLowTileByte() {
  uint16 fineY = (v >> 12) & 7;
  uint8 table = flagBackgroundTable;
  uint8 tile = nameTableByte;
  uint16 address = 0x1000*uint16(table) + uint16(tile)*16 + fineY;
  lowTileByte = Read(address);
}

void PPU::fetchHighTileByte() {
  uint16 fineY = (v >> 12) & 7;
  uint8 table = flagBackgroundTable;
  uint8 tile = nameTableByte;
  uint16 address = 0x1000*uint16(table) + uint16(tile)*16 + fineY;
  highTileByte = Read(address + 8);
}

void PPU::storeTileData() {
  uint32 data = 0;
  for (int32 i = 0; i < 8; i++) {
    uint8 a = attributeTableByte;
    uint8 p1 = (lowTileByte & 0x80) >> 7;
    uint8 p2 = (highTileByte & 0x80) >> 6;
    lowTileByte <<= 1;
    highTileByte <<= 1;
    data <<= 4;
    data |= uint32(a | p1 | p2);
  }
  tileData |= uint64(data);
}

uint32 PPU::fetchTileData() {
  return uint32(tileData >> 32);
}

uint8 PPU::backgroundPixel() {
  if (flagShowBackground == 0) {
    return 0;
  }
  uint32 data = fetchTileData() >> ((7 - x) * 4);
  return uint8(data & 0x0F);
}

void PPU::spritePixel(pair<uint8, uint8>* pixels) {
  if (flagShowSprites == 0) {
    return;
  }
  for (int32 i = 0; i < spriteCount; i++) {
    int32 offset = (cycle - 1) - int32(spritePositions[i]);
    if (offset < 0 || offset > 7) {
      continue;
    }
    offset = 7 - offset;
    uint8 color = uint8((spritePatterns[i] >> uint8(offset*4)) & 0x0F);
    if (color%4 == 0) {
      continue;
    }
    pixels->first = uint8(i);
    pixels->second = color;
    return;
  }
  return;
}

void PPU::renderPixel() {
  int32 x = cycle - 1;
  int32 y = scanline;
  uint8 background = backgroundPixel();
  pair<uint8, uint8> pixels;
  spritePixel(&pixels);
  uint8 i = pixels.first;
  uint8 sprite = pixels.second;
  if (x < 8 && flagShowLeftBackground == 0) {
    background = 0;
  }
  if (x < 8 && flagShowLeftSprites == 0) {
    sprite = 0;
  }
  bool b = (background%4) != 0;
  bool s = (sprite%4) != 0;
  uint8 color = 0;
  if (!b && !s) {
    color = 0;
  } else if (!b && s) {
    color = sprite | 0x10;
  } else if (b && !s) {
    color = background;
  } else {
    if (spriteIndexes[i] == 0 && x < 255) {
      flagSpriteZeroHit = 1;
    }
    if (spritePriorities[i] == 0) {
      color = sprite | 0x10;
    } else {
      color = background;
    }
  }
  uint8 pixel = readPalette(uint16(color))%64;
  back->SetPixel(x, y, pixel);
}

uint32 PPU::fetchSpritePattern(int32 i, int32 row) {
  uint8 tile = oamData[i*4+1];
  uint8 attributes = oamData[i*4+2];
  uint16 address = 0;
  if (flagSpriteSize == 0) {
    if ((attributes&0x80) == 0x80) {
      row = 7 - row;
    }
    uint8 table = flagSpriteTable;
    address = 0x1000*uint16(table) + uint16(tile)*16 + uint16(row);
  } else {
    if ((attributes&0x80) == 0x80) {
      row = 15 - row;
    }
    uint8 table = tile & 1;
    tile &= 0xFE;
    if (row > 7) {
      tile++;
      row -= 8;
    }
    address = 0x1000*uint16(table) + uint16(tile)*16 + uint16(row);
  }
  uint8 a = (attributes & 3) << 2;
  uint8 lowTileByte = Read(address);
  uint8 highTileByte = Read(address + 8);
  uint32 data = 0;
  for (int32 i = 0; i < 8; i++) {
    uint8 p1 = 0, p2 = 0;
    if ((attributes&0x40) == 0x40) {
      p1 = (lowTileByte & 1) << 0;
      p2 = (highTileByte & 1) << 1;
      lowTileByte >>= 1;
      highTileByte >>= 1;
    } else {
      p1 = (lowTileByte & 0x80) >> 7;
      p2 = (highTileByte & 0x80) >> 6;
      lowTileByte <<= 1;
      highTileByte <<= 1;
    }
    data <<= 4;
    data |= uint32(a | p1 | p2);
  }
  return data;
}

void PPU::evaluateSprites() {
  int32 h = 0;
  if (flagSpriteSize == 0) {
    h = 8;
  } else {
    h = 16;
  }
  int32 count = 0;
  for (int32 i = 0; i < 64; i++) {
    uint8 y = oamData[i*4+0];
    uint8 a = oamData[i*4+2];
    uint8 x = oamData[i*4+3];
    int32 row = scanline - int32(y);
    if (row < 0 || row >= h) {
      continue;
    }
    if (count < 8) {
      spritePatterns[count] = fetchSpritePattern(i, row);
      spritePositions[count] = x;
      spritePriorities[count] = (a >> 5) & 1;
      spriteIndexes[count] = uint8(i);
    }
    count++;
  }
  if (count > 8) {
    count = 8;
    flagSpriteOverflow = 1;
  }
  spriteCount = count;
}

// tick updates cycle, scanline and frame counters
void PPU::tick() {
  if (nmiDelay > 0) {
    nmiDelay--;
    if (nmiDelay == 0 && nmiOutput && nmiOccurred) {
      console->cpu->triggerNMI();
    }
  }

  if (flagShowBackground != 0 || flagShowSprites != 0) {
    if (f == 1 && scanline == 261 && cycle == 339) {
      cycle = 0;
      scanline = 0;
      frame++;
      f ^= 1;
      return;
    }
  }
  cycle++;
  if (cycle > 340) {
    cycle = 0;
    scanline++;
    if (scanline > 261) {
      scanline = 0;
      frame++;
      f ^= 1;
    }
  }
}

// Step executes a single PPU cycle
void PPU::Step() {
  tick();

  bool renderingEnabled = flagShowBackground != 0 || flagShowSprites != 0;
  bool preLine = (scanline == 261);
  bool visibleLine = (scanline < 240);
  // postLine := scanline == 240
  bool renderLine = (preLine || visibleLine);
  bool preFetchCycle = (cycle >= 321 && cycle <= 336);
  bool visibleCycle = (cycle >= 1 && cycle <= 256);
  bool fetchCycle = (preFetchCycle || visibleCycle);

  // background logic
  if (renderingEnabled) {
    if (visibleLine && visibleCycle) {
      renderPixel();
    }
    if (renderLine && fetchCycle) {
      tileData <<= 4;
      switch (cycle % 8) {
      case 1:
        fetchNameTableByte();
        break;
      case 3:
        fetchAttributeTableByte();
        break;
      case 5:
        fetchLowTileByte();
        break;
      case 7:
        fetchHighTileByte();
        break;
      case 0:
        storeTileData();
        break;
      }
    }
    if (preLine && cycle >= 280 && cycle <= 304) {
      copyY();
    }
    if (renderLine) {
      if (fetchCycle && cycle%8 == 0) {
        incrementX();
      }
      if (cycle == 256) {
        incrementY();
      }
      if (cycle == 257) {
        copyX();
      }
    }
  }

  // sprite logic
  if (renderingEnabled) {
    if (cycle == 257) {
      if (visibleLine) {
        evaluateSprites();
      } else {
        spriteCount = 0;
      }
    }
  }

  // vblank logic
  if (scanline == 241 && cycle == 1) {
    setVerticalBlank();
  }
  if (preLine && cycle == 1) {
    clearVerticalBlank();
    flagSpriteZeroHit = 0;
    flagSpriteOverflow = 0;
  }
}

}  // namespace nes
