#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H

#include <iostream>

#include "types.h"

using namespace std;

namespace nes {

class Console;

class Cartridge {
  public:
    Cartridge(uint8* prg, uint32 prgLength, uint8* chr, uint32 chrLength, uint8 mapper, uint8 mirror, uint8 battery) :
        PRG(prg),
        prgLength(prgLength),
        CHR(chr),
        chrLength(chrLength),
        SRAM{},
        mapper(mapper),
        mirror(mirror),
        battery(battery) {};
    uint8* PRG;         // PRG-ROM banks
    uint32 prgLength;
    uint8* CHR;         // CHR-ROM banks
    uint32 chrLength;
    uint8 SRAM[0x2000]; // Save RAM
    uint8 mapper;       // mapper type
    uint8 mirror;       // mirroring mode
    uint8 battery;      // battery present

    void Save(ostream& out);
    void Load(istream& in);
};

}  // namespace nes

#endif  // define NES_CARTRIDGE_H
