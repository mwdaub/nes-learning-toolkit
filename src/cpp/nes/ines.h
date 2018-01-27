#ifndef NES_INES_H
#define NES_INES_H

#include <iostream>
#include <fstream>

#include "types.h"
#include "cartridge.h"
#include "utils.h"

using namespace std;

namespace nes {

static uint32 iNESFileMagic = 0x1a53454e;

// LoadNESFile reads an iNES file (.nes) and returns a Cartridge on success.
// http://wiki.nesdev.com/w/index.php/INES
// http://nesdev.com/NESDoc.pdf (page 28)
Cartridge* LoadiNESFile(string path) {
  fstream fs;
  fs.open(path.c_str(), fstream::in);

  // read file header
  uint32 magic;
  uint8 numPRG;
  uint8 numCHR;
  uint8 control1;
  uint8 control2;
  uint8 numRAM;
  uint8 padding[7];
  utils::read(fs, magic);
  utils::read(fs, numPRG);
  utils::read(fs, numCHR);
  utils::read(fs, control1);
  utils::read(fs, control2);
  utils::read(fs, numRAM);
  utils::read(fs, reinterpret_cast<char*>(&padding[0]), sizeof(padding));
  if (magic != iNESFileMagic) {
    return NULL;
  }

  // mapper type
  uint8 mapper1 = control1 >> 4;
  uint8 mapper2 = control2 >> 4;
  uint8 mapper = mapper1 | (mapper2 << 4);

  // mirroring type
  uint8 mirror1 = control1 & 1;
  uint8 mirror2 = (control1 >> 3) & 1;
  uint8 mirror = mirror1 | (mirror2 << 1);

  // battery-backed RAM
  uint8 battery = (control1 >> 1) & 1;

  // read trainer if present (unused)
  if ((control1&4) == 4) {
    cout << "Shouldn't be here." << endl;
    // todo
  }

  // read prg-rom bank(s)
  uint32 prgLength = numPRG*16384;
  uint8* prg = new uint8[prgLength];
  utils::read(fs, reinterpret_cast<char*>(prg), prgLength);

  // read chr-rom bank(s)
  uint32 chrLength;
  uint8* chr;
  if (numCHR == 0) {
    // provide chr-rom/ram if not in file
    chrLength = 8192;
    chr = new uint8[chrLength];
  } else {
    chrLength = numCHR*8192;
    chr = new uint8[chrLength];
    utils::read(fs, reinterpret_cast<char*>(chr), chrLength);
  }

  if (fs.fail()) {
    return 0;
  }

  fs.close();

  // success
  return new Cartridge(prg, prgLength, chr, chrLength, mapper, mirror, battery);
}

}  // namespace nes

#endif  // define NES_INES_H
